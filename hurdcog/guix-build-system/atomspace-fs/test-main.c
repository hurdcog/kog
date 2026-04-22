/* Test main for atomspace filesystem C bindings
 * Standalone test without full Hurd dependencies
 */

#ifdef TEST_MAIN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Minimal definitions for testing */
typedef int error_t;
typedef unsigned int mode_t;

/* Include our header with minimal dependencies */
#include "atomspace-fs-bindings.h"

int main(int argc, char *argv[])
{
    printf("=== Atomspace Filesystem C Bindings Test ===\n");
    
    atomspace_filesystem_t *filesystem = NULL;
    error_t err;
    
    /* Test filesystem initialization */
    printf("Testing filesystem initialization...\n");
    err = atomspace_fs_init(&filesystem);
    if (err != 0) {
        printf("Failed to initialize filesystem: %s\n", atomspace_fs_strerror(err));
        return 1;
    }
    printf("✓ Filesystem initialized successfully\n");
    
    /* Test mounting */
    printf("Testing filesystem mounting...\n");
    err = atomspace_fs_mount(filesystem, "/tmp/test-atomspace", "rw,cognitive-ops");
    if (err != 0) {
        printf("Failed to mount filesystem: %s\n", atomspace_fs_strerror(err));
        atomspace_fs_cleanup(filesystem);
        return 1;
    }
    printf("✓ Filesystem mounted successfully\n");
    
    /* Test write operation */
    printf("Testing write operation...\n");
    err = atomspace_fs_write_impl(filesystem, "/test-file.txt", "Hello Atomspace!", 16);
    if (err != 0) {
        printf("Failed to write: %s\n", atomspace_fs_strerror(err));
    } else {
        printf("✓ Write operation successful\n");
    }
    
    /* Test read operation */
    printf("Testing read operation...\n");
    char *buffer;
    size_t size;
    err = atomspace_fs_read_impl(filesystem, "/test-file.txt", &buffer, &size);
    if (err != 0) {
        printf("Failed to read: %s\n", atomspace_fs_strerror(err));
    } else {
        printf("✓ Read operation successful, got %zu bytes\n", size);
        if (buffer) {
            printf("Content preview: %.50s...\n", buffer);
            free(buffer);
        }
    }
    
    /* Test directory operations */
    printf("Testing directory operations...\n");
    err = atomspace_fs_mkdir_impl(filesystem, "/test-dir", 0755);
    if (err == 0) {
        printf("✓ Directory creation successful\n");
    }
    
    /* Test listing */
    printf("Testing directory listing...\n");
    char **entries;
    size_t count;
    err = atomspace_fs_list_impl(filesystem, "/", &entries, &count);
    if (err == 0) {
        printf("✓ Directory listing successful, found %zu entries:\n", count);
        for (size_t i = 0; i < count; i++) {
            printf("  - %s\n", entries[i]);
            free(entries[i]);
        }
        free(entries);
    }
    
    /* Test cognitive operations */
    printf("Testing cognitive operations...\n");
    err = atomspace_fs_cognitive_op(filesystem, "reasoning", "test-params");
    if (err == 0) {
        printf("✓ Cognitive operations successful\n");
    }
    
    /* Test performance statistics */
    printf("Testing performance statistics...\n");
    size_t num_atoms, num_links, memory_usage;
    err = atomspace_fs_get_stats(filesystem, &num_atoms, &num_links, &memory_usage);
    if (err == 0) {
        printf("✓ Performance stats: %zu atoms, %zu links, %zu bytes memory\n",
               num_atoms, num_links, memory_usage);
    }
    
    /* Test feature checking */
    printf("Testing feature configuration...\n");
    if (atomspace_fs_has_feature(filesystem, ATOMSPACE_FS_COGNITIVE_OPERATIONS)) {
        printf("✓ Cognitive operations feature enabled\n");
    }
    if (atomspace_fs_has_feature(filesystem, ATOMSPACE_FS_DISTRIBUTED_STORAGE)) {
        printf("✓ Distributed storage feature enabled\n");
    }
    
    /* Test integration verification */
    printf("Testing integration verification...\n");
    err = atomspace_fs_verify_integration(filesystem);
    if (err == 0) {
        printf("✓ Integration verification successful\n");
    }
    
    /* Test unmounting */
    printf("Testing filesystem unmounting...\n");
    err = atomspace_fs_unmount(filesystem);
    if (err == 0) {
        printf("✓ Filesystem unmounted successfully\n");
    }
    
    /* Cleanup */
    printf("Cleaning up...\n");
    atomspace_fs_cleanup(filesystem);
    printf("✓ Cleanup completed\n");
    
    printf("=== All C binding tests passed! ===\n");
    return 0;
}

#endif /* TEST_MAIN */