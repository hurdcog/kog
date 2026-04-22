/* Atomspace Filesystem C Bindings
 * Integration layer between Scheme atomspace filesystem and GNU Hurd
 * Part of Phase 3: Build System Orchestration
 */

#ifndef ATOMSPACE_FS_BINDINGS_H
#define ATOMSPACE_FS_BINDINGS_H

#include <hurd.h>
#include <hurd/fs.h>
#include <hurd/fsys.h>
#include <libnetfs/netfs.h>
#include <mach.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/* Atomspace filesystem operations structure */
typedef struct atomspace_fs_ops {
    error_t (*read)(void *filesystem, const char *path, char **buffer, size_t *size);
    error_t (*write)(void *filesystem, const char *path, const char *content, size_t size);
    error_t (*mkdir)(void *filesystem, const char *path, mode_t mode);
    error_t (*rmdir)(void *filesystem, const char *path);
    error_t (*list)(void *filesystem, const char *path, char ***entries, size_t *count);
    error_t (*stat)(void *filesystem, const char *path, struct stat *st);
    const char* (*type)(void *filesystem);
} atomspace_fs_ops_t;

/* Atomspace filesystem instance */
typedef struct atomspace_filesystem {
    void *scheme_filesystem;  /* Pointer to Scheme filesystem object */
    atomspace_fs_ops_t ops;   /* Operations table */
    char *mount_point;        /* Mount point path */
    size_t partition_size;    /* Partition size in bytes */
    size_t partition_offset;  /* Partition offset in bytes */
    int features;             /* Feature flags */
} atomspace_filesystem_t;

/* Feature flags */
#define ATOMSPACE_FS_DISTRIBUTED_STORAGE   (1 << 0)
#define ATOMSPACE_FS_PARALLEL_COMPUTING    (1 << 1)
#define ATOMSPACE_FS_COGNITIVE_OPERATIONS  (1 << 2)
#define ATOMSPACE_FS_PLAN9_NAMESPACE       (1 << 3)
#define ATOMSPACE_FS_INFERNO_FEATURES      (1 << 4)

/* Function prototypes */

/* Initialize atomspace filesystem */
error_t atomspace_fs_init(atomspace_filesystem_t **filesystem);

/* Mount atomspace filesystem */
error_t atomspace_fs_mount(atomspace_filesystem_t *filesystem, 
                           const char *mount_point, 
                           const char *options);

/* Unmount atomspace filesystem */
error_t atomspace_fs_unmount(atomspace_filesystem_t *filesystem);

/* Filesystem operations */
error_t atomspace_fs_read_impl(atomspace_filesystem_t *filesystem, 
                               const char *path, 
                               char **buffer, 
                               size_t *size);

error_t atomspace_fs_write_impl(atomspace_filesystem_t *filesystem, 
                                const char *path, 
                                const char *content, 
                                size_t size);

error_t atomspace_fs_mkdir_impl(atomspace_filesystem_t *filesystem, 
                                const char *path, 
                                mode_t mode);

error_t atomspace_fs_rmdir_impl(atomspace_filesystem_t *filesystem, 
                                const char *path);

error_t atomspace_fs_list_impl(atomspace_filesystem_t *filesystem, 
                               const char *path, 
                               char ***entries, 
                               size_t *count);

error_t atomspace_fs_stat_impl(atomspace_filesystem_t *filesystem, 
                               const char *path, 
                               struct stat *st);

/* Cognitive operations interface */
error_t atomspace_fs_cognitive_op(atomspace_filesystem_t *filesystem,
                                  const char *operation_type,
                                  const char *params);

/* Performance monitoring */
error_t atomspace_fs_get_stats(atomspace_filesystem_t *filesystem,
                               size_t *num_atoms,
                               size_t *num_links,
                               size_t *memory_usage);

/* Distributed operations */
error_t atomspace_fs_replicate(atomspace_filesystem_t *filesystem,
                               const char *target_node);

/* Plan9/Inferno namespace operations */
error_t atomspace_fs_namespace_bind(atomspace_filesystem_t *filesystem,
                                    const char *local_path,
                                    const char *remote_path);

/* Parallel operations */
error_t atomspace_fs_parallel_op(atomspace_filesystem_t *filesystem,
                                 const char *operation,
                                 const char **data_list,
                                 size_t data_count);

/* Verification and diagnostics */
error_t atomspace_fs_verify_integration(atomspace_filesystem_t *filesystem);

/* Cleanup */
void atomspace_fs_cleanup(atomspace_filesystem_t *filesystem);

/* Error handling */
const char* atomspace_fs_strerror(error_t error);

/* Utility functions */
int atomspace_fs_has_feature(atomspace_filesystem_t *filesystem, int feature);
error_t atomspace_fs_set_feature(atomspace_filesystem_t *filesystem, int feature, int enabled);

/* Logging and debugging */
void atomspace_fs_log(const char *level, const char *format, ...);

#ifdef DEBUG
#define ATOMSPACE_FS_DEBUG(fmt, ...) atomspace_fs_log("DEBUG", fmt, ##__VA_ARGS__)
#else
#define ATOMSPACE_FS_DEBUG(fmt, ...)
#endif

#define ATOMSPACE_FS_INFO(fmt, ...)  atomspace_fs_log("INFO", fmt, ##__VA_ARGS__)
#define ATOMSPACE_FS_WARN(fmt, ...)  atomspace_fs_log("WARN", fmt, ##__VA_ARGS__)
#define ATOMSPACE_FS_ERROR(fmt, ...) atomspace_fs_log("ERROR", fmt, ##__VA_ARGS__)

#endif /* ATOMSPACE_FS_BINDINGS_H */