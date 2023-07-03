#pragma once

#include "mpegts/descriptor.h"

#define MPEG_TS_TELETEXT_DESCRIPTOR_SIZE      5

#define MPEG_TS_TELETEXT_TYPE_BITS            5
#define MPEG_TS_TELETEXT_TYPE_MASK            0xF8 // 0b11111000
#define MPEG_TS_TELETEXT_MAGAZINE_NUMBER_BITS 3
#define MPEG_TS_TELETEXT_MAGAZINE_NUMBER_MASK 0x07 // 0b00000111

typedef enum
{
    TELETEXT_TYPE_RESERVED,
    TELETEXT_TYPE_INITIAL_TELETEXT_PAGE,
    TELETEXT_TYPE_SUBTITLE_PAGE,
    TELETEXT_TYPE_ADDITIONAL_INFORMATION_PAGE,
    TELETEXT_TYPE_PROGRAMME_SCHEDULE_PAGE, // "programme" - from DVB std
    TELETEXT_TYPE_HEARING_IMPAIRED_PAGE,
    TELETEXT_TYPE_IMPOSSIBLE
} MpegTsTeletextDescriptorType_e;

typedef struct
{
    uint8_t language_code[3];
    MpegTsTeletextDescriptorType_e teletext_type;
    uint8_t teletext_type_num : MPEG_TS_TELETEXT_TYPE_BITS;
    uint8_t teletext_magazine_number : MPEG_TS_TELETEXT_MAGAZINE_NUMBER_BITS;
    uint8_t teletext_page_number;
} MpegTsTeletextDescriptor_t;

MpegTsTeletextDescriptorType_e mpeg_ts_teletext_descriptor_type_from_num(
    uint8_t teletext_type_as_num);

bool mpeg_ts_teletext_descriptor_from_raw_descriptor(MpegTsDescriptor_t *raw_descriptor,
    MpegTsTeletextDescriptor_t *output_teletext_descriptor);
