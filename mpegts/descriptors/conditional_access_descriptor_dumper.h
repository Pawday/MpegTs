#pragma once

#include <stdio.h>

#include "conditional_access_descriptor.h"

void mpeg_ts_dump_ca_descriptor_content_json_to_stream(MpegTsCADescriptor_t *descriptor_to_dump, FILE *stream);
