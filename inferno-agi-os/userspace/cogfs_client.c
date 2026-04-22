/*
 * Cognitive Filesystem Client Library
 * 
 * Provides userspace access to the cognitive kernel via 9P filesystem.
 * All cognitive operations are performed by reading/writing files
 * in the cognitive namespace (/atoms, /reasoning, /attention, etc.)
 *
 * This follows the Inferno/Plan 9 philosophy: everything is a file.
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
#include <io.h>
#define PATH_SEP "\\"
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#define PATH_SEP "/"
#endif

/* Cognitive filesystem mount point */
#ifndef COGFS_ROOT
#define COGFS_ROOT "/cognitive"
#endif

/* Maximum path length */
#define COGFS_MAXPATH 1024

/* Maximum read buffer */
#define COGFS_MAXBUF 8192

/* Error codes */
#define COGFS_OK        0
#define COGFS_ENOENT   -1
#define COGFS_EIO      -2
#define COGFS_ENOMEM   -3
#define COGFS_EINVAL   -4
#define COGFS_EPERM    -5

/* Cognitive filesystem handle */
typedef struct CogFS {
    char root[COGFS_MAXPATH];
    int  connected;
    int  debug;
} CogFS;

/* Atom representation from filesystem */
typedef struct CogAtom {
    uint32_t id;
    char     type[64];
    char     name[256];
    float    tv_strength;
    float    tv_confidence;
    int16_t  av_sti;
    int16_t  av_lti;
} CogAtom;

/* Inference result */
typedef struct CogInferenceResult {
    uint32_t conclusion_id;
    float    tv_strength;
    float    tv_confidence;
    char     rule[64];
    int      step_count;
} CogInferenceResult;

/*
 * Initialize cognitive filesystem client
 */
CogFS*
cogfs_init(const char *root)
{
    CogFS *cfs = (CogFS *)calloc(1, sizeof(CogFS));
    if (cfs == NULL)
        return NULL;

    if (root != NULL)
        strncpy(cfs->root, root, COGFS_MAXPATH - 1);
    else
        strncpy(cfs->root, COGFS_ROOT, COGFS_MAXPATH - 1);

    cfs->connected = 1;
    cfs->debug = 0;

    if (cfs->debug)
        fprintf(stderr, "[cogfs] Initialized with root: %s\n", cfs->root);

    return cfs;
}

/*
 * Close cognitive filesystem client
 */
void
cogfs_close(CogFS *cfs)
{
    if (cfs != NULL) {
        cfs->connected = 0;
        free(cfs);
    }
}

/*
 * Enable debug output
 */
void
cogfs_set_debug(CogFS *cfs, int debug)
{
    if (cfs != NULL)
        cfs->debug = debug;
}

/*
 * Internal: build full path from root + relative path
 */
static int
cogfs_path(CogFS *cfs, const char *relpath, char *fullpath, size_t maxlen)
{
    int n = snprintf(fullpath, maxlen, "%s%s%s", cfs->root, PATH_SEP, relpath);
    if (n < 0 || (size_t)n >= maxlen)
        return COGFS_EINVAL;
    return COGFS_OK;
}

/*
 * Internal: read a file and return contents
 */
static int
cogfs_read_file(const char *path, char *buf, size_t maxlen)
{
    FILE *f = fopen(path, "r");
    if (f == NULL)
        return COGFS_ENOENT;

    size_t n = fread(buf, 1, maxlen - 1, f);
    buf[n] = '\0';
    fclose(f);

    return (int)n;
}

/*
 * Internal: write to a file
 */
static int
cogfs_write_file(const char *path, const char *data, size_t len)
{
    FILE *f = fopen(path, "w");
    if (f == NULL)
        return COGFS_EIO;

    size_t n = fwrite(data, 1, len, f);
    fclose(f);

    return (n == len) ? COGFS_OK : COGFS_EIO;
}

/* ============================================================
 * AtomSpace Operations via /atoms filesystem
 * ============================================================ */

/*
 * Create a new atom by writing to /atoms/new
 * Returns atom ID on success, negative on error
 */
int
cogfs_create_atom(CogFS *cfs, const char *type, const char *name,
                  float strength, float confidence)
{
    char path[COGFS_MAXPATH];
    char data[COGFS_MAXBUF];
    char result[COGFS_MAXBUF];

    if (cogfs_path(cfs, "atoms/new", path, sizeof(path)) != COGFS_OK)
        return COGFS_EINVAL;

    /* Write atom specification */
    snprintf(data, sizeof(data), "%s %s %f %f", type, name, strength, confidence);

    if (cogfs_write_file(path, data, strlen(data)) != COGFS_OK)
        return COGFS_EIO;

    /* Read back the assigned atom ID */
    if (cogfs_read_file(path, result, sizeof(result)) <= 0)
        return COGFS_EIO;

    return atoi(result);
}

