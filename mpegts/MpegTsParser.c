#include <string.h>

#include "mpegts/Magics.h"
#include "mpegts/Parser.h"

#define MPEG_TS_PARSE_BUFFER_MIN_MULTIPLIER 2

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

bool mpeg_ts_parser_sync(MpegTsParser_t *parser)
{
    if (mpeg_ts_parser_is_synced(parser)) {
        return false;
    }

    return false;
}
