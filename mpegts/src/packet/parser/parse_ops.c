#include <assert.h>
#include <limits.h>
#include <memory.h>

#include <mpegts/parser.h>

#define MAX_ATTEMPTS_TO_REPARSE_BAD_PACKET_WITH_DROP 4

OptionalMpegTsPacketHeader_t mpeg_ts_parser_parse_packet_header(MpegTsParser_t *parser)
{
    const OptionalMpegTsPacketHeader_t bad_value = {.has_balue = false, .value = {0}};

    if (!mpeg_ts_parser_is_synced(parser)) {
        return bad_value;
    }

    uint8_t header_data_copy[MPEG_TS_PACKET_HEADER_SIZE];

    memcpy(header_data_copy, parser->parse_buffer, MPEG_TS_PACKET_HEADER_SIZE);

    if (header_data_copy[0] != MPEG_TS_SYNC_BYTE) {
        return bad_value;
    }

    // 0b000_11111
    //   ^^^ ^^^^^
    //    |    |
    //    |    |- pid's first 5 bits
    //    |
    //    |--- flags
    uint8_t flags_and_pid5 = header_data_copy[1];

    // 0b000_11111
    //   ^^^ select this
    uint8_t flags_only = flags_and_pid5 & MPEG_TS_HEADER_FLAGS_MASK;

    OptionalMpegTsPacketHeader_t ret_val;

    ret_val.value.error_indicator = (flags_only & MPEG_TS_HEADER_FLAGS_ERR_BIT) != 0;

    if (ret_val.value.error_indicator) {
        return bad_value;
    }

    ret_val.value.payload_unit_start_indicator =
        (flags_only & MPEG_TS_HEADER_FLAGS_PAYLOAD_UNIT_START_INDICATOR_BIT) != 0;

    ret_val.value.transport_priority =
        (flags_only & MPEG_TS_HEADER_FLAGS_TRANSPORT_PRIORITY_BIT) != 0;

    // 0b000_11111
    //       ^^^^^ select this
    uint8_t pid5_only = flags_and_pid5 & ~MPEG_TS_HEADER_FLAGS_MASK;

    ret_val.value.pid = 0;

    // ret_val.value.pid:                0b0000000000000
    // pid5_only:                          |  0b00011111
    // pid5_only:                     0b00011111<<<<<<<<

    // ret_val.value.pid:                0b1111100000000
    ret_val.value.pid |= (pid5_only << (MPEG_TS_PID_FIELD_SIZE_BITS - 5));

    uint8_t pid_remainder = header_data_copy[2];

    ret_val.value.pid |= pid_remainder;

    ret_val.value.scrambling_control = 0;
    ret_val.value.adaptation_field_control = 0;

    ret_val.value.scrambling_control |=
        (header_data_copy[3] & MPEG_TS_HEADER_FLAGS_SCRAMBLING_CONTROL_MASK) >>
        (CHAR_BIT - MPEG_TS_SCRAMBLING_CONTROL_SIZE_BITS);

    ret_val.value.adaptation_field_control |=
        (header_data_copy[3] & MPEG_TS_HEADER_FLAGS_ADAPT_FIELD_CONTROL_MASK) >>
        (CHAR_BIT - MPEG_TS_SCRAMBLING_CONTROL_SIZE_BITS - MPEG_TS_ADAPT_FIELD_CONTROL_SIZE_BITS);

    ret_val.value.continuity_counter = header_data_copy[3] & 0xf;

    ret_val.has_balue = true;
    return ret_val;
}

OptionalMpegTsPacket_t mpeg_ts_parser_parse_packet(MpegTsParser_t *parser)
{

    const OptionalMpegTsPacket_t bad_value = {.has_value = false, .value = {0}};

    if (!mpeg_ts_parser_is_synced(parser)) {
        return bad_value;
    }

    OptionalMpegTsPacketHeader_t header_mb = mpeg_ts_parser_parse_packet_header(parser);

    if (!header_mb.has_balue) {
        return bad_value;
    }

    OptionalMpegTsPacket_t ret_val;

    ret_val.value.header = header_mb.value;

    memcpy(ret_val.value.data,
        parser->parse_buffer + MPEG_TS_PACKET_HEADER_SIZE,
        MPEG_TS_PACKET_PAYLOAD_SIZE);

    ret_val.has_value = true;
    return ret_val;
}

