#include <arpa/inet.h>
#include <assert.h>
#include <bits/types/struct_iovec.h>
#include <errno.h>
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

#include "net/MulticastSocket.h"

#define MULTICAST_GROUP_CSTR               "239.0.0.10"
#define MULTICAST_GROUP_PORT               (uint16_t)1234
#define TIMEOUT_SCHED_SWITCH_REQUEST_BOUND (uint8_t)20
#define RECV_TIMOUT_SECONDS                2
#define RECV_BUFFER_SIZE                   2048

bool process_terminate_requested = false;

typedef enum ListenStatus_e
{
    LISTEN_OK,
    LISTEN_INVALID_ARGUMENT,
    LISTEN_TIMEOUT_FAIL,
    LISTEN_UNKNOWN_FAIL,
} ListenStatus_e;

typedef enum EndServiceStatus_e
{
    END_SERVICE_OK,
    END_SERVICE_SIGTERM,
    END_SERVICE_FAIL_SETUP,
    END_SERVICE_UNKNOWN_ERROR,
} EndServiceStatus_e;

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

ListenStatus_e listen_socket(MulticastSocket_t *sock, struct iovec span)
{
    if (span.iov_len == 0) {
        return LISTEN_INVALID_ARGUMENT;
    }

    ssize_t readen_or_errno = multicast_socket_recv(sock, span);

    if (readen_or_errno < 0) {
        if (readen_or_errno == -EAGAIN) {
            return LISTEN_TIMEOUT_FAIL;
        }

        return LISTEN_UNKNOWN_FAIL;
    }

    /*
     * span state
     * +-------------+-----------+
     * | recvd bytes | <garbage> |
     * +-------------+-----------+
     *               ^ span->data + readen_or_errno (start_offset below)
     */

    size_t bytes_to_clear = span.iov_len - readen_or_errno;
    char *start_offset = (char *)span.iov_base + span.iov_len - bytes_to_clear;
    memset(start_offset, 0, bytes_to_clear);

    /*
     * span state
     * +-------------+-----------+
     * | recvd bytes |  <zeros>  |
     * +-------------+-----------+
     *               ^ start_offset
     */

    // https://stackoverflow.com/questions/3767284/using-printf-with-a-non-null-terminated-string
    int bytes_to_print = readen_or_errno;
    DumpHex(span.iov_base, bytes_to_print);

    return LISTEN_OK;
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

    char recv_buffer[RECV_BUFFER_SIZE + 1];
    memset(recv_buffer, 0, RECV_BUFFER_SIZE + 1);

    struct iovec recv_span;

    recv_span.iov_len = RECV_BUFFER_SIZE;
    recv_span.iov_base = recv_buffer;

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

    uint8_t timeout_err_counter = 0;

    while (listen_loop_enabled) {

        if (process_terminate_requested) {
            listen_loop_enabled = false;
            end_service_status = END_SERVICE_SIGTERM;
            continue;
        }

        ListenStatus_e listen_status = listen_socket(&msock, recv_span);

        if (timeout_err_counter >= TIMEOUT_SCHED_SWITCH_REQUEST_BOUND) {
            timeout_err_counter = 0;
            sched_yield();
            continue;
        }

        switch (listen_status) {

        case LISTEN_OK:
            continue;
        case LISTEN_TIMEOUT_FAIL:
            timeout_err_counter++;
            continue;
        case LISTEN_INVALID_ARGUMENT:
        case LISTEN_UNKNOWN_FAIL:
        default:
            listen_loop_enabled = false;
            end_service_status = END_SERVICE_UNKNOWN_ERROR;
            continue;
        }

        timeout_err_counter = 0;

        if (listen_status == LISTEN_UNKNOWN_FAIL) {
            listen_loop_enabled = false;
            end_service_status = END_SERVICE_UNKNOWN_ERROR;
            continue;
        }
    }

    multicast_socket_close(&msock);

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

