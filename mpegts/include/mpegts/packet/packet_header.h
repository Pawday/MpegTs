#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "packet_magics.h"

typedef struct
{
    bool error_indicator;
    bool payload_unit_start_indicator;
    bool transport_priority;
    uint16_t pid : MPEG_TS_PID_FIELD_SIZE_BITS;
    uint8_t scrambling_control : MPEG_TS_SCRAMBLING_CONTROL_SIZE_BITS;
    uint8_t adaptation_field_control : MPEG_TS_ADAPT_FIELD_CONTROL_SIZE_BITS;
    uint8_t continuity_counter : MPEG_TS_CONTINUITY_COUNTER_SIZE_BITS;

} MpegTsPacketHeader_t;

typedef struct
{
    bool has_balue;
    MpegTsPacketHeader_t value;
} OptionalMpegTsPacketHeader_t;

OptionalMpegTsPacketHeader_t mpeg_ts_parse_packet_header(uint8_t *buffer, size_t buffer_size);

