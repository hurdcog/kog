/*
 * Action Filesystem (actionfs)
 *
 * Exposes motor/action output channels as a 9P filesystem.
 * Cognitive processes write action commands which are
 * dispatched to external actuators or systems.
 *
 * Filesystem layout:
 *   /action/
 *     execute      - Write schema_id + args to execute action
 *     queue        - Read pending action queue
 *     history      - Read recent action history
 *     stats        - Read action statistics
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Action entry */
typedef struct ActionEntry {
    uint32_t schema_id;
    char     args[512];
    int      status;  /* 0=pending, 1=executing, 2=completed, 3=failed */
    int64_t  timestamp;
} ActionEntry;

/* ActionFS state */
typedef struct ActionFS {
    ActionEntry queue[256];
    int         queue_count;
    int         queue_head;
    ActionEntry history[1024];
    int         history_count;
    int         history_head;
    int         total_actions;
    int         total_completed;
    int         total_failed;
    int         initialized;
} ActionFS;

static ActionFS g_actionfs;

/*
 * Initialize the action filesystem
 */
int
actionfs_init(void)
{
    memset(&g_actionfs, 0, sizeof(ActionFS));
    g_actionfs.initialized = 1;
    return 0;
}

/*
 * Shutdown the action filesystem
 */
void
actionfs_shutdown(void)
{
    g_actionfs.initialized = 0;
}

/*
 * Handle write to /action/execute
 * Input: "schema_id arg1 arg2 ..."
 */
int
actionfs_execute(uint32_t schema_id, const char *args)
{
    if (!g_actionfs.initialized)
        return -1;

    int idx = g_actionfs.queue_head;
    g_actionfs.queue[idx].schema_id = schema_id;
    strncpy(g_actionfs.queue[idx].args, args, 511);
    g_actionfs.queue[idx].status = 0;
    g_actionfs.queue[idx].timestamp = 0;

    g_actionfs.queue_head = (g_actionfs.queue_head + 1) % 256;
    if (g_actionfs.queue_count < 256)
        g_actionfs.queue_count++;
    g_actionfs.total_actions++;

    return 0;
}

/*
 * Handle read from /action/queue
 */
int
actionfs_get_queue(char *buf, size_t maxlen)
{
    if (!g_actionfs.initialized)
        return -1;

    int offset = snprintf(buf, maxlen, "Action Queue (%d pending)\n",
                          g_actionfs.queue_count);

    for (int i = 0; i < g_actionfs.queue_count && (size_t)offset < maxlen; i++) {
        const char *status_str[] = {"pending", "executing", "completed", "failed"};
        int s = g_actionfs.queue[i].status;
        if (s < 0 || s > 3) s = 0;
        offset += snprintf(buf + offset, maxlen - offset,
            "  [%d] schema=%u status=%s args=%s\n",
            i, g_actionfs.queue[i].schema_id,
            status_str[s], g_actionfs.queue[i].args);
    }

    return offset;
}

/*
 * Handle read from /action/stats
 */
int
actionfs_stats(char *buf, size_t maxlen)
{
    if (!g_actionfs.initialized)
        return -1;

    return snprintf(buf, maxlen,
        "Action Filesystem Statistics\n"
        "  Total actions: %d\n"
        "  Completed: %d\n"
        "  Failed: %d\n"
        "  Queue size: %d\n"
        "  History size: %d\n",
        g_actionfs.total_actions,
        g_actionfs.total_completed,
        g_actionfs.total_failed,
        g_actionfs.queue_count,
        g_actionfs.history_count);
}
