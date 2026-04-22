/*
 * AtomSpace Kernel Module Header
 * 
 * Public interface for AtomSpace kernel services
 * 
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#ifndef _ATOMSPACE_H_
#define _ATOMSPACE_H_

/*
 * Atom types
 */
enum {
    ATOM_TYPE_NODE = 0,
    ATOM_TYPE_LINK = 1,
    ATOM_TYPE_CONCEPT_NODE = 2,
    ATOM_TYPE_PREDICATE_NODE = 3,
    ATOM_TYPE_VARIABLE_NODE = 4,
    ATOM_TYPE_SCHEMA_NODE = 5,
    ATOM_TYPE_ANCHOR_NODE = 6,
    
    ATOM_TYPE_INHERITANCE_LINK = 10,
    ATOM_TYPE_SIMILARITY_LINK = 11,
    ATOM_TYPE_EVALUATION_LINK = 12,
    ATOM_TYPE_IMPLICATION_LINK = 13,
    ATOM_TYPE_EQUIVALENCE_LINK = 14,
    ATOM_TYPE_AND_LINK = 15,
    ATOM_TYPE_OR_LINK = 16,
    ATOM_TYPE_NOT_LINK = 17,
};

/*
 * AtomSpace kernel API
 */

/* Initialize AtomSpace */
void atomspaceinit(void);

/* Atom operations */
ulong atomcreate(ushort type, char *name);
Atom* atomget(ulong id);
int atomdelete(ulong id);

/* Truth value operations */
int atomsettv(ulong id, float strength, float confidence);
int atomgettv(ulong id, float *strength, float *confidence);
int atommergetv(ulong id, float strength, float confidence);

/* Attention value operations */
int atomsetav(ulong id, short sti, short lti, short vlti);
int atomgetav(ulong id, short *sti, short *lti, short *vlti);
int atomstimulate(ulong id, short amount);

/* Link operations */
ulong linkcreate(ushort type, ulong *targets, uint ntargets);
int linkgettargets(ulong id, ulong *targets, uint maxtargets);

/* Query operations */
int atomquery(ushort type, ulong *results, uint maxresults);
int atomquerbyname(char *name, ulong *results, uint maxresults);

/* Statistics */
void atomstats(void);

#endif /* _ATOMSPACE_H_ */
