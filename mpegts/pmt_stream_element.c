#include "pmt_stream_element.h"
#include "stream_type.h"

size_t mpeg_ts_count_stream_elements(uint8_t *element_stream_data, size_t data_size)
{
    size_t element_stream_amount_so_far = 0;
    size_t next_stream_element_offset = 0;

    while (next_stream_element_offset < data_size) {

        MpegTsElementStream_t next_stream_element;
        if (!mpeg_ts_parse_pmt_stream_element(&next_stream_element,
                element_stream_data + next_stream_element_offset,
                data_size - next_stream_element_offset)) {
            break;
        }

        element_stream_amount_so_far++;
        next_stream_element_offset +=
            next_stream_element.es_info_length + MPEG_TS_PSI_PMT_STREAM_ELEMENT_DESCRIPTORS_OFFSET;
    }

    return element_stream_amount_so_far;
}

bool mpeg_ts_parse_pmt_stream_element(MpegTsElementStream_t *output_element,
    uint8_t *element_stream_data, size_t data_size)
{

    if (data_size < MPEG_TS_PSI_PMT_STREAM_ELEMENT_DESCRIPTORS_OFFSET) {
        return false;
    }

    uint8_t stream_type_as_num = element_stream_data[0];

    MpegTsStreamType_e stream_type = mpeg_ts_int_to_stream_type(stream_type_as_num);

    output_element->stream_type = stream_type;

    /*
     * element_stream_data:
     *   |byte1  ||byte2 |
     * 0b000_0000000000000
     *    ^        ^
     *    |        |
     *    |        |
     *    |        +------ elementary_PID
     *    |
     *    |
     *    +--------  reserved
     *
     */

    /*
     * element_stream_data:
     *   |byte1  ||byte2 |
     * 0b000_0000000000000
     *       ^^^^ - select this
     */
    uint8_t elementary_pid_first_part =
        element_stream_data[1] & MPEG_TS_PSI_PMT_STREAM_ELEMENT_ELEMENTARY_PID_MASK;

    output_element->elementary_pid = 0;
    output_element->elementary_pid |= elementary_pid_first_part << 8;
    output_element->elementary_pid |= element_stream_data[2];

    /*
     * element_stream_data:
     *   |byte3  ||byte4 |
     * 0b0000_000000000000
     *     ^        ^
     *     |        |
     *     |        +-------- ES_info_length
     *     |
     *     |
     *     +-------- reserved
     *
     */

    output_element->es_info_length = 0;
    /*
     * element_stream_data:
     *   |byte3  |
     * 0b0000_0000
     *        ^^^^ - select this
     */
    uint8_t es_info_length_first_part =
        element_stream_data[3] & MPEG_TS_PSI_PMT_STREAM_ELEMENT_ES_INFO_LENGTH_MASK;
    output_element->es_info_length |= es_info_length_first_part << 8;
    output_element->es_info_length |= element_stream_data[4];

    output_element->es_info_descriptors_data = NULL;
    if (output_element->es_info_length != 0) {
        output_element->es_info_descriptors_data =
            element_stream_data + MPEG_TS_PSI_PMT_STREAM_ELEMENT_DESCRIPTORS_OFFSET;
    }

    return true;
}

