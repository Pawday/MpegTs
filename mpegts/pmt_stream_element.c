#include "mpegts/pmt_stream_element.h"
#include "mpegts/stream_type.h"

size_t mpeg_ts_count_stream_elements(uint8_t *element_stream_data, size_t data_size)
{
    size_t element_stream_amount_so_far = 0;
    size_t next_stream_element_offset = 0;

    bool last_stream_element_had_value = true;

    while (last_stream_element_had_value && next_stream_element_offset < data_size) {

        OptionalMpegTsElementStream_t next_stream_element = mpeg_ts_parse_pmt_stream_element_linked(
            element_stream_data + next_stream_element_offset,
            data_size - next_stream_element_offset);

        if (!next_stream_element.has_value) {
            last_stream_element_had_value = false;
            continue;
        }

        element_stream_amount_so_far++;
        next_stream_element_offset += next_stream_element.value.es_info_length +
                                      MPEG_TS_PSI_PMT_STREAM_ELEMENT_DESCRIPTORS_OFFSET;
    }

    return element_stream_amount_so_far;
}

OptionalMpegTsElementStream_t mpeg_ts_parse_pmt_stream_element_linked(uint8_t *element_stream_data,
    size_t data_size)
{
    const OptionalMpegTsElementStream_t bad_value = {.has_value = false, .value = {0}};

    if (data_size < MPEG_TS_PSI_PMT_STREAM_ELEMENT_DESCRIPTORS_OFFSET)
        return bad_value;

    uint8_t stream_type_as_num = element_stream_data[0];

    OptionalMpegTsElementStream_t ret_val;
    ret_val.has_value = true;

    MpegTsStreamType_e stream_type = mpeg_ts_int_to_stream_type(stream_type_as_num);

    ret_val.value.stream_type = stream_type;

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

    ret_val.value.elementary_pid = 0;
    ret_val.value.elementary_pid |= elementary_pid_first_part << 8;
    ret_val.value.elementary_pid |= element_stream_data[2];

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

    ret_val.value.es_info_length = 0;

    /*
     * element_stream_data:
     *   |byte3  |
     * 0b0000_0000
     *        ^^^^ - select this
     */
    uint8_t es_info_length_first_part =
        element_stream_data[3] & MPEG_TS_PSI_PMT_STREAM_ELEMENT_ES_INFO_LENGTH_MASK;

    ret_val.value.es_info_length = 0;
    ret_val.value.es_info_length |= es_info_length_first_part << 8;
    ret_val.value.es_info_length |= element_stream_data[4];

    ret_val.value.es_info_descriptors_data =
        element_stream_data + MPEG_TS_PSI_PMT_STREAM_ELEMENT_DESCRIPTORS_OFFSET;

    return ret_val;
}

