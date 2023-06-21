#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <memory.h>
#include <netinet/in.h>
#include <sched.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>

#include "mpegts/data/pmt_builder.h"
#include "mpegts/data/pmt_dumper.h"
#include "mpegts/packet/packet_inplace_parser.h"

#include "net/multicast_socket.h"

#define MULTICAST_GROUP_CSTR "239.0.0.10"
#define MULTICAST_GROUP_PORT 1234

#define NET_TIMOUT_SECONDS         2
#define SCHED_SWITCH_REQUEST_BOUND 20
#define PARSE_BUFFER_SIZE          1024
#define PMT_BUILD_BUFFER_SIZE      MPEG_TS_PSI_PMT_SECTION_MAX_LENGTH
#define PACKETS_REFS_COUNT         10

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

PerformParseStatus_e perform_PMT_parse(MulticastSocket_t *socket, struct iovec parse_buffer,
    MpegTsPMTBuilder_t *pmt_builder)
{
    ssize_t bytes_recvd_or_err = multicast_socket_recv(socket, parse_buffer);
    if (bytes_recvd_or_err < 0) {
        if (bytes_recvd_or_err == -EAGAIN) {
            return PARSE_NET_TIMEOUT;
        }
        if (bytes_recvd_or_err == -EINTR) {
            return PARSE_OK_WITHOUT_ACTION;
        }
        return PARSE_NET_ERROR;
    }


    static MpegTsPacketRef_t packet_refs[PACKETS_REFS_COUNT];

    size_t bytes_recvd = bytes_recvd_or_err;

    size_t linked_packets_count = mpeg_ts_parse_packets_inplace(parse_buffer.iov_base,
        bytes_recvd,
        packet_refs,
        PACKETS_REFS_COUNT);
    if (linked_packets_count == 0) {
        return PARSE_NO_DATA;
    }

    if (pmt_builder->state == PMT_BUILDER_STATE_TABLE_ASSEMBLED) {
        OptionalMpegTsPMT_t program_map_table = mpeg_ts_pmt_builder_try_build_table(pmt_builder);
        if (program_map_table.has_value) {
            static uint32_t last_table_crc = 0;
            if (last_table_crc != program_map_table.value.CRC) {
                mpeg_ts_dump_pmt_to_stream(&program_map_table.value, stdout);
                printf("\n");
                last_table_crc = program_map_table.value.CRC;
            }
            mpeg_ts_pmt_builder_reset(pmt_builder);
        }
    }

    for (size_t parsed_packet_index = 0; parsed_packet_index < linked_packets_count;
         parsed_packet_index++) {

        MpegTsPacketRef_t packet_ref = packet_refs[parsed_packet_index];

        if (pmt_builder->state == PMT_BUILDER_STATE_TABLE_IS_BUILDING &&
            pmt_builder->last_packet_header.pid != packet_ref.header.pid) {
            continue;
        }

        MpegTsPMTBuilderSendPacketStatus_e send_status =
            mpeg_ts_pmt_builder_try_send_packet(pmt_builder, &packet_ref);
        switch (send_status) {
        case PMT_BUILDER_SEND_STATUS_SMALL_TABLE_IS_ASSEMBLED:
        case PMT_BUILDER_SEND_STATUS_TABLE_IS_ASSEMBLED:
            return PARSE_OK;
        case PMT_BUILDER_SEND_STATUS_NEED_MORE_PACKETS:
            continue;
        case PMT_BUILDER_SEND_STATUS_UNORDERED_PACKET_REJECTED:
        case PMT_BUILDER_SEND_STATUS_REDUDANT_PACKET_REJECTED:
            mpeg_ts_pmt_builder_reset(pmt_builder);
            continue;
        case PMT_BUILDER_SEND_STATUS_NOT_ENOUGHT_MEMORY:
            return PARSE_NO_MEM_ERROR;
        default:
            break;
        }
    }

    mpeg_ts_pmt_builder_reset(pmt_builder);
    return PARSE_OK_WITHOUT_ACTION;
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
        goto end_service;
    }

    bool setup_timeout_status = multicast_socket_set_timeout_seconds(&msock, NET_TIMOUT_SECONDS);
    if (!setup_timeout_status) {
        end_service_status = END_SERVICE_FAIL_SETUP;
        goto end_service;
    }

    PerformParseStatus_e last_parse_status = PARSE_OK;

    in_addr_t mutlicast_group = inet_addr(MULTICAST_GROUP_CSTR);

    int bind_status =
        multicast_socket_bind_to_any(&msock, htons(MULTICAST_GROUP_PORT), mutlicast_group);
    if (!bind_status) {
        end_service_status = END_SERVICE_FAIL_SETUP;
        goto end_service;
    }

    uint8_t parse_buffer[PARSE_BUFFER_SIZE];
    memset(parse_buffer, 0, PARSE_BUFFER_SIZE);
    struct iovec parse_buffer_handle;
    parse_buffer_handle.iov_base = parse_buffer;
    parse_buffer_handle.iov_len = PARSE_BUFFER_SIZE;

    uint8_t pmt_build_buffer[PMT_BUILD_BUFFER_SIZE];
    memset(pmt_build_buffer, 0, PMT_BUILD_BUFFER_SIZE);
    MpegTsPMTBuilder_t pmt_builder;
    mpeg_ts_pmt_builder_init(&pmt_builder, pmt_build_buffer, PMT_BUILD_BUFFER_SIZE);

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

        last_parse_status = perform_PMT_parse(&msock, parse_buffer_handle, &pmt_builder);
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
    }

    multicast_socket_close(&msock);

end_service:

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

