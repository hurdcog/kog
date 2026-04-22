/*
 * cogserver.c - CogServer Kernel Module for Inferno
 *
 * Implements CogServer as a fundamental kernel service.
 * Provides network-accessible cognitive operations over the Styx (9P) protocol.
 * Exposes /dev/cogserver for application-level access.
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

#include "cogserver.h"

/*
 * CogServer state
 */
typedef struct CogServer CogServer;
typedef struct CogSession CogSession;
typedef struct CogRequest CogRequest;

struct CogRequest {
    int   type;          /* Request type */
    char  *payload;      /* Request payload */
    int   paylen;        /* Payload length */
    char  *response;     /* Response buffer */
    int   resplen;       /* Response length */
};

struct CogSession {
    int    id;           /* Session identifier */
    Chan   *chan;        /* Channel for this session */
    Lock   lock;        /* Session lock */
};

struct CogServer {
    Lock    lock;        /* Server lock */
    int     port;        /* Listening port */
    int     nsessions;   /* Active session count */
    int     maxsessions; /* Maximum sessions */
};

enum {
    /* CogServer request types */
    COG_REQ_QUERY    = 0,  /* AtomSpace query */
    COG_REQ_CREATE   = 1,  /* Create atom */
    COG_REQ_DELETE   = 2,  /* Delete atom */
    COG_REQ_SETTV    = 3,  /* Set truth value */
    COG_REQ_GETTV    = 4,  /* Get truth value */
    COG_REQ_STIMULATE= 5,  /* Stimulate atom */
    COG_REQ_REASON   = 6,  /* Execute reasoning */
    COG_REQ_STATS    = 7,  /* Get statistics */
};

enum {
    /* /dev/cogserver QIDs */
    Qdir,
    Qctl,
    Qstatus,
    Qsessions,
    Qrequest,
};

static Dirtab cogserverdir[] = {
    ".",        {Qdir, 0, QTDIR},  0,  0555,
    "ctl",      {Qctl},            0,  0666,
    "status",   {Qstatus},         0,  0444,
    "sessions", {Qsessions},       0,  0444,
    "request",  {Qrequest},        0,  0666,
};

static CogServer *cogserver;

/*
 * Initialize CogServer kernel module
 */
void
cogserverinit(void)
{
    cogserver = malloc(sizeof(CogServer));
    if(cogserver == nil)
        panic("cogserverinit: out of memory");

    cogserver->port = 17001;  /* Default CogServer port */
    cogserver->nsessions = 0;
    cogserver->maxsessions = 128;

    print("CogServer kernel module initialized (port %d)\n", cogserver->port);
}

/*
 * Handle a CogServer request
 */
static int
cogserverhandle(CogRequest *req)
{
    char buf[4096];
    int n;

    switch(req->type) {
    case COG_REQ_QUERY:
        /* Forward query to AtomSpace */
        n = snprint(buf, sizeof buf, "query-result:ok\n");
        req->response = malloc(n + 1);
        if(req->response != nil) {
            memmove(req->response, buf, n);
            req->response[n] = 0;
            req->resplen = n;
        }
        return 0;

    case COG_REQ_STATS:
        n = snprint(buf, sizeof buf,
            "sessions %d\nport %d\n",
            cogserver->nsessions, cogserver->port);
        req->response = malloc(n + 1);
        if(req->response != nil) {
            memmove(req->response, buf, n);
            req->response[n] = 0;
            req->resplen = n;
        }
        return 0;

    default:
        req->response = nil;
        req->resplen = 0;
        return -1;
    }
}

/*
 * /dev/cogserver device operations
 */

static Chan*
cogserverattach(char *spec)
{
    return devattach('C', spec);
}

static Walkqid*
cogserverwalk(Chan *c, Chan *nc, char **name, int nname)
{
    return devwalk(c, nc, name, nname, cogserverdir, nelem(cogserverdir), devgen);
}

static int
cogserverstat(Chan *c, uchar *db, int n)
{
    return devstat(c, db, n, cogserverdir, nelem(cogserverdir), devgen);
}

static Chan*
cogserveropen(Chan *c, int omode)
{
    return devopen(c, omode, cogserverdir, nelem(cogserverdir), devgen);
}

static void
cogserverclose(Chan *c)
{
    USED(c);
}

static long
cogserverread(Chan *c, void *buf, long n, vlong offset)
{
    char *p = buf;

    switch((ulong)c->qid.path) {
    case Qdir:
        return devdirread(c, buf, n, cogserverdir, nelem(cogserverdir), devgen);

    case Qstatus:
        if(cogserver != nil)
            return snprint(p, n, "running\nport %d\nsessions %d\n",
                cogserver->port, cogserver->nsessions);
        return snprint(p, n, "not initialized\n");

    case Qsessions:
        if(cogserver != nil)
            return snprint(p, n, "active %d\nmax %d\n",
                cogserver->nsessions, cogserver->maxsessions);
        return 0;

    default:
        error(Egreg);
    }
    return 0;
}

static long
cogserverwrite(Chan *c, void *buf, long n, vlong offset)
{
    char *cmd = buf;
    USED(offset);

    switch((ulong)c->qid.path) {
    case Qctl:
        if(strncmp(cmd, "port", 4) == 0) {
            int port = atoi(cmd + 5);
            if(port > 0 && port < 65536 && cogserver != nil)
                cogserver->port = port;
        } else if(strncmp(cmd, "stop", 4) == 0) {
            print("CogServer: stop requested\n");
        } else if(strncmp(cmd, "start", 5) == 0) {
            print("CogServer: start requested\n");
        }
        return n;

    case Qrequest:
        /* Parse and handle cognitive request */
        /* Format: "type payload" */
        if(cogserver != nil) {
            CogRequest req;
            req.type = atoi(cmd);
            req.payload = cmd;
            req.paylen = n;
            req.response = nil;
            req.resplen = 0;
            cogserverhandle(&req);
            if(req.response != nil)
                free(req.response);
        }
        return n;

    default:
        error(Eperm);
    }
    return 0;
}

Dev cogserverdevtab = {
    'C',
    "cogserver",

    devreset,
    cogserverinit,
    devshutdown,
    cogserverattach,
    cogserverwalk,
    cogserverstat,
    cogserveropen,
    devcreate,
    cogserverclose,
    cogserverread,
    devbread,
    cogserverwrite,
    devbwrite,
    devremove,
    devwstat,
};
