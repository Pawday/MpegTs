#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "packet_magics.h"

typedef struct
{
    const uint8_t dummy;
} *MpegTsPacket_t;

bool mpeg_ts_search_packet(MpegTsPacket_t *output_packet, const uint8_t *buffer,
    const size_t buffer_size);

size_t mpeg_ts_search_packets(const uint8_t *buffer, size_t buffer_size,
    MpegTsPacket_t *packets_output_array, size_t packet_ref_array_size);

uint8_t *mpeg_ts_packet_get_payload(MpegTsPacket_t packet);
