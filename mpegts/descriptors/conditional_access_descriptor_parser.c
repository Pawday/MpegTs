#include "conditional_access_descriptor.h"

bool mpeg_ts_conditional_access_descriptor_from_raw_descriptor(MpegTsDescriptor_t *raw_descriptor,
    MpegTsCADescriptor_t *output_CA_descriptor)
{
    if (raw_descriptor->tag != CA_DESCRIPTOR) {
        return false;
    }

    if (raw_descriptor->length < MPEG_TS_CONDITIONAL_ACCESS_DESCRIPTOR_HEADER_SIZE) {
        return false;
    }

    uint8_t ca_descriptor_private_data_length =
        raw_descriptor->length - MPEG_TS_CONDITIONAL_ACCESS_DESCRIPTOR_HEADER_SIZE;

    output_CA_descriptor->private_data_length = ca_descriptor_private_data_length;

    output_CA_descriptor->private_data = NULL;
    if (ca_descriptor_private_data_length != 0) {
        output_CA_descriptor->private_data =
            raw_descriptor->data + MPEG_TS_CONDITIONAL_ACCESS_DESCRIPTOR_HEADER_SIZE;
    }

    output_CA_descriptor->CA_system_ID = 0;
    output_CA_descriptor->CA_system_ID |= raw_descriptor->data[0] << 8;
    output_CA_descriptor->CA_system_ID |= raw_descriptor->data[1];

    output_CA_descriptor->CA_PID = 0;
    output_CA_descriptor->CA_PID |= raw_descriptor->data[2] << 8;
    output_CA_descriptor->CA_PID |= raw_descriptor->data[3];
    output_CA_descriptor->CA_PID &= MPEG_TS_CONDITIONAL_ACCESS_DESCRIPTOR_CA_PID_MASK;

    return true;
}
