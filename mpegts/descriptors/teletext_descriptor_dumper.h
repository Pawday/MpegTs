#pragma once

#include <stdio.h>

#include "teletext_descriptor.h"

void mpeg_ts_dump_teletext_descriptor_content_json_to_stream(MpegTsTeletextDescriptor_t *descriptor_to_dump, FILE *stream);
