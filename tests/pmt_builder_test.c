#include "mpegts/psi_magics.h"
#include "test_data.h"
#include <assert.h>

#include <mpegts/packet_inplace_parser.h>
#include <mpegts/pmt_builder.h>
#include <mpegts/pmt_stream_element.h>

static void test_pmt_from_single_packet(void)
{
    uint8_t builder_buff[PACKET_WITH_SMALL_PMT_SECTION_LENGTH];
    MpegTsPMTBuilder_t builder = {0};
    mpeg_ts_pmt_builder_init(&builder, builder_buff, sizeof(builder_buff));
    assert(builder.state == PMT_BUILDER_STATE_EMPTY);

    IOVec packet_with_small_pmt = test_data_get_packet_with_pmt();
    MpegTsPacket_t packet = {0};

    bool packet_parse_status = mpeg_ts_parse_packet_inplace(&packet,
        packet_with_small_pmt.data,
        packet_with_small_pmt.size);

    assert(packet_parse_status);

    MpegTsPMTBuilderSendPacketStatus_e send_status =
        mpeg_ts_pmt_builder_try_send_packet(&builder, &packet);

    assert(send_status == PMT_BUILDER_SEND_STATUS_TABLE_IS_ASSEMBLED);

    MpegTsPMT_t table = {0};

    bool build_table_status = mpeg_ts_pmt_builder_try_build_table(&builder, &table);

    assert(build_table_status);

    assert(table.section_length ==
           PACKET_WITH_SMALL_PMT_SECTION_LENGTH - MPEG_TS_PSI_PMT_SECTION_LENGTH_OFFSET);
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

static void test_pmt_from_2_packets(void)
{
    uint8_t builder_buff[MPEG_TS_PSI_PMT_SECTION_MAX_LENGTH];
    MpegTsPMTBuilder_t builder = {0};
    mpeg_ts_pmt_builder_init(&builder, builder_buff, sizeof(builder_buff));
    assert(builder.state == PMT_BUILDER_STATE_EMPTY);

    IOVec packet_data_1 = test_data_get_packet_with_pmt_part_1();
    MpegTsPacket_t packet_1 = {0};
    bool packet_1_parse_status =
        mpeg_ts_parse_packet_inplace(&packet_1, packet_data_1.data, packet_data_1.size);
    assert(packet_1_parse_status);

    IOVec packet_data_2 = test_data_get_packet_with_pmt_part_2();
    MpegTsPacket_t packet_2 = {0};
    bool packet_2_parse_status =
        mpeg_ts_parse_packet_inplace(&packet_2, packet_data_2.data, packet_data_2.size);
    assert(packet_2_parse_status);

    MpegTsPMTBuilderSendPacketStatus_e first_send_status =
        mpeg_ts_pmt_builder_try_send_packet(&builder, &packet_1);
    assert(first_send_status == PMT_BUILDER_SEND_STATUS_NEED_MORE_PACKETS);

    MpegTsPMTBuilderSendPacketStatus_e second_send_status =
        mpeg_ts_pmt_builder_try_send_packet(&builder, &packet_2);
    assert(second_send_status == PMT_BUILDER_SEND_STATUS_TABLE_IS_ASSEMBLED);

    MpegTsPMT_t table = {0};
    bool builde_table_status = mpeg_ts_pmt_builder_try_build_table(&builder, &table);
    assert(builde_table_status);

    assert(table.section_length == 232);
    assert(table.program_number == 8211);
    assert(table.version_number == 1);
    assert(table.current_next_indicator);
    assert(table.PCR_PID == 2221);
    assert(table.program_info_length == 0);
    assert(table.program_info_data == NULL);
    assert(table.program_elements_length == 219);
}

static void test_pmt_builder_messup_packets(void)
{
    uint8_t builder_buff[MPEG_TS_PSI_PMT_SECTION_MAX_LENGTH];
    MpegTsPMTBuilder_t builder = {0};
    mpeg_ts_pmt_builder_init(&builder, builder_buff, sizeof(builder_buff));
    assert(builder.state == PMT_BUILDER_STATE_EMPTY);

    IOVec packet_data_1 = test_data_get_packet_with_pmt_part_1();
    MpegTsPacket_t packet_1 = {0};
    bool packet_1_parse_status =
        mpeg_ts_parse_packet_inplace(&packet_1, packet_data_1.data, packet_data_1.size);
    assert(packet_1_parse_status);

    IOVec packet_data_2 = test_data_get_packet_with_pmt_part_2();
    MpegTsPacket_t packet_2 = {0};
    bool packet_2_parse_status =
        mpeg_ts_parse_packet_inplace(&packet_2, packet_data_2.data, packet_data_2.size);
    assert(packet_2_parse_status);

    IOVec unrelated_packet_data = test_data_get_raw_packet();
    MpegTsPacket_t unrelated_packet = {0};
    bool unrelated_packet_parse_status = mpeg_ts_parse_packet_inplace(&unrelated_packet,
        unrelated_packet_data.data,
        unrelated_packet_data.size);
    assert(unrelated_packet_parse_status);

    MpegTsPMTBuilderSendPacketStatus_e unrelated_send_status_init =
        mpeg_ts_pmt_builder_try_send_packet(&builder, &unrelated_packet);
    assert(unrelated_send_status_init == PMT_BUILDER_SEND_STATUS_INVALID_PACKET_REJECTED);

    MpegTsPMTBuilderSendPacketStatus_e first_send_status =
        mpeg_ts_pmt_builder_try_send_packet(&builder, &packet_1);
    assert(first_send_status == PMT_BUILDER_SEND_STATUS_NEED_MORE_PACKETS);

    MpegTsPMTBuilderSendPacketStatus_e unrelated_send_status_while_assembling =
        mpeg_ts_pmt_builder_try_send_packet(&builder, &unrelated_packet);
    assert(
        unrelated_send_status_while_assembling == PMT_BUILDER_SEND_STATUS_INVALID_PACKET_REJECTED);

    MpegTsPMTBuilderSendPacketStatus_e second_send_status =
        mpeg_ts_pmt_builder_try_send_packet(&builder, &packet_2);
    assert(second_send_status == PMT_BUILDER_SEND_STATUS_TABLE_IS_ASSEMBLED);

    MpegTsPMTBuilderSendPacketStatus_e redundant_send_status =
        mpeg_ts_pmt_builder_try_send_packet(&builder, &packet_2);
    assert(redundant_send_status == PMT_BUILDER_SEND_STATUS_REDUNDANT_PACKET_REJECTED);

    MpegTsPMT_t table = {0};
    bool builde_table_status = mpeg_ts_pmt_builder_try_build_table(&builder, &table);
    assert(builde_table_status);

    assert(table.section_length == 232);
    assert(table.program_number == 8211);
    assert(table.version_number == 1);
    assert(table.current_next_indicator);
    assert(table.PCR_PID == 2221);
    assert(table.program_info_length == 0);
    assert(table.program_info_data == NULL);
    assert(table.program_elements_length == 219);
}

static void test_pmt_low_memory_report(void)
{
    uint8_t builder_buff[PACKET_WITH_SMALL_PMT_SECTION_LENGTH - 1];
    MpegTsPMTBuilder_t builder = {0};
    mpeg_ts_pmt_builder_init(&builder, builder_buff, sizeof(builder_buff));
    assert(builder.state == PMT_BUILDER_STATE_EMPTY);

    IOVec packet_with_small_pmt = test_data_get_packet_with_pmt();
    MpegTsPacket_t packet = {0};

    bool packet_parse_status = mpeg_ts_parse_packet_inplace(&packet,
        packet_with_small_pmt.data,
        packet_with_small_pmt.size);

    assert(packet_parse_status);

    MpegTsPMTBuilderSendPacketStatus_e send_status =
        mpeg_ts_pmt_builder_try_send_packet(&builder, &packet);

    assert(send_status == PMT_BUILDER_SEND_STATUS_NOT_ENOUGHT_MEMORY);
}

int main(void)
{
    test_pmt_from_single_packet();
    test_pmt_from_2_packets();
    test_pmt_builder_messup_packets();
    test_pmt_low_memory_report();
}
