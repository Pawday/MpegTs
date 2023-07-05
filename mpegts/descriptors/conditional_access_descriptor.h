#pragma once

#include <stdint.h>

#include "mpegts/descriptor.h"

#define MPEG_TS_CONDITIONAL_ACCESS_DESCRIPTOR_HEADER_SIZE 4
#define MPEG_TS_CONDITIONAL_ACCESS_DESCRIPTOR_PRIVATE_DATA_MAX_SIZE                                \
    (UINT8_MAX - MPEG_TS_CONDITIONAL_ACCESS_DESCRIPTOR_HEADER_SIZE)

#define MPEG_TS_CONDITIONAL_ACCESS_DESCRIPTOR_CA_PID_BITS 13
#define MPEG_TS_CONDITIONAL_ACCESS_DESCRIPTOR_CA_PID_MASK 0x1fff // 0b0001111111111111

typedef struct
{
    uint16_t CA_system_ID;
    uint16_t CA_PID : MPEG_TS_CONDITIONAL_ACCESS_DESCRIPTOR_CA_PID_BITS;
    uint8_t private_data_length;
    const uint8_t *private_data;
} MpegTsCADescriptor_t;

bool mpeg_ts_conditional_access_descriptor_from_raw_descriptor(MpegTsDescriptor_t *raw_descriptor,
    MpegTsCADescriptor_t *output_CA_descriptor);
