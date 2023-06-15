#include "mpeg/ts/data/psi/psi_magics.h"
#include <arpa/inet.h>
#include <assert.h>
#include <bits/types/struct_iovec.h>
#include <errno.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <sched.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <unistd.h>

#include <net/MulticastSocket.h>

#include <mpeg/ts/mpeg_ts_magics.h>
#include <mpeg/ts/parser.h>

// #define MULTICAST_GROUP_CSTR "239.0.0.10"
#define MULTICAST_GROUP_CSTR               "239.255.2.114" // Местный ТНТ
#define MULTICAST_GROUP_PORT               (uint16_t)1234
#define TIMEOUT_SCHED_SWITCH_REQUEST_BOUND (uint8_t)20
#define RECV_TIMOUT_SECONDS                2
#define TRANSFER_BUFFER_SIZE               2048

bool process_terminate_requested = false;

// Stolen from https://gist.github.com/ccbrown/9722406
static void DumpHex(const void *data, size_t size)
{
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';
    for (i = 0; i < size; ++i) {
        printf("%02X ", ((unsigned char *)data)[i]);
        if (((unsigned char *)data)[i] >= ' ' && ((unsigned char *)data)[i] <= '~') {
            ascii[i % 16] = ((unsigned char *)data)[i];
        } else {
            ascii[i % 16] = '.';
        }
        if ((i + 1) % 8 == 0 || i + 1 == size) {
            printf(" ");
            if ((i + 1) % 16 == 0) {
                printf("|  %s \n", ascii);
            } else if (i + 1 == size) {
                ascii[(i + 1) % 16] = '\0';
                if ((i + 1) % 16 <= 8) {
                    printf(" ");
                }
                for (j = (i + 1) % 16; j < 16; ++j) {
                    printf("   ");
                }
                printf("|  %s \n", ascii);
            }
        }
    }
}

typedef enum EndServiceStatus_e
{
    END_SERVICE_OK,
    END_SERVICE_SIGTERM,
    END_SERVICE_FAIL_SETUP,
    END_SERVICE_INTERNAL_ERROR,
    END_SERVICE_UNKNOWN_ERROR,
} EndServiceStatus_e;

typedef enum PerformParseStatus_e
{
    PARSE_OK,
    PARSE_NO_DATA,
    PARSE_NET_TIMEOUT,
    PARSE_NET_ERROR,
    PARSE_NO_MEM_ERROR,
    PARSE_DATA_FORMAT_ERROR,
} PerformParseStatus_e;

char *parse_status_to_string(PerformParseStatus_e status)
{
    switch (status) {

    case PARSE_OK:
        return "PARSE_OK";
    case PARSE_NET_TIMEOUT:
        return "PARSE_NET_TIMEOUT";
    case PARSE_NET_ERROR:
        return "PARSE_NET_ERROR";
    case PARSE_NO_MEM_ERROR:
        return "PARSE_NO_MEM_ERROR";
    case PARSE_DATA_FORMAT_ERROR:
        return "PARSE_DATA_FORMAT_ERROR";
    case PARSE_NO_DATA:
        return "PARSE_NO_DATA";
    default:
        assert(true || "unmapped internal error, please map it in parse_status_to_string()");
        return "UNKNOWN";
    }
}

PerformParseStatus_e perform_PMT_parse(MpegTsParser_t *parser, MulticastSocket_t *socket, const struct iovec transfer_buffer)
{
    ssize_t bytes_recvd_or_err = multicast_socket_recv(socket, transfer_buffer);

    if (bytes_recvd_or_err < 0) {
        return PARSE_NET_ERROR;
    }

    // printf("Data \n");
    // DumpHex(transfer_buffer.iov_base, bytes_recvd_or_err);

    if (bytes_recvd_or_err < 0) {

        if (bytes_recvd_or_err == -EAGAIN) {
            return PARSE_NET_TIMEOUT;
        }

        return PARSE_NET_ERROR;
    }

    size_t bytes_recvd = bytes_recvd_or_err;

    // size_t bytes_sent_to_parser =
    mpeg_ts_parser_send_data(parser, transfer_buffer.iov_base, bytes_recvd);

    // printf("Data in parser: \n");
    // DumpHex(parser->parse_buffer + parser->parse_data_put_offset - MPEG_TS_PACKET_SIZE,
    // bytes_sent_to_parser);

    bool sync_status = mpeg_ts_parser_sync(parser);

    if (!sync_status && !mpeg_ts_parser_is_synced(parser)) {
        return PARSE_DATA_FORMAT_ERROR;
    }

    size_t packets_parsed = mpeg_ts_parser_parse_many(parser);

    if (packets_parsed == 0) {
        return PARSE_NO_DATA;
    }

    for (size_t parsed_packet_index = 0; parsed_packet_index < packets_parsed; parsed_packet_index++) {

        MpegTsPacket_t *packet = mpeg_ts_parser_next_parsed_packet(parser);

        if (packet->header.pid == MPEG_TS_NULL_PACKET_PID) {
            continue;
        }

        if (packet->header.adaptation_field_control == MPEG_TS_ADAPT_CONTROL_ONLY) {
            continue;
        }
#if 0

        if (packet->header.adaptation_field_control == MPEG_TS_ADAPT_CONTROL_WITH_PAYLOAD) {
            continue;
        }
#endif

        if (!packet->header.payload_unit_start_indicator) {
            continue;
        }

        bool is_PES_packet_at_start = true;

        if (is_PES_packet_at_start) {
            is_PES_packet_at_start &= packet->data[0] == 0;
        }

        if (is_PES_packet_at_start) {
            is_PES_packet_at_start &= packet->data[1] == 0;
        }

        if (is_PES_packet_at_start) {
            is_PES_packet_at_start &= packet->data[2] == 1;
        }

        if (is_PES_packet_at_start) {
            continue;
        }

        uint8_t section_offset = packet->data[0]; // aka PSI pointer
        section_offset += 1;                      // include itself to offset

        bool is_PMT = true;

        if (packet->data[section_offset] != MPEG_TS_PSI_PMT_SECTION_ID) {
            is_PMT = false;
        }

        if (!is_PMT) {
            continue;
        }

        printf("Packet PID: 0x%04" PRIx16 " | "
               "Error: %" PRIx8 " | "
               "Unit Start: %" PRIx8 " | "
               "Priority %" PRIx8 " | "
               "CC: %2" PRIu16 " | "
               "\n",
            packet->header.pid,
            packet->header.error_indicator,
            packet->header.payload_unit_start_indicator,
            packet->header.transport_priority,
            packet->header.continuity_counter);

        DumpHex(packet->data, MPEG_TS_PACKET_PAYLOAD_SIZE);
    }

    return PARSE_OK;
}

