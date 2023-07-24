#include <memory.h>

#include <mpegts/packet.h>
#include <mpegts/packet_header.h>

#include "runtime_assert.h"
#include "test_data.h"

static void parse_single_packet_test(void)
{
    IOVec data_block = test_data_get_raw_packet();

    MpegTsPacket_t packet;
    bool packet_search_status = mpeg_ts_search_packet(&packet, data_block.data, data_block.size);
    assert(packet_search_status);

    uint8_t *payload = mpeg_ts_packet_get_payload(packet);

    MpegTsPacketHeader_t header = {0};
    mpeg_ts_parse_packet_header(&header, packet);

    assert(header.error_indicator == false);
    assert(header.payload_unit_start_indicator == false);
    assert(header.transport_priority == false);
    assert(header.pid == 0x08ad);
    assert(header.scrambling_control == 0);
    assert(header.adaptation_field_control == ADAPTATION_FIELD_PAYLOAD_ONLY);

    assert(payload == data_block.data + MPEG_TS_PACKET_HEADER_SIZE);
}

static void parse_two_merged_packets_test(void)
{
    IOVec data_block = test_data_get_two_packets();

    MpegTsPacket_t packets_buffer[5];

    memset(packets_buffer, 0, sizeof(packets_buffer));

    size_t parsed_packets_amount =
        mpeg_ts_search_packets(data_block.data, data_block.size, packets_buffer, 5);

    assert(parsed_packets_amount == 2);

    MpegTsPacketHeader_t header_0 = {0};
    MpegTsPacketHeader_t header_1 = {0};
    bool parse_header_0_status = mpeg_ts_parse_packet_header(&header_0, packets_buffer[0]);
    bool parse_header_1_status = mpeg_ts_parse_packet_header(&header_1, packets_buffer[1]);

    assert(parse_header_0_status);
    assert(parse_header_1_status);

    assert(header_0.pid == 0x8ad);
    assert(header_1.pid == 0x8ab);
    assert(packets_buffer[2] == NULL);
}

int main(void)
{
    parse_single_packet_test();
    parse_two_merged_packets_test();

    return 0;
}
