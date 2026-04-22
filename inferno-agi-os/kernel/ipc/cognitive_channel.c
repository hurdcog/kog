/*
 * Cognitive IPC Channels Implementation
 * Inter-Process Communication for Cognitive Atoms
 *
 * Ring-buffer based message passing with priority support,
 * local and remote channel types, and broadcast capability.
 *
 * Copyright (c) 2026 OpenCog Inferno Project
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "cognitive_channel.h"

#ifdef _WIN32
#include <windows.h>
#define LOCK_TYPE CRITICAL_SECTION
#define LOCK_INIT(l) InitializeCriticalSection(&(l))
#define LOCK_ACQUIRE(l) EnterCriticalSection(&(l))
#define LOCK_RELEASE(l) LeaveCriticalSection(&(l))
#define LOCK_DESTROY(l) DeleteCriticalSection(&(l))
static LARGE_INTEGER g_freq;
static int g_freq_init = 0;
static uint64_t get_timestamp(void) {
    LARGE_INTEGER t;
    if (!g_freq_init) { QueryPerformanceFrequency(&g_freq); g_freq_init = 1; }
    QueryPerformanceCounter(&t);
    return (uint64_t)((t.QuadPart * 1000000) / g_freq.QuadPart);
}
#else
#include <pthread.h>
#include <sys/time.h>
#define LOCK_TYPE pthread_mutex_t
#define LOCK_INIT(l) pthread_mutex_init(&(l), NULL)
#define LOCK_ACQUIRE(l) pthread_mutex_lock(&(l))
#define LOCK_RELEASE(l) pthread_mutex_unlock(&(l))
#define LOCK_DESTROY(l) pthread_mutex_destroy(&(l))
static uint64_t get_timestamp(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec;
}
#endif

static LOCK_TYPE g_channel_lock;
static int g_channel_lock_init = 0;

/* ========================================================================
 * Ring Buffer Operations
 * ======================================================================== */

static int ring_init(MessageRing *ring, uint32_t capacity)
{
    ring->messages = (CogMessage *)calloc(capacity, sizeof(CogMessage));
    if (!ring->messages) return -1;
    ring->capacity = capacity;
    ring->head = 0;
    ring->tail = 0;
    ring->count = 0;
    return 0;
}

static void ring_destroy(MessageRing *ring)
{
    if (ring->messages) {
        free(ring->messages);
        ring->messages = NULL;
    }
}

static int ring_push(MessageRing *ring, const CogMessage *msg)
{
    if (ring->count >= ring->capacity) return -1;  /* Full */

    ring->messages[ring->head] = *msg;
    ring->head = (ring->head + 1) % ring->capacity;
    ring->count++;
    return 0;
}

static int ring_pop(MessageRing *ring, CogMessage *msg)
{
    if (ring->count == 0) return -1;  /* Empty */

    *msg = ring->messages[ring->tail];
    ring->tail = (ring->tail + 1) % ring->capacity;
    ring->count--;
    return 0;
}

static int ring_peek_count(MessageRing *ring)
{
    return (int)ring->count;
}

/* ========================================================================
 * Channel Manager Implementation
 * ======================================================================== */

ChannelManager* channel_manager_create(void)
{
    ChannelManager *mgr;

    mgr = (ChannelManager *)calloc(1, sizeof(ChannelManager));
    if (!mgr) return NULL;

    mgr->channel_count = 0;
    mgr->next_id = 1;
    mgr->total_messages = 0;

    if (!g_channel_lock_init) {
        LOCK_INIT(g_channel_lock);
        g_channel_lock_init = 1;
    }

    printf("Cognitive IPC: Channel manager created\n");
    return mgr;
}

void channel_manager_destroy(ChannelManager *mgr)
{
    uint32_t i;

    if (!mgr) return;

    for (i = 0; i < mgr->channel_count; i++) {
        ring_destroy(&mgr->channels[i].send_buffer);
        ring_destroy(&mgr->channels[i].recv_buffer);
    }

    printf("Cognitive IPC: Channel manager destroyed (total messages: %lu)\n",
           (unsigned long)mgr->total_messages);

    free(mgr);
}

int channel_create(ChannelManager *mgr, uint32_t owner_pid,
                   uint32_t peer_pid, const char *name,
                   ChannelType type)
{
    CogChannel *ch;

    if (!mgr || mgr->channel_count >= COG_MAX_CHANNELS) return -1;

    LOCK_ACQUIRE(g_channel_lock);

    ch = &mgr->channels[mgr->channel_count];
    ch->id = mgr->next_id++;
    strncpy(ch->name, name ? name : "unnamed", sizeof(ch->name) - 1);
    ch->name[sizeof(ch->name) - 1] = '\0';
    ch->type = type;
    ch->state = CHANNEL_STATE_OPEN;
    ch->owner_pid = owner_pid;
    ch->peer_pid = peer_pid;
    ch->messages_sent = 0;
    ch->messages_received = 0;
    ch->bytes_transferred = 0;
    ch->created_at = get_timestamp();

    if (ring_init(&ch->send_buffer, COG_CHANNEL_BUFFER_SIZE) != 0 ||
        ring_init(&ch->recv_buffer, COG_CHANNEL_BUFFER_SIZE) != 0) {
        ring_destroy(&ch->send_buffer);
        ring_destroy(&ch->recv_buffer);
        LOCK_RELEASE(g_channel_lock);
        return -1;
    }

    mgr->channel_count++;

    LOCK_RELEASE(g_channel_lock);

    printf("Cognitive IPC: Channel %u created '%s' (pid %u <-> %u, type %d)\n",
           ch->id, ch->name, owner_pid, peer_pid, type);

    return (int)ch->id;
}

