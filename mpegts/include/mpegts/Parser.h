#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <mpegts/Packet.h>

#define MPEG_TS_CONTEXT_PARSE_BUFFER_SIZE        4096
#define MPEG_TS_PARSED_PACKETS_IN_CONTEXT_AMOUNT (uint8_t)16

#define MPEG_TS_PID_FIELD_SIZE 13
#define MPEG_TS_SYNC_BYTE 0x47

typedef void (*deleter_t)(void *data_ptr);

typedef struct MpegTsParser_t
{
    uint8_t *parse_buffer_ptr;
    size_t parse_buffer_size;
    size_t parse_data_put_offset;

    MpegTsPacket_t **parsed_packets;
    size_t parsed_packets_amount;
    size_t next_get_packet_index;
    size_t next_put_packet_index;

} MpegTsParser_t;

bool mpeg_ts_init_parser_ex(MpegTsParser_t *parser, uint8_t *parse_buffer, size_t parse_buffer_size,
    MpegTsPacket_t **parsed_packets_pointer_array_location,
    size_t parsed_packets_pointer_array_size);



size_t mpeg_ts_parser_send_data(MpegTsParser_t *parser, char* buffer, size_t buffer_size);


/* 
 * @return parser->parse_buffer[0] == MPEG_TS_SYNC_BYTE
 */
bool mpeg_ts_parser_is_synced(MpegTsParser_t *parser);

/*
 * Will drop bytes from parse_buffer until MPEG_TS_SYNC_BYTE 
 *
 * Example: (assuming MPEG_TS_SYNC_BYTE is 0x47)
 *
 * Before sync:
 * +----------------------------------------------------------------+
 * |                      parser->parse_buffer                      |
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+
 * | 00 | 01 | 02 | 03 | 04 | 05 | 47 | 05 | 04 | 03 | 02 | 01 | 00 |
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+
 *                                 ^^ - sync_byte                ^^ - parser->parse_data_put_offset
 *
 *
 * After sync:
 * +----------------------------------------------------------------+
 * |                      parser->parse_buffer                      |
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+
 * | 47 | 05 | 04 | 03 | 02 | 01 | 00 | 00 | 00 | 00 | 00 | 00 | 00 |
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+
 *   ^^ - sync_byte                ^^ - parser->parse_data_put_offset
 *
 *
 * @return false if:
 *     1. mpeg_ts_parser_check_sync(parser) == true
 *     2. no sync byte was found
 *
 */
bool mpeg_ts_parser_sync(MpegTsParser_t *parser);