/*
 * Get atom by ID via /atoms/<id>
 */
int
cogfs_get_atom(CogFS *cfs, uint32_t atom_id, CogAtom *atom)
{
    char path[COGFS_MAXPATH];
    char buf[COGFS_MAXBUF];
    char relpath[128];

    snprintf(relpath, sizeof(relpath), "atoms/%u", atom_id);
    if (cogfs_path(cfs, relpath, path, sizeof(path)) != COGFS_OK)
        return COGFS_EINVAL;

    int n = cogfs_read_file(path, buf, sizeof(buf));
    if (n <= 0)
        return COGFS_ENOENT;

    /* Parse atom data: "id type name strength confidence sti lti" */
    memset(atom, 0, sizeof(CogAtom));
    sscanf(buf, "%u %63s %255s %f %f %hd %hd",
           &atom->id, atom->type, atom->name,
           &atom->tv_strength, &atom->tv_confidence,
           &atom->av_sti, &atom->av_lti);

    return COGFS_OK;
}

/*
 * Delete atom by writing to /atoms/delete
 */
int
cogfs_delete_atom(CogFS *cfs, uint32_t atom_id)
{
    char path[COGFS_MAXPATH];
    char data[64];

    if (cogfs_path(cfs, "atoms/delete", path, sizeof(path)) != COGFS_OK)
        return COGFS_EINVAL;

    snprintf(data, sizeof(data), "%u", atom_id);
    return cogfs_write_file(path, data, strlen(data));
}

/*
 * Set truth value via /atoms/truth/<id>
 */
int
cogfs_set_truth_value(CogFS *cfs, uint32_t atom_id,
                      float strength, float confidence)
{
    char path[COGFS_MAXPATH];
    char data[128];
    char relpath[128];

    snprintf(relpath, sizeof(relpath), "atoms/truth/%u", atom_id);
    if (cogfs_path(cfs, relpath, path, sizeof(path)) != COGFS_OK)
        return COGFS_EINVAL;

    snprintf(data, sizeof(data), "%f %f", strength, confidence);
    return cogfs_write_file(path, data, strlen(data));
}

/*
 * Query atoms by type via /atoms/query
 */
int
cogfs_query_atoms(CogFS *cfs, const char *type_filter,
                  uint32_t *atom_ids, uint32_t max_results)
{
    char path[COGFS_MAXPATH];
    char buf[COGFS_MAXBUF];

    if (cogfs_path(cfs, "atoms/query", path, sizeof(path)) != COGFS_OK)
        return COGFS_EINVAL;

    /* Write query */
    if (cogfs_write_file(path, type_filter, strlen(type_filter)) != COGFS_OK)
        return COGFS_EIO;

    /* Read results */
    int n = cogfs_read_file(path, buf, sizeof(buf));
    if (n <= 0)
        return 0;

    /* Parse space-separated atom IDs */
    uint32_t count = 0;
    char *tok = strtok(buf, " \n");
    while (tok != NULL && count < max_results) {
        atom_ids[count++] = (uint32_t)atoi(tok);
        tok = strtok(NULL, " \n");
    }

    return (int)count;
}

/*
 * Get AtomSpace statistics via /atoms/stats
 */
int
cogfs_get_stats(CogFS *cfs, char *buf, size_t maxlen)
{
    char path[COGFS_MAXPATH];

    if (cogfs_path(cfs, "atoms/stats", path, sizeof(path)) != COGFS_OK)
        return COGFS_EINVAL;

    return cogfs_read_file(path, buf, maxlen);
}

/* ============================================================
 * Reasoning Operations via /reasoning filesystem
 * ============================================================ */

/*
 * Perform PLN inference via /reasoning/pln
 */
int
cogfs_infer_pln(CogFS *cfs, uint32_t *premises, uint32_t premise_count,
                CogInferenceResult *result)
{
    char path[COGFS_MAXPATH];
    char data[COGFS_MAXBUF];
    char buf[COGFS_MAXBUF];

    if (cogfs_path(cfs, "reasoning/pln", path, sizeof(path)) != COGFS_OK)
        return COGFS_EINVAL;

    /* Write premises as space-separated IDs */
    int offset = 0;
    for (uint32_t i = 0; i < premise_count; i++) {
        offset += snprintf(data + offset, sizeof(data) - offset, "%u ", premises[i]);
    }

    if (cogfs_write_file(path, data, strlen(data)) != COGFS_OK)
        return COGFS_EIO;

    /* Read inference result */
    int n = cogfs_read_file(path, buf, sizeof(buf));
    if (n <= 0)
        return COGFS_EIO;

    /* Parse result: "conclusion_id strength confidence rule steps" */
    memset(result, 0, sizeof(CogInferenceResult));
    sscanf(buf, "%u %f %f %63s %d",
           &result->conclusion_id,
           &result->tv_strength, &result->tv_confidence,
           result->rule, &result->step_count);

    return COGFS_OK;
}

