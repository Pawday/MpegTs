#include <arpa/inet.h>
#include <assert.h>
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

#include <mpegts/data/psi/pmt_dumper.h>
#include <mpegts/data/psi/pmt_parser.h>
#include <mpegts/parser.h>
#include <net/multicast_socket.h>

#define MULTICAST_GROUP_CSTR "239.0.0.10"
#define MULTICAST_GROUP_PORT 1234

#define NET_TIMOUT_SECONDS         2
#define SCHED_SWITCH_REQUEST_BOUND 20
#define TRANSFER_BUFFER_SIZE       2048

bool process_terminate_requested = false;

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
    PARSE_OK_WITHOUT_ACTION,
    PARSE_NO_DATA,
    PARSE_NET_TIMEOUT,
    PARSE_NET_ERROR,
    PARSE_NET_INTERUPTED,
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
    case PARSE_NET_INTERUPTED:
        return "NET_INTERUPTED";
    default:
        return "UNKNOWN";
    }
}

PerformParseStatus_e perform_PMT_parse(MpegTsParser_t *parser, MulticastSocket_t *socket,
    const struct iovec transfer_buffer)
{
    ssize_t bytes_recvd_or_err = multicast_socket_recv(socket, transfer_buffer);

    static uint32_t last_table_crc = 0;

    if (bytes_recvd_or_err < 0) {

        if (bytes_recvd_or_err == -EAGAIN) {
            return PARSE_NET_TIMEOUT;
        }

        if (bytes_recvd_or_err == -EINTR) {
            return PARSE_NET_INTERUPTED;
        }

        return PARSE_NET_ERROR;
    }

    size_t bytes_recvd = bytes_recvd_or_err;

    mpeg_ts_parser_send_data(parser, transfer_buffer.iov_base, bytes_recvd);

    bool sync_status = mpeg_ts_parser_sync(parser);

    if (!sync_status && !mpeg_ts_parser_is_synced(parser)) {
        return PARSE_DATA_FORMAT_ERROR;
    }

    size_t packets_parsed = mpeg_ts_parser_parse_all_packets_in_buffer(parser);

    if (packets_parsed == 0) {
        return PARSE_NO_DATA;
    }

    bool new_table_arrived = false;

    for (size_t parsed_packet_index = 0; parsed_packet_index < packets_parsed;
         parsed_packet_index++) {

        MpegTsPacket_t *packet = mpeg_ts_parser_next_parsed_packet(parser);

        MpegTsPMTMaybe_t program_map_table = mpeg_ts_parse_pmt_from_packet(packet);

        if (!program_map_table.has_value) {
            continue;
        }

        uint32_t new_pmt_crc = program_map_table.value.CRC;

        if (new_pmt_crc == last_table_crc) {
            continue;
        }

        new_table_arrived = true;

        last_table_crc = new_pmt_crc;

        mpeg_ts_dump_pmt_to_stream(&program_map_table.value, stdout);
        printf("\n");
    }

    if (!new_table_arrived) {
        return PARSE_OK_WITHOUT_ACTION;
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
    bool multicast_socket_setup_status = multicast_socket_create(&msock);

    if (!multicast_socket_setup_status) {
        end_service_status = END_SERVICE_FAIL_SETUP;
        listen_loop_enabled = false;
    }

    bool setup_timeout_status = multicast_socket_set_timeout_sec(&msock, NET_TIMOUT_SECONDS);

    if (!setup_timeout_status) {
        end_service_status = END_SERVICE_FAIL_SETUP;
        listen_loop_enabled = false;
    }

    PerformParseStatus_e last_parse_status = PARSE_OK;

    in_addr_t mutlicast_group = inet_addr(MULTICAST_GROUP_CSTR);

    int bind_status =
        multicast_socket_bind_to_any(&msock, htons(MULTICAST_GROUP_PORT), mutlicast_group);

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


        if (timeout_err_counter >= SCHED_SWITCH_REQUEST_BOUND) {
            timeout_err_counter = 0;
            sched_yield();
            continue;
        }

        last_parse_status = perform_PMT_parse(&mpeg_ts_parser, &msock, transfer_buffer_handle);

        switch (last_parse_status) {

        case PARSE_OK:
        case PARSE_NO_DATA:
            continue;
        case PARSE_NET_TIMEOUT:
        case PARSE_OK_WITHOUT_ACTION:
            timeout_err_counter++;
            continue;
        case PARSE_NET_INTERUPTED:
            listen_loop_enabled = false;
            end_service_status = END_SERVICE_SIGTERM;
            continue;
        case PARSE_NET_ERROR:
        case PARSE_NO_MEM_ERROR:
        case PARSE_DATA_FORMAT_ERROR:
        default:
            listen_loop_enabled = false;
            end_service_status = END_SERVICE_INTERNAL_ERROR;
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

