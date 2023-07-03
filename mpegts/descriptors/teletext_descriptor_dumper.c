#include "teletext_descriptor_converters.h"
#include "teletext_descriptor_dumper.h"

void mpeg_ts_dump_teletext_descriptor_content_json_to_stream(
    MpegTsTeletextDescriptor_t *descriptor_to_dump, FILE *stream)
{
    fprintf(stream, "{");
    fprintf(stream, "\"language_code\":\"%.3s\",", descriptor_to_dump->language_code);
    fprintf(stream, "\"teletext_type\":%" PRIx8 ",", descriptor_to_dump->teletext_type);
    fprintf(stream,
        "\"teletext_type_string\":\"%s\",",
        mpeg_ts_teletext_descriptor_type_to_string(descriptor_to_dump->teletext_type));

    fprintf(stream,
        "\"teletext_magazine_number\":%" PRIx8 ",",
        descriptor_to_dump->teletext_magazine_number);

    fprintf(stream, "\"teletext_page_number\":%" PRIx8, descriptor_to_dump->teletext_page_number);
    fprintf(stream, "}");
}
