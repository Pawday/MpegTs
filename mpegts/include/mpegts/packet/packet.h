#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "packet_header.h"
#include "packet_magics.h"

typedef struct
{
    MpegTsPacketHeader_t header;
    uint8_t data[MPEG_TS_PACKET_PAYLOAD_SIZE];
} MpegTsPacket_t;

typedef struct
{
    MpegTsPacket_t value;
    bool has_value;
} OptionalMpegTsPacket_t;

typedef struct
{
    MpegTsPacketHeader_t header;
    uint8_t *data;
} MpegTsPacketRef_t;

typedef struct
{
    MpegTsPacketRef_t value;
    bool has_value;
} OptionalMpegTsPacketRef_t;

MpegTsPacket_t mpeg_ts_packet_clone(MpegTsPacketRef_t *ref);

