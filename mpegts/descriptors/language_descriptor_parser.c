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

bool mpeg_ts_language_descriptor_from_raw_descriptor(MpegTsDescriptor_t *raw_descriptor,
    MpegTsLanguageDescriptor_t *output_lang_descriptor)
{
    if (raw_descriptor->tag != ISO_639_LANGUAGE_DESCRIPTOR) {
        return false;
    }

    if (raw_descriptor->length != MPEG_TS_LANGUAGE_DESCRIPTOR_SIZE) {
        return false;
    }

    uint8_t audio_type_num = raw_descriptor->data[3];

    output_lang_descriptor->audio_type =
        mpeg_ts_language_descriptor_audio_type_from_num(audio_type_num);
    output_lang_descriptor->audio_type_num = audio_type_num;
    memcpy(output_lang_descriptor->language_code,
        raw_descriptor->data,
        sizeof(output_lang_descriptor->language_code));

    return true;
}
