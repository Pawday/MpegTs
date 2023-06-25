#include "packet_inplace_parser.h"


static OptionalByteLocation_t find_first_sync_byte(const uint8_t *buffer, size_t buffer_size)
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

    OptionalByteLocation_t ret_val = {.has_value = sync_byte_found,
        .value = current_sync_byte_location};
    return ret_val;
}

OptionalMpegTsPacket_t mpeg_ts_parse_packet_inplace(const uint8_t *buffer, size_t buffer_size)
{
    const OptionalMpegTsPacket_t bad_value = {.has_value = false, .value = {0}};

    if (buffer_size < MPEG_TS_PACKET_SIZE) {
        return bad_value;
    }

    OptionalByteLocation_t optional_sync_byte_location = find_first_sync_byte(buffer, buffer_size);
    if (!optional_sync_byte_location.has_value) {
        return bad_value;
    }
    size_t sync_byte_location = optional_sync_byte_location.value;
    if ((sync_byte_location + MPEG_TS_PACKET_SIZE) > buffer_size ||
        buffer[sync_byte_location + MPEG_TS_PACKET_SIZE] != MPEG_TS_SYNC_BYTE) {
        return bad_value;
    }

    const uint8_t *packet_location = buffer + sync_byte_location;

    OptionalMpegTsPacketHeader_t optional_packet_header =
        mpeg_ts_parse_packet_header(packet_location, MPEG_TS_PACKET_SIZE);
    if (!optional_packet_header.has_balue) {
        return bad_value;
    }

    OptionalMpegTsPacket_t ret_val;
    ret_val.has_value = true;
    ret_val.value.header = optional_packet_header.value;
    ret_val.value.payload = packet_location + MPEG_TS_PACKET_HEADER_SIZE;
    return ret_val;
}

size_t mpeg_ts_parse_packets_inplace(const uint8_t *buffer, size_t buffer_size,
    MpegTsPacket_t *packets_array, size_t packets_array_size)
{
    size_t packets_parsed_so_far = 0;
    size_t next_packet_location_offset = 0;

    while (packets_parsed_so_far < packets_array_size &&
           next_packet_location_offset < buffer_size) {

        OptionalByteLocation_t optional_next_packet_location_offset;
        optional_next_packet_location_offset =
            find_first_sync_byte(buffer + next_packet_location_offset,
                buffer_size - next_packet_location_offset);
        if (!optional_next_packet_location_offset.has_value) {
            break;
        }

        OptionalMpegTsPacket_t next_packet_ref =
            mpeg_ts_parse_packet_inplace(buffer + next_packet_location_offset,
                buffer_size - next_packet_location_offset);
        if (!next_packet_ref.has_value) {
            break;
        }

        next_packet_location_offset += MPEG_TS_PACKET_SIZE;

        packets_array[packets_parsed_so_far] = next_packet_ref.value;

        packets_parsed_so_far++;
    }

    return packets_parsed_so_far;
}

