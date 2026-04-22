/*
 * AtomSpace Kernel Module for Inferno
 * 
 * Implements AtomSpace as a fundamental kernel service
 * Atoms become first-class kernel objects
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

#include "atomspace.h"

/*
 * AtomSpace kernel data structures
 */

typedef struct Atom Atom;
typedef struct Link Link;
typedef struct TruthValue TruthValue;
typedef struct AttentionValue AttentionValue;
typedef struct AtomSpace AtomSpace;

struct TruthValue {
    float strength;      /* Confidence in truth (0.0 - 1.0) */
    float confidence;    /* Confidence in the measurement (0.0 - 1.0) */
};

struct AttentionValue {
    short sti;          /* Short-term importance */
    short lti;          /* Long-term importance */
    short vlti;         /* Very long-term importance */
};

struct Atom {
    ulong id;           /* Unique atom identifier */
    ushort type;        /* Atom type */
    char *name;         /* Atom name (for nodes) */
    TruthValue tv;      /* Truth value */
    AttentionValue av;  /* Attention value */
    Link **incoming;    /* Incoming links */
    uint nincoming;     /* Number of incoming links */
    Atom **outgoing;    /* Outgoing atoms (for links) */
    uint noutgoing;     /* Number of outgoing atoms */
    Lock lock;          /* Atom lock */
};

struct Link {
    Atom atom;          /* Link is a special type of atom */
    Atom **targets;     /* Target atoms */
    uint ntargets;      /* Number of targets */
};

struct AtomSpace {
    Lock lock;          /* AtomSpace lock */
    Atom **atoms;       /* Hash table of atoms */
    uint natoms;        /* Number of atoms */
    uint maxatoms;      /* Maximum atoms */
    ulong nextid;       /* Next atom ID */
};

/*
 * Global AtomSpace instance (kernel-wide)
 */
static AtomSpace *kernelspace;

/*
 * Atom type definitions
 */
enum {
    ATOM_TYPE_NODE = 0,
    ATOM_TYPE_LINK = 1,
    ATOM_TYPE_CONCEPT_NODE = 2,
    ATOM_TYPE_PREDICATE_NODE = 3,
    ATOM_TYPE_VARIABLE_NODE = 4,
    ATOM_TYPE_INHERITANCE_LINK = 5,
    ATOM_TYPE_SIMILARITY_LINK = 6,
    ATOM_TYPE_EVALUATION_LINK = 7,
};

/*
 * Initialize AtomSpace kernel module
 */
void
atomspaceinit(void)
{
    kernelspace = malloc(sizeof(AtomSpace));
    if(kernelspace == nil)
        panic("atomspaceinit: out of memory");
    
    kernelspace->maxatoms = 1024 * 1024;  /* 1M atoms initially */
    kernelspace->atoms = malloc(kernelspace->maxatoms * sizeof(Atom*));
    if(kernelspace->atoms == nil)
        panic("atomspaceinit: out of memory for atom table");
    
    kernelspace->natoms = 0;
    kernelspace->nextid = 1;
    
    print("AtomSpace kernel module initialized\n");
}

/*
 * Create a new atom
 */
ulong
atomcreate(ushort type, char *name)
{
    Atom *atom;
    ulong id;
    
    if(kernelspace == nil)
        error(Enomem);
    
    lock(&kernelspace->lock);
    
    if(kernelspace->natoms >= kernelspace->maxatoms) {
        unlock(&kernelspace->lock);
        error("atomspace full");
    }
    
    atom = malloc(sizeof(Atom));
    if(atom == nil) {
        unlock(&kernelspace->lock);
        error(Enomem);
    }
    
    id = kernelspace->nextid++;
    atom->id = id;
    atom->type = type;
    
    if(name != nil) {
        atom->name = malloc(strlen(name) + 1);
        if(atom->name != nil)
            strcpy(atom->name, name);
    } else {
        atom->name = nil;
    }
    
    /* Initialize truth value */
    atom->tv.strength = 0.5;
    atom->tv.confidence = 0.0;
    
    /* Initialize attention value */
    atom->av.sti = 0;
    atom->av.lti = 0;
    atom->av.vlti = 0;
    
    /* Initialize links */
    atom->incoming = nil;
    atom->nincoming = 0;
    atom->outgoing = nil;
    atom->noutgoing = 0;
    
    /* Add to atom table */
    kernelspace->atoms[kernelspace->natoms++] = atom;
    
    unlock(&kernelspace->lock);
    
    return id;
}

/*
 * Get atom by ID
 */
Atom*
atomget(ulong id)
{
    Atom *atom;
    uint i;
    
    if(kernelspace == nil)
        return nil;
    
    lock(&kernelspace->lock);
    
    for(i = 0; i < kernelspace->natoms; i++) {
        atom = kernelspace->atoms[i];
        if(atom->id == id) {
            unlock(&kernelspace->lock);
            return atom;
        }
    }
    
    unlock(&kernelspace->lock);
    return nil;
}

/*
 * Set truth value
 */
int
atomsettv(ulong id, float strength, float confidence)
{
    Atom *atom;
    
    atom = atomget(id);
    if(atom == nil)
        return -1;
    
    lock(&atom->lock);
    atom->tv.strength = strength;
    atom->tv.confidence = confidence;
    unlock(&atom->lock);
    
    return 0;
}

/*
 * Get truth value
 */
int
atomgettv(ulong id, float *strength, float *confidence)
{
    Atom *atom;
    
    atom = atomget(id);
    if(atom == nil)
        return -1;
    
    lock(&atom->lock);
    *strength = atom->tv.strength;
    *confidence = atom->tv.confidence;
    unlock(&atom->lock);
    
    return 0;
}

