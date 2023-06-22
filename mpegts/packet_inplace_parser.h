#pragma once

#include <stddef.h>

#include "packet.h"


typedef struct
{
    size_t value;
    bool has_value;
} OptionalByteLocation_t;

OptionalMpegTsPacket_t mpeg_ts_parse_packet_inplace(uint8_t *buffer, size_t buffer_size);

size_t mpeg_ts_parse_packets_inplace(uint8_t *buffer, size_t buffer_size,
    MpegTsPacket_t *packet_ref_array, size_t packet_ref_array_size);

