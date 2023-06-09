#include <arpa/inet.h>
#include <assert.h>
#include <bits/types/struct_iovec.h>
#include <errno.h>
#include <mpegts/Parser.h>
#include <net/MulticastSocket.h>
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

#define MULTICAST_GROUP_CSTR               "239.0.0.10"
#define MULTICAST_GROUP_PORT               (uint16_t)1234
#define TIMEOUT_SCHED_SWITCH_REQUEST_BOUND (uint8_t)20
#define RECV_TIMOUT_SECONDS                2
#define RECV_BUFFER_SIZE                   1024

bool process_terminate_requested = false;

typedef enum EndServiceStatus_e
{
    END_SERVICE_OK,
    END_SERVICE_SIGTERM,
    END_SERVICE_FAIL_SETUP,
    END_SERVICE_UNKNOWN_ERROR,
} EndServiceStatus_e;

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

    uint8_t timeout_err_counter = 0;

    while (listen_loop_enabled) {

        if (process_terminate_requested) {
            listen_loop_enabled = false;
            end_service_status = END_SERVICE_SIGTERM;
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
        printf("[Error] Fail setup\n");
        return -1;
    case END_SERVICE_UNKNOWN_ERROR:
    default:
        printf("[Error] Unknown error\n");
        return -1;
    }
}

