#include <assert.h>
#include <memory.h>

#include "pmt_builder.h"

void mpeg_ts_pmt_builder_init(MpegTsPMTBuilder_t *builder, uint8_t *build_buffer,
    size_t buffer_size)
{
    builder->table_data_capacity = buffer_size;
    builder->table_data = build_buffer;

    mpeg_ts_pmt_builder_reset(builder);
}

void mpeg_ts_pmt_builder_reset(MpegTsPMTBuilder_t *builder)
{
    builder->state = PMT_BUILDER_STATE_EMPTY;
    builder->table_length = 0;
    builder->table_data_put_offset = 0;
    memset(builder->table_data, 0, builder->table_data_capacity);

    MpegTsPacketHeader_t empty_header = {0};
    builder->last_packet_header = empty_header;
}

static bool is_start_pmt_packet(MpegTsPacket_t *packet)
{
    if (packet->header.pid == MPEG_TS_NULL_PACKET_PID ||
        packet->header.adaptation_field_control == ADAPTATION_FIELD_ONLY) {
        return false;
    }

    if (!packet->header.payload_unit_start_indicator) {
        return false;
    }

    uint8_t current_packet_adapt_field_length = 0;
    if (packet->header.adaptation_field_control == ADAPTATION_FIELD_AND_PAYLOAD) {
        current_packet_adapt_field_length += packet->payload[0];
    }

    bool is_PES_packet_at_start = true;
    if (is_PES_packet_at_start) {

        is_PES_packet_at_start &= packet->payload[current_packet_adapt_field_length] == 0;
        //                             ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        //                                 can be PSI pointer_field
    }
    if (is_PES_packet_at_start) {
        // can be PAS table_id if pointer_field == 0
        is_PES_packet_at_start &= packet->payload[current_packet_adapt_field_length + 1] == 0;
    }
    if (is_PES_packet_at_start) {
        is_PES_packet_at_start &= packet->payload[current_packet_adapt_field_length + 2] == 1;
    }
    // actualy it can be PAS table if pointer_field (packet->data[0]) is 0
    // Filters above cannot distinct PES and PAS
    // but its anought to distinct PMT from PES
    if (is_PES_packet_at_start) {
        return false;
    }

    uint8_t section_offset =
        packet->payload[current_packet_adapt_field_length]; // aka PSI pointer_field
    section_offset += 1;                                    // include itself to make offset

    /*
     * section_data:
     *    |byte0 |
     *  0b00000010
     *       ^
     *       |
     *       |
     *   table_id
     *    (0x02)
     */
    const uint8_t *section_data =
        packet->payload + section_offset + current_packet_adapt_field_length;

    if (section_data[0] != MPEG_TS_PSI_PMT_SECTION_ID) {
        return false;
    }

    return true;
}

static MpegTsPMTBuilderSendPacketStatus_e send_first_packet(MpegTsPMTBuilder_t *builder,
    MpegTsPacket_t *packet)
{
    assert(builder->state == PMT_BUILDER_STATE_EMPTY);
    assert(builder->table_data_put_offset == 0);

    uint8_t current_packet_payload_offset = 0;
    if (packet->header.adaptation_field_control == ADAPTATION_FIELD_AND_PAYLOAD) {
        current_packet_payload_offset += packet->payload[0];
    }

    uint8_t section_offset =
        packet->payload[current_packet_payload_offset]; // aka PSI pointer_field
    section_offset += 1;                                // include itself to make offset

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
    const uint8_t *section_data = packet->payload + section_offset + current_packet_payload_offset;

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
            MPEG_TS_PSI_PMT_SECTION_SYNTAX_INDICATOR_BIT ||
        (flag_len & MPEG_TS_PSI_PMT_SHOULD_BE_ZERO_BIT) != 0) {
        return PMT_BUILDER_SEND_STATUS_INVALID_PACKET_REJECTED;
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
        return PMT_BUILDER_SEND_STATUS_INVALID_PACKET_REJECTED;
    }

    if (section_length >= builder->table_data_capacity) {
        return PMT_BUILDER_SEND_STATUS_NOT_ENOUGHT_MEMORY;
    }

    bool is_pmt_single_packed =
        section_length < MPEG_TS_PACKET_SIZE - (MPEG_TS_PACKET_HEADER_SIZE + section_offset +
                                                   current_packet_payload_offset +
                                                   MPEG_TS_PSI_PMT_SECTION_LENGTH_OFFSET);

    uint16_t mpeg_ts_payload_or_section_size = MPEG_TS_PACKET_PAYLOAD_SIZE;

    if (section_length < mpeg_ts_payload_or_section_size) {
        mpeg_ts_payload_or_section_size =
            section_length + MPEG_TS_PSI_PMT_SECTION_LENGTH_LSB_OFFSET;
    }

    memcpy(builder->table_data + builder->table_data_put_offset,
        section_data,
        mpeg_ts_payload_or_section_size - section_offset - current_packet_payload_offset);

    builder->table_data_put_offset +=
        mpeg_ts_payload_or_section_size - section_offset - current_packet_payload_offset;

    builder->last_packet_header = packet->header;
    builder->table_length = section_length;

    if (is_pmt_single_packed) {
        builder->state = PMT_BUILDER_STATE_TABLE_ASSEMBLED;
        return PMT_BUILDER_SEND_STATUS_SMALL_TABLE_IS_ASSEMBLED;
    } else {
        builder->state = PMT_BUILDER_STATE_TABLE_IS_BUILDING;
        return PMT_BUILDER_SEND_STATUS_NEED_MORE_PACKETS;
    }
}

