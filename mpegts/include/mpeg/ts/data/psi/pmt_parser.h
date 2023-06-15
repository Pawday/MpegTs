#pragma once

#include <mpeg/ts/data/psi/pmt.h>
#include <mpeg/ts/packet/packet.h>

MpegTsPMTMaybe_t mpeg_ts_parse_pmt_table_from_packet(MpegTsPacket_t *packet);



