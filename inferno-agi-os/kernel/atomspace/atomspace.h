/*
 * AtomSpace Kernel Module for OpenCog Inferno
 * 
 * This module implements the AtomSpace hypergraph database as a kernel-level
 * service in the Inferno operating system. It provides system calls for
 * creating, querying, and manipulating atoms and their relationships.
 *
 * Copyright (c) 2025 OpenCog Inferno Project
 * Licensed under AGPL-3.0
 */

#ifndef _ATOMSPACE_H_
#define _ATOMSPACE_H_

#include <lib9.h>

/* Atom Types */
typedef enum {
    /* Node Types */
    ATOM_TYPE_NODE = 1,
    ATOM_TYPE_CONCEPT = 2,
    ATOM_TYPE_PREDICATE = 3,
    ATOM_TYPE_SCHEMA = 4,
    ATOM_TYPE_GROUNDED_SCHEMA = 5,
    ATOM_TYPE_VARIABLE = 6,
    ATOM_TYPE_NUMBER = 7,
    
    /* Link Types */
    ATOM_TYPE_LINK = 100,
    ATOM_TYPE_ORDERED_LINK = 101,
    ATOM_TYPE_UNORDERED_LINK = 102,
    ATOM_TYPE_INHERITANCE = 103,
    ATOM_TYPE_SIMILARITY = 104,
    ATOM_TYPE_IMPLICATION = 105,
    ATOM_TYPE_EQUIVALENCE = 106,
    ATOM_TYPE_EVALUATION = 107,
    ATOM_TYPE_EXECUTION = 108,
    ATOM_TYPE_MEMBER = 109,
    ATOM_TYPE_LIST = 110,
    ATOM_TYPE_AND = 111,
    ATOM_TYPE_OR = 112,
    ATOM_TYPE_NOT = 113,
    
    ATOM_TYPE_MAX = 255
} AtomType;

/* Truth Value */
typedef struct TruthValue {
    float strength;     /* Probability [0.0, 1.0] */
    float confidence;   /* Confidence [0.0, 1.0] */
    u32int count;       /* Evidence count */
} TruthValue;

/* Attention Value */
typedef struct AttentionValue {
    s16int sti;         /* Short-term importance [-32768, 32767] */
    s16int lti;         /* Long-term importance [-32768, 32767] */
    s16int vlti;        /* Very long-term importance [-32768, 32767] */
} AttentionValue;

/* Atom Structure */
typedef struct Atom {
    u32int id;          /* Unique atom identifier */
    u16int type;        /* Atom type */
    u16int flags;       /* Atom flags */
    TruthValue tv;      /* Truth value */
    AttentionValue av;  /* Attention value */
    u32int incoming;    /* Count of incoming links */
    u32int refcount;    /* Reference count */
    void *data;         /* Type-specific data */
    u32int datalen;     /* Data length */
} Atom;

/* Atom Flags */
#define ATOM_FLAG_DELETED   (1 << 0)
#define ATOM_FLAG_MARKED    (1 << 1)
#define ATOM_FLAG_CHECKED   (1 << 2)
#define ATOM_FLAG_IMMUTABLE (1 << 3)

/* Node Data */
typedef struct NodeData {
    char *name;         /* Node name */
} NodeData;

/* Link Data */
typedef struct LinkData {
    u32int arity;       /* Number of outgoing atoms */
    u32int *outgoing;   /* Array of outgoing atom IDs */
} LinkData;

/* AtomSpace Handle */
typedef struct AtomSpace {
    u32int next_id;     /* Next atom ID to allocate */
    u32int atom_count;  /* Total number of atoms */
    u32int node_count;  /* Number of nodes */
    u32int link_count;  /* Number of links */
    void *atom_table;   /* Hash table of atoms */
    void *name_index;   /* Index by name */
    void *type_index;   /* Index by type */
    Lock lock;          /* Concurrency control */
} AtomSpace;

/* Query Pattern */
typedef struct Pattern {
    u16int type;        /* Pattern type */
    char *pattern;      /* Pattern string */
    u32int max_results; /* Maximum results */
} Pattern;

/* System Calls */
int atom_create(u16int type, void *data, u32int datalen);
int atom_link(u16int type, u32int *atoms, u32int count);
int atom_query(Pattern *pattern, u32int *results, u32int max);
int atom_delete(u32int atom_id);
int atom_get(u32int atom_id, Atom *atom);
int tv_set(u32int atom_id, TruthValue *tv);
int tv_get(u32int atom_id, TruthValue *tv);
int av_set(u32int atom_id, AttentionValue *av);
int av_get(u32int atom_id, AttentionValue *av);
int atom_get_incoming(u32int atom_id, u32int *results, u32int max);
int atom_get_outgoing(u32int atom_id, u32int *results, u32int max);

/* Kernel Functions */
void atomspace_init(void);
void atomspace_shutdown(void);
AtomSpace* atomspace_create(void);
void atomspace_destroy(AtomSpace *as);
Atom* atomspace_get_atom(AtomSpace *as, u32int atom_id);
u32int atomspace_add_node(AtomSpace *as, u16int type, char *name);
u32int atomspace_add_link(AtomSpace *as, u16int type, u32int *outgoing, u32int arity);
int atomspace_remove_atom(AtomSpace *as, u32int atom_id);
int atomspace_set_tv(AtomSpace *as, u32int atom_id, TruthValue *tv);
int atomspace_get_tv(AtomSpace *as, u32int atom_id, TruthValue *tv);
int atomspace_set_av(AtomSpace *as, u32int atom_id, AttentionValue *av);
int atomspace_get_av(AtomSpace *as, u32int atom_id, AttentionValue *av);

/* Query Functions */
int atomspace_query_by_name(AtomSpace *as, char *name, u32int *results, u32int max);
int atomspace_query_by_type(AtomSpace *as, u16int type, u32int *results, u32int max);
int atomspace_query_incoming(AtomSpace *as, u32int atom_id, u32int *results, u32int max);
int atomspace_query_outgoing(AtomSpace *as, u32int atom_id, u32int *results, u32int max);

/* Utility Functions */
char* atom_type_to_string(u16int type);
u16int string_to_atom_type(char *str);
void print_atom(Atom *atom);
void print_atomspace_stats(AtomSpace *as);

/* Truth Value Operations */
TruthValue tv_default(void);
TruthValue tv_create(float strength, float confidence);
TruthValue tv_merge(TruthValue tv1, TruthValue tv2);
float tv_mean(TruthValue tv);
float tv_confidence(TruthValue tv);

/* Attention Value Operations */
AttentionValue av_default(void);
AttentionValue av_create(s16int sti, s16int lti, s16int vlti);
void av_stimulate(AttentionValue *av, s16int amount);
void av_decay(AttentionValue *av, float rate);

#endif /* _ATOMSPACE_H_ */
