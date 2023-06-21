#include "mpegts/data/descriptor_dumper.h"

void mpeg_ts_dump_descriptor_to_stream(MpegTsDescriptor_t *descriptor_to_dump, FILE *stream)
{
    fprintf(stream, "{");
    fprintf(stream,
        "\"desctiptor_tag\":\"%s\",",
        mpeg_ts_descriptor_tag_to_string(descriptor_to_dump->tag));

    fprintf(stream, "\"descriptor_data\":");

    if (descriptor_to_dump->length == 0) {
        fprintf(stream, "[]}");
        return;
    }

    fprintf(stream, "[");
    for (size_t byte_index = 0; byte_index < descriptor_to_dump->length; byte_index++) {
        fprintf(stream, "\"0x%" PRIx8 "\"", descriptor_to_dump->data[byte_index]);

        if (byte_index + 1 != descriptor_to_dump->length) {
            fputc(',', stream);
        }
    }

    fprintf(stream, "]}");
}

void mpeg_ts_dump_descriptors_to_stream(uint8_t *first_descriptor_location, size_t data_length,
    size_t descriptors_amount, FILE *stream)
{
    size_t current_descriptor_data_offset = 0;

    for (size_t descriptor_index = 0; descriptor_index < descriptors_amount; descriptor_index++) {

        OptionalMpegTsDescriptor_t descriptor = mpeg_ts_psi_parse_descriptor_linked(
            first_descriptor_location + current_descriptor_data_offset,
            data_length - current_descriptor_data_offset);

        if (!descriptor.has_value) {
            break;
        }

        mpeg_ts_dump_descriptor_to_stream(&descriptor.value, stream);

        if (descriptor_index + 1 != descriptors_amount) {
            fputc(',', stream);
        }

        current_descriptor_data_offset += descriptor.value.length + MPEG_TS_DESCRIPTOR_HEADER_SIZE;
    }
}

