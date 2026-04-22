/*
 * attention.c - Attention Kernel Module for Inferno
 *
 * Implements the Economic Attention Network (ECAN) as a kernel service.
 * Exposes /dev/attention for application-level attention management.
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

#include "attention.h"

/*
 * Attention bank state
 */
typedef struct AttentionBank AttentionBank;

struct AttentionBank {
    Lock  lock;
    int   total_sti;     /* Total STI in the system */
    int   total_lti;     /* Total LTI in the system */
    int   af_boundary;   /* Attentional focus boundary */
    int   af_size;       /* Target attentional focus size */
    float rent_rate;     /* Fraction of STI collected as rent */
    float wage_rate;     /* STI wage for active atoms */
};

static AttentionBank *attnbank;

enum {
    Qdir,
    Qctl,
    Qstatus,
    Qfocus,
    Qspread,
};

static Dirtab attentiondir[] = {
    ".",      {Qdir, 0, QTDIR},  0,  0555,
    "ctl",    {Qctl},            0,  0666,
    "status", {Qstatus},         0,  0444,
    "focus",  {Qfocus},          0,  0444,
    "spread", {Qspread},         0,  0222,
};

/*
 * Initialize attention kernel module
 */
void
attentioninit(void)
{
    attnbank = malloc(sizeof(AttentionBank));
    if(attnbank == nil)
        panic("attentioninit: out of memory");

    attnbank->total_sti  = 0;
    attnbank->total_lti  = 0;
    attnbank->af_boundary = 0;
    attnbank->af_size     = 100;
    attnbank->rent_rate   = 0.05f;
    attnbank->wage_rate   = 1.0f;

    print("Attention kernel module initialized\n");
}

/*
 * Run one full attention cycle:
 *   1. Collect rent from all atoms above boundary
 *   2. Pay wages to perception/action atoms
 *   3. Update AF boundary
 */
void
attentioncycle(void)
{
    if(attnbank == nil)
        return;

    lock(&attnbank->lock);
    /* Decay global STI pool */
    attnbank->total_sti = (int)(attnbank->total_sti * (1.0f - attnbank->rent_rate));
    unlock(&attnbank->lock);

    /* Delegate per-atom decay to spreading module */
    atomdecayattn(attnbank->rent_rate);
}

/*
 * Get attentional focus boundary
 */
int
getafboundary(void)
{
    if(attnbank == nil)
        return 0;
    return attnbank->af_boundary;
}

/*
 * /dev/attention device operations
 */

static Chan*
attentionattach(char *spec)
{
    return devattach('T', spec);
}

static Walkqid*
attentionwalk(Chan *c, Chan *nc, char **name, int nname)
{
    return devwalk(c, nc, name, nname, attentiondir, nelem(attentiondir), devgen);
}

static int
attentionstat(Chan *c, uchar *db, int n)
{
    return devstat(c, db, n, attentiondir, nelem(attentiondir), devgen);
}

static Chan*
attentionopen(Chan *c, int omode)
{
    return devopen(c, omode, attentiondir, nelem(attentiondir), devgen);
}

static void
attentionclose(Chan *c)
{
    USED(c);
}

static long
attentionread(Chan *c, void *buf, long n, vlong offset)
{
    char *p = buf;

    switch((ulong)c->qid.path) {
    case Qdir:
        return devdirread(c, buf, n, attentiondir, nelem(attentiondir), devgen);

    case Qstatus:
        if(attnbank != nil)
            return snprint(p, n,
                "total_sti %d\ntotal_lti %d\naf_boundary %d\naf_size %d\n",
                attnbank->total_sti, attnbank->total_lti,
                attnbank->af_boundary, attnbank->af_size);
        return snprint(p, n, "not initialized\n");

    case Qfocus:
        /* Return IDs of atoms in the attentional focus */
        return snprint(p, n, "af_boundary %d\n", getafboundary());

    default:
        error(Egreg);
    }
    return 0;
}

static long
attentionwrite(Chan *c, void *buf, long n, vlong offset)
{
    char *cmd = buf;
    USED(offset);

    switch((ulong)c->qid.path) {
    case Qctl:
        if(strncmp(cmd, "cycle", 5) == 0) {
            attentioncycle();
        } else if(strncmp(cmd, "afsize", 6) == 0) {
            int sz = atoi(cmd + 7);
            if(sz > 0 && attnbank != nil)
                attnbank->af_size = sz;
        } else if(strncmp(cmd, "rentrate", 8) == 0) {
            float r = (float)atof(cmd + 9);
            if(r >= 0.0f && r <= 1.0f && attnbank != nil)
                attnbank->rent_rate = r;
        }
        return n;

    case Qspread:
        /* Format: "atom_id amount" */
        if(attnbank != nil) {
            ulong id = (ulong)atoi(cmd);
            char *sp = strchr(cmd, ' ');
            if(sp != nil) {
                short amount = (short)atoi(sp + 1);
                atomstimulate(id, amount);
                lock(&attnbank->lock);
                attnbank->total_sti += amount;
                unlock(&attnbank->lock);
            }
        }
        return n;

    default:
        error(Eperm);
    }
    return 0;
}

Dev attentiondevtab = {
    'T',
    "attention",

    devreset,
    attentioninit,
    devshutdown,
    attentionattach,
    attentionwalk,
    attentionstat,
    attentionopen,
    devcreate,
    attentionclose,
    attentionread,
    devbread,
    attentionwrite,
    devbwrite,
    devremove,
    devwstat,
};
