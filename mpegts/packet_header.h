#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "packet.h"

typedef enum
{
    ADAPTATION_FIELD_RESERVED,
    ADAPTATION_FIELD_PAYLOAD_ONLY,
    ADAPTATION_FIELD_ONLY,
    ADAPTATION_FIELD_AND_PAYLOAD
} MpegTsPacketHeaderAdaptationFieldControl_e;

typedef struct
{
    bool error_indicator;
    bool payload_unit_start_indicator;
    bool transport_priority;
    uint16_t pid : MPEG_TS_PID_FIELD_SIZE_BITS;
    uint8_t scrambling_control : MPEG_TS_SCRAMBLING_CONTROL_SIZE_BITS;
    MpegTsPacketHeaderAdaptationFieldControl_e adaptation_field_control;
    uint8_t continuity_counter : MPEG_TS_CONTINUITY_COUNTER_SIZE_BITS;

} MpegTsPacketHeader_t;

bool mpeg_ts_parse_packet_header(MpegTsPacketHeader_t *output_header, MpegTsPacket_t packet);

