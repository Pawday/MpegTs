#include <assert.h>

#include "teletext_descriptor_converters.h"

const char *mpeg_ts_teletext_descriptor_type_to_string(MpegTsTeletextDescriptorType_e teletext_type)
{
    switch (teletext_type) {

    case TELETEXT_TYPE_RESERVED:
        return "Teletext Type Reserved";
    case TELETEXT_TYPE_INITIAL_TELETEXT_PAGE:
        return "Initial Teletext Page";
    case TELETEXT_TYPE_SUBTITLE_PAGE:
        return "Subtitle Page";
    case TELETEXT_TYPE_ADDITIONAL_INFORMATION_PAGE:
        return "Additional Information Page";
    case TELETEXT_TYPE_PROGRAMME_SCHEDULE_PAGE:
        return "Programme Schedule Page";
    case TELETEXT_TYPE_HEARING_IMPAIRED_PAGE:
        return "Hearing Impaired Page";
    case TELETEXT_TYPE_IMPOSSIBLE:
        assert(false && "IMPOSSIBLE TELETEXT TYPE");
        break;
    }

    assert(false && "Should be unreachable");

    return "UNHANDLED MPEG_TS_TELETEXT_TYPE";
}
