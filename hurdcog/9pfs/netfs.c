/* netfs.c - libnetfs interface for 9P filesystem
 *
 * Copyright (C) 2024 Free Software Foundation, Inc.
 *
 * This file implements the libnetfs interface functions required
 * for the 9P filesystem translator.
 */

#include "9pfs.h"
#include <hurd/fshelp.h>
#include <fcntl.h>
#include <string.h>

/* Attempt to create a file named NAME in DIR for USER with MODE.  Set *NODE
   to the returned file upon success.  On any error, clear *NODE.  *NODE, if
   returned, should be locked. */
error_t
netfs_attempt_create_file (struct iouser *user, struct node *dir,
                          const char *name, mode_t mode, struct node **node)
{
    (void)user; (void)dir; (void)name; (void)mode;
    *node = NULL;
    return EOPNOTSUPP; /* Not implemented yet */
}

/* Node NODE is being opened by USER, with FLAGS.  NEWNODE is nonzero if we
   just created this node.  Return an error if we should not permit the open
   to complete because of a permission restriction. */
error_t
netfs_check_open_permissions (struct iouser *user, struct node *node,
                             int flags, int newnode)
{
    error_t err;
    
    (void)newnode;
    
    err = p9fs_refresh_node(node);
    if (err)
        return err;
    
    if (flags & O_READ)
        err = fshelp_access(&node->nn_stat, S_IREAD, user);
    if (!err && (flags & O_WRITE))
        err = fshelp_access(&node->nn_stat, S_IWRITE, user);
    if (!err && (flags & O_EXEC))
        err = fshelp_access(&node->nn_stat, S_IEXEC, user);
    
    return err;
}

/* This should attempt a utimes call for the user specified by CRED on node
   NODE, to change the atime to ATIME and the mtime to MTIME. */
error_t
netfs_attempt_utimes (struct iouser *cred, struct node *node,
                     struct timespec *atime, struct timespec *mtime)
{
    (void)cred; (void)node; (void)atime; (void)mtime;
    return EOPNOTSUPP; /* Would require implementing p9_wstat */
}

/* Return the valid access types (bitwise OR of O_READ, O_WRITE, and O_EXEC)
   in *TYPES for file NODE and user CRED.  */
error_t
netfs_report_access (struct iouser *cred, struct node *node, int *types)
{
    error_t err = p9fs_refresh_node(node);
    
    if (err)
        return err;
    
    *types = 0;
    if (fshelp_access(&node->nn_stat, S_IREAD, cred) == 0)
        *types |= O_READ;
    if (fshelp_access(&node->nn_stat, S_IWRITE, cred) == 0)
        *types |= O_WRITE;
    if (fshelp_access(&node->nn_stat, S_IEXEC, cred) == 0)
        *types |= O_EXEC;
    
    return 0;
}

/* Read data from an open file.  Read up to LEN bytes from OFFSET, into DATA.
   Set *BYTES_READ to the amount actually read, which must be no more than
   LEN.  */
error_t
netfs_attempt_read (struct iouser *cred, struct node *node,
                   loff_t offset, size_t len, void *data, size_t *bytes_read)
{
    (void)cred;
    return p9fs_file_read(node, offset, len, data, bytes_read);
}

/* Write data to an open file.  Write up to LEN bytes from OFFSET to DATA.
   Set *BYTES_WRITTEN to the amount actually written, which must be no more
   than LEN.  */
error_t
netfs_attempt_write (struct iouser *cred, struct node *node,
                    loff_t offset, size_t len, const void *data, 
                    size_t *bytes_written)
{
    (void)cred;
    return p9fs_file_write(node, offset, len, data, bytes_written);
}

/* This should sync file NODE completely to disk, for the user CRED.  If
   WAIT is set, return only after sync is completely finished.  */
error_t
netfs_attempt_sync (struct iouser *cred, struct node *node, int wait)
{
    (void)cred; (void)node; (void)wait;
    return 0; /* 9P doesn't require explicit sync */
}

