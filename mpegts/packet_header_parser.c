#include <limits.h>
#include <memory.h>


#include "packet_header.h"


OptionalMpegTsPacketHeader_t mpeg_ts_parse_packet_header(const uint8_t *buffer, size_t buffer_size)
{
    const OptionalMpegTsPacketHeader_t bad_value = {.has_balue = false, .value = {0}};

    if (buffer_size < MPEG_TS_PACKET_HEADER_SIZE) {
        return bad_value;
    }

    if (buffer[0] != MPEG_TS_SYNC_BYTE) {
        return bad_value;
    }

    // 0b000_11111
    //   ^^^ ^^^^^
    //    |    |
    //    |    |- pid's first 5 bits
    //    |
    //    |--- flags
    uint8_t flags_and_pid5 = buffer[1];

    // 0b000_11111
    //   ^^^ select this
    uint8_t flags_only = flags_and_pid5 & MPEG_TS_HEADER_FLAGS_MASK;

    OptionalMpegTsPacketHeader_t ret_val;

    ret_val.value.error_indicator = (flags_only & MPEG_TS_HEADER_FLAGS_ERR_BIT) != 0;

    if (ret_val.value.error_indicator) {
        return bad_value;
    }

    ret_val.value.payload_unit_start_indicator =
        (flags_only & MPEG_TS_HEADER_FLAGS_PAYLOAD_UNIT_START_INDICATOR_BIT) != 0;

    ret_val.value.transport_priority =
        (flags_only & MPEG_TS_HEADER_FLAGS_TRANSPORT_PRIORITY_BIT) != 0;

    // 0b000_11111
    //       ^^^^^ select this
    uint8_t pid5_only = flags_and_pid5 & ~MPEG_TS_HEADER_FLAGS_MASK;

    ret_val.value.pid = 0;

    // ret_val.value.pid:                0b0000000000000
    // pid5_only:                          |  0b00011111
    // pid5_only:                     0b00011111<<<<<<<<

    // ret_val.value.pid:                0b1111100000000
    ret_val.value.pid |= (pid5_only << (MPEG_TS_PID_FIELD_SIZE_BITS - 5));

    uint8_t pid_remainder = buffer[2];

    ret_val.value.pid |= pid_remainder;

    ret_val.value.scrambling_control = 0;
    ret_val.value.adaptation_field_control = 0;

    ret_val.value.scrambling_control |=
        (buffer[3] & MPEG_TS_HEADER_FLAGS_SCRAMBLING_CONTROL_MASK) >>
        (CHAR_BIT - MPEG_TS_SCRAMBLING_CONTROL_SIZE_BITS);

    ret_val.value.adaptation_field_control |=
        (buffer[3] & MPEG_TS_HEADER_FLAGS_ADAPT_FIELD_CONTROL_MASK) >>
        (CHAR_BIT - MPEG_TS_SCRAMBLING_CONTROL_SIZE_BITS - MPEG_TS_ADAPT_FIELD_CONTROL_SIZE_BITS);

    ret_val.value.continuity_counter = buffer[3] & 0xf;

    ret_val.has_balue = true;
    return ret_val;
}

