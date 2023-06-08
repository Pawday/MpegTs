#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#include "Magics.h"


typedef void (*deleter_t)(void *data_ptr);


typedef struct MpegTsPacketHeader_t
{
    bool error_indicator;
    bool payload_unit_start_indicator;
    bool transport_priority;
    uint16_t pid : MPEG_TS_PID_FIELD_SIZE;
    uint16_t scrambling_control;
    uint16_t adaptation_field_control;
    uint32_t continuity_counter;

} MpegTsPacketHeader_t;

typedef struct MpegTsPacket_t
{
    MpegTsPacketHeader_t header;
    uint8_t *data;
    size_t packet_size;
    deleter_t packet_data_deleter;
} MpegTsPacket_t;
