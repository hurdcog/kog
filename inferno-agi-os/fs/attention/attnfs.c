/*
 * Attention Filesystem (attnfs)
 *
 * Exposes ECAN attention allocation as a 9P filesystem.
 *
 * Filesystem layout:
 *   /attention/
 *     stimulate    - Write "atom_id amount" to stimulate
 *     focus        - Read for current attentional focus atoms
 *     threshold    - Read/write attention focus threshold
 *     spread       - Write atom_id to spread importance from
 *     rent         - Write to trigger rent collection
 *     stats        - Read for attention statistics
 *     bank/        - Attention bank state
 *       sti_funds  - Read current STI funds
 *       lti_funds  - Read current LTI funds
 *       total_sti  - Read total STI in system
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#endif

/* Maximum atoms tracked for attention */
#define ATTNFS_MAX_ATOMS    65536
#define ATTNFS_MAX_FOCUS    256

/* Attention entry */
typedef struct AttnFSEntry {
    uint32_t atom_id;
    int16_t  sti;
    int16_t  lti;
    int16_t  vlti;
    int      active;
} AttnFSEntry;

/* Attention bank */
typedef struct AttnFSBank {
    int32_t  total_sti;
    int32_t  sti_funds;
    int32_t  lti_funds;
    int32_t  target_sti;
    int16_t  af_threshold;
    uint32_t af_max_size;
} AttnFSBank;

/* AttnFS state */
typedef struct AttnFS {
    AttnFSEntry entries[ATTNFS_MAX_ATOMS];
    uint32_t    entry_count;
    AttnFSBank  bank;
    int         initialized;
    int         total_stimulations;
    int         total_spreads;
    int         total_rent_collections;
#ifdef PLATFORM_WINDOWS
    CRITICAL_SECTION lock;
#else
    pthread_mutex_t  lock;
#endif
} AttnFS;

static AttnFS g_attnfs;

/*
 * Initialize the attention filesystem
 */
int
attnfs_init(void)
{
    memset(&g_attnfs, 0, sizeof(AttnFS));
    g_attnfs.initialized = 1;

    /* Default attention bank parameters */
    g_attnfs.bank.total_sti = 0;
    g_attnfs.bank.sti_funds = 10000;
    g_attnfs.bank.lti_funds = 10000;
    g_attnfs.bank.target_sti = 10000;
    g_attnfs.bank.af_threshold = 50;
    g_attnfs.bank.af_max_size = ATTNFS_MAX_FOCUS;

#ifdef PLATFORM_WINDOWS
    InitializeCriticalSection(&g_attnfs.lock);
#else
    pthread_mutex_init(&g_attnfs.lock, NULL);
#endif

    return 0;
}

/*
 * Shutdown the attention filesystem
 */
void
attnfs_shutdown(void)
{
    g_attnfs.initialized = 0;

#ifdef PLATFORM_WINDOWS
    DeleteCriticalSection(&g_attnfs.lock);
#else
    pthread_mutex_destroy(&g_attnfs.lock);
#endif
}

/*
 * Find or create entry for atom
 */
static AttnFSEntry*
attnfs_find_or_create(uint32_t atom_id)
{
    /* Find existing */
    for (uint32_t i = 0; i < ATTNFS_MAX_ATOMS; i++) {
        if (g_attnfs.entries[i].active && g_attnfs.entries[i].atom_id == atom_id)
            return &g_attnfs.entries[i];
    }

    /* Create new */
    for (uint32_t i = 0; i < ATTNFS_MAX_ATOMS; i++) {
        if (!g_attnfs.entries[i].active) {
            g_attnfs.entries[i].atom_id = atom_id;
            g_attnfs.entries[i].sti = 0;
            g_attnfs.entries[i].lti = 0;
            g_attnfs.entries[i].vlti = 0;
            g_attnfs.entries[i].active = 1;
            g_attnfs.entry_count++;
            return &g_attnfs.entries[i];
        }
    }

    return NULL;
}

/*
 * Handle write to /attention/stimulate
 * Input: "atom_id amount"
 */
