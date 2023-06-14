#pragma once

#include "mpeg/ts/packet.h"
#include "section.h"

MpegTsPSISectionMaybe_t parse_with_link_to_packet(MpegTsPacket_t *packet);
