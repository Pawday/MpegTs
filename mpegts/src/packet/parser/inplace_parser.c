#include <mpegts/packet/inplace_parser.h>

typedef struct
{
    size_t value;
    bool has_value;
} OptionalByteLocation_t;

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

OptionalMpegTsPacketRef_t mpeg_ts_parse_packet_inplace(uint8_t *buffer, size_t buffer_size)
{
    const OptionalMpegTsPacketRef_t bad_value = {.has_value = false, .value = {0}};

    if (buffer_size < MPEG_TS_PACKET_SIZE) {
        return bad_value;
    }

    OptionalByteLocation_t sync_byte_location_maybe = find_first_sync_byte(buffer, buffer_size);
    if (!sync_byte_location_maybe.has_value) {
        return bad_value;
    }
    size_t sync_byte_location = sync_byte_location_maybe.value;
    if ((sync_byte_location + MPEG_TS_PACKET_SIZE) > buffer_size ||
        buffer[sync_byte_location + MPEG_TS_PACKET_SIZE] != MPEG_TS_SYNC_BYTE) {
        return bad_value;
    }

    uint8_t *packet_location = buffer + sync_byte_location;

    OptionalMpegTsPacketHeader_t packet_header_maybe =
        mpeg_ts_parse_packet_header(packet_location, MPEG_TS_PACKET_SIZE);
    if (!packet_header_maybe.has_balue) {
        return bad_value;
    }

    OptionalMpegTsPacketRef_t ret_val;
    ret_val.has_value = true;
    ret_val.value.header = packet_header_maybe.value;
    ret_val.value.data = packet_location + MPEG_TS_PACKET_HEADER_SIZE;

    return ret_val;
}

size_t mpeg_ts_parse_packets_inplace(uint8_t *buffer, size_t buffer_size,
    MpegTsPacketRef_t *packet_ref_array, size_t packet_ref_array_size)
{
    size_t packets_parsed_so_far = 0;
    size_t next_packet_location_offset = 0;

    while (packets_parsed_so_far < packet_ref_array_size &&
           next_packet_location_offset < buffer_size) {

        OptionalByteLocation_t next_packet_location_offset_maybe;

        next_packet_location_offset_maybe =
            find_first_sync_byte(buffer + next_packet_location_offset,
                buffer_size - next_packet_location_offset);
        if (!next_packet_location_offset_maybe.has_value) {
            break;
        }
        next_packet_location_offset += MPEG_TS_PACKET_SIZE;

        OptionalMpegTsPacketRef_t next_packet_ref =
            mpeg_ts_parse_packet_inplace(buffer + next_packet_location_offset,
                buffer_size - next_packet_location_offset);

        if (!next_packet_ref.has_value) {
            break;
        }

        packet_ref_array[packets_parsed_so_far] = next_packet_ref.value;

        packets_parsed_so_far++;
    }

    return packets_parsed_so_far;
}