/*
 * Stimulate atom (increase attention)
 */
int
atomstimulate(ulong id, short amount)
{
    Atom *atom;
    
    atom = atomget(id);
    if(atom == nil)
        return -1;
    
    lock(&atom->lock);
    atom->av.sti += amount;
    unlock(&atom->lock);
    
    return 0;
}

/*
 * Create link between atoms
 */
ulong
linkcreate(ushort type, ulong *targets, uint ntargets)
{
    Link *link;
    Atom *atom;
    ulong id;
    uint i;
    
    if(kernelspace == nil || targets == nil || ntargets == 0)
        error(Ebadarg);
    
    /* Create link as special atom */
    id = atomcreate(type, nil);
    atom = atomget(id);
    if(atom == nil)
        error(Enomem);
    
    /* Allocate link structure */
    link = (Link*)atom;
    link->targets = malloc(ntargets * sizeof(Atom*));
    if(link->targets == nil) {
        /* TODO: Clean up atom */
        error(Enomem);
    }
    
    link->ntargets = ntargets;
    
    /* Set up outgoing connections */
    atom->outgoing = malloc(ntargets * sizeof(Atom*));
    atom->noutgoing = ntargets;
    
    for(i = 0; i < ntargets; i++) {
        Atom *target = atomget(targets[i]);
        if(target == nil) {
            /* TODO: Clean up */
            error("target atom not found");
        }
        link->targets[i] = target;
        atom->outgoing[i] = target;
        
        /* Add to target's incoming links */
        /* TODO: Implement incoming link management */
    }
    
    return id;
}

/*
 * Query atoms by type
 */
int
atomquery(ushort type, ulong *results, uint maxresults)
{
    uint i, count;
    Atom *atom;
    
    if(kernelspace == nil || results == nil)
        return -1;
    
    lock(&kernelspace->lock);
    
    count = 0;
    for(i = 0; i < kernelspace->natoms && count < maxresults; i++) {
        atom = kernelspace->atoms[i];
        if(atom->type == type) {
            results[count++] = atom->id;
        }
    }
    
    unlock(&kernelspace->lock);
    
    return count;
}

/*
 * Get atom statistics
 */
void
atomstats(void)
{
    if(kernelspace == nil) {
        print("AtomSpace not initialized\n");
        return;
    }
    
    lock(&kernelspace->lock);
    print("AtomSpace Statistics:\n");
    print("  Total atoms: %ud\n", kernelspace->natoms);
    print("  Max atoms: %ud\n", kernelspace->maxatoms);
    print("  Next ID: %uld\n", kernelspace->nextid);
    unlock(&kernelspace->lock);
}

/*
 * /dev/atomspace device operations
 */

enum {
    Qdir,
    Qatoms,
    Qctl,
    Qstats,
};

static Dirtab atomspacedir[] = {
    ".",        {Qdir, 0, QTDIR},   0,  0555,
    "atoms",    {Qatoms},           0,  0666,
    "ctl",      {Qctl},             0,  0222,
    "stats",    {Qstats},           0,  0444,
};

static Chan*
atomspaceattach(char *spec)
{
    return devattach('A', spec);
}

static Walkqid*
atomspacewalk(Chan *c, Chan *nc, char **name, int nname)
{
    return devwalk(c, nc, name, nname, atomspacedir, nelem(atomspacedir), devgen);
}

static int
atomspacestat(Chan *c, uchar *db, int n)
{
    return devstat(c, db, n, atomspacedir, nelem(atomspacedir), devgen);
}

static Chan*
atomspaceopen(Chan *c, int omode)
{
    return devopen(c, omode, atomspacedir, nelem(atomspacedir), devgen);
}

static void
atomspaceclose(Chan *c)
{
    USED(c);
}

static long
atomspaceread(Chan *c, void *buf, long n, vlong offset)
{
    char *p;
    
    switch((ulong)c->qid.path) {
    case Qdir:
        return devdirread(c, buf, n, atomspacedir, nelem(atomspacedir), devgen);
    
    case Qstats:
        /* Return atomspace statistics */
        p = buf;
        if(kernelspace != nil) {
            snprint(p, n, "atoms %ud\nmax %ud\nnextid %uld\n",
                kernelspace->natoms, kernelspace->maxatoms, kernelspace->nextid);
        } else {
            snprint(p, n, "not initialized\n");
        }
        return strlen(p);
    
    case Qatoms:
        /* TODO: Return atom data */
        return 0;
    
    default:
        error(Egreg);
    }
    
    return 0;
}

static long
atomspacewrite(Chan *c, void *buf, long n, vlong offset)
{
    char *cmd;
    
    USED(offset);
    
    switch((ulong)c->qid.path) {
    case Qctl:
        /* Parse control commands */
        cmd = buf;
        /* TODO: Implement command parsing */
        if(strncmp(cmd, "create", 6) == 0) {
            /* Create atom */
        } else if(strncmp(cmd, "delete", 6) == 0) {
            /* Delete atom */
        } else if(strncmp(cmd, "settv", 5) == 0) {
            /* Set truth value */
        }
        return n;
    
    case Qatoms:
        /* Write atom data */
        /* TODO: Parse and create atoms */
        return n;
    
    default:
        error(Eperm);
    }
    
    return 0;
}

Dev atomspacedevtab = {
    'A',
    "atomspace",
    
    devreset,
    atomspaceinit,
    devshutdown,
    atomspaceattach,
    atomspacewalk,
    atomspacestat,
    atomspaceopen,
    devcreate,
    atomspaceclose,
    atomspaceread,
    devbread,
    atomspacewrite,
    devbwrite,
    devremove,
    devwstat,
};
