#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "magics.h"

typedef struct MpegTsPacketHeader_t
{
    bool error_indicator;
    bool payload_unit_start_indicator;
    bool transport_priority;
    uint16_t pid : MPEG_TS_PID_FIELD_SIZE_BITS;
    uint8_t scrambling_control : MPEG_TS_SCRAMBLING_CONTROL_SIZE_BITS;
    uint8_t adaptation_field_control : MPEG_TS_ADAPT_FIELD_CONTROL_SIZE_BITS;
    uint8_t continuity_counter : MPEG_TS_CONTINUITY_COUNTER_SIZE_BITS;

} MpegTsPacketHeader_t;

typedef struct MpegTsPacketHeaderMaybe_t
{
    bool has_balue;
    MpegTsPacketHeader_t value;
} MpegTsPacketHeaderMaybe_t;

typedef struct MpegTsPacket_t
{
    MpegTsPacketHeader_t header;
    uint8_t data[MPEG_TS_PACKET_PAYLOAD_SIZE];
} MpegTsPacket_t;

typedef struct MpegTsPacketMaybe_t
{
    MpegTsPacket_t value;
    bool has_value;
} MpegTsPacketMaybe_t;
