#include <assert.h>
#include <memory.h>

#include "language_descriptor.h"

MpegTsLanguageDescriptorAudioType_e mpeg_ts_language_descriptor_audio_type_from_num(uint8_t num)
{
    switch (num) {
    case 0x00:
        return MPEGTS_AUDIO_TYPE_UNDEFINED;
    case 0x01:
        return MPEGTS_AUDIO_TYPE_CLEAN_EFFECTS;
    case 0x02:
        return MPEGTS_AUDIO_TYPE_HEARING_IMPAIRED;
    case 0x03:
        return MPEGTS_AUDIO_TYPE_VISUAL_IMPAIRED_COMMENTARY;
    }

    if (num >= 0x04 && num <= 0x7f) {
        return MPEGTS_AUDIO_TYPE_USER_PRIVATE;
    }

    assert(num >= 0x80 && num <= 0xFF);

    return MPEGTS_AUDIO_TYPE_RESERVED;
}

OptionalMpegTsLanguageDescriptor_t mpeg_ts_language_descriptor_from_raw_descriptor(
    MpegTsDescriptor_t *raw_descriptor)
{
    const OptionalMpegTsLanguageDescriptor_t bad_value = {.has_value = false, .value = {0}};

    if (raw_descriptor->tag != ISO_639_LANGUAGE_DESCRIPTOR) {
        return bad_value;
    }

    if (raw_descriptor->length != MPEG_TS_LANGUAGE_DESCRIPTOR_SIZE) {
        return bad_value;
    }

    OptionalMpegTsLanguageDescriptor_t ret_val = {0};

    uint8_t audio_type_num = raw_descriptor->data[3];

    ret_val.has_value = true;
    ret_val.value.audio_type = mpeg_ts_language_descriptor_audio_type_from_num(audio_type_num);
    ret_val.value.audio_type_num = audio_type_num;
    memcpy(ret_val.value.language_code, raw_descriptor->data, 3);

    return ret_val;
}
