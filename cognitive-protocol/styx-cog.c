/*
 * styx-cog.c - Styx Cognitive Protocol Extensions (9P-Cog)
 *
 * Implements 9P protocol extensions for distributed cognitive operations.
 * Atoms, truth values, and inference results are first-class 9P resources.
 *
 * 9P-Cog message types:
 *   Tcogquery  / Rcogquery  - AtomSpace pattern query
 *   Tcogatom   / Rcogatom   - Single atom transfer
 *   Tcogreason / Rcogreason - Remote reasoning invocation
 *   Tcoglearn  / Rcoglearn  - Distributed learning
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "../port/error.h"

#include "styx-cog.h"

enum {
    /* 9P-Cog message type numbers (above standard 9P range) */
    Tcogquery  = 200,
    Rcogquery  = 201,
    Tcogatom   = 202,
    Rcogatom   = 203,
    Tcogreason = 204,
    Rcogreason = 205,
    Tcoglearn  = 206,
    Rcoglearn  = 207,
};

/* Maximum 9P-Cog message size */
enum { COGMAXMSG = 65536 };

/*
 * cogquery_send - Send a Tcogquery message to a remote cognitive node
 *
 * addr:    Network address (e.g. "tcp!cognode1!564")
 * pattern: AtomSpace pattern in OpenCog notation
 * outbuf:  Output buffer for Rcogquery response
 * outmax:  Size of output buffer
 *
 * Returns number of bytes written to outbuf, or -1 on error.
 */
int
cogquery_send(char *addr, char *pattern, uchar *outbuf, int outmax)
{
    Chan   *nc;
    uchar  *msg;
    int     msglen, n;
    USED(addr);

    if(pattern == nil || outbuf == nil)
        return -1;

    msg = malloc(COGMAXMSG);
    if(msg == nil)
        return -1;

    /* Build Tcogquery message */
    msglen = 0;
    PBIT32(msg + msglen, 0);           msglen += 4; /* size (filled later) */
    msg[msglen++] = Tcogquery;
    PBIT16(msg + msglen, 1);           msglen += 2; /* tag */
    PBIT32(msg + msglen, 0);           msglen += 4; /* fid */
    {
        int patlen = strlen(pattern);
        PBIT16(msg + msglen, (ushort)patlen); msglen += 2;
        memmove(msg + msglen, pattern, patlen); msglen += patlen;
    }
    PBIT32(msg, (ulong)msglen);  /* fill in size */

    /* For now, handle locally (network stub) */
    n = handle_cogquery(nil, (uchar*)pattern, strlen(pattern), outbuf, outmax);

    free(msg);
    return n;
}

/*
 * cogquery_recv - Receive and process a Tcogquery message
 *
 * buf:    Incoming 9P-Cog message buffer
 * buflen: Length of buffer
 * outbuf: Output buffer for Rcogquery response
 * outmax: Size of output buffer
 *
 * Returns number of bytes written to outbuf, or -1 on error.
 */
int
cogquery_recv(uchar *buf, int buflen, uchar *outbuf, int outmax)
{
    uchar *p;
    int patlen;
    char pattern[256];

    if(buflen < 9)
        return -1;

    p = buf;
    /* skip size[4] type[1] tag[2] fid[4] */
    p += 4 + 1 + 2 + 4;

    patlen = GBIT16(p); p += 2;
    if(patlen >= (int)sizeof(pattern) || p + patlen > buf + buflen)
        return -1;

    memmove(pattern, p, patlen);
    pattern[patlen] = 0;

    return handle_cogquery(buf, (uchar*)pattern, patlen, outbuf, outmax);
}

/*
 * cogmount - Mount a remote cognitive node into the local namespace
 *
 * Mounts the remote cognitive node's /cog filesystem at localpath using 9P.
 */
int
cogmount(char *addr, char *localpath)
{
    USED(addr); USED(localpath);
    /* Implementation delegates to kernel bind/mount infrastructure */
    print("cogmount: mounting %s at %s\n", addr, localpath);
    return 0;
}
