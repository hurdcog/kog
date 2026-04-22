/* 9p-test.c - Simple test program for 9P protocol implementation
 *
 * Copyright (C) 2024 Free Software Foundation, Inc.
 *
 * This program demonstrates basic 9P protocol functionality for GNU Hurd.
 */

#include "9p.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(int argc, char **argv)
{
    struct p9_connection *conn;
    struct p9_fid *root_fid, *file_fid;
    char buffer[1024];
    ssize_t bytes_read;
    const char *path[] = {"etc", "hostname"};
    int ret = 0;
    
    printf("9P Protocol Library Test\n");
    printf("========================\n\n");
    
    /* Test 1: Error handling */
    printf("Test 1: Error handling\n");
    printf("Error for code %d: %s\n\n", P9_ENOENT, p9_strerror(P9_ENOENT));
    
    /* Test 2: Namespace creation */
    printf("Test 2: Namespace creation\n");
    struct p9_namespace *ns = p9_namespace_create();
    if (ns) {
        printf("Namespace created successfully\n");
        p9_namespace_destroy(ns);
        printf("Namespace destroyed\n");
    } else {
        printf("Failed to create namespace: %s\n", p9_strerror(p9_errno));
    }
    printf("\n");
    
    /* Test 3: Connection attempt (will fail in test environment) */
    printf("Test 3: Connection test (expected to fail)\n");
    conn = p9_connect("localhost", 564);
    if (!conn) {
        printf("Connection failed as expected: %s\n", p9_strerror(p9_errno));
        printf("This is normal in a test environment without a 9P server\n");
    } else {
        printf("Unexpected connection success!\n");
        
        /* If connection works, test version negotiation */
        if (p9_version(conn, P9_VERSION, 8192) == 0) {
            printf("Version negotiation successful: %s, msize=%u\n", 
                   p9_get_version(conn), p9_get_msize(conn));
            
            /* Try to attach to root */
            root_fid = p9_attach(conn, "root", "");
            if (root_fid) {
                printf("Attached to filesystem root\n");
                
                /* Try to walk to a file */
                file_fid = p9_walk(root_fid, path, 2);
                if (file_fid) {
                    printf("Walked to /etc/hostname\n");
                    
                    /* Try to open and read */
                    if (p9_open(file_fid, P9_OREAD) == 0) {
                        printf("File opened for reading\n");
                        
                        bytes_read = p9_read(file_fid, buffer, sizeof(buffer) - 1, 0);
                        if (bytes_read > 0) {
                            buffer[bytes_read] = '\0';
                            printf("Read %zd bytes: %s\n", bytes_read, buffer);
                        }
                    }
                    
                    p9_clunk(file_fid);
                }
                
                p9_clunk(root_fid);
            }
        }
        
        p9_disconnect(conn);
    }
    
    printf("\n");
    
    /* Test 4: Data structure tests */
    printf("Test 4: Data structure tests\n");
    struct p9_qid qid;
    p9_init_qid(&qid, P9_QTFILE, 1, 12345);
    printf("QID initialized: type=0x%02x, version=%u, path=%llu\n",
           qid.type, qid.version, (unsigned long long)qid.path);
    
    printf("\nAll tests completed.\n");
    return ret;
}