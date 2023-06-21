#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <mpegts/data/psi/pmt.h>
#include <mpegts/packet/packet.h>

#define MPEG_TS_MAX_PACKETS_AMOUNT_FOR_SINGLE_PMT_TABLE 6

typedef enum
{
    PMT_BUILDER_EMPTY,
    PMT_BUILDER_TABLE_IS_BUILDING,
    PMT_BUILDER_TABLE_DONE
} MpegTsPMTAssemblerState_e;

typedef struct
{
    MpegTsPMTAssemblerState_e state;
    MpegTsPacketHeader_t last_packet_header;

    uint16_t table_length : MPEG_TS_PSI_SECTION_LENGTH_BITS;

    size_t table_data_capacity;
    uint16_t table_data_put_offset : MPEG_TS_PSI_SECTION_LENGTH_BITS;
    uint8_t *table_data;
} MpegTsPMTBuilder_t;

typedef enum
{
    PMT_BUILDER_SMALL_TABLE_IS_ASSEMBLED,
    PMT_BUILDER_TABLE_IS_ASSEMBLED,
    PMT_BUILDER_NEED_MORE_PACKETS,
    PMT_BUILDER_INVALID_PACKET_REJECTED,
    PMT_BUILDER_UNORDERED_PACKET_REJECTED,
    PMT_BUILDER_REDUDANT_PACKET_REJECTED,
    PMT_BUILDER_NOT_ENOUGHT_MEMORY,
} MpegTsPMTBuilderSendPacketStatus_e;

void mpeg_ts_pmt_builder_init(MpegTsPMTBuilder_t *builder, uint8_t *build_buffer,
    size_t buffer_size);

void mpeg_ts_pmt_builder_reset(MpegTsPMTBuilder_t *builder);

MpegTsPMTBuilderSendPacketStatus_e mpeg_ts_pmt_builder_try_send_packet(
    MpegTsPMTBuilder_t *builder, MpegTsPacketRef_t *packet);


OptionalMpegTsPMT_t mpeg_ts_pmt_builder_try_build_table(MpegTsPMTBuilder_t *builder);

