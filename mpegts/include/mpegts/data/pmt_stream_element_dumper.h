#pragma once

#include <stdio.h>

#include "mpegts/data/pmt_stream_element.h"

void mpeg_ts_dump_stream_element_to_stream(MpegTsElementStream_t *element, FILE *output_stream);

void mpeg_ts_dump_stream_elements_to_stream(uint8_t *program_elements_start,
    size_t program_elements_data_size, size_t elements_amount, FILE *output_stream);

