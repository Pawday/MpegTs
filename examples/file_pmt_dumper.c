#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <mpegts/packet_parser.h>
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
    FILE *ts_file = fopen(file_name, "r");
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
        return EXIT_FAILURE;
    }

    fread(file_data, 1, file_size, ts_file);

    MpegTsPacket_t packets[MAX_PACKETS_TO_HANDLE] = {0};

    size_t packets_am = mpeg_ts_parse_packets(file_data, file_size, packets, MAX_PACKETS_TO_HANDLE);

    fprintf(stderr, "Found %" PRIuPTR " packets in %s file\n", packets_am, file_name);

    uint8_t builder_buff[MPEG_TS_PSI_PMT_SECTION_MAX_LENGTH];
    MpegTsPMTBuilder_t builder = {0};
    mpeg_ts_pmt_builder_init(&builder, builder_buff, sizeof(builder_buff));

    for (size_t i = 0; i < packets_am; i++) {
        MpegTsPacket_t packet = packets[i];
        mpeg_ts_pmt_builder_try_send_packet(&builder, &packet);
        MpegTsPMT_t pmt;
        if (!mpeg_ts_pmt_builder_try_build_table(&builder, &pmt)) {
            continue;
        }
        mpeg_ts_dump_pmt_to_stream(&pmt, stdout);
        mpeg_ts_pmt_builder_reset(&builder);
    }

    free(file_data);
    return EXIT_SUCCESS;
}
