#include <assert.h>

#include "descriptor_dumper.h"
#include "descriptor_parser.h"

#include "descriptors/language_descriptor_dumper.h"
#include "descriptors/teletext_descriptor_dumper.h"

#define STREAM_IDENTIFIER_DESCRIPTOR_SIZE 1

static bool try_dump_descriptor_data_as_object(MpegTsDescriptor_t *descriptor_to_dump, FILE *stream)
{
    switch (descriptor_to_dump->tag) {
    case ISO_639_LANGUAGE_DESCRIPTOR: {
        MpegTsLanguageDescriptor_t lang_descriptor = {0};

        bool parse_lang_descriptor_status =
            mpeg_ts_language_descriptor_from_raw_descriptor(descriptor_to_dump, &lang_descriptor);

        assert(parse_lang_descriptor_status);

        if (parse_lang_descriptor_status) {
            mpeg_ts_dump_language_descriptor_content_json_to_stream(&lang_descriptor, stream);
            return true;
        } else {
            return false;
        }
    }
    case TELETEXT_DESCRIPTOR: {
        MpegTsTeletextDescriptor_t teletext_descriptor = {0};

        bool parse_teletext_descriptor_status =
            mpeg_ts_teletext_descriptor_from_raw_descriptor(descriptor_to_dump,
                &teletext_descriptor);

        assert(parse_teletext_descriptor_status);

        if (parse_teletext_descriptor_status) {
            mpeg_ts_dump_teletext_descriptor_content_json_to_stream(&teletext_descriptor, stream);
            return true;
        } else {
            return false;
        }
    }
    case STREAM_IDENTIFIER_DESCRIPTOR: {
        if (descriptor_to_dump->length != STREAM_IDENTIFIER_DESCRIPTOR_SIZE) {
            return false;
        }

        fprintf(stream, "{");
        fprintf(stream, "\"component_tag\":0x%" PRIx8, descriptor_to_dump->data[0]);
        fprintf(stream, "}");

        return true;
    }
    default:
        return false;
    };
}

void mpeg_ts_dump_descriptor_json_to_stream(MpegTsDescriptor_t *descriptor_to_dump, FILE *stream)
{
    fprintf(stream, "{");
    fprintf(stream, "\"descriptor_tag\":0x%" PRIx8 ",", descriptor_to_dump->tag_num);
    fprintf(stream,
        "\"descriptor_tag_string\":\"%s\"",
        mpeg_ts_descriptor_tag_to_string(descriptor_to_dump->tag));

    if (descriptor_to_dump->length == 0) {
        goto end_object;
    }
    fprintf(stream, ",");

    fprintf(stream, "\"descriptor_data\":");

    if (try_dump_descriptor_data_as_object(descriptor_to_dump, stream)) {
        goto end_object;
    }

    fprintf(stream, "[");
    for (size_t byte_index = 0; byte_index < descriptor_to_dump->length; byte_index++) {
        fprintf(stream, "0x%" PRIx8 "", descriptor_to_dump->data[byte_index]);

        if (byte_index + 1 != descriptor_to_dump->length) {
            fputc(',', stream);
        }
    }
    fprintf(stream, "]");

end_object:
    fprintf(stream, "}");
}

void mpeg_ts_dump_descriptors_json_to_stream(uint8_t *first_descriptor_location, size_t data_length,
    size_t descriptors_amount, FILE *stream)
{
    size_t current_descriptor_data_offset = 0;

    for (size_t descriptor_index = 0; descriptor_index < descriptors_amount; descriptor_index++) {

        MpegTsDescriptor_t descriptor_to_dump = {0};
        if (!mpeg_ts_psi_parse_descriptor_linked(&descriptor_to_dump,
                first_descriptor_location + current_descriptor_data_offset,
                data_length - current_descriptor_data_offset)) {
            break;
        }

        mpeg_ts_dump_descriptor_json_to_stream(&descriptor_to_dump, stream);

        if (descriptor_index + 1 != descriptors_amount) {
            fputc(',', stream);
        }

        current_descriptor_data_offset +=
            descriptor_to_dump.length + MPEG_TS_DESCRIPTOR_HEADER_SIZE;
    }
}

