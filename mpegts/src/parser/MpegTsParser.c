#include <assert.h>
#include <limits.h>
#include <memory.h>

#include "mpegts/Parser.h"


static size_t mpeg_ts_parser_get_free_space(MpegTsParser_t *parser)
{
    if (parser->parse_data_put_offset >= parser->parse_buffer_size) {
        assert(parser->parse_data_put_offset == parser->parse_buffer_size &&
               "Someone taintet buffer state");
        return 0;
    }

    return parser->parse_buffer_size - parser->parse_data_put_offset;
}

size_t mpeg_ts_parser_send_data(MpegTsParser_t *parser, char *restrict source_buffer,
    size_t buffer_size)
{
    size_t free_space = mpeg_ts_parser_get_free_space(parser);
    assert(parser->parse_data_put_offset + free_space == parser->parse_buffer_size);

    if (free_space == 0) {
        return 0;
    }

    size_t bytes_to_send = buffer_size;

    if (bytes_to_send > free_space) {
        bytes_to_send = free_space;
    }

    memcpy(parser->parse_buffer + parser->parse_data_put_offset, source_buffer, bytes_to_send);

    parser->parse_data_put_offset += bytes_to_send;

    if (bytes_to_send == free_space) {
        assert(parser->parse_data_put_offset == parser->parse_buffer_size);
        assert(mpeg_ts_parser_get_free_space(parser) == 0);
    }

    return bytes_to_send;
}

/*
 * @return 0 if not found or sync byte at 0
 */
static size_t mpeg_ts_parser_find_first_sync_byte_location(MpegTsParser_t *parser)
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

    size_t sync_byte_pos = mpeg_ts_parser_find_first_sync_byte_location(parser);

    if (sync_byte_pos == 0) {
        return false;
    }

    if (sync_byte_pos > parser->parse_data_put_offset) {
        return false;
    }

    if (parser->parse_buffer_size <= sync_byte_pos) {
        assert(true && "sync_byte_pos outside parser->parse_buffer");
    }

    size_t bytes_to_move = parser->parse_data_put_offset - sync_byte_pos;

    memmove(parser->parse_buffer, parser->parse_buffer + sync_byte_pos, bytes_to_move);
    memset(parser->parse_buffer + bytes_to_move, 0, sync_byte_pos);

    parser->parse_data_put_offset = bytes_to_move;

    return true;
}

bool mpeg_ts_parser_drop_packet(MpegTsParser_t *parser)
{
    if (!mpeg_ts_parser_is_synced(parser)) {
        return false;
    }

    if (parser->parse_data_put_offset < MPEG_TS_PACKET_SIZE) {
        return false;
    }

    memmove(parser->parse_buffer,
        parser->parse_buffer + MPEG_TS_PACKET_SIZE,
        parser->parse_buffer_size - MPEG_TS_PACKET_SIZE);

    parser->parse_data_put_offset -= MPEG_TS_PACKET_SIZE;

    return true;
}

MpegTsPacketHeaderMaybe_t mpeg_ts_parser_parse_packet_header(MpegTsParser_t *parser)
{
    MpegTsPacketHeaderMaybe_t ret_val;
    ret_val.has_balue = false;

    if (!mpeg_ts_parser_is_synced(parser)) {
        return ret_val;
    }

#if MPEG_TS_PACKET_HEADER_SIZE != 4
#error This function rely on MPEG_TS_PACKET_HEADER_SIZE == 4
#endif

    uint8_t header_data_copy[MPEG_TS_PACKET_HEADER_SIZE];

    memcpy(header_data_copy, parser->parse_buffer, MPEG_TS_PACKET_HEADER_SIZE);

    if (header_data_copy[0] != MPEG_TS_SYNC_BYTE) {
        assert(true && "First byte is not a sync byte");
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

MpegTsPacketMaybe_t mpeg_ts_parser_parse_packet(MpegTsParser_t *parser)
{
    MpegTsPacketMaybe_t ret_val;
    ret_val.has_value = false;

    if (!mpeg_ts_parser_is_synced(parser)) {
        return ret_val;
    }

    MpegTsPacketHeaderMaybe_t header_mb = mpeg_ts_parser_parse_packet_header(parser);

    if (!header_mb.has_balue) {
        return ret_val;
    }

    ret_val.value.header = header_mb.value;

    memcpy(ret_val.value.data,
        parser->parse_buffer + MPEG_TS_PACKET_HEADER_SIZE,
        MPEG_TS_PACKET_PAYLOAD_SIZE);

    ret_val.has_value = true;
    return ret_val;
}
