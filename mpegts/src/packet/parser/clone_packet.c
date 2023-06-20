#include <mpegts/packet/packet.h>

#include <memory.h>

MpegTsPacket_t mpeg_ts_packet_clone(MpegTsPacketRef_t *ref)
{
    MpegTsPacket_t ret;

    ret.header = ref->header;
    memcpy(ret.data, ref->data, MPEG_TS_PACKET_SIZE - MPEG_TS_PACKET_HEADER_SIZE);

    return ret;
}
