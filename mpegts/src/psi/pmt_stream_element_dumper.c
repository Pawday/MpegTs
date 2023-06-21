#include <mpegts/data/psi/descriptor_dumper.h>
#include <mpegts/data/psi/pmt_stream_element_dumper.h>

void mpeg_ts_dump_stream_element_to_stream(MpegTsElementStream_t *element, FILE *output_stream)
{
    fprintf(output_stream, "{");

    fprintf(output_stream,
        "\"stream_type\":\"%s\",",
        mpeg_ts_stream_type_to_string(element->stream_type));

    fprintf(output_stream, "\"elementary_PID\":%" PRIu16 ",", element->elementary_pid);
    fprintf(output_stream, "\"es_info_length\":%" PRIu16 ",", element->es_info_length);

    fprintf(output_stream, "\"descriptors\":");

    size_t stream_element_descriptors_amount =
        mpeg_ts_psi_count_descriptors_in_buffer(element->es_info_descriptors_data,
            element->es_info_length);

    fprintf(output_stream, "[");

    if (stream_element_descriptors_amount != 0) {
        mpeg_ts_dump_descriptors_to_stream(element->es_info_descriptors_data,
            element->es_info_length,
            stream_element_descriptors_amount,
            output_stream);
    }

    fprintf(output_stream, "]}");
}

void mpeg_ts_dump_stream_elements_to_stream(uint8_t *program_stream_elements_start,
    size_t program_elements_data_size, size_t elements_amount, FILE *output_stream)
{
    size_t current_element_data_offset = 0;

    for (size_t element_index = 0; element_index < elements_amount; element_index++) {

        MpegTsElementStreamMaybe_t element = mpeg_ts_parse_pmt_stream_element_linked(
            program_stream_elements_start + current_element_data_offset,
            program_elements_data_size - current_element_data_offset);

        if (!element.has_value) {
            break;
        }

        mpeg_ts_dump_stream_element_to_stream(&element.value, output_stream);

        if (element_index + 1 != elements_amount) {
            fputc(',', output_stream);
        }

        current_element_data_offset +=
            element.value.es_info_length + MPEG_TS_PSI_PMT_STREAM_ELEMENT_DESCRIPTORS_OFFSET;
    }
}

