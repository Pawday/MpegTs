#include "descriptor.h"

bool mpeg_ts_psi_parse_descriptor(MpegTsDescriptor_t *output_descriptor, const uint8_t *buffer, size_t buffer_size)
{
    if (buffer_size < MPEG_TS_DESCRIPTOR_HEADER_SIZE) {
        return false;
    }

    const uint8_t descriptor_tag_num = buffer[0];

    MpegTsDescriptorTag_e parsed_tag = mpeg_ts_num_to_descriptor_tag(descriptor_tag_num);
    if (parsed_tag == MPEG_DESCRIPTOR_FORBIDDEN) {
        return false;
    }

    uint8_t descriptor_data_size = buffer[1];
    if (buffer_size < (size_t)(descriptor_data_size + MPEG_TS_DESCRIPTOR_HEADER_SIZE)) {
        return false;
    }

    output_descriptor->length = descriptor_data_size;
    output_descriptor->tag = parsed_tag;
    output_descriptor->tag_num = descriptor_tag_num;
    output_descriptor->data = buffer + MPEG_TS_DESCRIPTOR_HEADER_SIZE;
    return true;
}

size_t mpeg_ts_psi_count_descriptors_in_buffer(const uint8_t *buffer, size_t buffer_size)
{
    size_t descriptors_amount_so_far = 0;
    size_t offset_in_buffer_to_next_descriptor = 0;

    bool last_descriptor_has_value = true;

    while (last_descriptor_has_value && offset_in_buffer_to_next_descriptor < buffer_size) {

        MpegTsDescriptor_t next_descriptor;
        if (!mpeg_ts_psi_parse_descriptor(&next_descriptor,
                buffer + offset_in_buffer_to_next_descriptor,
                buffer_size - offset_in_buffer_to_next_descriptor)) {
            break;
        }

        descriptors_amount_so_far++;
        offset_in_buffer_to_next_descriptor +=
            next_descriptor.length + MPEG_TS_DESCRIPTOR_HEADER_SIZE;
    }

    return descriptors_amount_so_far;
}