int
attnfs_stimulate(uint32_t atom_id, int16_t amount)
{
    if (!g_attnfs.initialized)
        return -1;

#ifdef PLATFORM_WINDOWS
    EnterCriticalSection(&g_attnfs.lock);
#else
    pthread_mutex_lock(&g_attnfs.lock);
#endif

    AttnFSEntry *entry = attnfs_find_or_create(atom_id);
    if (entry == NULL) {
#ifdef PLATFORM_WINDOWS
        LeaveCriticalSection(&g_attnfs.lock);
#else
        pthread_mutex_unlock(&g_attnfs.lock);
#endif
        return -1;
    }

    /* Apply stimulus, bounded by available funds */
    int16_t actual = amount;
    if (actual > g_attnfs.bank.sti_funds)
        actual = (int16_t)g_attnfs.bank.sti_funds;

    entry->sti += actual;
    g_attnfs.bank.sti_funds -= actual;
    g_attnfs.bank.total_sti += actual;
    g_attnfs.total_stimulations++;

#ifdef PLATFORM_WINDOWS
    LeaveCriticalSection(&g_attnfs.lock);
#else
    pthread_mutex_unlock(&g_attnfs.lock);
#endif

    return 0;
}

/*
 * Handle read from /attention/focus
 * Returns atom IDs with STI above threshold
 */
int
attnfs_get_focus(uint32_t *atoms, uint32_t max_atoms)
{
    if (!g_attnfs.initialized)
        return 0;

#ifdef PLATFORM_WINDOWS
    EnterCriticalSection(&g_attnfs.lock);
#else
    pthread_mutex_lock(&g_attnfs.lock);
#endif

    uint32_t count = 0;
    for (uint32_t i = 0; i < ATTNFS_MAX_ATOMS && count < max_atoms; i++) {
        if (g_attnfs.entries[i].active &&
            g_attnfs.entries[i].sti >= g_attnfs.bank.af_threshold) {
            atoms[count++] = g_attnfs.entries[i].atom_id;
        }
    }

#ifdef PLATFORM_WINDOWS
    LeaveCriticalSection(&g_attnfs.lock);
#else
    pthread_mutex_unlock(&g_attnfs.lock);
#endif

    return (int)count;
}

/*
 * Handle write to /attention/threshold
 */
int
attnfs_set_threshold(int16_t threshold)
{
    if (!g_attnfs.initialized)
        return -1;

    g_attnfs.bank.af_threshold = threshold;
    return 0;
}

/*
 * Handle read from /attention/threshold
 */
int16_t
attnfs_get_threshold(void)
{
    return g_attnfs.bank.af_threshold;
}

/*
 * Handle write to /attention/rent
 * Collects rent from all atoms (reduces STI)
 */
int
attnfs_rent_collection(void)
{
    if (!g_attnfs.initialized)
        return -1;

#ifdef PLATFORM_WINDOWS
    EnterCriticalSection(&g_attnfs.lock);
#else
    pthread_mutex_lock(&g_attnfs.lock);
#endif

    int collected = 0;
    for (uint32_t i = 0; i < ATTNFS_MAX_ATOMS; i++) {
        if (g_attnfs.entries[i].active && g_attnfs.entries[i].sti > 0) {
            int16_t rent = g_attnfs.entries[i].sti / 10; /* 10% rent */
            if (rent < 1) rent = 1;
            g_attnfs.entries[i].sti -= rent;
            g_attnfs.bank.sti_funds += rent;
            g_attnfs.bank.total_sti -= rent;
            collected += rent;
        }
    }

    g_attnfs.total_rent_collections++;

#ifdef PLATFORM_WINDOWS
    LeaveCriticalSection(&g_attnfs.lock);
#else
    pthread_mutex_unlock(&g_attnfs.lock);
#endif

    return collected;
}

/*
 * Handle read from /attention/stats
 */
int
attnfs_stats(char *buf, size_t maxlen)
{
    if (!g_attnfs.initialized)
        return -1;

    /* Count atoms in focus */
    int in_focus = 0;
    for (uint32_t i = 0; i < ATTNFS_MAX_ATOMS; i++) {
        if (g_attnfs.entries[i].active &&
            g_attnfs.entries[i].sti >= g_attnfs.bank.af_threshold)
            in_focus++;
    }

    return snprintf(buf, maxlen,
        "Attention Filesystem Statistics\n"
        "  Tracked atoms: %u\n"
        "  Atoms in focus: %d\n"
        "  AF threshold: %d\n"
        "  STI funds: %d\n"
        "  LTI funds: %d\n"
        "  Total STI: %d\n"
        "  Total stimulations: %d\n"
        "  Total spreads: %d\n"
        "  Total rent collections: %d\n",
        g_attnfs.entry_count, in_focus,
        g_attnfs.bank.af_threshold,
        g_attnfs.bank.sti_funds,
        g_attnfs.bank.lti_funds,
        g_attnfs.bank.total_sti,
        g_attnfs.total_stimulations,
        g_attnfs.total_spreads,
        g_attnfs.total_rent_collections);
}