static MpegTsPMTBuilderSendPacketStatus_e send_continuation_packet(MpegTsPMTBuilder_t *builder,
    MpegTsPacket_t *packet)
{
    assert(builder->state != PMT_BUILDER_STATE_EMPTY);
    assert(builder->table_data_put_offset != 0);
    assert(builder->table_data_put_offset < builder->table_data_capacity);
    assert(builder->table_length <= builder->table_data_capacity);

    if (packet->header.pid != builder->last_packet_header.pid) {
        return PMT_BUILDER_SEND_STATUS_INVALID_PACKET_REJECTED;
    }

    if (packet->header.continuity_counter != builder->last_packet_header.continuity_counter + 1) {
        return PMT_BUILDER_SEND_STATUS_UNORDERED_PACKET_REJECTED;
    }

    if (builder->state == PMT_BUILDER_STATE_TABLE_ASSEMBLED) {
        return PMT_BUILDER_SEND_STATUS_REDUDANT_PACKET_REJECTED;
    }

    builder->last_packet_header = packet->header;

    uint16_t data_to_send = MPEG_TS_PACKET_PAYLOAD_SIZE;
    uint16_t remainding_data = (builder->table_length + MPEG_TS_PSI_PMT_SECTION_LENGTH_OFFSET) -
                               builder->table_data_put_offset;
    if (data_to_send > remainding_data) {
        data_to_send = remainding_data;
        builder->state = PMT_BUILDER_STATE_TABLE_ASSEMBLED;
    }

    memcpy(builder->table_data + builder->table_data_put_offset, packet->payload, data_to_send);

    builder->table_data_put_offset += data_to_send;

    if (builder->state == PMT_BUILDER_STATE_TABLE_ASSEMBLED) {
        assert(builder->table_data_put_offset ==
               builder->table_length + MPEG_TS_PSI_PMT_SECTION_LENGTH_OFFSET);
        return PMT_BUILDER_SEND_STATUS_TABLE_IS_ASSEMBLED;
    }

    return PMT_BUILDER_SEND_STATUS_NEED_MORE_PACKETS;
}

MpegTsPMTBuilderSendPacketStatus_e mpeg_ts_pmt_builder_try_send_packet(MpegTsPMTBuilder_t *builder,
    MpegTsPacket_t *packet)
{
    if (builder->state == PMT_BUILDER_STATE_TABLE_ASSEMBLED) {
        return PMT_BUILDER_SEND_STATUS_REDUDANT_PACKET_REJECTED;
    }

    if (builder->state == PMT_BUILDER_STATE_EMPTY) {
        if (!is_start_pmt_packet(packet)) {
            return PMT_BUILDER_SEND_STATUS_INVALID_PACKET_REJECTED;
        }
        return send_first_packet(builder, packet);
    }

    return send_continuation_packet(builder, packet);
}

