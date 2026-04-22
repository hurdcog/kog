/* 9p.h - Plan9 9P Protocol Interface for GNU Hurd
 *
 * Copyright (C) 2024 Free Software Foundation, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#ifndef _9P_H
#define _9P_H

#include <sys/types.h>
#include <stdint.h>

/* 9P Protocol Version */
#define P9_VERSION "9P2000"

/* 9P Message Types */
#define P9_TVERSION    100
#define P9_RVERSION    101
#define P9_TAUTH       102
#define P9_RAUTH       103
#define P9_TATTACH     104
#define P9_RATTACH     105
#define P9_TERROR      106  /* Invalid */
#define P9_RERROR      107
#define P9_TFLUSH      108
#define P9_RFLUSH      109
#define P9_TWALK       110
#define P9_RWALK       111
#define P9_TOPEN       112
#define P9_ROPEN       113
#define P9_TCREATE     114
#define P9_RCREATE     115
#define P9_TREAD       116
#define P9_RREAD       117
#define P9_TWRITE      118
#define P9_RWRITE      119
#define P9_TCLUNK      120
#define P9_RCLUNK      121
#define P9_TREMOVE     122
#define P9_RREMOVE     123
#define P9_TSTAT       124
#define P9_RSTAT       125
#define P9_TWSTAT      126
#define P9_RWSTAT      127

/* 9P Open Modes */
#define P9_OREAD       0x00
#define P9_OWRITE      0x01
#define P9_ORDWR       0x02
#define P9_OEXEC       0x03
#define P9_OTRUNC      0x10
#define P9_ORCLOSE     0x40

/* 9P QID Types */
#define P9_QTDIR       0x80
#define P9_QTAPPEND    0x40
#define P9_QTEXCL      0x20
#define P9_QTMOUNT     0x10
#define P9_QTAUTH      0x08
#define P9_QTTMP       0x04
#define P9_QTFILE      0x00

/* 9P Directory Mode Bits */
#define P9_DMDIR       0x80000000
#define P9_DMAPPEND    0x40000000
#define P9_DMEXCL      0x20000000
#define P9_DMMOUNT     0x10000000
#define P9_DMAUTH      0x08000000
#define P9_DMTMP       0x04000000

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

/* Basic 9P data structures */

struct p9_qid {
    uint8_t type;    /* File type */
    uint32_t version; /* Version number */
    uint64_t path;   /* Unique path identifier */
};

struct p9_stat {
    uint16_t type;   /* Server type */
    uint32_t dev;    /* Server device */
    struct p9_qid qid; /* File QID */
    uint32_t mode;   /* Permissions */
    uint32_t atime;  /* Access time */
    uint32_t mtime;  /* Modification time */
    uint64_t length; /* File length */
    char *name;      /* File name */
    char *uid;       /* Owner user ID */
    char *gid;       /* Group ID */
    char *muid;      /* Last modifier user ID */
};

/* Forward declarations */
struct p9_connection;
struct p9_fid;

/* Core 9P Functions */
struct p9_connection *p9_connect(const char *address, uint16_t port);
void p9_disconnect(struct p9_connection *conn);
int p9_version(struct p9_connection *conn, const char *version, uint32_t msize);
const char *p9_get_version(struct p9_connection *conn);
uint32_t p9_get_msize(struct p9_connection *conn);
struct p9_fid *p9_attach(struct p9_connection *conn, const char *uname, 
                         const char *aname);
struct p9_fid *p9_walk(struct p9_fid *fid, const char **names, int nnames);
int p9_open(struct p9_fid *fid, uint8_t mode);
int p9_create(struct p9_fid *fid, const char *name, uint32_t perm, uint8_t mode);
ssize_t p9_read(struct p9_fid *fid, void *buf, size_t count, uint64_t offset);
ssize_t p9_write(struct p9_fid *fid, const void *buf, size_t count, uint64_t offset);
int p9_clunk(struct p9_fid *fid);
int p9_remove(struct p9_fid *fid);
int p9_stat(struct p9_fid *fid, struct p9_stat *stat);
int p9_wstat(struct p9_fid *fid, const struct p9_stat *stat);

/* Server-side Functions */
struct p9_server;
typedef int (*p9_handler_t)(struct p9_connection *conn, void *request, void *response);

struct p9_server *p9_server_create(const char *address, uint16_t port);
void p9_server_destroy(struct p9_server *server);
int p9_server_run(struct p9_server *server);
void p9_server_stop(struct p9_server *server);
int p9_server_register_handler(struct p9_server *server, uint8_t type, 
                               p9_handler_t handler);

/* Namespace Functions */
struct p9_namespace;
struct p9_namespace *p9_namespace_create(void);
void p9_namespace_destroy(struct p9_namespace *ns);
int p9_namespace_mount(struct p9_namespace *ns, const char *path, 
                       struct p9_connection *conn);
int p9_namespace_unmount(struct p9_namespace *ns, const char *path);

/* Authentication Functions */
struct p9_auth;
struct p9_auth *p9_auth_create(const char *method);
void p9_auth_destroy(struct p9_auth *auth);
int p9_auth_challenge(struct p9_auth *auth, const void *chal, size_t chal_len,
                      void *resp, size_t *resp_len);

/* Utility functions */
void p9_init_qid(struct p9_qid *qid, uint8_t type, uint32_t version, uint64_t path);

/* Error handling */
extern int p9_errno;
const char *p9_strerror(int error);

#endif /* _9P_H */