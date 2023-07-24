#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <mpegts/pmt_builder.h>
#include <mpegts/pmt_dumper.h>

#define MAX_PACKETS_TO_HANDLE 8196

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "USAGE:\n\n");
        fprintf(stderr, "%s file.ts\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *file_name = argv[1];
    fprintf(stderr, "[INFO]: opening file %s\n", file_name);
    FILE *ts_file = fopen(file_name, "rb");
    if (ts_file == NULL) {
        fprintf(stderr, "[ERROR]: Can not read file %s\n", file_name);
        return EXIT_FAILURE;
    }

    fseek(ts_file, 0L, SEEK_END);
    size_t file_size = ftell(ts_file);
    fseek(ts_file, 0L, SEEK_SET);

    uint8_t *file_data = malloc(file_size);

    if (file_data == NULL) {
        fprintf(stderr, "[ERROR]: No memory\n");
        fclose(ts_file);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "[INFO]: reading file %s\n", file_name);
    size_t file_bytes_readen = fread(file_data, 1, file_size, ts_file);
    if (file_bytes_readen != file_size) {
        fprintf(stderr,
            "[ERROR]: readen %" PRIuPTR " of %" PRIuPTR " bytes from %s file\n",
            file_bytes_readen,
            file_size,
            file_name);
        fclose(ts_file);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "[INFO]: readen %" PRIuPTR " bytes from %s\n", file_bytes_readen, file_name);

    size_t max_packets_in_file = file_size / MPEG_TS_PACKET_SIZE + 1;

    MpegTsPacket_t *packets = malloc(max_packets_in_file * sizeof(MpegTsPacket_t));

    size_t packets_am = mpeg_ts_search_packets(file_data, file_size, packets, max_packets_in_file);

    fprintf(stderr, "[INFO]: Found %" PRIuPTR " packets in %s file\n", packets_am, file_name);

    uint8_t builder_buff[MPEG_TS_PSI_PMT_SECTION_MAX_LENGTH];
    MpegTsPMTBuilder_t builder = {0};
    mpeg_ts_pmt_builder_init(&builder, builder_buff, sizeof(builder_buff));

    for (size_t i = 0; i < packets_am; i++) {
        fprintf(stderr, "[INFO]: Scanning packet #%" PRIuPTR "\n", i + 1);
        switch (mpeg_ts_pmt_builder_try_send_packet(&builder, packets[i])) {
        case PMT_BUILDER_SEND_STATUS_NEED_MORE_PACKETS:
            continue;
        case PMT_BUILDER_SEND_STATUS_INVALID_PACKET_REJECTED:
        case PMT_BUILDER_SEND_STATUS_UNORDERED_PACKET_REJECTED:
        case PMT_BUILDER_SEND_STATUS_NOT_ENOUGHT_MEMORY:
            mpeg_ts_pmt_builder_reset(&builder);
            continue;
        case PMT_BUILDER_SEND_STATUS_REDUNDANT_PACKET_REJECTED:
        case PMT_BUILDER_SEND_STATUS_TABLE_IS_ASSEMBLED:
            break;
        }
        MpegTsPMT_t pmt;
        if (!mpeg_ts_pmt_builder_try_build_table(&builder, &pmt)) {
            mpeg_ts_pmt_builder_reset(&builder);
            continue;
        }
        mpeg_ts_dump_pmt_to_stream(&pmt, stdout);
        break;
    }

    free(packets);
    free(file_data);
    fclose(ts_file);
    return EXIT_SUCCESS;
}
