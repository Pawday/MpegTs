#include <assert.h>


#include "descriptor_dumper.h"
#include "descriptor_parser.h"

#include "descriptors/language_descriptor_convertors.h"
#include "descriptors/language_descriptor_dumper.h"
#include "mpegts/descriptors/language_descriptor.h"

static bool try_dump_descriptor_data_as_object(MpegTsDescriptor_t *descriptor_to_dump, FILE *stream)
{
    switch (descriptor_to_dump->tag) {
    case ISO_639_LANGUAGE_DESCRIPTOR: {
        OptionalMpegTsLanguageDescriptor_t opt_lang_desc =
            mpeg_ts_language_descriptor_from_raw_descriptor(descriptor_to_dump);

        assert(opt_lang_desc.has_value);

        if (opt_lang_desc.has_value) {
            mpeg_ts_dump_language_descriptor_content_json_to_stream(&opt_lang_desc.value, stream);
            return true;
        }
    }
    default:
        return false;
    };
}

void mpeg_ts_dump_descriptor_json_to_stream(MpegTsDescriptor_t *descriptor_to_dump, FILE *stream)
{
    fprintf(stream, "{");
    fprintf(stream, "\"desctiptor_tag\":0x%" PRIx8 ",", descriptor_to_dump->tag_num);
    fprintf(stream,
        "\"desctiptor_tag_string\":\"%s\",",
        mpeg_ts_descriptor_tag_to_string(descriptor_to_dump->tag));

    fprintf(stream, "\"descriptor_data\":");

    if (descriptor_to_dump->length == 0) {
        fprintf(stream, "\"[]\"");
        goto end_dumping;
    }

    if (try_dump_descriptor_data_as_object(descriptor_to_dump, stream)) {
        goto end_dumping;
    }

    fprintf(stream, "[");
    for (size_t byte_index = 0; byte_index < descriptor_to_dump->length; byte_index++) {
        fprintf(stream, "0x%" PRIx8 "", descriptor_to_dump->data[byte_index]);

        if (byte_index + 1 != descriptor_to_dump->length) {
            fputc(',', stream);
        }
    }
    fprintf(stream, "]");

end_dumping:
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

