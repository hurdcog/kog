/* 9pfs.h - 9P filesystem translator header
 *
 * Copyright (C) 2024 Free Software Foundation, Inc.
 *
 * This file provides the main definitions for the 9P filesystem
 * translator for GNU Hurd.
 */

#ifndef _9PFS_H
#define _9PFS_H

#include <hurd/netfs.h>
#include <pthread.h>
#include "9p.h"

/* Node-specific information */
struct netnode
{
    /* 9P connection and FID information */
    struct p9_connection *conn;    /* 9P connection */
    struct p9_fid *fid;           /* 9P file identifier */
    
    /* File information */
    struct p9_qid qid;            /* 9P QID */
    struct p9_stat stat;          /* 9P stat information */
    
    /* Hurd-specific information */
    pthread_mutex_t lock;         /* Node lock */
    int refs;                     /* Reference count */
    
    /* Caching */
    time_t stat_cache_time;       /* Last stat update time */
    int stat_cache_valid;         /* Stat cache validity */
};

/* Global 9P filesystem state */
extern struct p9_connection *p9_conn;
extern struct p9_namespace *p9_ns;
extern char *p9_server_addr;
extern int p9_server_port;
extern char *p9_username;
extern char *p9_attach_name;

/* Function declarations */
error_t p9fs_init(void);
void p9fs_shutdown(void);

/* Node operations */
struct node *p9fs_make_node(struct p9_fid *fid);
void p9fs_free_node(struct node *node);
error_t p9fs_refresh_node(struct node *node);

/* Directory operations */
error_t p9fs_dir_lookup(struct node *dir, const char *name, struct node **node);
error_t p9fs_dir_readdir(struct node *dir, char **entries, size_t *count);

/* File operations */
error_t p9fs_file_read(struct node *node, off_t offset, size_t count, 
                       void *buf, size_t *bytes_read);
error_t p9fs_file_write(struct node *node, off_t offset, size_t count,
                        const void *buf, size_t *bytes_written);

/* Utility functions */
error_t p9_to_hurd_error(int p9_error);
mode_t p9_to_hurd_mode(uint32_t p9_mode);
uint32_t hurd_to_p9_mode(mode_t mode);

#endif /* _9PFS_H */