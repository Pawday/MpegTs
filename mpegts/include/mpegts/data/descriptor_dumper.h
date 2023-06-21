#pragma once

#include <stdio.h>

#include "mpegts/data/descriptor.h"

void mpeg_ts_dump_descriptor_to_stream(MpegTsDescriptor_t *descriptor_to_dump, FILE *stream);
void mpeg_ts_dump_descriptors_to_stream(uint8_t *first_descriptor_location, size_t data_length,
    size_t descriptors_amount, FILE *stream);

