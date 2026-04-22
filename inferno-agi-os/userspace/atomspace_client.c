/*
 * AtomSpace Client Library
 *
 * High-level userspace API for AtomSpace operations.
 * Wraps the cognitive filesystem client with type-safe
 * AtomSpace-specific operations.
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Forward declarations from cogfs_client */
typedef struct CogFS CogFS;
typedef struct CogAtom CogAtom;

extern CogFS* cogfs_init(const char *root);
extern void   cogfs_close(CogFS *cfs);
extern int    cogfs_create_atom(CogFS *cfs, const char *type, const char *name,
                                float strength, float confidence);
extern int    cogfs_get_atom(CogFS *cfs, uint32_t atom_id, CogAtom *atom);
extern int    cogfs_delete_atom(CogFS *cfs, uint32_t atom_id);
extern int    cogfs_set_truth_value(CogFS *cfs, uint32_t atom_id,
                                    float strength, float confidence);
extern int    cogfs_query_atoms(CogFS *cfs, const char *type_filter,
                                uint32_t *atom_ids, uint32_t max_results);
extern int    cogfs_get_stats(CogFS *cfs, char *buf, size_t maxlen);

/* AtomSpace atom types */
#define ATOM_CONCEPT_NODE       "ConceptNode"
#define ATOM_PREDICATE_NODE     "PredicateNode"
#define ATOM_SCHEMA_NODE        "SchemaNode"
#define ATOM_GROUNDED_SCHEMA    "GroundedSchemaNode"
#define ATOM_NUMBER_NODE        "NumberNode"
#define ATOM_VARIABLE_NODE      "VariableNode"
#define ATOM_INHERITANCE_LINK   "InheritanceLink"
#define ATOM_EVALUATION_LINK    "EvaluationLink"
#define ATOM_LIST_LINK          "ListLink"
#define ATOM_AND_LINK           "AndLink"
#define ATOM_OR_LINK            "OrLink"
#define ATOM_NOT_LINK           "NotLink"
#define ATOM_BIND_LINK          "BindLink"
#define ATOM_IMPLICATION_LINK   "ImplicationLink"
#define ATOM_EXECUTION_LINK     "ExecutionLink"
#define ATOM_MEMBER_LINK        "MemberLink"
#define ATOM_SIMILARITY_LINK    "SimilarityLink"

/* AtomSpace client handle */
typedef struct AtomSpaceClient {
    CogFS *cfs;
    int    auto_close;
} AtomSpaceClient;

/*
 * Create an AtomSpace client connected to the cognitive filesystem
 */
AtomSpaceClient*
atomspace_client_create(const char *cogfs_root)
{
    AtomSpaceClient *asc = (AtomSpaceClient *)calloc(1, sizeof(AtomSpaceClient));
    if (asc == NULL)
        return NULL;

    asc->cfs = cogfs_init(cogfs_root);
    if (asc->cfs == NULL) {
        free(asc);
        return NULL;
    }
    asc->auto_close = 1;

    return asc;
}

/*
 * Create an AtomSpace client from an existing CogFS handle
 */
AtomSpaceClient*
atomspace_client_from_cogfs(CogFS *cfs)
{
    AtomSpaceClient *asc = (AtomSpaceClient *)calloc(1, sizeof(AtomSpaceClient));
    if (asc == NULL)
        return NULL;

    asc->cfs = cfs;
    asc->auto_close = 0;

    return asc;
}

/*
 * Destroy the AtomSpace client
 */
void
atomspace_client_destroy(AtomSpaceClient *asc)
{
    if (asc != NULL) {
        if (asc->auto_close && asc->cfs != NULL)
            cogfs_close(asc->cfs);
        free(asc);
    }
}

/*
 * Add a ConceptNode to the AtomSpace
 */
int
atomspace_add_concept(AtomSpaceClient *asc, const char *name,
                      float strength, float confidence)
{
    return cogfs_create_atom(asc->cfs, ATOM_CONCEPT_NODE, name,
                             strength, confidence);
}

/*
 * Add a PredicateNode to the AtomSpace
 */
