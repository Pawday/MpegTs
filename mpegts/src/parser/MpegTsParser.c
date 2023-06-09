#include <assert.h>
#include <limits.h>
#include <netinet/in.h>
#include <string.h>

#include "mpegts/Magics.h"
#include "mpegts/Packet.h"
#include "mpegts/Parser.h"

#define MPEG_TS_PARSE_BUFFER_MIN_MULTIPLIER 2

// TODO: Deal with parse_put_offset
bool mpeg_ts_init_parser_ex(MpegTsParser_t *parser, uint8_t *parse_buffer, size_t parse_buffer_size,
    MpegTsPacket_t **parsed_packets_pointer_array_location,
    size_t parsed_packets_pointer_array_size)
{
    if (parser == NULL || parse_buffer == NULL || parsed_packets_pointer_array_location == NULL ||
        parse_buffer_size == 0 || parsed_packets_pointer_array_size == 0) {
        return false;
    }

    if (parse_buffer_size < MPEG_TS_PACKET_SIZE * MPEG_TS_PARSE_BUFFER_MIN_MULTIPLIER) {
        return false;
    }

    parser->parse_buffer = parse_buffer;
    parser->parse_buffer_size = parse_buffer_size;

    parser->parsed_packets = parsed_packets_pointer_array_location;
    parser->parsed_packets_size = parsed_packets_pointer_array_size;

    parser->next_get_packet_index = 0;
    parser->next_put_packet_index = 0;

    parser->parse_data_put_offset = 0;

    memset(parser->parse_buffer, 0, parser->parse_buffer_size);

    return true;
}

/*
 * @return 0 if not found or sync byte at 0
 */
static size_t find_first_sync_byte_location(MpegTsParser_t *parser)
{
    size_t current_sync_byte_location = 0;

    while (parser->parse_buffer[current_sync_byte_location] != MPEG_TS_SYNC_BYTE &&
           current_sync_byte_location < parser->parse_buffer_size) {
        current_sync_byte_location++;
    }

    return current_sync_byte_location;
}

bool mpeg_ts_parser_sync(MpegTsParser_t *parser)
{
    if (mpeg_ts_parser_is_synced(parser)) {
        return false;
    }

    size_t sync_byte_pos = find_first_sync_byte_location(parser);

    if (sync_byte_pos == 0) {
        return false;
    }

    if (parser->parse_buffer_size <= sync_byte_pos) {
        assert(true && "sync_byte_pos outside parser->parse_buffer_size");
    }

    size_t bytes_to_move = parser->parse_buffer_size - sync_byte_pos;

    memmove(parser->parse_buffer, parser->parse_buffer + sync_byte_pos, bytes_to_move);
    memset(parser->parse_buffer + bytes_to_move, 0, sync_byte_pos);

    parser->parse_data_put_offset = bytes_to_move;

    return true;
}

MpegTsPacketHeaderMaybe_t mpeg_ts_parse_packet_header(MpegTsParser_t *parser)
{
    MpegTsPacketHeaderMaybe_t ret_val;

    ret_val.has_balue = false;

    if (!mpeg_ts_parser_is_synced(parser)) {
        return ret_val;
    }

    uint8_t header_data_copy[MPEG_TS_PACKET_HEADER_SIZE];

    memcpy(header_data_copy, parser->parse_buffer, MPEG_TS_PACKET_HEADER_SIZE);

    if (header_data_copy[0] != MPEG_TS_SYNC_BYTE) {
        assert(true && "[MPEGTS PARSER]: First byte is not a sync byte");
        return ret_val;
    }

    // 0b000 11111
    //   ^^^ ^^^^^
    //    |    |
    //    |    |- pid's first 5 bits
    //    |
    //    |--- flags
    uint8_t flags_and_pid5 = header_data_copy[1];

    // 0b000_11111
    //   ^^^ select this
    uint8_t flags_only = flags_and_pid5 & MPEG_TS_HEADER_FLAGS_MASK;

    ret_val.value.error_indicator = (flags_only & MPEG_TS_HEADER_FLAGS_ERR_BIT) != 0;

    ret_val.value.payload_unit_start_indicator =
        (flags_only & MPEG_TS_HEADER_FLAGS_PAYLOAD_UNIT_START_INDICATOR_BIT) != 0;

    ret_val.value.transport_priority =
        (flags_only & MPEG_TS_HEADER_FLAGS_TRANSPORT_PRIORITY_BIT) != 0;

    // 0b000_11111
    //       ^^^^^ select this
    uint8_t pid5_only = flags_and_pid5 & ~MPEG_TS_HEADER_FLAGS_MASK;

    // ret_val.value.pid:                0b0000000000000
    // pid5_only:                             0b00011111
    // pid5_only:                     0b00011111<<<<<<<<
    //
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

    ret_val.has_balue = true;
    return ret_val;
}