/*
 * Perform forward chaining via /reasoning/forward
 */
int
cogfs_forward_chain(CogFS *cfs, uint32_t *initial_atoms, uint32_t count,
                    int max_steps, uint32_t *results, uint32_t max_results)
{
    char path[COGFS_MAXPATH];
    char data[COGFS_MAXBUF];
    char buf[COGFS_MAXBUF];

    if (cogfs_path(cfs, "reasoning/forward", path, sizeof(path)) != COGFS_OK)
        return COGFS_EINVAL;

    /* Write initial atoms and max_steps */
    int offset = snprintf(data, sizeof(data), "%d ", max_steps);
    for (uint32_t i = 0; i < count; i++) {
        offset += snprintf(data + offset, sizeof(data) - offset, "%u ", initial_atoms[i]);
    }

    if (cogfs_write_file(path, data, strlen(data)) != COGFS_OK)
        return COGFS_EIO;

    /* Read results */
    int n = cogfs_read_file(path, buf, sizeof(buf));
    if (n <= 0)
        return 0;

    /* Parse result atom IDs */
    uint32_t result_count = 0;
    char *tok = strtok(buf, " \n");
    while (tok != NULL && result_count < max_results) {
        results[result_count++] = (uint32_t)atoi(tok);
        tok = strtok(NULL, " \n");
    }

    return (int)result_count;
}

/* ============================================================
 * Attention Operations via /attention filesystem
 * ============================================================ */

/*
 * Stimulate atom via /attention/stimulate
 */
int
cogfs_stimulate(CogFS *cfs, uint32_t atom_id, int16_t amount)
{
    char path[COGFS_MAXPATH];
    char data[128];

    if (cogfs_path(cfs, "attention/stimulate", path, sizeof(path)) != COGFS_OK)
        return COGFS_EINVAL;

    snprintf(data, sizeof(data), "%u %d", atom_id, amount);
    return cogfs_write_file(path, data, strlen(data));
}

/*
 * Get attentional focus via /attention/focus
 */
int
cogfs_get_focus(CogFS *cfs, uint32_t *atoms, uint32_t max_atoms)
{
    char path[COGFS_MAXPATH];
    char buf[COGFS_MAXBUF];

    if (cogfs_path(cfs, "attention/focus", path, sizeof(path)) != COGFS_OK)
        return COGFS_EINVAL;

    int n = cogfs_read_file(path, buf, sizeof(buf));
    if (n <= 0)
        return 0;

    /* Parse atom IDs */
    uint32_t count = 0;
    char *tok = strtok(buf, " \n");
    while (tok != NULL && count < max_atoms) {
        atoms[count++] = (uint32_t)atoi(tok);
        tok = strtok(NULL, " \n");
    }

    return (int)count;
}

/* ============================================================
 * Distributed Cognitive Operations
 * ============================================================ */

/*
 * Connect to remote cognitive node via /net/cognitive/<host>
 */
int
cogfs_connect_node(CogFS *cfs, const char *host, int port)
{
    char path[COGFS_MAXPATH];
    char data[256];

    if (cogfs_path(cfs, "net/cognitive/connect", path, sizeof(path)) != COGFS_OK)
        return COGFS_EINVAL;

    snprintf(data, sizeof(data), "%s %d", host, port);
    return cogfs_write_file(path, data, strlen(data));
}

/*
 * Replicate atom to remote node
 */
int
cogfs_replicate_atom(CogFS *cfs, uint32_t atom_id, const char *target_node)
{
    char path[COGFS_MAXPATH];
    char data[256];

    if (cogfs_path(cfs, "net/cognitive/replicate", path, sizeof(path)) != COGFS_OK)
        return COGFS_EINVAL;

    snprintf(data, sizeof(data), "%u %s", atom_id, target_node);
    return cogfs_write_file(path, data, strlen(data));
}

/*
 * Get distributed cluster status
 */
int
cogfs_cluster_status(CogFS *cfs, char *buf, size_t maxlen)
{
    char path[COGFS_MAXPATH];

    if (cogfs_path(cfs, "net/cognitive/status", path, sizeof(path)) != COGFS_OK)
        return COGFS_EINVAL;

    return cogfs_read_file(path, buf, maxlen);
}
