/* Atomspace Filesystem C Bindings - Minimal Version
 * For testing without full Hurd environment
 */

#ifndef ATOMSPACE_FS_MINIMAL_H
#define ATOMSPACE_FS_MINIMAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

/* Minimal type definitions for testing */
typedef int error_t;

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
    void *scheme_filesystem;
    atomspace_fs_ops_t ops;
    char *mount_point;
    size_t partition_size;
    size_t partition_offset;
    int features;
} atomspace_filesystem_t;

/* Feature flags */
#define ATOMSPACE_FS_DISTRIBUTED_STORAGE   (1 << 0)
#define ATOMSPACE_FS_PARALLEL_COMPUTING    (1 << 1)
#define ATOMSPACE_FS_COGNITIVE_OPERATIONS  (1 << 2)
#define ATOMSPACE_FS_PLAN9_NAMESPACE       (1 << 3)
#define ATOMSPACE_FS_INFERNO_FEATURES      (1 << 4)

/* Function prototypes */
error_t atomspace_fs_init_minimal(atomspace_filesystem_t **filesystem);
error_t atomspace_fs_mount_minimal(atomspace_filesystem_t *filesystem, const char *mount_point, const char *options);
error_t atomspace_fs_write_minimal(atomspace_filesystem_t *filesystem, const char *path, const char *content, size_t size);
error_t atomspace_fs_read_minimal(atomspace_filesystem_t *filesystem, const char *path, char **buffer, size_t *size);
error_t atomspace_fs_mkdir_minimal(atomspace_filesystem_t *filesystem, const char *path, mode_t mode);
error_t atomspace_fs_list_minimal(atomspace_filesystem_t *filesystem, const char *path, char ***entries, size_t *count);
error_t atomspace_fs_stat_minimal(atomspace_filesystem_t *filesystem, const char *path, struct stat *st);
error_t atomspace_fs_cognitive_op_minimal(atomspace_filesystem_t *filesystem, const char *operation_type, const char *params);
error_t atomspace_fs_get_stats_minimal(atomspace_filesystem_t *filesystem, size_t *num_atoms, size_t *num_links, size_t *memory_usage);
int atomspace_fs_has_feature_minimal(atomspace_filesystem_t *filesystem, int feature);
error_t atomspace_fs_verify_integration_minimal(atomspace_filesystem_t *filesystem);
void atomspace_fs_cleanup_minimal(atomspace_filesystem_t *filesystem);
const char* atomspace_fs_strerror_minimal(error_t error);

#endif /* ATOMSPACE_FS_MINIMAL_H */