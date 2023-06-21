#pragma once

#define MPEG_TS_PACKET_SIZE 188
#define MPEG_TS_SYNC_BYTE   0x47

#define MPEG_TS_PACKET_HEADER_SIZE            4
#define MPEG_TS_PACKET_PAYLOAD_SIZE           (MPEG_TS_PACKET_SIZE - MPEG_TS_PACKET_HEADER_SIZE)
#define MPEG_TS_PID_FIELD_SIZE_BITS           13
#define MPEG_TS_SCRAMBLING_CONTROL_SIZE_BITS  2
#define MPEG_TS_ADAPT_FIELD_CONTROL_SIZE_BITS 2
#define MPEG_TS_CONTINUITY_COUNTER_SIZE_BITS  4

#define MPEG_TS_HEADER_FLAGS_ERR_BIT                          (1 << 7)
#define MPEG_TS_HEADER_FLAGS_PAYLOAD_UNIT_START_INDICATOR_BIT (1 << 6)
#define MPEG_TS_HEADER_FLAGS_TRANSPORT_PRIORITY_BIT           (1 << 5)

#define MPEG_TS_HEADER_FLAGS_MASK                                                                  \
    (MPEG_TS_HEADER_FLAGS_ERR_BIT | MPEG_TS_HEADER_FLAGS_PAYLOAD_UNIT_START_INDICATOR_BIT |        \
        MPEG_TS_HEADER_FLAGS_TRANSPORT_PRIORITY_BIT)

#define MPEG_TS_HEADER_FLAGS_SCRAMBLING_CONTROL_MASK  (3 << 6)
#define MPEG_TS_HEADER_FLAGS_ADAPT_FIELD_CONTROL_MASK (3 << 4)
#define MPEG_TS_HEADER_FLAGS_CONTINUITY_COUNTER_MASK  0xff

#define MPEG_TS_ADAPT_CONTROL_RESERVED     0x0
#define MPEG_TS_ADAPT_CONTROL_PAYLOAD_ONLY 0x1
#define MPEG_TS_ADAPT_CONTROL_ONLY         0x2
#define MPEG_TS_ADAPT_CONTROL_WITH_PAYLOAD 0x3

#define MPEG_TS_NULL_PACKET_PID 0x1fff

