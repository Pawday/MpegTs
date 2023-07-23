#include "packet_parser.h"

static bool find_first_sync_byte(size_t *output_byte_location_offset, const uint8_t *buffer,
    size_t buffer_size)
{
    size_t current_sync_byte_location = 0;
    bool sync_byte_found = false;

    while (current_sync_byte_location < buffer_size) {
        if (buffer[current_sync_byte_location] == MPEG_TS_SYNC_BYTE) {
            sync_byte_found = true;
            break;
        }
        current_sync_byte_location++;
    }

    if (!sync_byte_found) {
        return false;
    }

    *output_byte_location_offset = current_sync_byte_location;

    return true;
}

bool mpeg_ts_parse_packet(MpegTsPacket_t *output_packet, const uint8_t *buffer,
    size_t buffer_size)
{
    if (buffer_size < MPEG_TS_PACKET_SIZE) {
        return false;
    }

    size_t sync_byte_location = 0;
    if (!find_first_sync_byte(&sync_byte_location, buffer, buffer_size)) {
        return false;
    }

    if ((sync_byte_location + MPEG_TS_PACKET_SIZE) > buffer_size) {
        return false;
    }

    if (buffer[sync_byte_location + MPEG_TS_PACKET_SIZE] != MPEG_TS_SYNC_BYTE) {
        return false;
    }

    const uint8_t *packet_location = buffer + sync_byte_location;
    if (!mpeg_ts_parse_packet_header(&output_packet->header,
            packet_location,
            MPEG_TS_PACKET_SIZE)) {
        return false;
    }

    output_packet->payload = packet_location + MPEG_TS_PACKET_HEADER_SIZE;
    return true;
}

size_t mpeg_ts_parse_packets(const uint8_t *buffer, size_t buffer_size,
    MpegTsPacket_t *packets_array, size_t packets_array_size)
{
    size_t packets_parsed_so_far = 0;
    size_t next_packet_location_offset = 0;

    while (
        packets_parsed_so_far < packets_array_size && next_packet_location_offset < buffer_size) {

        size_t next_packet_offset_related = 0;
        if (!find_first_sync_byte(&next_packet_offset_related,
                buffer + next_packet_location_offset,
                buffer_size - next_packet_location_offset)) {
            break;
        }

        MpegTsPacket_t next_packet = {0};
        bool is_next_packet_parsed = mpeg_ts_parse_packet(&next_packet,
            buffer + next_packet_location_offset,
            buffer_size - next_packet_location_offset);

        next_packet_location_offset += MPEG_TS_PACKET_SIZE;

        if (!is_next_packet_parsed) {
            continue;
        }

        packets_array[packets_parsed_so_far] = next_packet;
        packets_parsed_so_far++;
    }

    return packets_parsed_so_far;
}
