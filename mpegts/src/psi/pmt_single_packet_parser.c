#include <assert.h>

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
        //                        ^^^^^^^^^^^^^^^
        //                    can be PSI pointer_field
    }

    if (is_PES_packet_at_start) {
        is_PES_packet_at_start &= packet->data[1] == 0; // can be PAS table_id if pointer_field == 0
    }

    if (is_PES_packet_at_start) {
        is_PES_packet_at_start &= packet->data[2] == 1;
    }

    // actualy it can be PAS table if pointer_field (packet->data[0]) is 0
    // Filters above cannot distinct PES and PAS
    // but its anought to distinct PMT from PES
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
     *       ^     ^ ^ ^      ^
     *       |     | | |      |
     *       |     | z |      +---- length
     *   table_id  | e |
     *    (0x02)   | r +----- reserved
     *             | o
     *             |
     *   section_syntax_indicator
     *        (should be set)
     */

    bool is_PMT = true;

    /*
     * section_data:
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

    if ((flag_len & MPEG_TS_PSI_PMT_SHOULD_BE_ZERO_BIT) != 0) {
        return bad_value;
    }

    uint16_t section_length = 0;

    /*
     *     |  byte1  |
     *     1_0_00_0000
     *            ^^^^
     *         select this
     */
    uint8_t length_part_from_byte1 = flag_len & 0x0f;
    uint8_t length_part_from_byte2 = section_data[2];

    section_length |= (length_part_from_byte1 << 8) | length_part_from_byte2;

    if (section_length >= MPEG_TS_PSI_PMT_SECTION_MAX_LENGTH) {
        return bad_value;
    }

    MpegTsPMTMaybe_t ret_val;
    ret_val.has_value = true;

    MpegTsPMT_t *value_ref = &ret_val.value;

    value_ref->section_length = section_length;

    /*
     * section_data:
     *    |byte3 | |byte4 |
     *  0b01000010_00110010 <-- all 16 bits is program number
     */
    value_ref->program_number = 0;
    value_ref->program_number |= section_data[3] << 8;
    value_ref->program_number |= section_data[4];

    /*
     * section_data:
     *    | byte5  |
     *  0b00_00001_0
     *    ^^ ^^^^^ ^
     *     | |-+-| |
     *     |   |   |
     *     |   |   +----- current_next_indicator
     *     |   |
     *     |   |
     *     |   +-------- version_number
     *     |
     *     +--- reserved
     *
     */

    value_ref->version_number = 0;
    /*
     * section_data:
     *    | byte5  |
     *  0b00_00101_0
     *       ^^^^^ --select this and shift to 1 byte rigth
     */
    value_ref->version_number = (section_data[5] & MPEG_TS_PSI_PMT_VERSION_NUMBER_MASK) >> 1;

    /*
     * section_data:
     *    | byte5  |
     *  0b00_00000_0
     *             ^
     *             |
     *             +--- current_next_indicator is set?
     */

    value_ref->current_next_indicator =
        (section_data[5] & MPEG_TS_PSI_PMT_CURRENT_NEXT_INDICATOR_BIT) ==
        MPEG_TS_PSI_PMT_CURRENT_NEXT_INDICATOR_BIT;

    if (!value_ref->current_next_indicator) {
        return bad_value;
    }

    /*
     * section_data:
     *    |byte6 |
     *  0b00000000 - section_number ("shall be set to zero")
     */

    if (section_data[6] != 0) {
        return bad_value;
    }

    /*
     * section_data:
     *    |byte7 |
     *  0b00000000 - last_section_number ("shall be set to zero")
     */

    if (section_data[7] != 0) {
        return bad_value;
    }

    /*
     * section_data:
     *    |byte8   |byte9 |
     *  0b000_0000000000000
     *     ^        ^
     *     |        |
     *     |        |
     *     |      PCR_PID
     *     |
     *     +--------- reserved
     */

    value_ref->PCR_PID = 0;

    /*
     * section_data:
     *    |byte8   |
     *  0b000_0000000000000
     *        ^^^^^ -select this than shift it left by 8 bits than merge to PCR_PID MSB
     */

    value_ref->PCR_PID |= (section_data[8] & MPEG_TS_PSI_PMT_PCR_PID_BYTE_0_MASK) << 8;

    /*
     * section_data:
     *    |byte8   |byte9 |
     *  0b000_0000000000000
     *             ^^^^^^^^ - merge it to PCR_PID LSB
     */

    value_ref->PCR_PID |= section_data[9];

    /*
     * section_data:
     *    |byte10||byte11|
     *  0b0000_000000000000
     *     ^        ^
     *     |        |
     *     |        +---- program_info_length
     *     |
     *     +---- reserved
     */

    value_ref->program_info_length = 0;

    value_ref->program_info_length |=
        (section_data[10] & MPEG_TS_PSI_PMT_PROGRAM_INFO_LENGTH_BYTE_0_MASK) << 8;

    value_ref->program_info_length |= section_data[11];

    value_ref->program_info_descriptors_data =
        section_data + MPEG_TS_PSI_PMT_INFO_DESCRIPTORS_OFFSET;

    uint8_t *elements_stream_data =
        section_data + MPEG_TS_PSI_PMT_INFO_DESCRIPTORS_OFFSET + value_ref->program_info_length;

    value_ref->program_elements = elements_stream_data;

    assert(
        value_ref->section_length > (MPEG_TS_PSI_PMT_INFO_DESCRIPTORS_OFFSET +
                                        value_ref->program_info_length + sizeof(value_ref->CRC)));

    uint16_t elements_stream_data_size = value_ref->section_length -
                                         MPEG_TS_PSI_PMT_INFO_DESCRIPTORS_OFFSET -
                                         value_ref->program_info_length - sizeof(value_ref->CRC);

    value_ref->program_elements_length = elements_stream_data_size;

    value_ref->CRC = 0;

    uint16_t full_section_length = section_length + MPEG_TS_PSI_PMT_SECTION_LENGTH_OFFSET;

    // in MSBF order
    uint8_t CRC_byte_0 = section_data[full_section_length - 4];
    uint8_t CRC_byte_1 = section_data[full_section_length - 3];
    uint8_t CRC_byte_2 = section_data[full_section_length - 2];
    uint8_t CRC_byte_3 = section_data[full_section_length - 1];

    value_ref->CRC = 0;
    value_ref->CRC |= CRC_byte_0 << (8 * 3);
    value_ref->CRC |= CRC_byte_1 << (8 * 2);
    value_ref->CRC |= CRC_byte_2 << (8 * 1);
    value_ref->CRC |= CRC_byte_3 << (8 * 0);

    return ret_val;
}