/* Delete NAME in DIR for USER. */
error_t
netfs_attempt_unlink (struct iouser *user, struct node *dir, const char *name)
{
    (void)user; (void)dir; (void)name;
    return EOPNOTSUPP; /* Not implemented yet */
}

/* Attempt to rename the file FROM_NAME in FROM_DIR to TO_NAME in TO_DIR,
   and excise TO_NAME if TO_EXCL is zero.  */
error_t
netfs_attempt_rename (struct iouser *user, struct node *from_dir,
                     const char *from_name, struct node *to_dir,
                     const char *to_name, int to_excl)
{
    (void)user; (void)from_dir; (void)from_name; (void)to_dir; (void)to_name; (void)to_excl;
    return EOPNOTSUPP; /* Not implemented yet */
}

/* Lookup NAME in DIR for USER; set *NODE to the found name upon return.  If
   the name was not found, then return ENOENT.  On any error, clear *NODE.
   (*NODE, if found, should be locked, this call should unlock DIR no matter
   what.)  */
error_t
netfs_attempt_lookup (struct iouser *user, struct node *dir,
                     const char *name, struct node **node)
{
    error_t err;
    
    (void)user;
    
    err = p9fs_dir_lookup(dir, name, node);
    pthread_mutex_unlock(&dir->lock);
    
    if (!err && *node)
        pthread_mutex_lock(&(*node)->lock);
    
    return err;
}

/* Delete the file NODE.  If this operation is not supported, return
   EOPNOTSUPP.  */
error_t
netfs_attempt_remove_file (struct iouser *user, struct node *dir,
                          struct node *node)
{
    (void)user; (void)dir; (void)node;
    return EOPNOTSUPP; /* Not implemented yet */
}

/* This should attempt a chmod call for the user specified by CRED on node
   NODE, to change the mode to MODE.  Unlike the normal Unix and Hurd case,
   it is permissible for this to be done by someone other than the owner of
   the file (contrary to POSIX.1), because a number of filesystems we might
   want don't permit such flexibility.  */
error_t
netfs_attempt_chmod (struct iouser *cred, struct node *node, mode_t mode)
{
    (void)cred; (void)node; (void)mode;
    return EOPNOTSUPP; /* Would require implementing p9_wstat */
}

/* Attempt to turn NODE (which must be a directory) into a symlink with target
   NAME.  */
error_t
netfs_attempt_mkdev (struct iouser *cred, struct node *node,
                    mode_t type, dev_t indexes)
{
    (void)cred; (void)node; (void)type; (void)indexes;
    return EOPNOTSUPP; /* Not supported by 9P */
}

/* This should attempt a chown call for the user specified by CRED on node
   NODE, to change the owner to UID and the group to GID.  */
error_t
netfs_attempt_chown (struct iouser *cred, struct node *node,
                    uid_t uid, uid_t gid)
{
    (void)cred; (void)node; (void)uid; (void)gid;
    return EOPNOTSUPP; /* Would require implementing p9_wstat */
}

/* This should attempt a chauthor call for the user specified by CRED on node
   NODE, to change the author to AUTHOR.  */
error_t
netfs_attempt_chauthor (struct iouser *cred, struct node *node,
                       uid_t author)
{
    (void)cred; (void)node; (void)author;
    return EOPNOTSUPP; /* Not supported by standard 9P */
}

/* This should attempt a chflags call for the user specified by CRED on node
   NODE, to change the flags to FLAGS.  */
error_t
netfs_attempt_chflags (struct iouser *cred, struct node *node, int flags)
{
    (void)cred; (void)node; (void)flags;
    return EOPNOTSUPP; /* Not supported by 9P */
}

/* This should attempt to set the size of the file NODE (for user CRED) to
   SIZE bytes long.  */
error_t
netfs_attempt_set_size (struct iouser *cred, struct node *node, loff_t size)
{
    (void)cred; (void)node; (void)size;
    return EOPNOTSUPP; /* Would require implementing truncate via p9_wstat */
}

