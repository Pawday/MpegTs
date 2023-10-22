#include "packet.h"
#include "packet_header.h"

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

uint8_t *mpeg_ts_packet_get_payload(MpegTsPacket_t packet)
{
    uint8_t *packet_location = (uint8_t *)packet;
    return packet_location + MPEG_TS_PACKET_HEADER_SIZE;
}

bool mpeg_ts_search_packet(MpegTsPacket_t *output_packet, const uint8_t *buffer, size_t buffer_size)
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

    const uint8_t *packet_location = buffer + sync_byte_location;
    if (packet_location[MPEG_TS_PACKET_SIZE] != MPEG_TS_SYNC_BYTE) {
        return false;
    }

    MpegTsPacketHeader_t header;
    if (!mpeg_ts_parse_packet_header(&header, (MpegTsPacket_t)packet_location)) {
        return false;
    }

    *output_packet = (MpegTsPacket_t)packet_location;
    return true;
}

size_t mpeg_ts_search_packets(const uint8_t *buffer, size_t buffer_size,
    MpegTsPacket_t *packets_array, size_t packets_array_size)
{
    size_t packets_parsed_so_far = 0;
    size_t next_packet_location_offset = 0;
    size_t sync_byte_for_next_packet = 0;

    while (packets_parsed_so_far < packets_array_size && next_packet_location_offset < buffer_size) {

        if (!find_first_sync_byte(&sync_byte_for_next_packet,
                buffer + next_packet_location_offset,
                buffer_size - next_packet_location_offset)) {
            break;
        }

        if (sync_byte_for_next_packet != 0 && next_packet_location_offset + sync_byte_for_next_packet >= buffer_size) {
            break;
        }
        next_packet_location_offset += sync_byte_for_next_packet;

        MpegTsPacket_t next_packet = NULL;
        bool is_next_packet_parsed = mpeg_ts_search_packet(&next_packet,
            buffer + next_packet_location_offset,
            buffer_size - next_packet_location_offset);

        if (!is_next_packet_parsed) {
            next_packet_location_offset++;
            continue;
        }

        next_packet_location_offset += MPEG_TS_PACKET_SIZE;


        packets_array[packets_parsed_so_far] = next_packet;
        packets_parsed_so_far++;
    }

    return packets_parsed_so_far;
}
