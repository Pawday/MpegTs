#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "magics.h"

typedef enum MpegTsPSITableID_e
{
    MPEG_TS_PSI_PROGRAM_MAP_SECTION,
    MPEG_TS_PSI_UNKNOWN_SECTION
} MpegTsPSITableID_e;

typedef struct MpegTsPSIHeader_t
{
    MpegTsPSITableID_e table_id;
    bool section_syntax_indicator;
    uint16_t section_length : MPEG_TS_PSI_SECTION_LENGTH_BITS;
} MpegTsPSIHeader_t;

static inline MpegTsPSITableID_e table_id_from_int(uint8_t table_id)
{
    switch (table_id) {
    case 0x02:
        return MPEG_TS_PSI_PROGRAM_MAP_SECTION;
    default:
        return MPEG_TS_PSI_UNKNOWN_SECTION;
    }
}