int channel_close(ChannelManager *mgr, uint32_t channel_id)
{
    CogChannel *ch = channel_get(mgr, channel_id);
    if (!ch) return -1;

    LOCK_ACQUIRE(g_channel_lock);

    ch->state = CHANNEL_STATE_CLOSED;
    ring_destroy(&ch->send_buffer);
    ring_destroy(&ch->recv_buffer);

    LOCK_RELEASE(g_channel_lock);

    printf("Cognitive IPC: Channel %u closed '%s'\n", ch->id, ch->name);
    return 0;
}

CogChannel* channel_get(ChannelManager *mgr, uint32_t channel_id)
{
    uint32_t i;
    if (!mgr) return NULL;

    for (i = 0; i < mgr->channel_count; i++) {
        if (mgr->channels[i].id == channel_id) {
            return &mgr->channels[i];
        }
    }
    return NULL;
}

int channel_send(ChannelManager *mgr, uint32_t channel_id,
                 uint32_t *atom_ids, uint32_t count,
                 MessagePriority priority)
{
    CogChannel *ch;
    CogMessage msg;

    if (!mgr || !atom_ids || count == 0) return -1;
    if (count > COG_MAX_MSG_ATOMS) count = COG_MAX_MSG_ATOMS;

    ch = channel_get(mgr, channel_id);
    if (!ch || ch->state != CHANNEL_STATE_OPEN) return -1;

    LOCK_ACQUIRE(g_channel_lock);

    memset(&msg, 0, sizeof(msg));
    msg.sender_pid = ch->owner_pid;
    msg.sequence = (uint32_t)(ch->messages_sent + 1);
    msg.priority = priority;
    msg.atom_count = count;
    memcpy(msg.atom_ids, atom_ids, count * sizeof(uint32_t));
    msg.timestamp = get_timestamp();

    /* Push to send buffer (which is the peer's recv buffer in local mode) */
    if (ring_push(&ch->recv_buffer, &msg) != 0) {
        LOCK_RELEASE(g_channel_lock);
        return -1;  /* Buffer full */
    }

    ch->messages_sent++;
    ch->bytes_transferred += count * sizeof(uint32_t);
    mgr->total_messages++;

    LOCK_RELEASE(g_channel_lock);

    return (int)count;
}

int channel_recv(ChannelManager *mgr, uint32_t channel_id,
                 CogMessage *msg)
{
    CogChannel *ch;

    if (!mgr || !msg) return -1;

    ch = channel_get(mgr, channel_id);
    if (!ch || ch->state != CHANNEL_STATE_OPEN) return -1;

    LOCK_ACQUIRE(g_channel_lock);

    if (ring_pop(&ch->recv_buffer, msg) != 0) {
        LOCK_RELEASE(g_channel_lock);
        return 0;  /* No messages */
    }

    ch->messages_received++;

    LOCK_RELEASE(g_channel_lock);

    return (int)msg->atom_count;
}

int channel_peek(ChannelManager *mgr, uint32_t channel_id)
{
    CogChannel *ch;
    int count;

    if (!mgr) return -1;

    ch = channel_get(mgr, channel_id);
    if (!ch) return -1;

    LOCK_ACQUIRE(g_channel_lock);
    count = ring_peek_count(&ch->recv_buffer);
    LOCK_RELEASE(g_channel_lock);

    return count;
}

int channel_broadcast(ChannelManager *mgr, uint32_t sender_pid,
                      uint32_t *atom_ids, uint32_t count)
{
    uint32_t i;
    int sent = 0;

    if (!mgr || !atom_ids || count == 0) return -1;

    for (i = 0; i < mgr->channel_count; i++) {
        CogChannel *ch = &mgr->channels[i];
        if (ch->state == CHANNEL_STATE_OPEN &&
            ch->owner_pid != sender_pid) {
            if (channel_send(mgr, ch->id, atom_ids, count,
                             MSG_PRIORITY_NORMAL) > 0) {
                sent++;
            }
        }
    }

    return sent;
}

void channel_print_stats(ChannelManager *mgr)
{
    uint32_t i;

    if (!mgr) return;

    printf("\nCognitive IPC Statistics:\n");
    printf("  Total channels: %u\n", mgr->channel_count);
    printf("  Total messages: %lu\n", (unsigned long)mgr->total_messages);
    printf("\n  Channels:\n");

    for (i = 0; i < mgr->channel_count; i++) {
        CogChannel *ch = &mgr->channels[i];
        printf("    [%u] '%s': %s, pid %u<->%u, sent=%lu, recv=%lu, bytes=%lu\n",
               ch->id, ch->name,
               ch->state == CHANNEL_STATE_OPEN ? "OPEN" :
               ch->state == CHANNEL_STATE_CLOSED ? "CLOSED" : "ERROR",
               ch->owner_pid, ch->peer_pid,
               (unsigned long)ch->messages_sent,
               (unsigned long)ch->messages_received,
               (unsigned long)ch->bytes_transferred);
    }
}
