#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#include "psi_magics.h"

typedef struct MpegTsPMT_t
{
    // uint8_t table_id; // for program map table always 0x02
    // bool section_syntax_indicator; // always true

    /*
     * NOTE: section_length decreased by MPEG_TS_PSI_PMT_SECTION_LENGTH_OFFSET because
     * ISO/IEC 13818-1: "specify the number of bytes of the section starting immediately following
     *                       the section_length field"
     */
    uint16_t section_length : MPEG_TS_PSI_SECTION_LENGTH_BITS;

    uint16_t program_number;
    uint8_t version_number : MPET_TS_PSI_PMT_VERSION_NUMBER_BITS;
    bool current_next_indicator;
    // uint8_t section_number;      // for program map table always 0x00
    // uint8_t last_section_number; // for program map table always 0x00
    uint16_t PCR_PID : MPET_TS_PSI_PMT_PCR_PID_BITS;

    uint16_t program_info_length : MPET_TS_PSI_PMT_PROGRAM_INFO_LENGTH_BITS;
    uint8_t *program_info_descriptors_data; // Iterpret it as MpegTsDescriptor_t

    uint16_t program_elements_length;
    uint8_t *program_elements; // Iterpret it as array of MpegTsElementStream_t

    uint32_t CRC;

} MpegTsPMT_t;

typedef struct
{
    bool has_value;
    MpegTsPMT_t value;
} OptionalMpegTsPMT_t;

