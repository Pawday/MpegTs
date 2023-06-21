#include "mpegts/data/descriptor.h"

OptionalMpegTsDescriptor_t mpeg_ts_psi_parse_descriptor_linked(const uint8_t *buffer,
    size_t buffer_size)
{
    const OptionalMpegTsDescriptor_t bad_value = {.has_value = false,
        .value = {.length = 0, .data = NULL, .tag = MPEG_DESCRIPTOR_FORBIDDEN}};

    if (buffer_size < MPEG_TS_DESCRIPTOR_HEADER_SIZE) {
        return bad_value;
    }

    MpegTsDescriptorTag_e parsed_tag = mpeg_ts_num_to_descriptor_tag(buffer[0]);
    if (parsed_tag == MPEG_DESCRIPTOR_FORBIDDEN) {
        return bad_value;
    }

    uint8_t descriptor_data_size = buffer[1];
    if (buffer_size < descriptor_data_size + MPEG_TS_DESCRIPTOR_HEADER_SIZE) {
        return bad_value;
    }

    OptionalMpegTsDescriptor_t ret_val;
    ret_val.has_value = true;
    ret_val.value.length = descriptor_data_size;
    ret_val.value.tag = parsed_tag;
    ret_val.value.data = buffer + MPEG_TS_DESCRIPTOR_HEADER_SIZE;
    return ret_val;
}

size_t mpeg_ts_psi_count_descriptors_in_buffer(const uint8_t *buffer, size_t buffer_size)
{
    size_t descriptors_amount_so_far = 0;
    size_t offset_in_buffer_to_next_descriptor = 0;

    bool last_descriptor_has_value = true;

    while (last_descriptor_has_value && offset_in_buffer_to_next_descriptor < buffer_size) {
        OptionalMpegTsDescriptor_t next_descriptor =
            mpeg_ts_psi_parse_descriptor_linked(buffer + offset_in_buffer_to_next_descriptor,
                buffer_size - offset_in_buffer_to_next_descriptor);

        if (!next_descriptor.has_value) {
            last_descriptor_has_value = false;
            continue;
        }

        descriptors_amount_so_far++;
        offset_in_buffer_to_next_descriptor +=
            next_descriptor.value.length + MPEG_TS_DESCRIPTOR_HEADER_SIZE;
    }

    return descriptors_amount_so_far;
}

