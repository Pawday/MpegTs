#include <assert.h>
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
