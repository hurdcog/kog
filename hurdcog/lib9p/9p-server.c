/* 9p-server.c - 9P protocol server implementation (stub)
 *
 * Copyright (C) 2024 Free Software Foundation, Inc.
 */

#include "9p-internal.h"
#include <stdlib.h>
#include <unistd.h>

struct p9_server *
p9_server_create(const char *address, uint16_t port)
{
    (void)address; (void)port;
    p9_errno = P9_EPROTO; /* Not implemented yet */
    return NULL;
}

void
p9_server_destroy(struct p9_server *server)
{
    (void)server;
}

int
p9_server_run(struct p9_server *server)
{
    (void)server;
    return -1;
}

void
p9_server_stop(struct p9_server *server)
{
    (void)server;
}

int
p9_server_register_handler(struct p9_server *server, uint8_t type, 
                           p9_handler_t handler)
{
    (void)server; (void)type; (void)handler;
    return -1;
}

/* Message I/O functions */
int
p9_send_message(struct p9_connection *conn, struct p9_message *msg)
{
    if (!conn || !msg || conn->fd < 0)
        return -1;
        
    /* Send message size first */
    uint32_t size = msg->size;
    if (write(conn->fd, &size, 4) != 4)
        return -1;
        
    /* Send message type */
    if (write(conn->fd, &msg->type, 1) != 1)
        return -1;
        
    /* Send message tag */
    if (write(conn->fd, &msg->tag, 2) != 2)
        return -1;
        
    /* Send message data */
    if (msg->size > 7) {
        if (write(conn->fd, msg->data, msg->size - 7) != (ssize_t)(msg->size - 7))
            return -1;
    }
    
    return 0;
}

struct p9_message *
p9_receive_message(struct p9_connection *conn)
{
    struct p9_message *msg;
    uint32_t size;
    
    if (!conn || conn->fd < 0)
        return NULL;
        
    /* Read message size */
    if (read(conn->fd, &size, 4) != 4)
        return NULL;
        
    if (size < 7 || size > conn->msize)
        return NULL;
        
    /* Allocate message */
    msg = malloc(sizeof(struct p9_message) + size - 7);
    if (!msg)
        return NULL;
        
    msg->size = size;
    
    /* Read message type */
    if (read(conn->fd, &msg->type, 1) != 1) {
        free(msg);
        return NULL;
    }
    
    /* Read message tag */
    if (read(conn->fd, &msg->tag, 2) != 2) {
        free(msg);
        return NULL;
    }
    
    /* Read message data */
    if (size > 7) {
        if (read(conn->fd, msg->data, size - 7) != (ssize_t)(size - 7)) {
            free(msg);
            return NULL;
        }
    }
    
    return msg;
}

void
p9_free_message(struct p9_message *msg)
{
    free(msg);
}