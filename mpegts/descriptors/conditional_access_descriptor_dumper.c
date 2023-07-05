#include <assert.h>
#include <inttypes.h>

#include "conditional_access_descriptor_dumper.h"

void mpeg_ts_dump_ca_descriptor_content_json_to_stream(MpegTsCADescriptor_t *descriptor_to_dump,
    FILE *stream)
{
    fprintf(stream, "{");
    fprintf(stream, "\"CA_system_ID\":0x%" PRIx16 ",", descriptor_to_dump->CA_system_ID);
    fprintf(stream, "\"CA_PID\":0x%" PRIx16 ",", descriptor_to_dump->CA_PID);
    fprintf(stream, "\"CA_private_data\":");
    fprintf(stream, "[");

    uint8_t CA_private_data_length = descriptor_to_dump->private_data_length;

    assert(CA_private_data_length <= MPEG_TS_CONDITIONAL_ACCESS_DESCRIPTOR_PRIVATE_DATA_MAX_SIZE);

    for (uint8_t ca_private_byte_index = 0; ca_private_byte_index < CA_private_data_length;
         ca_private_byte_index++) {
        fprintf(stream, "0x%" PRIx8, descriptor_to_dump->private_data[ca_private_byte_index]);

        if (ca_private_byte_index + 1 != CA_private_data_length) {
            fprintf(stream, ",");
        }
    }

    fprintf(stream, "]");
    fprintf(stream, "}");
}