void handle_sigterm(int signo)
{
    (void)signo;
    process_terminate_requested = true;
}

int main(void)
{
    bool listen_loop_enabled = true;
    EndServiceStatus_e end_service_status = END_SERVICE_OK;

    struct sigaction term_action;
    term_action.sa_handler = handle_sigterm;
    sigaction(SIGTERM, &term_action, NULL);
    signal(SIGINT, handle_sigterm);

    MulticastSocket_t msock;
    bool multicast_setup_status = multicast_socket_create(&msock);

    if (!multicast_setup_status) {
        end_service_status = END_SERVICE_FAIL_SETUP;
        listen_loop_enabled = false;
    }

    PerformParseStatus_e last_parse_status = PARSE_OK;

    in_addr_t mutlicast_group = inet_addr(MULTICAST_GROUP_CSTR);

    int bind_status = multicast_socket_bind_to_any(&msock, htons(MULTICAST_GROUP_PORT), mutlicast_group);

    if (!bind_status) {
        end_service_status = END_SERVICE_FAIL_SETUP;
        listen_loop_enabled = false;
    }

    MpegTsParser_t mpeg_ts_parser;
    bool parser_init_status = mpeg_ts_parser_init(&mpeg_ts_parser);

    if (!parser_init_status) {
        listen_loop_enabled = false;
        end_service_status = END_SERVICE_FAIL_SETUP;
    }

    uint8_t transfer_buffer[TRANSFER_BUFFER_SIZE];
    memset(transfer_buffer, 0, TRANSFER_BUFFER_SIZE);

    struct iovec transfer_buffer_handle;

    transfer_buffer_handle.iov_base = transfer_buffer;
    transfer_buffer_handle.iov_len = TRANSFER_BUFFER_SIZE;

    uint8_t timeout_err_counter = 0;

    while (listen_loop_enabled) {

        if (process_terminate_requested) {
            listen_loop_enabled = false;
            end_service_status = END_SERVICE_SIGTERM;
            continue;
        }

        last_parse_status = perform_PMT_parse(&mpeg_ts_parser, &msock, transfer_buffer_handle);

        switch (last_parse_status) {

        case PARSE_OK:
        case PARSE_NO_DATA:
            continue;
        case PARSE_NET_TIMEOUT:
            timeout_err_counter++;
        case PARSE_NET_ERROR:
        case PARSE_NO_MEM_ERROR:
        case PARSE_DATA_FORMAT_ERROR:
        default:
            listen_loop_enabled = false;
            end_service_status = END_SERVICE_INTERNAL_ERROR;
            continue;
        }

        if (timeout_err_counter >= TIMEOUT_SCHED_SWITCH_REQUEST_BOUND) {
            timeout_err_counter = 0;
            sched_yield();
            continue;
        }

        timeout_err_counter = 0;
    }

    multicast_socket_close(&msock);
    mpeg_ts_parser_free(&mpeg_ts_parser);

    switch (end_service_status) {

    case END_SERVICE_OK:
        printf("[INFO] Exiting normaly\n");
        return 0;
    case END_SERVICE_SIGTERM:
        printf("\n[INFO] Terminating normaly\n");
        return 0;
    case END_SERVICE_FAIL_SETUP:
        printf("[ERROR] Fail setup\n");
        return -1;
    case END_SERVICE_INTERNAL_ERROR:
        assert(last_parse_status != PARSE_OK);
        printf("[ERROR] Internal error: %s\n", parse_status_to_string(last_parse_status));
        return -1;
    case END_SERVICE_UNKNOWN_ERROR:
    default:
        printf("[ERROR] Unknown error\n");
        return -1;
    }
}

