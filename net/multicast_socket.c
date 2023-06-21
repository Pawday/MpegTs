#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "net/multicast_socket.h"

bool multicast_socket_create(MulticastSocket_t *multicast_socket)
{
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
        return false;
    }

    multicast_socket->socket_fd = socket_fd;

    return true;
}

static bool validate_socket(MulticastSocket_t *sock)
{
    if (sock->socket_fd <= 0) {
        return false;
    }

    return true;
}

bool multicast_socket_set_timeout_seconds(MulticastSocket_t *socket, uint8_t seconds)
{
    if (!validate_socket(socket)) {
        return false;
    }

    struct timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;

    if (setsockopt(socket->socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout) < 0) {
        return false;
    }

    return true;
}

bool multicast_socket_bind_to(MulticastSocket_t *multicast_socket, in_addr_t bind_point,
    uint16_t bind_port, in_addr_t multicast_group)
{
    if (!validate_socket(multicast_socket)) {
        return false;
    }

    multicast_socket->bind_point.sin_family = AF_INET;
    multicast_socket->bind_point.sin_addr.s_addr = multicast_group;
    multicast_socket->bind_point.sin_port = bind_port;

    int reuse_val = 1;
    setsockopt(multicast_socket->socket_fd,
        SOL_SOCKET,
        SO_REUSEPORT,
        &reuse_val,
        sizeof(reuse_val));

    int bind_status = bind(multicast_socket->socket_fd,
        (struct sockaddr *)&multicast_socket->bind_point,
        sizeof(multicast_socket->bind_point));
    if (bind_status != 0) {
        memset(&multicast_socket->bind_point, 0, sizeof(multicast_socket->bind_point));
        multicast_socket_close(multicast_socket);
        return false;
    }

    multicast_socket->multcst_req.imr_multiaddr.s_addr = multicast_group;
    multicast_socket->multcst_req.imr_interface.s_addr = bind_point;

    int subscribe_status = setsockopt(multicast_socket->socket_fd,
        IPPROTO_IP,
        IP_ADD_MEMBERSHIP,
        (char *)&multicast_socket->multcst_req,
        sizeof(multicast_socket->multcst_req));
    if (subscribe_status != 0) {
        memset(&multicast_socket->multcst_req, 0, sizeof(multicast_socket->multcst_req));
        return false;
    }

    return true;
}

bool multicast_socket_bind_to_any(MulticastSocket_t *multicast_socket, uint16_t bind_port,
    in_addr_t multicast_group)
{
    return multicast_socket_bind_to(multicast_socket, INADDR_ANY, bind_port, multicast_group);
}

ssize_t multicast_socket_recv(MulticastSocket_t *sock, struct iovec span)
{
    if (span.iov_len == 0) {
        return -ENOMEM;
    }

    socklen_t bind_point_struct_size_val = sizeof(sock->bind_point);
    ssize_t readen_or_err = recvfrom(sock->socket_fd,
        span.iov_base,
        span.iov_len,
        0,
        (struct sockaddr *)&sock->bind_point,
        &bind_point_struct_size_val);
    if (readen_or_err < 0) {
        int err = errno;
        return -err;
    }

    return readen_or_err;
}

void multicast_socket_close(MulticastSocket_t *socket)
{
    close(socket->socket_fd);
    socket->socket_fd = 0;
}

