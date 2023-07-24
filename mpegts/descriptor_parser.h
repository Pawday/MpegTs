#pragma once

#include "descriptor.h"

/*
 * Will parse MpegTsDescriptor from buffer with "data" field linked to "buffer"
 */
bool mpeg_ts_psi_parse_descriptor(MpegTsDescriptor_t *output_descriptor,
    const uint8_t *buffer, size_t buffer_size);

size_t mpeg_ts_psi_count_descriptors_in_buffer(const uint8_t *buffer, size_t buffer_size);

