#pragma once

#include <mpegts/data/psi/pmt.h>
#include <mpegts/packet/packet.h>

MpegTsPMTMaybe_t mpeg_ts_parse_pmt_from_packet(MpegTsPacket_t *packet);

