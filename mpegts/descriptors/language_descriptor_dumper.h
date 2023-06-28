#pragma once

#include <stdio.h>

#include "language_descriptor.h"

void mpeg_ts_dump_language_descriptor_content_json_to_stream(
    MpegTsLanguageDescriptor_t *descriptor_to_dump, FILE *stream);
