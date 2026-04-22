/* 9p-namespace.c - 9P namespace implementation (stub)
 *
 * Copyright (C) 2024 Free Software Foundation, Inc.
 */

#include "9p-internal.h"
#include <stdlib.h>

struct p9_namespace *
p9_namespace_create(void)
{
    struct p9_namespace *ns;
    
    ns = calloc(1, sizeof(struct p9_namespace));
    if (!ns) {
        p9_errno = P9_ENOMEM;
        return NULL;
    }
    
    pthread_mutex_init(&ns->lock, NULL);
    return ns;
}

void
p9_namespace_destroy(struct p9_namespace *ns)
{
    if (!ns)
        return;
        
    pthread_mutex_lock(&ns->lock);
    
    /* Free mount table */
    for (int i = 0; i < ns->nmounts; i++) {
        free(ns->mounts[i].path);
    }
    free(ns->mounts);
    
    pthread_mutex_unlock(&ns->lock);
    pthread_mutex_destroy(&ns->lock);
    free(ns);
}

int
p9_namespace_mount(struct p9_namespace *ns, const char *path, 
                   struct p9_connection *conn)
{
    (void)ns; (void)path; (void)conn;
    p9_errno = P9_EPROTO; /* Not implemented yet */
    return -1;
}

int
p9_namespace_unmount(struct p9_namespace *ns, const char *path)
{
    (void)ns; (void)path;
    p9_errno = P9_EPROTO; /* Not implemented yet */
    return -1;
}