int
atomspace_add_predicate(AtomSpaceClient *asc, const char *name,
                        float strength, float confidence)
{
    return cogfs_create_atom(asc->cfs, ATOM_PREDICATE_NODE, name,
                             strength, confidence);
}

/*
 * Add a SchemaNode to the AtomSpace
 */
int
atomspace_add_schema(AtomSpaceClient *asc, const char *name,
                     float strength, float confidence)
{
    return cogfs_create_atom(asc->cfs, ATOM_SCHEMA_NODE, name,
                             strength, confidence);
}

/*
 * Add a NumberNode to the AtomSpace
 */
int
atomspace_add_number(AtomSpaceClient *asc, double value,
                     float strength, float confidence)
{
    char name[64];
    snprintf(name, sizeof(name), "%g", value);
    return cogfs_create_atom(asc->cfs, ATOM_NUMBER_NODE, name,
                             strength, confidence);
}

/*
 * Add an InheritanceLink between two atoms
 */
int
atomspace_add_inheritance(AtomSpaceClient *asc, uint32_t child, uint32_t parent,
                          float strength, float confidence)
{
    char name[128];
    snprintf(name, sizeof(name), "%u:%u", child, parent);
    return cogfs_create_atom(asc->cfs, ATOM_INHERITANCE_LINK, name,
                             strength, confidence);
}

/*
 * Add a SimilarityLink between two atoms
 */
int
atomspace_add_similarity(AtomSpaceClient *asc, uint32_t a, uint32_t b,
                         float strength, float confidence)
{
    char name[128];
    snprintf(name, sizeof(name), "%u:%u", a, b);
    return cogfs_create_atom(asc->cfs, ATOM_SIMILARITY_LINK, name,
                             strength, confidence);
}

/*
 * Add a MemberLink (set membership)
 */
int
atomspace_add_member(AtomSpaceClient *asc, uint32_t member, uint32_t set,
                     float strength, float confidence)
{
    char name[128];
    snprintf(name, sizeof(name), "%u:%u", member, set);
    return cogfs_create_atom(asc->cfs, ATOM_MEMBER_LINK, name,
                             strength, confidence);
}

/*
 * Add an ImplicationLink
 */
int
atomspace_add_implication(AtomSpaceClient *asc, uint32_t antecedent,
                          uint32_t consequent,
                          float strength, float confidence)
{
    char name[128];
    snprintf(name, sizeof(name), "%u:%u", antecedent, consequent);
    return cogfs_create_atom(asc->cfs, ATOM_IMPLICATION_LINK, name,
                             strength, confidence);
}

/*
 * Get all ConceptNodes
 */
int
atomspace_get_concepts(AtomSpaceClient *asc, uint32_t *ids, uint32_t max)
{
    return cogfs_query_atoms(asc->cfs, ATOM_CONCEPT_NODE, ids, max);
}

/*
 * Get all InheritanceLinks
 */
int
atomspace_get_inheritances(AtomSpaceClient *asc, uint32_t *ids, uint32_t max)
{
    return cogfs_query_atoms(asc->cfs, ATOM_INHERITANCE_LINK, ids, max);
}

/*
 * Get atom details
 */
int
atomspace_get_atom(AtomSpaceClient *asc, uint32_t id, CogAtom *atom)
{
    return cogfs_get_atom(asc->cfs, id, atom);
}

/*
 * Remove atom
 */
int
atomspace_remove_atom(AtomSpaceClient *asc, uint32_t id)
{
    return cogfs_delete_atom(asc->cfs, id);
}

/*
 * Set truth value
 */
int
atomspace_set_tv(AtomSpaceClient *asc, uint32_t id,
                 float strength, float confidence)
{
    return cogfs_set_truth_value(asc->cfs, id, strength, confidence);
}

/*
 * Print AtomSpace statistics
 */
void
atomspace_print_stats(AtomSpaceClient *asc)
{
    char buf[4096];
    int n = cogfs_get_stats(asc->cfs, buf, sizeof(buf));
    if (n > 0)
        printf("%s\n", buf);
    else
        printf("(no statistics available)\n");
}
