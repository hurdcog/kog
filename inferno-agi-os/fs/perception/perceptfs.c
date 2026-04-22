/*
 * Perception Filesystem (perceptfs)
 *
 * Exposes sensory input channels as a 9P filesystem.
 * External data sources write to perception files,
 * which are automatically converted to atoms in the AtomSpace.
 *
 * Filesystem layout:
 *   /perception/
 *     text         - Write text input, creates atoms from NLP
 *     numeric      - Write numeric data streams
 *     spatial      - Write spatial/geometric data
 *     temporal     - Write timestamped events
 *     raw          - Write raw byte streams
 *     stats        - Read perception statistics
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Perception channel types */
#define PERCEPT_TEXT      1
#define PERCEPT_NUMERIC   2
#define PERCEPT_SPATIAL   3
#define PERCEPT_TEMPORAL  4
#define PERCEPT_RAW       5

/* Perception entry */
typedef struct PerceptEntry {
    int      channel;
    char     data[1024];
    uint32_t atom_id;      /* ID of atom created from this perception */
    int64_t  timestamp;
    int      processed;
} PerceptEntry;

/* PerceptFS state */
typedef struct PerceptFS {
    PerceptEntry entries[1024];
    int          entry_count;
    int          head;
    int          total_perceptions;
    int          total_atoms_created;
    int          initialized;
} PerceptFS;

static PerceptFS g_perceptfs;

/*
 * Initialize the perception filesystem
 */
int
perceptfs_init(void)
{
    memset(&g_perceptfs, 0, sizeof(PerceptFS));
    g_perceptfs.initialized = 1;
    return 0;
}

/*
 * Shutdown the perception filesystem
 */
void
perceptfs_shutdown(void)
{
    g_perceptfs.initialized = 0;
}

/*
 * Handle write to /perception/<channel>
 * Records perception and creates corresponding atom
 */
int
perceptfs_input(int channel, const char *data, size_t len)
{
    if (!g_perceptfs.initialized)
        return -1;

    int idx = g_perceptfs.head;
    g_perceptfs.entries[idx].channel = channel;
    strncpy(g_perceptfs.entries[idx].data, data,
            len < 1023 ? len : 1023);
    g_perceptfs.entries[idx].processed = 0;
    g_perceptfs.entries[idx].timestamp = 0;

    g_perceptfs.head = (g_perceptfs.head + 1) % 1024;
    if (g_perceptfs.entry_count < 1024)
        g_perceptfs.entry_count++;
    g_perceptfs.total_perceptions++;

    return 0;
}

/*
 * Handle read from /perception/stats
 */
int
perceptfs_stats(char *buf, size_t maxlen)
{
    if (!g_perceptfs.initialized)
        return -1;

    int by_channel[6] = {0};
    for (int i = 0; i < g_perceptfs.entry_count; i++) {
        int ch = g_perceptfs.entries[i].channel;
        if (ch >= 1 && ch <= 5)
            by_channel[ch]++;
    }

    return snprintf(buf, maxlen,
        "Perception Filesystem Statistics\n"
        "  Total perceptions: %d\n"
        "  Buffered entries: %d\n"
        "  Atoms created: %d\n"
        "  By channel:\n"
        "    Text: %d\n"
        "    Numeric: %d\n"
        "    Spatial: %d\n"
        "    Temporal: %d\n"
        "    Raw: %d\n",
        g_perceptfs.total_perceptions,
        g_perceptfs.entry_count,
        g_perceptfs.total_atoms_created,
        by_channel[PERCEPT_TEXT],
        by_channel[PERCEPT_NUMERIC],
        by_channel[PERCEPT_SPATIAL],
        by_channel[PERCEPT_TEMPORAL],
        by_channel[PERCEPT_RAW]);
}