OptionalMpegTsPacket_t mpeg_ts_parser_parse_packet_with_drop(MpegTsParser_t *parser)
{
    OptionalMpegTsPacket_t ret_val;
    ret_val.has_value = false;

    for (size_t parse_attempt = 0; parse_attempt < MAX_ATTEMPTS_TO_REPARSE_BAD_PACKET_WITH_DROP;
         parse_attempt++) {

        ret_val = mpeg_ts_parser_parse_packet(parser);
        mpeg_ts_parser_drop_packet(parser);
        if (ret_val.has_value) {
            break;
        }
    }

    return ret_val;
}

static void mpeg_ts_parser_try_reset_parsed_offsets(MpegTsParser_t *parser)
{
    if (parser->put_parsed_packet_index != parser->get_parsed_packet_index) {
        return;
    }

    parser->put_parsed_packet_index = 0;
    parser->get_parsed_packet_index = 0;
}

static size_t mpeg_ts_parser_get_free_space_in_parsed_packets(MpegTsParser_t *parser)
{
    if (parser->put_parsed_packet_index >= parser->parsed_packets_capacity) {
        assert(parser->put_parsed_packet_index == parser->parsed_packets_capacity &&
               "next_put_packet_index is out of bound");

        mpeg_ts_parser_try_reset_parsed_offsets(parser);

        return parser->parsed_packets_capacity;
    }

    return parser->parsed_packets_capacity - parser->put_parsed_packet_index;
}

/*
 * Will COPY each packet from *packets and return how much was copied
 */
static size_t mpeg_ts_parser_add_new_parsed_packet(MpegTsParser_t *parser,
    const MpegTsPacket_t *packets, size_t packets_size)
{

    size_t free_space = mpeg_ts_parser_get_free_space_in_parsed_packets(parser);

    if (free_space == 0) {
        return 0;
    }

    size_t packets_to_put = 0;

    if (free_space > packets_size) {
        packets_to_put = packets_size;
    } else {
        packets_to_put = free_space;
    }

    for (size_t source_packet_index = 0;
         source_packet_index < packets_to_put &&
         parser->put_parsed_packet_index < parser->parsed_packets_capacity;
         source_packet_index++) {

        *parser->parsed_packets[parser->put_parsed_packet_index] = packets[source_packet_index];
        parser->put_parsed_packet_index++;
    }

    return packets_to_put;
}

size_t mpeg_ts_parser_parse_all_packets_in_buffer(MpegTsParser_t *parser)
{
    if (parser->parse_buffer_size == 0) {
        return 0;
    }

    size_t allowed_to_parse = mpeg_ts_parser_get_free_space_in_parsed_packets(parser);
    size_t parsed_packets_amount = 0;

    while (true) {
        if (parsed_packets_amount == allowed_to_parse) {
            break;
        }

        OptionalMpegTsPacket_t packet_mb = mpeg_ts_parser_parse_packet_with_drop(parser);

        if (!packet_mb.has_value) {
            break;
        }

        size_t copied_packets = mpeg_ts_parser_add_new_parsed_packet(parser, &packet_mb.value, 1);

        if (copied_packets != 1) {
            break;
        }

        parsed_packets_amount++;
    }

    return parsed_packets_amount;
}

MpegTsPacket_t *mpeg_ts_parser_next_parsed_packet(MpegTsParser_t *parser)
{
    if (parser->get_parsed_packet_index == parser->put_parsed_packet_index) {
        mpeg_ts_parser_try_reset_parsed_offsets(parser);
        return NULL;
    }

    size_t current_get_index = parser->get_parsed_packet_index;

    parser->get_parsed_packet_index++;
    mpeg_ts_parser_try_reset_parsed_offsets(parser);

    return parser->parsed_packets[current_get_index];
}
