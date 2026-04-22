/* 9p-auth.c - 9P authentication implementation (stub)
 *
 * Copyright (C) 2024 Free Software Foundation, Inc.
 */

#include "9p-internal.h"
#include <stdlib.h>

struct p9_auth *
p9_auth_create(const char *method)
{
    (void)method;
    p9_errno = P9_EPROTO; /* Not implemented yet */
    return NULL;
}

void
p9_auth_destroy(struct p9_auth *auth)
{
    (void)auth;
}

int
p9_auth_challenge(struct p9_auth *auth, const void *chal, size_t chal_len,
                  void *resp, size_t *resp_len)
{
    (void)auth; (void)chal; (void)chal_len; (void)resp; (void)resp_len;
    return -1;
}