bool mpeg_ts_pmt_builder_try_build_table(MpegTsPMTBuilder_t *builder, MpegTsPMT_t *output_table)
{
    if (builder->state != PMT_BUILDER_STATE_TABLE_ASSEMBLED) {
        return false;
    }

    /*
     * table_data:
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
     *       (should be set)
     */
    uint8_t *table_data = builder->table_data;

    /*
     *
     *    |byte0 | |  byte1  || byte2|
     *  0b00000010_1_0_00_000000000000
     *             ^ ^ ^^ ^^^^
     *             +---------+
     *                  |
     *///               |
    uint8_t flags_and_length = table_data[1];

    uint8_t syntax_indicator_byte_masked =
        flags_and_length & MPEG_TS_PSI_PMT_SECTION_SYNTAX_INDICATOR_BIT;

    if (syntax_indicator_byte_masked == 0) {
        return false;
    }

    if ((flags_and_length & MPEG_TS_PSI_PMT_SHOULD_BE_ZERO_BIT) != 0) {
        return false;
    }

    uint16_t section_length = 0;

    /*
     *     |  byte1  |
     *     1_0_00_0000
     *            ^^^^
     *         select this
     */
    uint8_t length_part_from_byte1 = flags_and_length & 0x0f;
    uint8_t length_part_from_byte2 = table_data[2];
    section_length |= (length_part_from_byte1 << 8) | length_part_from_byte2;
    if (section_length >= MPEG_TS_PSI_PMT_SECTION_MAX_LENGTH) {
        return false;
    }
    output_table->section_length = section_length;

    /*
     * table_data:
     *    |byte3 | |byte4 |
     *  0b01000010_00110010 <-- all 16 bits is program number
     */
    output_table->program_number = 0;
    output_table->program_number |= table_data[3] << 8;
    output_table->program_number |= table_data[4];

    /*
     * table_data:
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

    output_table->version_number = 0;
    /*
     * table_data:
     *    | byte5  |
     *  0b00_00101_0
     *       ^^^^^ --select this and shift to 1 byte rigth
     */
    output_table->version_number = (table_data[5] & MPEG_TS_PSI_PMT_VERSION_NUMBER_MASK) >> 1;

    /*
     * table_data:
     *    | byte5  |
     *  0b00_00000_0
     *             ^
     *             |
     *             +--- current_next_indicator is set?
     */

    output_table->current_next_indicator =
        (table_data[5] & MPEG_TS_PSI_PMT_CURRENT_NEXT_INDICATOR_BIT) ==
        MPEG_TS_PSI_PMT_CURRENT_NEXT_INDICATOR_BIT;
    if (!output_table->current_next_indicator) {
        return false;
    }

    /*
     * table_data:
     *    |byte6 |
     *  0b00000000 - section_number ("shall be set to zero")
     */

    if (table_data[6] != 0) {
        return false;
    }

    /*
     * table_data:
     *    |byte7 |
     *  0b00000000 - last_section_number ("shall be set to zero")
     */
    if (table_data[7] != 0) {
        return false;
    }

    /*
     * table_data:
     *    |byte8  ||byte9 |
     *  0b000_0000000000000
     *     ^        ^
     *     |        |
     *     |        |
     *     |      PCR_PID
     *     |
     *     +--------- reserved
     */

    output_table->PCR_PID = 0;

    /*
     * table_data:
     *    |byte8  |
     *  0b000_0000000000000
     *        ^^^^^ -select this than shift it left by 8 bits than merge to PCR_PID MSB
     */

    output_table->PCR_PID |= (table_data[8] & MPEG_TS_PSI_PMT_PCR_PID_MSB_MASK) << 8;

    /*
     * table_data:
     *    |byte8  ||byte9 |
     *  0b000_0000000000000
     *             ^^^^^^^^ - merge it to PCR_PID LSB
     */
    output_table->PCR_PID |= table_data[9];

    /*
     * table_data:
     *    |byte10 ||byte11|
     *  0b0000_000000000000
     *     ^        ^
     *     |        |
     *     |        +---- program_info_length
     *     |
     *     +---- reserved
     */
    output_table->program_info_length = 0;

    output_table->program_info_length |=
        (table_data[10] & MPEG_TS_PSI_PMT_PROGRAM_INFO_LENGTH_MSB_MASK) << 8;

    output_table->program_info_length |= table_data[11];

    output_table->program_info_data = NULL;
    if (output_table->program_info_length != 0) {
        output_table->program_info_data = table_data + MPEG_TS_PSI_PMT_INFO_DESCRIPTORS_OFFSET;
    }

    uint8_t *elements_stream_data =
        table_data + MPEG_TS_PSI_PMT_INFO_DESCRIPTORS_OFFSET + output_table->program_info_length;

    output_table->program_elements = elements_stream_data;

    assert(output_table->section_length >
           (MPEG_TS_PSI_PMT_INFO_DESCRIPTORS_OFFSET + output_table->program_info_length +
               sizeof(output_table->CRC)));

    uint16_t elements_stream_data_size =
        output_table->section_length + MPEG_TS_PSI_PMT_SECTION_LENGTH_OFFSET -
        MPEG_TS_PSI_PMT_INFO_DESCRIPTORS_OFFSET - output_table->program_info_length -
        sizeof(output_table->CRC);

    output_table->program_elements_length = elements_stream_data_size;

    uint16_t full_section_length = section_length + MPEG_TS_PSI_PMT_SECTION_LENGTH_OFFSET;

    // in MSBF order
    uint8_t CRC_byte_0 = table_data[full_section_length - 4];
    uint8_t CRC_byte_1 = table_data[full_section_length - 3];
    uint8_t CRC_byte_2 = table_data[full_section_length - 2];
    uint8_t CRC_byte_3 = table_data[full_section_length - 1];

    output_table->CRC = 0;
    output_table->CRC |= CRC_byte_0 << (8 * 3);
    output_table->CRC |= CRC_byte_1 << (8 * 2);
    output_table->CRC |= CRC_byte_2 << (8 * 1);
    output_table->CRC |= CRC_byte_3 << (8 * 0);

    return true;
}

