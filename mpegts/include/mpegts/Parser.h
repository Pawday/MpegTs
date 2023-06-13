#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "mpegts/Packet.h"

typedef struct MpegTsParser_t
{
    uint8_t *parse_buffer;
    size_t parse_buffer_size;
    size_t parse_data_put_offset;

    MpegTsPacket_t **parsed_packets;
    size_t parsed_packets_capacity;
    size_t put_parsed_packet_index;
    size_t get_parsed_packet_index;

} MpegTsParser_t;

/*
 * verbose init with user defined memory buffers
 *
 * responsibility for memory deallocation remains with the user
 */
bool mpeg_ts_parser_init_ex(MpegTsParser_t *parser, uint8_t *parse_buffer, size_t parse_buffer_size,
    MpegTsPacket_t **parsed_packets_pointer_array_location,
    size_t parsed_packets_pointer_array_size);

size_t mpeg_ts_parser_send_data(MpegTsParser_t *parser, char *source_buffer, size_t buffer_size);

static inline bool mpeg_ts_parser_is_synced(MpegTsParser_t *parser)
{
    return parser->parse_buffer[0] == MPEG_TS_SYNC_BYTE;
}

/*
 * Will drop bytes from parse_buffer until MPEG_TS_SYNC_BYTE
 *
 * Example: (assuming MPEG_TS_SYNC_BYTE is 0x47)
 *
 * Before sync:
 * +---------------------------------------------------------------------+
 * |                          parser->parse_buffer                       |
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+-----
 * | 00 | 01 | 02 | 03 | 04 | 05 | 47 | 05 | 04 | 03 | 02 | 01 | 00 | ff |
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 *                                 ^^ - sync_byte                ^^ - parser->parse_data_put_offset
 *
 *
 * After sync:
 * +---------------------------------------------------------------------+
 * |                          parser->parse_buffer                       |
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+-----
 * | 47 | 05 | 04 | 03 | 02 | 01 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | ff |
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 *   ^^ - sync_byte                ^^ - parser->parse_data_put_offset
 *
 *
 * @return false if:
 *     1. mpeg_ts_parser_is_synced(parser) == true
 *     2. no sync byte was found
 *     3. first sync byte comes after parser->parse_data_put_offset
 *
 */
bool mpeg_ts_parser_sync(MpegTsParser_t *parser);

/*
 * Drop single packet from parse buffer begining
 * return false when:
 *     1. No packet at the begining (parser not synced)
 *     2. Packet not formed (not enougth data)
 */
bool mpeg_ts_parser_drop_packet(MpegTsParser_t *parser);

/*
 * Extract header from synced parse buffer without dropping
 */
MpegTsPacketHeaderMaybe_t mpeg_ts_parser_parse_packet_header(MpegTsParser_t *parser);

/*
 * Parse packet from parse buffer without dropping data from it
 */
MpegTsPacketMaybe_t mpeg_ts_parser_parse_packet(MpegTsParser_t *parser);

/*
 * Will parse packet and perform drop
 */
MpegTsPacketMaybe_t mpeg_ts_parser_parse_packet_with_drop(MpegTsParser_t *parser);

/*
 * Will perform mpeg_ts_parser_parse_packet_with_drop() to internal packet storage until
 *     1. Source buffer end up
 *     2. Packet storage end up
 *
 *     @return amount of sucsessfully parsed packets
 */
size_t mpeg_ts_parser_parse_many(MpegTsParser_t *parser);

/*
 * @return pointer to parsed packet or NULL if there is no new packets
 * you MUST NOT:
 *    1. free the packet
 *        packet_ptr = next_parsed_packet(parser)'
 *        ...
 *        free(packet_ptr); // DONT DO IT
 *
 *    2. use packet after ANY parser functions
 *        packet_ptr = next_parsed_packet(parser);
 *        *packet_ptr //OK
 *        ...
 *        mpeg_ts_parser_SOME_ORERATION(parser);
 *        *packet_ptr;  //DONT DO IT AFTER ABOVE CALL
 */
MpegTsPacket_t *mpeg_ts_parser_next_parsed_packet(MpegTsParser_t *parser);

/*
 * Interface for memory responsible parser initialyser
 */
bool mpeg_ts_parser_init(MpegTsParser_t *parser);
void mpeg_ts_parser_free(MpegTsParser_t *parser);
