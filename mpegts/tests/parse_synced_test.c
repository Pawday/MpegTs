#include <assert.h>
#include <inttypes.h>
#include <stdio.h>

#include <mpeg/ts/packet.h>
#include <mpeg/ts/parser.h>

#include "test_data.h"

void parse_one_packet_test(void)
{
    MpegTsParser_t parser;
    uint8_t parse_buff[MPEG_TS_PACKET_SIZE * 3];

    MpegTsPacket_t parsed_packets[4];
    MpegTsPacket_t *parsed_packets_ptr_array[4];

    for (size_t index = 0; index < sizeof(parsed_packets_ptr_array) / sizeof(void *); index++) {
        parsed_packets_ptr_array[index] = &parsed_packets[0];
    }

    mpeg_ts_parser_init_ex(&parser,
        parse_buff,
        sizeof(parse_buff),
        parsed_packets_ptr_array,
        sizeof(parsed_packets_ptr_array) / sizeof(void *));

    mpeg_ts_parser_send_data(&parser, get_packet_1(), get_packet_size());

    MpegTsPacketMaybe_t packet = mpeg_ts_parser_parse_packet(&parser);

    assert(packet.has_value);

    MpegTsPacketHeader_t header = packet.value.header;

    printf("PID: %" PRIx16 "\n", header.pid);
    assert(header.pid == 0x975);

    printf("CC: %" PRIu8 "\n", header.continuity_counter);
    assert(header.continuity_counter == 0xe);
}

void parse_two_packet_test(void)
{
    MpegTsParser_t parser;
    uint8_t parse_buff[MPEG_TS_PACKET_SIZE * 3];

    MpegTsPacket_t parsed_packets[4];
    MpegTsPacket_t *parsed_packets_ptr_array[4];

    for (size_t index = 0; index < sizeof(parsed_packets_ptr_array) / sizeof(void *); index++) {
        parsed_packets_ptr_array[index] = &parsed_packets[0];
    }

    mpeg_ts_parser_init_ex(&parser,
        parse_buff,
        sizeof(parse_buff),
        parsed_packets_ptr_array,
        sizeof(parsed_packets_ptr_array) / sizeof(void *));

    mpeg_ts_parser_send_data(&parser, get_packet_1(), get_packet_size());
    mpeg_ts_parser_send_data(&parser, get_packet_2(), get_packet_size());

    MpegTsPacketMaybe_t packet_1_parsed = mpeg_ts_parser_parse_packet(&parser);

    mpeg_ts_parser_drop_packet(&parser);

    MpegTsPacketMaybe_t packet_2_parsed = mpeg_ts_parser_parse_packet(&parser);

    assert(packet_1_parsed.has_value);
    assert(packet_2_parsed.has_value);

    MpegTsPacketHeader_t header_1 = packet_1_parsed.value.header;
    MpegTsPacketHeader_t header_2 = packet_2_parsed.value.header;

    printf("PID[0]: %" PRIx16 "\n", header_1.pid);
    assert(header_1.pid == 0x975);

    printf("CC[0]: %" PRIu8 "\n", header_1.continuity_counter);
    assert(header_1.continuity_counter == 0xe);

    printf("PID[1]: %" PRIx16 "\n", header_2.pid);
    assert(header_2.pid == 0x976);

    printf("CC[1]: %" PRIu8 "\n", header_2.continuity_counter);
    assert(header_2.continuity_counter == 0x1);
}

int main(void)
{
    printf("----- Performing parse_one_packet_test -----\n");
    parse_one_packet_test();
    printf("----- Performing parse_two_packet_test -----\n");
    parse_two_packet_test();
}
