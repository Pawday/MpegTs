#include "test_data.h"
#include <assert.h>

#include <mpegts/packet_inplace_parser.h>
#include <mpegts/pmt_builder.h>
#include <mpegts/pmt_stream_element.h>

int main()
{
    uint8_t builder_buff[MPEG_TS_PSI_PMT_SECTION_MAX_LENGTH];

    MpegTsPMTBuilder_t builder = {0};

    mpeg_ts_pmt_builder_init(&builder, builder_buff, sizeof(builder_buff));

    IOVec packet_with_small_pmt = test_data_get_packet_with_pmt();

    MpegTsPacket_t packet = {0};

    assert(mpeg_ts_parse_packet_inplace(&packet,
        packet_with_small_pmt.data,
        packet_with_small_pmt.size));

    MpegTsPMTBuilderSendPacketStatus_e parse_status =
        mpeg_ts_pmt_builder_try_send_packet(&builder, &packet);

    assert(parse_status == PMT_BUILDER_SEND_STATUS_TABLE_IS_ASSEMBLED);

    MpegTsPMT_t table = {0};

    bool build_table_status = mpeg_ts_pmt_builder_try_build_table(&builder, &table);

    assert(build_table_status);

    assert(table.section_length == 23);
    assert(table.CRC == 0xf64a0355);
    assert(table.program_number == 0x1);
    assert(table.version_number == 0);
    assert(table.current_next_indicator);
    assert(table.PCR_PID == 0x100);
    assert(table.program_info_length == 0);
    assert(table.program_info_data == NULL);
    assert(table.program_elements_length == 10);

    size_t stream_element_count =
        mpeg_ts_count_stream_elements(table.program_elements, table.program_elements_length);

    assert(stream_element_count == 2);

    MpegTsElementStream_t elements[2];

    bool parse_el_1_status = mpeg_ts_parse_pmt_stream_element_linked(&elements[0],
        table.program_elements,
        table.program_elements_length);
    assert(parse_el_1_status);

    size_t second_stream_element_offset =
        elements[0].es_info_length + MPEG_TS_PSI_PMT_STREAM_ELEMENT_DESCRIPTORS_OFFSET;

    bool parse_el_2_status = mpeg_ts_parse_pmt_stream_element_linked(&elements[1],
        table.program_elements + second_stream_element_offset,
        table.program_elements_length - second_stream_element_offset);
    assert(parse_el_2_status);

    assert(elements[0].stream_type ==
           ISO_IEC_13818_2_VIDEO_OR_ISO_IEC_11172_2_CONSTRAINED_PARAMETER_VIDEO_STREAM);

    assert(elements[1].stream_type == ISO_IEC_11172_3_AUDIO);

    assert(elements[0].es_info_length == 0);
    assert(elements[1].es_info_length == 0);

    assert(elements[0].es_info_descriptors_data == NULL);
    assert(elements[1].es_info_descriptors_data == NULL);

    assert(elements[0].elementary_pid == 0x100);
    assert(elements[1].elementary_pid == 0x101);
}
