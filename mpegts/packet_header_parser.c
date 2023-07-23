#include <limits.h>
#include <memory.h>

#include "packet_header.h"
#include "packet_magics.h"

bool mpeg_ts_parse_packet_header(MpegTsPacketHeader_t *output_header, MpegTsPacket_t packet)
{
    uint8_t *packet_location = (uint8_t *)packet;
    if (packet_location[0] != MPEG_TS_SYNC_BYTE) {
        return false;
    }

    // 0b000_11111
    //   ^^^ ^^^^^
    //    |    |
    //    |    |- pid's first 5 bits
    //    |
    //    |--- flags
    uint8_t flags_and_pid5 = packet_location[1];

    // 0b000_11111
    //   ^^^ select this
    uint8_t flags_only = flags_and_pid5 & MPEG_TS_HEADER_FLAGS_MASK;

    bool error_indicator = (flags_only & MPEG_TS_HEADER_FLAGS_ERR_BIT) != 0;
    if (error_indicator) {
        return false;
    }

    MpegTsPacketHeader_t return_header = {0};

    return_header.payload_unit_start_indicator =
        (flags_only & MPEG_TS_HEADER_FLAGS_PAYLOAD_UNIT_START_INDICATOR_BIT) != 0;

    return_header.transport_priority =
        (flags_only & MPEG_TS_HEADER_FLAGS_TRANSPORT_PRIORITY_BIT) != 0;

    // 0b000_11111
    //       ^^^^^ select this
    uint8_t pid5_only = flags_and_pid5 & ~MPEG_TS_HEADER_FLAGS_MASK;

    return_header.pid = 0;

    // ret_val.value.pid:                0b0000000000000
    // pid5_only:                          |  0b00011111
    // pid5_only:                     0b00011111<<<<<<<<

    // ret_val.value.pid:                0b1111100000000
    return_header.pid |= (pid5_only << (MPEG_TS_PID_FIELD_SIZE_BITS - 5));

    uint8_t pid_remainder = packet_location[2];

    return_header.pid |= pid_remainder;

    return_header.scrambling_control = 0;

    return_header.scrambling_control |=
        (packet_location[3] & MPEG_TS_HEADER_FLAGS_SCRAMBLING_CONTROL_MASK) >>
        (CHAR_BIT - MPEG_TS_SCRAMBLING_CONTROL_SIZE_BITS);

    uint8_t adaptation_field_control_num =
        (packet_location[3] & MPEG_TS_HEADER_FLAGS_ADAPT_FIELD_CONTROL_MASK) >>
        (8 - MPEG_TS_SCRAMBLING_CONTROL_SIZE_BITS - MPEG_TS_ADAPT_FIELD_CONTROL_SIZE_BITS);

    switch (adaptation_field_control_num) {
    case 0x0:
        return_header.adaptation_field_control = ADAPTATION_FIELD_RESERVED;
        break;
    case 0x1:
        return_header.adaptation_field_control = ADAPTATION_FIELD_PAYLOAD_ONLY;
        break;
    case 0x2:
        return_header.adaptation_field_control = ADAPTATION_FIELD_ONLY;
        break;
    case 0x3:
        return_header.adaptation_field_control = ADAPTATION_FIELD_AND_PAYLOAD;
        break;
    default:
        return false;
    }

    return_header.continuity_counter = packet_location[3] & 0xf;

    if (output_header != NULL) {
        *output_header = return_header;
    }

    return true;
}

