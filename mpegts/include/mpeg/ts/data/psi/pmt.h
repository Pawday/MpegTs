#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#include <mpeg/ts/data/psi/descriptor.h>
#include <mpeg/ts/data/psi/stream_type.h>

#include "psi_magics.h"

typedef struct MpegTsElement_t
{
    MpegTsStreamType_e stream_type;
    uint16_t elementary_pid : MPET_TS_PSI_PMT_ELEMENTARY_PID_BITS;
    uint16_t es_info_length : MPET_TS_PSI_PMT_ES_INFO_LENGTH_BITS;
    uint8_t *es_info_descriptors_data; // Iterpret it as MpegTsDescriptor_t

} MpegTsElement_t;

typedef struct MpegTsPMT_t
{
    // uint8_t table_id; // for program map table always 0x02
    // bool section_syntax_indicator; // always true
    uint16_t section_length : MPEG_TS_PSI_SECTION_LENGTH_BITS;
    uint16_t program_number;
    uint8_t version_number : MPET_TS_PSI_PMT_VERSION_NUMBER_BITS;
    bool current_next_indicator;
    // uint8_t section_number;      // for program map table always 0x00
    // uint8_t last_section_number; // for program map table always 0x00
    uint16_t PCR_PID : MPET_TS_PSI_PMT_PCR_PID_BITS;

    uint16_t program_info_length : MPET_TS_PSI_PMT_PROGRAM_INFO_LENGTH_BITS;
    uint8_t *program_info_descriptors_data; // Iterpret it as MpegTsDescriptor_t

    size_t program_elements_size;
    MpegTsElement_t *program_elements;

    uint32_t CRC;

} MpegTsPMT_t;

typedef struct MpegTsPMTMaybe_t
{
    bool has_value;
    MpegTsPMT_t value;
} MpegTsPMTMaybe_t;


