/* ops.c - File operations for 9P filesystem
 *
 * Copyright (C) 2024 Free Software Foundation, Inc.
 *
 * This file implements file and directory operations for the 9P filesystem.
 */

#include "9pfs.h"
#include <string.h>

/* Directory lookup */
error_t
p9fs_dir_lookup(struct node *dir, const char *name, struct node **node)
{
    struct netnode *nn = dir->nn;
    struct p9_fid *new_fid;
    const char *names[1];
    
    if (!S_ISDIR(dir->nn_stat.st_mode))
        return ENOTDIR;
    
    /* Handle current directory */
    if (strcmp(name, ".") == 0) {
        netfs_nref(dir);
        *node = dir;
        return 0;
    }
    
    /* Walk to the named file */
    names[0] = name;
    new_fid = p9_walk(nn->fid, names, 1);
    if (!new_fid)
        return p9_to_hurd_error(p9_errno);
    
    /* Create new node */
    *node = p9fs_make_node(new_fid);
    if (!*node) {
        p9_clunk(new_fid);
        return ENOMEM;
    }
    
    /* Refresh node information */
    return p9fs_refresh_node(*node);
}

/* Directory readdir (stub) */
error_t
p9fs_dir_readdir(struct node *dir, char **entries, size_t *count)
{
    (void)dir;
    (void)entries;
    (void)count;
    
    /* This would require implementing directory reading with 9P
     * For now, return empty directory */
    *entries = NULL;
    *count = 0;
    return 0;
}

/* File read */
error_t
p9fs_file_read(struct node *node, off_t offset, size_t count, 
               void *buf, size_t *bytes_read)
{
    struct netnode *nn = node->nn;
    ssize_t result;
    
    if (S_ISDIR(node->nn_stat.st_mode))
        return EISDIR;
    
    if (!nn->fid)
        return EBADF;
    
    /* Ensure file is open for reading */
    if (!nn->fid->open) {
        if (p9_open(nn->fid, P9_OREAD) < 0)
            return p9_to_hurd_error(p9_errno);
    }
    
    /* Read data */
    result = p9_read(nn->fid, buf, count, offset);
    if (result < 0)
        return p9_to_hurd_error(p9_errno);
    
    *bytes_read = result;
    return 0;
}

/* File write */
error_t
p9fs_file_write(struct node *node, off_t offset, size_t count,
                const void *buf, size_t *bytes_written)
{
    struct netnode *nn = node->nn;
    ssize_t result;
    
    if (S_ISDIR(node->nn_stat.st_mode))
        return EISDIR;
    
    if (!nn->fid)
        return EBADF;
    
    /* Ensure file is open for writing */
    if (!nn->fid->open) {
        if (p9_open(nn->fid, P9_OWRITE) < 0)
            return p9_to_hurd_error(p9_errno);
    }
    
    /* Write data */
    result = p9_write(nn->fid, buf, count, offset);
    if (result < 0)
        return p9_to_hurd_error(p9_errno);
    
    *bytes_written = result;
    return 0;
}