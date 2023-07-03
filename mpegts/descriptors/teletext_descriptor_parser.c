#include <assert.h>
#include <memory.h>

#include "teletext_descriptor.h"

MpegTsTeletextDescriptorType_e mpeg_ts_teletext_descriptor_type_from_num(
    uint8_t teletext_type_as_num)
{
    switch (teletext_type_as_num) {
    case 0x00:
        return TELETEXT_TYPE_RESERVED;
    case 0x01:
        return TELETEXT_TYPE_INITIAL_TELETEXT_PAGE;
    case 0x02:
        return TELETEXT_TYPE_SUBTITLE_PAGE;
    case 0x03:
        return TELETEXT_TYPE_ADDITIONAL_INFORMATION_PAGE;
    case 0x04:
        return TELETEXT_TYPE_PROGRAMME_SCHEDULE_PAGE;
    case 0x05:
        return TELETEXT_TYPE_HEARING_IMPAIRED_PAGE;
    }

    if (teletext_type_as_num >= 0x06 && teletext_type_as_num <= 0x1f) {
        return TELETEXT_TYPE_RESERVED;
    }

    assert("IMPOSSIBLE TELETEXT TYPE");

    return TELETEXT_TYPE_IMPOSSIBLE;
}

bool mpeg_ts_teletext_descriptor_from_raw_descriptor(MpegTsDescriptor_t *raw_descriptor,
    MpegTsTeletextDescriptor_t *output_teletext_descriptor)
{
    if (raw_descriptor->tag != TELETEXT_DESCRIPTOR) {
        return false;
    }

    if (raw_descriptor->length != MPEG_TS_TELETEXT_DESCRIPTOR_SIZE) {
        return false;
    }

    memcpy(output_teletext_descriptor->language_code,
        raw_descriptor->data,
        sizeof(output_teletext_descriptor->language_code));

    uint8_t type_and_magazine_byte = raw_descriptor->data[3];
    uint8_t teletext_type_num =
        (type_and_magazine_byte & MPEG_TS_TELETEXT_TYPE_MASK) >> (8 - MPEG_TS_TELETEXT_TYPE_BITS);
    uint8_t magazine_num = type_and_magazine_byte & MPEG_TS_TELETEXT_MAGAZINE_NUMBER_MASK;

    MpegTsTeletextDescriptorType_e teletext_type =
        mpeg_ts_teletext_descriptor_type_from_num(teletext_type_num);

    if (teletext_type == TELETEXT_TYPE_IMPOSSIBLE) {
        return false;
    }

    output_teletext_descriptor->teletext_type_num = teletext_type_num;
    output_teletext_descriptor->teletext_type = teletext_type;
    output_teletext_descriptor->teletext_magazine_number = magazine_num;
    output_teletext_descriptor->teletext_page_number = raw_descriptor->data[4];

    return true;
}
