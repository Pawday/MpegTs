#include <assert.h>

#include "language_descriptor_convertors.h"

const char *mpeg_ts_language_descriptor_audio_type_to_string(
    MpegTsLanguageDescriptorAudioType_e audio_type)
{
    switch (audio_type) {
    case MPEGTS_AUDIO_TYPE_UNDEFINED:
        return "Undefined";
    case MPEGTS_AUDIO_TYPE_CLEAN_EFFECTS:
        return "Clean Effects";
    case MPEGTS_AUDIO_TYPE_HEARING_IMPAIRED:
        return "Hearing Impaired";
    case MPEGTS_AUDIO_TYPE_VISUAL_IMPAIRED_COMMENTARY:
        return "Visual Impaired Commentary";
    case MPEGTS_AUDIO_TYPE_USER_PRIVATE:
        return "User Private";
    case MPEGTS_AUDIO_TYPE_RESERVED:
        return "Reserved";
    }

    assert("Should be unreachable");

    return "UNHANDLED MPEGTS_AUDIO_TYPE";
}
