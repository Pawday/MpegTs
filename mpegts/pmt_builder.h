#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include "packet.h"
#include "pmt.h"

#define MPEG_TS_MAX_PACKETS_AMOUNT_FOR_SINGLE_PMT_TABLE 6

typedef enum
{
    PMT_BUILDER_STATE_EMPTY,
    PMT_BUILDER_STATE_TABLE_IS_BUILDING,
    PMT_BUILDER_STATE_TABLE_ASSEMBLED
} MpegTsPMTBuilderState_e;

typedef enum
{
    PMT_BUILDER_SEND_STATUS_SMALL_TABLE_IS_ASSEMBLED,
    PMT_BUILDER_SEND_STATUS_TABLE_IS_ASSEMBLED,
    PMT_BUILDER_SEND_STATUS_NEED_MORE_PACKETS,
    PMT_BUILDER_SEND_STATUS_INVALID_PACKET_REJECTED,
    PMT_BUILDER_SEND_STATUS_UNORDERED_PACKET_REJECTED,
    PMT_BUILDER_SEND_STATUS_REDUDANT_PACKET_REJECTED,
    PMT_BUILDER_SEND_STATUS_NOT_ENOUGHT_MEMORY,
} MpegTsPMTBuilderSendPacketStatus_e;

typedef struct
{
    MpegTsPMTBuilderState_e state;
    MpegTsPacketHeader_t last_packet_header;

    uint16_t table_length : MPEG_TS_PSI_SECTION_LENGTH_BITS;

    size_t table_data_capacity;
    uint16_t table_data_put_offset : MPEG_TS_PSI_SECTION_LENGTH_BITS;
    uint8_t *table_data;
} MpegTsPMTBuilder_t;

void mpeg_ts_pmt_builder_init(MpegTsPMTBuilder_t *builder, uint8_t *build_buffer,
    size_t buffer_size);

void mpeg_ts_pmt_builder_reset(MpegTsPMTBuilder_t *builder);

MpegTsPMTBuilderSendPacketStatus_e mpeg_ts_pmt_builder_try_send_packet(MpegTsPMTBuilder_t *builder,
    MpegTsPacket_t *packet);

/**
 * all pointers in "output_table" will lead to memory inside the "builder" struct
 */
bool mpeg_ts_pmt_builder_try_build_table(MpegTsPMTBuilder_t *builder, MpegTsPMT_t *output_table);

