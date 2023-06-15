#include <stdlib.h>

#include <mpeg/ts/parser.h>

#define PARSER_BUFFER_SIZE        4096
#define PARSER_PACKETS_ARRAY_SIZE 10

bool mpeg_ts_parser_init(MpegTsParser_t *parser)
{
    uint8_t *parser_buffer = malloc(PARSER_BUFFER_SIZE);

    if (parser_buffer == NULL) {
        return false;
    }

    MpegTsPacket_t *parsed_packets_array =
        calloc(PARSER_PACKETS_ARRAY_SIZE, sizeof(MpegTsPacket_t));

    if (parsed_packets_array == NULL) {
        free(parser_buffer);
        return false;
    }

    MpegTsPacket_t **packets_ptr_array =
        calloc(PARSER_PACKETS_ARRAY_SIZE, sizeof(MpegTsPacket_t *));

    if (packets_ptr_array == NULL) {
        free(parser_buffer);
        free(parsed_packets_array);

        return false;
    }

    for (size_t packet_index = 0; packet_index < PARSER_PACKETS_ARRAY_SIZE; packet_index++) {
        packets_ptr_array[packet_index] = &parsed_packets_array[packet_index];
    }

    bool init_status = mpeg_ts_parser_init_ex(parser,
        parser_buffer,
        PARSER_BUFFER_SIZE,
        packets_ptr_array,
        PARSER_PACKETS_ARRAY_SIZE);

    if (!init_status) {
        free(parser_buffer);
        free(parsed_packets_array);
        free(packets_ptr_array);
        return false;
    }

    return true;
}

void mpeg_ts_parser_free(MpegTsParser_t *parser)
{

    if (parser == NULL) {
        return;
    }

    uint8_t *parser_buffer = parser->parse_buffer;
    MpegTsPacket_t *parsed_packets_array = parser->parsed_packets[0];
    MpegTsPacket_t **packets_ptr_array = parser->parsed_packets;

    free(parser_buffer);
    free(parsed_packets_array);
    free(packets_ptr_array);
}