/* 9p-internal.h - Internal definitions for 9P protocol implementation
 *
 * Copyright (C) 2024 Free Software Foundation, Inc.
 * Internal header for lib9p implementation details
 */

#ifndef _9P_INTERNAL_H
#define _9P_INTERNAL_H

#include "9p.h"
#include <pthread.h>
#include <sys/socket.h>

/* Maximum message size */
#define P9_MAX_MSG_SIZE  8192
#define P9_MIN_MSG_SIZE  1024

/* Maximum string length in 9P */
#define P9_MAX_STRING    256

/* FID management */
#define P9_NOFID         (~0U)
#define P9_MAX_FID       65536

struct p9_connection {
    int fd;                     /* Socket file descriptor */
    uint32_t msize;             /* Maximum message size */
    uint16_t tag;               /* Current message tag */
    pthread_mutex_t lock;       /* Connection lock */
    pthread_mutex_t tag_lock;   /* Tag allocation lock */
    char version[32];           /* Protocol version */
    uint32_t fid_pool;          /* FID allocation pool */
    struct p9_fid **fid_table;  /* FID lookup table */
    pthread_mutex_t fid_lock;   /* FID table lock */
};

struct p9_fid {
    uint32_t fid;               /* File identifier */
    struct p9_connection *conn; /* Associated connection */
    struct p9_qid qid;          /* File QID */
    uint8_t mode;               /* Open mode */
    int open;                   /* Open flag */
};

struct p9_server {
    int fd;                     /* Listening socket */
    pthread_t thread;           /* Server thread */
    int running;                /* Running flag */
    p9_handler_t handlers[256]; /* Message handlers */
    pthread_mutex_t lock;       /* Server lock */
};

struct p9_message {
    uint32_t size;              /* Message size */
    uint8_t type;               /* Message type */
    uint16_t tag;               /* Message tag */
    uint8_t data[];             /* Message data */
};

struct p9_namespace {
    struct {
        char *path;             /* Mount point */
        struct p9_connection *conn; /* Connection */
    } *mounts;                  /* Mount table */
    int nmounts;                /* Number of mounts */
    int capacity;               /* Mount table capacity */
    pthread_mutex_t lock;       /* Namespace lock */
};

struct p9_auth {
    char method[64];            /* Authentication method */
    void *state;                /* Method-specific state */
};

/* Internal protocol functions */
int p9_send_message(struct p9_connection *conn, struct p9_message *msg);
struct p9_message *p9_receive_message(struct p9_connection *conn);
void p9_free_message(struct p9_message *msg);
uint16_t p9_alloc_tag(struct p9_connection *conn);
void p9_free_tag(struct p9_connection *conn, uint16_t tag);
uint32_t p9_alloc_fid(struct p9_connection *conn);
void p9_free_fid(struct p9_connection *conn, uint32_t fid);

/* Encoding/decoding functions */
int p9_encode_string(uint8_t **buf, size_t *len, const char *str);
int p9_decode_string(uint8_t **buf, size_t *len, char **str);
int p9_encode_qid(uint8_t **buf, size_t *len, const struct p9_qid *qid);
int p9_decode_qid(uint8_t **buf, size_t *len, struct p9_qid *qid);
int p9_encode_stat(uint8_t **buf, size_t *len, const struct p9_stat *stat);
int p9_decode_stat(uint8_t **buf, size_t *len, struct p9_stat *stat);

/* Utility functions */
void p9_init_qid(struct p9_qid *qid, uint8_t type, uint32_t version, uint64_t path);
void p9_free_stat(struct p9_stat *stat);
int p9_copy_stat(struct p9_stat *dest, const struct p9_stat *src);

/* Error codes */
#define P9_EIO          1       /* I/O error */
#define P9_EPROTO       2       /* Protocol error */
#define P9_ENOMEM       3       /* Out of memory */
#define P9_EINVAL       4       /* Invalid argument */
#define P9_ENOENT       5       /* No such file or directory */
#define P9_EACCES       6       /* Permission denied */
#define P9_EEXIST       7       /* File exists */
#define P9_EISDIR       8       /* Is a directory */
#define P9_ENOTDIR      9       /* Not a directory */
#define P9_EMFILE       10      /* Too many open files */

#endif /* _9P_INTERNAL_H */