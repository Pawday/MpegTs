#include <assert.h>
#include <string.h>

#include "mpegts/Magics.h"
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
