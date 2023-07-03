#include "language_descriptor_dumper.h"

#include "language_descriptor_converters.h"

void mpeg_ts_dump_language_descriptor_content_json_to_stream(
    MpegTsLanguageDescriptor_t *descriptor_to_dump, FILE *stream)
{
    fprintf(stream, "{");
    fprintf(stream, "\"language_code\":\"%.3s\",", descriptor_to_dump->language_code);
    fprintf(stream, "\"audio_type\":0x%" PRIx8 ",", descriptor_to_dump->audio_type_num);
    fprintf(stream,
        "\"audio_type_string\":\"%s\"",
        mpeg_ts_language_descriptor_audio_type_to_string(descriptor_to_dump->audio_type));
    fprintf(stream, "}");
}
