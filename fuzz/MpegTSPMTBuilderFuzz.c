#include <stdlib.h>

#include <mpegts/pmt_builder.h>

static uint8_t pmt_build_buffer[MPEG_TS_PSI_PMT_SECTION_MAX_LENGTH];
static MpegTsPacket_t *packets = NULL;
static size_t packets_amount = 0;

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    MpegTsPMTBuilder_t pmt_builder;
    mpeg_ts_pmt_builder_init(&pmt_builder, pmt_build_buffer, sizeof(pmt_build_buffer));

    size_t input_packets_amount = Size / MPEG_TS_PACKET_SIZE + 1;

    if (input_packets_amount > packets_amount) {
        MpegTsPacket_t *new_packets_location  = realloc(packets, input_packets_amount * sizeof(MpegTsPacket_t));
        if (new_packets_location == NULL){
            return -1;
        }

        packets_amount = input_packets_amount;
        packets = new_packets_location;
    }

    size_t packets_found = mpeg_ts_search_packets(Data, Size, packets, packets_amount);

    MpegTsPMT_t output_table;

    for (size_t packet_index = 0; packet_index < packets_found; packet_index++)
    {
        switch(mpeg_ts_pmt_builder_try_send_packet(&pmt_builder,packets[packet_index])){

        case PMT_BUILDER_SEND_STATUS_TABLE_IS_ASSEMBLED:
        mpeg_ts_pmt_builder_try_build_table(&pmt_builder, &output_table);
        mpeg_ts_pmt_builder_reset(&pmt_builder);
        continue;
        case PMT_BUILDER_SEND_STATUS_NEED_MORE_PACKETS:
        continue;
        case PMT_BUILDER_SEND_STATUS_INVALID_PACKET_REJECTED:
        case PMT_BUILDER_SEND_STATUS_UNORDERED_PACKET_REJECTED:
        mpeg_ts_pmt_builder_reset(&pmt_builder);
        continue;
        case PMT_BUILDER_SEND_STATUS_REDUNDANT_PACKET_REJECTED:
        mpeg_ts_pmt_builder_try_build_table(&pmt_builder, &output_table);
        mpeg_ts_pmt_builder_reset(&pmt_builder);
        continue;
        case PMT_BUILDER_SEND_STATUS_NOT_ENOUGHT_MEMORY:
        return -1;
          break;
        }
    }


    return 0;
}
