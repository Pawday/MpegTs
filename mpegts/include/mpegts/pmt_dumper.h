#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "mpegts/pmt.h"

void mpeg_ts_dump_pmt_to_stream(MpegTsPMT_t *pmt_to_dump, FILE *stream);

