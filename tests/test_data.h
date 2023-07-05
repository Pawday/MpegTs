#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct
{
    const uint8_t *data;
    const size_t size;
} IOVec;

IOVec test_data_get_raw_packet(void);
IOVec test_data_get_two_packets(void);
IOVec test_data_get_packet_with_pmt(void);
