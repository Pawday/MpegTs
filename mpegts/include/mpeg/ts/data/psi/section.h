#pragma once

#include "header.h"
#include "magics.h"


typedef struct MpegTsPSISection_t
{
    MpegTsPSIHeader_t header;
    uint8_t *section_data;
} MpegTsPSISection_t;

typedef struct MpegTsPSISectionMaybe_t
{
    bool has_value;
    MpegTsPSISection_t value;
} MpegTsPSISectionMaybe_t;


