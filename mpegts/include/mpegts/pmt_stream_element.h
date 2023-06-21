#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "psi_magics.h"
#include "stream_type.h"

#define MPEG_TS_PSI_PMT_STREAM_ELEMENT_DESCRIPTORS_OFFSET ((8 + 3 + 13 + 4 + 12) / 8)

/*
 *   |byte1  ||byte2 |
 * 0b000_1111100000000
 *       ^^^^^
 */
#define MPEG_TS_PSI_PMT_STREAM_ELEMENT_ELEMENTARY_PID_MASK 0x1f

/*
 * element_stream_data:
 *   |byte3  ||byte4 |
 * 0b0000_111100000000
 *        ^^^^
 */
#define MPEG_TS_PSI_PMT_STREAM_ELEMENT_ES_INFO_LENGTH_MASK 0x0f

typedef struct
{
    MpegTsStreamType_e stream_type;
    uint16_t elementary_pid : MPEG_TS_PSI_PMT_ELEMENTARY_PID_BITS;
    uint16_t es_info_length : MPEG_TS_PSI_PMT_ES_INFO_LENGTH_BITS;
    uint8_t *es_info_descriptors_data; // Iterpret it as array of MpegTsDescriptor_t

} MpegTsElementStream_t;

typedef struct
{
    bool has_value;
    MpegTsElementStream_t value;

} OptionalMpegTsElementStream_t;

OptionalMpegTsElementStream_t mpeg_ts_parse_pmt_stream_element_linked(uint8_t *element_stream_data,
    size_t data_size);

/*
 * return amount of MpegTsElementStream_t in "element_stream_data"
 */
size_t mpeg_ts_count_stream_elements(uint8_t *element_stream_data, size_t data_size);

