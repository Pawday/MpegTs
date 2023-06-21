#include <inttypes.h>

#include <mpegts/data/psi/descriptor.h>
#include <mpegts/data/psi/descriptor_dumper.h>
#include <mpegts/data/psi/pmt_dumper.h>
#include <mpegts/data/psi/pmt_stream_element.h>
#include <mpegts/data/psi/pmt_stream_element_dumper.h>

void mpeg_ts_dump_pmt_to_stream(MpegTsPMT_t *pmt_to_dump, FILE *stream)
{
    fprintf(stream, "{");
    fprintf(stream, "\"program_number\":%" PRIu16 ",", pmt_to_dump->program_number);
    fprintf(stream, "\"version_number\":%" PRIu8 ",", pmt_to_dump->version_number);

    fprintf(stream, "\"current_next_indicator\":");
    pmt_to_dump->current_next_indicator ? fprintf(stream, "true") : fprintf(stream, "false");
    fprintf(stream, ",");
    fprintf(stream, "\"PCR_PID\":%" PRIu16 ",", pmt_to_dump->PCR_PID);

    fprintf(stream, "\"program_info_descriptors\":");
    size_t info_descriptors_amount =
        mpeg_ts_psi_count_descriptors_in_buffer(pmt_to_dump->program_info_descriptors_data,
            pmt_to_dump->program_info_length);

    fprintf(stream, "[");
    if (info_descriptors_amount != 0) {
        mpeg_ts_dump_descriptors_to_stream(pmt_to_dump->program_info_descriptors_data,
            pmt_to_dump->program_info_length,
            info_descriptors_amount,
            stream);
    }
    fprintf(stream, "],");

    fprintf(stream, "\"element_streams\":");
    fprintf(stream, "[");

    size_t element_streams_amount = mpeg_ts_count_stream_elements(pmt_to_dump->program_elements,
        pmt_to_dump->program_elements_length);

    if (element_streams_amount != 0) {

        mpeg_ts_dump_stream_elements_to_stream(pmt_to_dump->program_elements,
            pmt_to_dump->program_elements_length,
            element_streams_amount,
            stream);
    }
    fprintf(stream, "],");

    fprintf(stream, "\"crc\":%" PRIu32, pmt_to_dump->CRC);

    fprintf(stream, "}");
}

