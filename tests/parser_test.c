#include <assert.h>
#include <memory.h>
#include <stdio.h>

#include <mpegts/packet_inplace_parser.h>

#include "test_data.h"

static void parse_single_packet_test(void)
{
    IOVec data_block = test_data_get_raw_packet();

    MpegTsPacket_t packet;

    bool packet_parse_status =
        mpeg_ts_parse_packet_inplace(&packet, data_block.data, data_block.size);

    assert(packet_parse_status);

    assert(packet.header.error_indicator == false);
    assert(packet.header.payload_unit_start_indicator == false);
    assert(packet.header.transport_priority == false);
    assert(packet.header.pid == 0x08ad);
    assert(packet.header.scrambling_control == 0);
    assert(packet.header.adaptation_field_control == ADAPTATION_FIELD_PAYLOAD_ONLY);

    assert(packet.payload == data_block.data + MPEG_TS_PACKET_HEADER_SIZE);
}

static void parse_two_merged_packets_test(void)
{
    IOVec data_block = test_data_get_two_packets();

    MpegTsPacket_t packets_buffer[5];

    memset(packets_buffer, 0, sizeof(packets_buffer));

    size_t parsed_packets_amount =
        mpeg_ts_parse_packets_inplace(data_block.data, data_block.size, packets_buffer, 5);

    assert(parsed_packets_amount == 2);

    assert(packets_buffer[0].header.pid == 0x8ad);
    assert(packets_buffer[1].header.pid == 0x8ab);
    assert(packets_buffer[2].payload == NULL);
}

int main()
{
    parse_single_packet_test();
    parse_two_merged_packets_test();

    return 0;
}
