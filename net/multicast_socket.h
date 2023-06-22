#pragma once

#include <netinet/in.h>
#include <stdbool.h>

typedef struct
{
    int socket_fd;
    struct ip_mreq multcst_req;
    struct sockaddr_in bind_point;
} MulticastSocket_t;



bool multicast_socket_create(MulticastSocket_t *multicast_socket);
bool multicast_socket_set_timeout_seconds(MulticastSocket_t *socket, uint8_t seconds);

/*
 * bind_point is the net interface addres to send IGMP subscribe packet
 */
bool multicast_socket_bind_to(MulticastSocket_t *multicast_socket, in_addr_t bind_point,
    uint16_t bind_port, in_addr_t multicast_group);

/*
 * calls multicast_socket_bind_to() with ANY interface
 * multicast traffic from 2 or more interfaces will be merged
 */
bool multicast_socket_bind_to_any(MulticastSocket_t *multicast_socket, uint16_t bind_port,
    in_addr_t multicast_group);

/*
 * can return -errno
 */
ssize_t multicast_socket_recv(MulticastSocket_t *sock, uint8_t *recv_buffer, size_t recv_buffer_size);

void multicast_socket_close(MulticastSocket_t *socket);

