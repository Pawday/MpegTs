#include "test_data.h"

static const char single_raw_packet[] =
    "\x47\x08\xad\x16\x43\xa3\x4a\x5b\x69\xd6\x4a\xd0\xfb\x5b\x7a\x00"
    "\x02\xe0\x06\x5f\x4f\x77\x44\x9d\x57\xe8\x6b\x7a\x00\x06\x5f\x6f"
    "\x8f\xbe\x53\x27\x06\xd0\xd6\xf1\x80\x87\xfe\xd7\x1b\xbf\xc0\x21"
    "\x7f\xa0\x1e\xd8\x88\xe3\x3a\xa5\x97\x6b\x5f\x3b\x74\x21\xb7\xac"
    "\xfe\x08\xeb\xbc\xa5\x3a\x5a\xda\x3e\x9c\x75\xb0\x04\x91\x5b\x8d"
    "\x90\x00\xfe\x52\x4b\x69\x31\x5b\x98\x40\x8c\x1a\x96\xd6\x62\x76"
    "\xa1\x67\x46\xa2\xf4\xfe\xdc\x0a\xa0\x8a\x00\x91\x5a\x8f\xe0\x81"
    "\x42\xb6\xa9\x3a\x28\x63\x67\xd7\x1a\xac\xbb\x09\xca\x48\x6a\xf6"
    "\xf8\x70\x4e\xb1\xf3\xb6\xd2\x86\x35\x1b\xcb\x80\x13\x00\x3e\x23"
    "\x02\x00\x1a\x80\x35\xff\x80\x56\xfe\x0d\x7c\x91\x3a\x78\x8e\x9f"
    "\xb4\xb1\xbb\x2e\x99\x87\x06\xb7\xcf\x01\x00\x0e\x80\x0a\x00\x1d"
    "\x3a\x96\x40\x7f\x7c\xb7\x47\xe9\x01\x04\x10\xc5\x47";

static const char two_sequantial_packets[] =
    "\x47\x08\xad\x17\x00\x36\xf9\xa3\x68\xdd\x23\xf7\x60\xc3\x9c\x6f"
    "\x06\x43\xc7\x9e\xde\x2c\x98\xdd\xf8\xe8\x26\x3e\x43\xc2\x5b\x0d"
    "\xb7\x83\x14\x84\xdb\xb6\x08\x00\x7a\x00\x48\x00\xe0\x0c\x50\x0a"
    "\x1e\x2f\x91\xf5\xb1\x80\x26\x00\x74\x46\x00\x78\x00\xd3\x8e\xbe"
    "\x82\x1d\x77\x6e\xdc\x99\xbb\x77\x6a\xd5\x3e\x95\x1b\x8c\x37\x82"
    "\x4b\x69\x6d\x60\x76\x5b\x23\x27\x64\xef\xb7\x39\xf4\x71\x59\x69"
    "\x67\xb5\x11\xfe\x82\x38\x28\x98\xb5\x87\xd8\x47\x05\x13\x16\xb5"
    "\x67\x30\xad\x22\xf9\xd0\x88\xdd\x40\x0a\x80\x1f\x08\x90\x66\xbf"
    "\x5b\x5b\xf4\xf6\xe0\x07\x8b\x5a\x7d\x04\x71\x40\x85\xad\x51\xca"
    "\xc1\x81\x3c\xea\x8e\x8c\x00\x2a\x00\x7d\xed\x21\x49\xbc\xeb\x6b"
    "\x7e\xde\xdc\x14\x72\xd6\x9f\x6f\x6e\x00\x78\xb5\xaa\xa5\xab\x18"
    "\x7d\xac\xc4\xf7\xa1\x12\x30\x00\xa8\x01\xe7\xb0\x47\x08\xab\x18"
    "\x66\xbf\x5b\x59\xf6\xf6\xe2\xc1\x8b\x5a\x7d\xbd\xb8\x01\xe2\xd6"
    "\xa8\x35\x81\xd5\x81\x3c\xe8\x44\x60\x01\x50\x03\xef\x65\xb4\xad"
    "\xe7\x5b\x5b\xf6\xf6\xe2\x0e\x5a\xcb\xfe\xf8\xe8\x07\x96\xd6\xae"
    "\x0d\x02\x98\x32\x13\xce\x84\x48\xc8\x01\x40\x03\xcf\x60\xc7\xf3"
    "\xad\xad\xff\x7c\x75\x14\x98\xb5\x8f\xf3\xe3\x8a\xa4\xc5\xad\x4d"
    "\x63\x4b\x8c\x09\xe7\x42\x24\x60\x01\x60\x03\xef\x64\x18\xfe\x75"
    "\xb5\xbf\xcf\x8e\xa8\x10\xb5\xa7\xd8\x8d\xd0\x0f\x2d\xad\x55\x2d"
    "\x1a\x00\xcd\x81\x3c\xe8\x44\x8c\x00\x2a\x00\x79\xec\x19\xbc\xeb"
    "\x6b\x7f\x9f\x1d\x00\xf1\x6b\x4f\xe7\xc7\x49\xfa\xb6\xb5\x27\xc6"
    "\x34\x6b\x02\x79\xd0\x89\x18\x00\x50\x00\xf3\xd8\xa3\xb7\x9d\x08"
    "\x6d\x6f\xf0\x8d\xd0\x0f\x16\xb0\xfe\x11\xb8\xb2\x50\xe5\xad\x53"
    "\x10\x09\xfc\x89\x20\x73\x02\x79\x47";

IOVec test_data_get_raw_packet(void)
{
    const IOVec ret = {.data = (const uint8_t *)single_raw_packet,
        .size = sizeof(single_raw_packet)};
    return ret;
}

IOVec test_data_get_two_packets(void)
{
    const IOVec ret = {.data = (const uint8_t *)two_sequantial_packets,
        .size = sizeof(two_sequantial_packets)};
    return ret;
}
