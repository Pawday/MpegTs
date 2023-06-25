#pragma once

#include <stdbool.h>

#include "packet_header.h"
#include "packet_magics.h"

typedef struct
{
    MpegTsPacketHeader_t header;
    const uint8_t *payload;
} MpegTsPacket_t;

typedef struct
{
    MpegTsPacket_t value;
    bool has_value;
} OptionalMpegTsPacket_t;

