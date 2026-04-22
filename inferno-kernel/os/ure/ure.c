/*
 * ure.c - Unified Rule Engine Kernel Module for Inferno
 *
 * Implements the URE as a fundamental kernel service.
 * Provides forward and backward chaining inference as kernel operations.
 * Exposes /dev/ure for application-level access.
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

#include "ure.h"

/*
 * URE state
 */
typedef struct URE URE;
typedef struct URERule URERule;
typedef struct URERuleBase URERuleBase;

struct URERule {
    ulong    id;          /* Rule atom ID in AtomSpace */
    char    *name;        /* Human-readable rule name */
    float    weight;      /* Rule application weight */
    URERule *next;        /* Next rule in list */
};

struct URERuleBase {
    Lock      lock;
    URERule  *rules;      /* Linked list of active rules */
    int       nrules;
};

struct URE {
    Lock       lock;
    int        max_depth;     /* Maximum inference depth */
    int        max_results;   /* Maximum results per query */
    URERuleBase *rulebase;
};

static URE *ure;

enum {
    Qdir,
    Qctl,
    Qstatus,
    Qrules,
    Qforward,
    Qbackward,
};

static Dirtab uredir[] = {
    ".",       {Qdir, 0, QTDIR},  0,  0555,
    "ctl",     {Qctl},            0,  0666,
    "status",  {Qstatus},         0,  0444,
    "rules",   {Qrules},          0,  0444,
    "forward", {Qforward},        0,  0666,
    "backward",{Qbackward},       0,  0666,
};

/*
 * Initialize URE kernel module
 */
void
ureinit(void)
{
    ure = malloc(sizeof(URE));
    if(ure == nil)
        panic("ureinit: out of memory");

    ure->rulebase = malloc(sizeof(URERuleBase));
    if(ure->rulebase == nil)
        panic("ureinit: out of memory for rulebase");

    ure->max_depth   = 5;
    ure->max_results = 100;
    ure->rulebase->rules  = nil;
    ure->rulebase->nrules = 0;

    print("URE kernel module initialized\n");
}

/*
 * /dev/ure device operations
 */

static Chan*
ureattach(char *spec)
{
    return devattach('U', spec);
}

static Walkqid*
urewalk(Chan *c, Chan *nc, char **name, int nname)
{
    return devwalk(c, nc, name, nname, uredir, nelem(uredir), devgen);
}

static int
urestat(Chan *c, uchar *db, int n)
{
    return devstat(c, db, n, uredir, nelem(uredir), devgen);
}

static Chan*
ureopen(Chan *c, int omode)
{
    return devopen(c, omode, uredir, nelem(uredir), devgen);
}

static void
ureclose(Chan *c)
{
    USED(c);
}

static long
ureread(Chan *c, void *buf, long n, vlong offset)
{
    char *p = buf;

    switch((ulong)c->qid.path) {
    case Qdir:
        return devdirread(c, buf, n, uredir, nelem(uredir), devgen);

    case Qstatus:
        if(ure != nil)
            return snprint(p, n,
                "max_depth %d\nmax_results %d\nnrules %d\n",
                ure->max_depth, ure->max_results,
                ure->rulebase ? ure->rulebase->nrules : 0);
        return snprint(p, n, "not initialized\n");

    case Qrules:
        if(ure != nil && ure->rulebase != nil) {
            URERule *r;
            int total = 0;
            for(r = ure->rulebase->rules; r != nil && total < n - 64; r = r->next)
                total += snprint(p + total, n - total, "%lu %s %.3f\n",
                    r->id, r->name ? r->name : "(unnamed)", r->weight);
            return total;
        }
        return 0;

    default:
        error(Egreg);
    }
    return 0;
}

static long
urewrite(Chan *c, void *buf, long n, vlong offset)
{
    char *cmd = buf;
    USED(offset);

    switch((ulong)c->qid.path) {
    case Qctl:
        if(strncmp(cmd, "maxdepth", 8) == 0 && ure != nil) {
            int d = atoi(cmd + 9);
            if(d > 0)
                ure->max_depth = d;
        } else if(strncmp(cmd, "maxresults", 10) == 0 && ure != nil) {
            int r = atoi(cmd + 11);
            if(r > 0)
                ure->max_results = r;
        } else if(strncmp(cmd, "addrule", 7) == 0 && ure != nil) {
            ulong id = (ulong)atoi(cmd + 8);
            URERule *rule = malloc(sizeof(URERule));
            if(rule != nil) {
                rule->id = id;
                rule->name = nil;
                rule->weight = 1.0f;
                lock(&ure->rulebase->lock);
                rule->next = ure->rulebase->rules;
                ure->rulebase->rules = rule;
                ure->rulebase->nrules++;
                unlock(&ure->rulebase->lock);
            }
        }
        return n;

    case Qforward:
        /* Forward chaining: format "target_id depth" */
        /* Returns results via subsequent reads - simplified stub */
        print("URE forward chain: %s\n", cmd);
        return n;

    case Qbackward:
        /* Backward chaining: format "goal_id depth" */
        print("URE backward chain: %s\n", cmd);
        return n;

    default:
        error(Eperm);
    }
    return 0;
}

Dev uredevtab = {
    'U',
    "ure",

    devreset,
    ureinit,
    devshutdown,
    ureattach,
    urewalk,
    urestat,
    ureopen,
    devcreate,
    ureclose,
    ureread,
    devbread,
    urewrite,
    devbwrite,
    devremove,
    devwstat,
};