/* This should attempt to fetch filesystem status information for the remote
   filesystem, for the user CRED. */
error_t
netfs_attempt_statfs (struct iouser *cred, struct node *node, fsys_statfsbuf_t *st)
{
    (void)cred; (void)node;
    
    /* Return generic filesystem information */
    memset(st, 0, sizeof(*st));
    st->f_type = FSTYPE_MISC;
    st->f_bsize = 4096;
    st->f_blocks = 1000000;
    st->f_bfree = 500000;
    st->f_bavail = 500000;
    st->f_files = 100000;
    st->f_ffree = 50000;
    st->f_fsid = 0x9999;  /* Magic number for 9P */
    st->f_namelen = 255;
    
    return 0;
}

/* Make sure that NP->nn_stat is filled with current information.  CRED
   identifies the user responsible for the operation.  */
error_t
netfs_validate_stat (struct node *node, struct iouser *cred)
{
    (void)cred;
    return p9fs_refresh_node(node);
}

/* This should sync the entire remote filesystem.  If WAIT is set, return
   only after sync is completely finished.  */
error_t
netfs_attempt_syncfs (struct iouser *cred, int wait)
{
    (void)cred; (void)wait;
    return 0; /* 9P doesn't require explicit filesystem sync */
}

/* Create a link in DIR with name NAME to FILE for USER.  Note that neither
   DIR nor FILE are locked.  If EXCL is set, do not delete the target, but
   return EEXIST if NAME is already found in DIR.  */
error_t
netfs_attempt_link (struct iouser *user, struct node *dir,
                   struct node *file, const char *name, int excl)
{
    (void)user; (void)dir; (void)file; (void)name; (void)excl;
    return EOPNOTSUPP; /* Not supported by 9P */
}

/* Attempt to create a new directory named NAME in DIR (which must be a
   directory) for USER with mode MODE.  */
error_t
netfs_attempt_mkdir (struct iouser *user, struct node *dir,
                    const char *name, mode_t mode)
{
    (void)user; (void)dir; (void)name; (void)mode;
    return EOPNOTSUPP; /* Not implemented yet */
}

/* Attempt to remove directory named NAME in DIR for USER. */
error_t
netfs_attempt_rmdir (struct iouser *user, struct node *dir, const char *name)
{
    (void)user; (void)dir; (void)name;
    return EOPNOTSUPP; /* Not implemented yet */
}

/* Create a file named NAME in DIR for USER with MODE.  Set *NODE to the
   returned file upon success.  On any error, clear *NODE.  *NODE, if
   returned, should be locked. */
error_t
netfs_attempt_mkfile (struct iouser *user, struct node *dir,
                     mode_t mode, struct node **node)
{
    (void)user; (void)dir; (void)mode;
    *node = NULL;
    return EOPNOTSUPP; /* Not implemented yet */
}

/* Read the contents of NODE (a symlink), for USER, into BUF. */
error_t
netfs_attempt_readlink (struct iouser *user, struct node *node, char *buf)
{
    (void)user; (void)node; (void)buf;
    return EOPNOTSUPP; /* Symlinks not implemented yet */
}

/* Read from the directory NODE (which must be locked) for USER the next NENTRIES
   entries starting with ENTRY.  If ENTRY is -1, read from the beginning of the
   directory.  Exactly what is meant by an "entry" is a matter of convention; it
   should be a unit such that no partial entries will ever be returned.  Set
   *ENTRIES to point to an array holding the results; this should be set to 0
   upon any error.  The results should be returned in malloced storage which will
   be freed by the caller.  */
error_t
netfs_get_dirents (struct iouser *cred, struct node *dir,
                  int first_entry, int num_entries, char **data,
                  mach_msg_type_number_t *data_len,
                  vm_size_t max_entries, int *data_entries)
{
    (void)cred; (void)dir; (void)first_entry; (void)num_entries;
    (void)max_entries;
    
    /* Return empty directory for now */
    *data = NULL;
    *data_len = 0;
    *data_entries = 0;
    
    return 0;
}