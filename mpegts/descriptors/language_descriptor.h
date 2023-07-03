#pragma once

#include "mpegts/descriptor.h"

#define MPEG_TS_LANGUAGE_DESCRIPTOR_SIZE 4

typedef enum
{
    MPEGTS_AUDIO_TYPE_UNDEFINED,
    MPEGTS_AUDIO_TYPE_CLEAN_EFFECTS,
    MPEGTS_AUDIO_TYPE_HEARING_IMPAIRED,
    MPEGTS_AUDIO_TYPE_VISUAL_IMPAIRED_COMMENTARY,
    MPEGTS_AUDIO_TYPE_USER_PRIVATE,
    MPEGTS_AUDIO_TYPE_RESERVED
} MpegTsLanguageDescriptorAudioType_e;

typedef struct
{
    uint8_t language_code[3];
    uint8_t audio_type_num;
    MpegTsLanguageDescriptorAudioType_e audio_type;
} MpegTsLanguageDescriptor_t;


MpegTsLanguageDescriptorAudioType_e mpeg_ts_language_descriptor_audio_type_from_num(uint8_t num);
bool mpeg_ts_language_descriptor_from_raw_descriptor(MpegTsDescriptor_t *raw_descriptor,
    MpegTsLanguageDescriptor_t *output_lang_descriptor);
