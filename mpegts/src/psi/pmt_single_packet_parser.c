#include "mpeg/ts/data/psi/psi_magics.h"
#include "mpeg/ts/packet/packet_magics.h"
#include <mpeg/ts/data/psi/pmt_parser.h>

/*
 * |byte0 | |  byte1  || byte2|
 * 0b00000010_1_0_00_000000000000
 */
#define PMT_MAX_SECTION_OFFSET_IN_SINGLE_PACKET (MPEG_TS_PACKET_SIZE - 3)

MpegTsPMTMaybe_t mpeg_ts_parse_pmt_from_packet(MpegTsPacket_t *packet)
{
    const MpegTsPMTMaybe_t bad_value = {.has_value = false, .value = {0}};

    if (packet->header.pid == MPEG_TS_NULL_PACKET_PID) {
        return bad_value;
    }

    if (packet->header.adaptation_field_control == MPEG_TS_ADAPT_CONTROL_ONLY) {
        return bad_value;
    }

    if (packet->header.adaptation_field_control == MPEG_TS_ADAPT_CONTROL_WITH_PAYLOAD) {
        return bad_value;
    }

    if (!packet->header.payload_unit_start_indicator) {
        return bad_value;
    }

    bool is_PES_packet_at_start = true;

    if (is_PES_packet_at_start) {
        is_PES_packet_at_start &= packet->data[0] == 0;
    }

    if (is_PES_packet_at_start) {
        is_PES_packet_at_start &= packet->data[1] == 0;
    }

    if (is_PES_packet_at_start) {
        is_PES_packet_at_start &= packet->data[2] == 1;
    }

    if (is_PES_packet_at_start) {
        return bad_value;
    }

    uint8_t section_offset = packet->data[0]; // aka PSI pointer_field
    section_offset += 1;                      // include itself to make offset

    if (section_offset >= PMT_MAX_SECTION_OFFSET_IN_SINGLE_PACKET) {
        return bad_value;
    }

    uint8_t *section_data = packet->data + section_offset;

    /*
     * section_data:
     *    |byte0 | |  byte1  || byte2|
     *  0b00000010_1_0_00_000000000000
     *       ^     ^ ^ ^  ^
     *       |     | | |  |
     *       |     | z |  +-------- length
     *   table_id  | e |
     *    (0x02)   | r +----- reserved
     *             | o
     *             |
     *   section_syntax_indicator
     *        (should be set)
     */

    bool is_PMT = true;

    /*
     *
     *    |byte0 | |  byte1  || byte2|
     *  0b00000010_1_0_00_000000000000
     *    ^^^^^^^^
     *    +------+
     *        |
     *///     |
    if (section_data[0] != MPEG_TS_PSI_PMT_SECTION_ID) {
        is_PMT = false;
    }

    if (!is_PMT) {
        return bad_value;
    }

    /*
     *
     *    |byte0 | |  byte1  || byte2|
     *  0b00000010_1_0_00_000000000000
     *             ^ ^ ^^ ^^^^
     *             +---------+
     *                  |
     *///               |
    uint8_t flag_len = section_data[1];

    if ((flag_len & MPEG_TS_PSI_PMT_SECTION_SYNTAX_INDICATOR_BIT) !=
        MPEG_TS_PSI_PMT_SECTION_SYNTAX_INDICATOR_BIT) {
        return bad_value;
    }

    if ((flag_len & MPEG_TS_PSI_PMT_ZERO_BIT) != 0) {
        return bad_value;
    }

    uint16_t length = 0;

    /*
     *                                  |  byte1  |
     *                                  1_0_00_0000
     *                                         ^^^^
     *                                       select this
     */
    uint8_t length_part_from_byte1 = flag_len & 0x0f;
    uint8_t length_part_from_byte2 = section_data[2];

    length |= (length_part_from_byte1 << 8) | length_part_from_byte2;

    if (length >= MPEG_TS_PSI_PMT_MAX_SIZE) {
        return bad_value;
    }

    MpegTsPMTMaybe_t ret_val;
    ret_val.has_value = true;

    MpegTsPMT_t *value_ref = &ret_val.value;

    value_ref->section_length = length;

    return bad_value; // TODO: finalyze and return ret_value instead
}
