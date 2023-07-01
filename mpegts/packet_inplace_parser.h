#pragma once

#include <stddef.h>

#include "packet.h"


bool mpeg_ts_parse_packet_inplace(MpegTsPacket_t *output_packet, const uint8_t *buffer,
    const size_t buffer_size);

size_t mpeg_ts_parse_packets_inplace(const uint8_t *buffer, size_t buffer_size,
    MpegTsPacket_t *packet_ref_array, size_t packet_ref_array_size);

