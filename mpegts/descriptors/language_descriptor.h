#pragma once

#include <mpegts/descriptor.h>

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

typedef struct
{
    bool has_value;
    MpegTsLanguageDescriptor_t value;
} OptionalMpegTsLanguageDescriptor_t;

MpegTsLanguageDescriptorAudioType_e mpeg_ts_language_descriptor_audio_type_from_num(uint8_t num);
OptionalMpegTsLanguageDescriptor_t mpeg_ts_language_descriptor_from_raw_descriptor(MpegTsDescriptor_t *raw_descriptor);
