/*
 * Cognitive IPC Channels
 * Inter-Process Communication for Cognitive Atoms
 *
 * Provides high-performance channels for sending and receiving
 * atoms between cognitive processes, supporting both local and
 * distributed communication via 9P/Styx.
 *
 * Copyright (c) 2026 OpenCog Inferno Project
 * Licensed under AGPL-3.0
 */

#ifndef _COGNITIVE_CHANNEL_H_
#define _COGNITIVE_CHANNEL_H_

#include <stdint.h>

/* Maximum channels per process */
#define COG_MAX_CHANNELS 64

/* Maximum atoms per message */
#define COG_MAX_MSG_ATOMS 256

/* Channel buffer size (number of messages) */
#define COG_CHANNEL_BUFFER_SIZE 1024

/* Channel states */
typedef enum {
    CHANNEL_STATE_CLOSED = 0,
    CHANNEL_STATE_OPEN,
    CHANNEL_STATE_CONNECTING,
    CHANNEL_STATE_ERROR
} ChannelState;

/* Channel types */
typedef enum {
    CHANNEL_TYPE_LOCAL = 1,     /* Same-node IPC */
    CHANNEL_TYPE_REMOTE,        /* Cross-node via 9P */
    CHANNEL_TYPE_BROADCAST      /* One-to-many */
} ChannelType;

/* Message priority levels */
typedef enum {
    MSG_PRIORITY_LOW = 0,
    MSG_PRIORITY_NORMAL = 1,
    MSG_PRIORITY_HIGH = 2,
    MSG_PRIORITY_URGENT = 3
} MessagePriority;

/* Cognitive message */
typedef struct CogMessage {
    uint32_t sender_pid;
    uint32_t sequence;
    MessagePriority priority;
    uint32_t atom_count;
    uint32_t atom_ids[COG_MAX_MSG_ATOMS];
    uint64_t timestamp;
    uint32_t flags;
} CogMessage;

/* Message flags */
#define MSG_FLAG_SYNC       (1 << 0)  /* Synchronous (wait for ack) */
#define MSG_FLAG_BROADCAST  (1 << 1)  /* Broadcast to all listeners */
#define MSG_FLAG_COMPRESS   (1 << 2)  /* Compress payload */
#define MSG_FLAG_PRIORITY   (1 << 3)  /* Priority message */

/* Ring buffer for messages */
typedef struct MessageRing {
    CogMessage *messages;
    uint32_t capacity;
    uint32_t head;          /* Write position */
    uint32_t tail;          /* Read position */
    uint32_t count;         /* Current message count */
} MessageRing;

/* Cognitive channel */
typedef struct CogChannel {
    uint32_t id;
    char name[64];
    ChannelType type;
    ChannelState state;
    uint32_t owner_pid;
    uint32_t peer_pid;
    MessageRing send_buffer;
    MessageRing recv_buffer;
    uint64_t messages_sent;
    uint64_t messages_received;
    uint64_t bytes_transferred;
    uint64_t created_at;
} CogChannel;

/* Channel manager */
typedef struct ChannelManager {
    CogChannel channels[COG_MAX_CHANNELS];
    uint32_t channel_count;
    uint32_t next_id;
    uint64_t total_messages;
} ChannelManager;

/* ========================================================================
 * API Functions
 * ======================================================================== */

/* Manager lifecycle */
ChannelManager* channel_manager_create(void);
void channel_manager_destroy(ChannelManager *mgr);

/* Channel operations */
int channel_create(ChannelManager *mgr, uint32_t owner_pid,
                   uint32_t peer_pid, const char *name,
                   ChannelType type);
int channel_close(ChannelManager *mgr, uint32_t channel_id);
CogChannel* channel_get(ChannelManager *mgr, uint32_t channel_id);

/* Message operations */
int channel_send(ChannelManager *mgr, uint32_t channel_id,
                 uint32_t *atom_ids, uint32_t count,
                 MessagePriority priority);
int channel_recv(ChannelManager *mgr, uint32_t channel_id,
                 CogMessage *msg);
int channel_peek(ChannelManager *mgr, uint32_t channel_id);

/* Broadcast */
int channel_broadcast(ChannelManager *mgr, uint32_t sender_pid,
                      uint32_t *atom_ids, uint32_t count);

/* Statistics */
void channel_print_stats(ChannelManager *mgr);

#endif /* _COGNITIVE_CHANNEL_H_ */
