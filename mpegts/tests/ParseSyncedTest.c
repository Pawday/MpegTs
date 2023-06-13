#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>

#include "mpegts/Packet.h"
#include "mpegts/Parser.h"

static char packet_1[] = "\x47\x09\x75\x1e\xf5\xf6\xed\x93\xdc\x34\x49\x78\xa0\xe2\xa1\xe1"
                         "\x03\x30\x64\x15\x7e\xc9\x87\xfe\xce\x74\xd0\x7c\x4c\x1c\xeb\x51"
                         "\x0f\xa4\xd3\xd2\x0e\xa1\xf0\xb8\xc8\xf6\x10\x43\xf9\xc7\x06\x78"
                         "\x2a\x73\xe1\x81\x30\x43\xbf\x6a\x77\x6b\x86\xe8\x9d\x14\x66\xc7"
                         "\xac\xa1\x59\xff\x89\x48\x48\xc2\x64\xdf\x5c\xc6\xef\x95\x68\x8a"
                         "\x00\x92\xcf\x05\xa3\xc9\x57\xa0\xc3\x60\x24\x0d\xfb\xb3\x46\x13"
                         "\xff\x6c\x78\xbf\x47\x81\xe0\x87\x9e\x9c\xbc\xc3\xb2\xea\x30\x4b"
                         "\xbf\x32\x6b\x8f\xf7\xd0\xd1\x7b\x6d\x74\xce\x7d\xaa\xd1\x6d\x2d"
                         "\x8f\xf5\x14\xdf\x3b\xa3\x80\xf5\x0c\x2f\xf5\x2c\xdc\xe7\xd9\xe3"
                         "\xb7\xc8\xf0\x8b\xaf\x04\x62\xbd\xb3\x5c\xc1\xf8\x28\x0f\xbb\xd2"
                         "\x5c\xa1\xee\x2c\xee\xe8\xcc\xa7\xaa\x28\x69\x9c\x08\xf6\x80\x4e"
                         "\x49\x15\x07\xfc\xcf\x8e\x34\x54\xe0\xe0\xfb\xac";

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

    mpeg_ts_parser_send_data(&parser, packet_1, sizeof(packet_1) - 1);


    MpegTsPacketMaybe_t packet = mpeg_ts_parser_parse_packet(&parser);


    assert(packet.has_value);


    MpegTsPacketHeader_t header = packet.value.header;


    printf("PID: %"PRIx16  "\n", header.pid);
    assert(header.pid == 0x975);


    printf("CC: %"PRIu8  "\n", header.continuity_counter);
    assert(header.continuity_counter == 0xe);

}

int main(void)
{
    parse_one_packet_test();
}
