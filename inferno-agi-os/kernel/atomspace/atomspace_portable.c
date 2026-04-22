/*
 * AtomSpace Portable Implementation
 * 
 * Cross-platform implementation of the AtomSpace hypergraph database
 * that can be compiled on Windows, Linux, and macOS.
 * 
 * This is a userspace-compatible version of the kernel module.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#define LOCK_TYPE CRITICAL_SECTION
#define LOCK_INIT(l) InitializeCriticalSection(&(l))
#define LOCK_ACQUIRE(l) EnterCriticalSection(&(l))
#define LOCK_RELEASE(l) LeaveCriticalSection(&(l))
#define LOCK_DESTROY(l) DeleteCriticalSection(&(l))
#else
#include <pthread.h>
#define LOCK_TYPE pthread_mutex_t
#define LOCK_INIT(l) pthread_mutex_init(&(l), NULL)
#define LOCK_ACQUIRE(l) pthread_mutex_lock(&(l))
#define LOCK_RELEASE(l) pthread_mutex_unlock(&(l))
#define LOCK_DESTROY(l) pthread_mutex_destroy(&(l))
#endif

/* Atom types */
#define ATOM_TYPE_NODE          0x0001
#define ATOM_TYPE_CONCEPT       0x0002
#define ATOM_TYPE_PREDICATE     0x0003
#define ATOM_TYPE_VARIABLE      0x0004
#define ATOM_TYPE_NUMBER        0x0005
#define ATOM_TYPE_LINK          0x0100
#define ATOM_TYPE_INHERITANCE   0x0101
#define ATOM_TYPE_SIMILARITY    0x0102
#define ATOM_TYPE_EVALUATION    0x0103
#define ATOM_TYPE_LIST          0x0104
#define ATOM_TYPE_AND           0x0105
#define ATOM_TYPE_OR            0x0106
#define ATOM_TYPE_NOT           0x0107
#define ATOM_TYPE_IMPLICATION   0x0108

/* Truth Value */
typedef struct TruthValue {
    float strength;
    float confidence;
    uint32_t count;
} TruthValue;

/* Attention Value */
typedef struct AttentionValue {
    int16_t sti;    /* Short-term importance */
    int16_t lti;    /* Long-term importance */
    int16_t vlti;   /* Very long-term importance */
} AttentionValue;

/* Atom structure */
typedef struct Atom {
    uint32_t id;
    uint16_t type;
    uint16_t flags;
    TruthValue tv;
    AttentionValue av;
    uint32_t incoming;
    uint32_t refcount;
    void *data;
    uint32_t datalen;
} Atom;

/* Node data */
typedef struct NodeData {
    char *name;
} NodeData;

/* Link data */
typedef struct LinkData {
    uint32_t *outgoing;
    uint32_t arity;
} LinkData;

/* Hash table entry */
#define ATOM_TABLE_SIZE 65536

typedef struct AtomTableEntry {
    uint32_t atom_id;
    Atom *atom;
    struct AtomTableEntry *next;
} AtomTableEntry;

/* AtomSpace structure */
typedef struct AtomSpace {
    uint32_t next_id;
    uint32_t atom_count;
    uint32_t node_count;
    uint32_t link_count;
    AtomTableEntry *buckets[ATOM_TABLE_SIZE];
    LOCK_TYPE lock;
} AtomSpace;

/* Global AtomSpace */
static AtomSpace *global_atomspace = NULL;

/* Function prototypes */
AtomSpace* atomspace_create(void);
void atomspace_destroy(AtomSpace *as);
Atom* atomspace_get_atom(AtomSpace *as, uint32_t atom_id);
uint32_t atomspace_add_node(AtomSpace *as, uint16_t type, const char *name);
uint32_t atomspace_add_link(AtomSpace *as, uint16_t type, uint32_t *outgoing, uint32_t arity);
void atomspace_remove_atom(AtomSpace *as, uint32_t atom_id);
TruthValue tv_default(void);
TruthValue tv_create(float strength, float confidence);
AttentionValue av_default(void);
AttentionValue av_create(int16_t sti, int16_t lti, int16_t vlti);

/*
 * Hash function for atom IDs
 */
static uint32_t atom_hash(uint32_t atom_id)
{
    return atom_id % ATOM_TABLE_SIZE;
}

/*
 * Initialize the AtomSpace subsystem
 */
void atomspace_init(void)
{
    printf("AtomSpace: Initializing portable module\n");
    
    global_atomspace = atomspace_create();
    if (global_atomspace == NULL) {
        fprintf(stderr, "AtomSpace: Failed to create global atomspace\n");
        exit(1);
    }
    
    printf("AtomSpace: Portable module initialized\n");
}

/*
 * Shutdown the AtomSpace subsystem
 */
void atomspace_shutdown(void)
{
    printf("AtomSpace: Shutting down portable module\n");
    
    if (global_atomspace != NULL) {
        atomspace_destroy(global_atomspace);
        global_atomspace = NULL;
    }
    
    printf("AtomSpace: Portable module shutdown complete\n");
}

/*
 * Get the global AtomSpace instance
 */
AtomSpace* get_global_atomspace(void)
{
    return global_atomspace;
}

/*
 * Create a new AtomSpace instance
 */
AtomSpace* atomspace_create(void)
{
    AtomSpace *as;
    int i;
    
    as = (AtomSpace*)calloc(1, sizeof(AtomSpace));
    if (as == NULL) {
        return NULL;
    }
    
    /* Initialize hash table buckets */
    for (i = 0; i < ATOM_TABLE_SIZE; i++) {
        as->buckets[i] = NULL;
    }
    
    as->next_id = 1;
    as->atom_count = 0;
    as->node_count = 0;
    as->link_count = 0;
    
    LOCK_INIT(as->lock);
    
    return as;
}

/*
 * Destroy an AtomSpace instance
 */
void atomspace_destroy(AtomSpace *as)
{
    AtomTableEntry *entry, *next;
    int i;
    
    if (as == NULL) {
        return;
    }
    
    /* Free all atoms in the hash table */
    for (i = 0; i < ATOM_TABLE_SIZE; i++) {
        entry = as->buckets[i];
        while (entry != NULL) {
            next = entry->next;
            
            /* Free atom data */
            if (entry->atom != NULL) {
                if (entry->atom->data != NULL) {
                    if (entry->atom->type < ATOM_TYPE_LINK) {
                        /* Node data */
                        NodeData *nd = (NodeData*)entry->atom->data;
                        free(nd->name);
                    } else {
                        /* Link data */
                        LinkData *ld = (LinkData*)entry->atom->data;
                        free(ld->outgoing);
                    }
                    free(entry->atom->data);
                }
                free(entry->atom);
            }
            
            free(entry);
            entry = next;
        }
    }
    
    LOCK_DESTROY(as->lock);
    free(as);
}

/*
 * Get an atom by ID
 */
Atom* atomspace_get_atom(AtomSpace *as, uint32_t atom_id)
{
    AtomTableEntry *entry;
    uint32_t hash;
    
    if (as == NULL || atom_id == 0) {
        return NULL;
    }
    
    hash = atom_hash(atom_id);
    
    LOCK_ACQUIRE(as->lock);
    
    entry = as->buckets[hash];
    while (entry != NULL) {
        if (entry->atom_id == atom_id) {
            LOCK_RELEASE(as->lock);
            return entry->atom;
        }
        entry = entry->next;
    }
    
    LOCK_RELEASE(as->lock);
    return NULL;
}

/*
 * Add a node to the AtomSpace
 */
uint32_t atomspace_add_node(AtomSpace *as, uint16_t type, const char *name)
{
    AtomTableEntry *entry;
    Atom *atom;
    NodeData *data;
    uint32_t atom_id, hash;
    
    if (as == NULL || name == NULL) {
        return 0;
    }
    
    /* Allocate atom structure */
    atom = (Atom*)calloc(1, sizeof(Atom));
    if (atom == NULL) {
        return 0;
    }
    
    /* Allocate node data */
    data = (NodeData*)calloc(1, sizeof(NodeData));
    if (data == NULL) {
        free(atom);
        return 0;
    }
    
    data->name = strdup(name);
    if (data->name == NULL) {
        free(data);
        free(atom);
        return 0;
    }
    
    /* Allocate hash table entry */
    entry = (AtomTableEntry*)calloc(1, sizeof(AtomTableEntry));
    if (entry == NULL) {
        free(data->name);
        free(data);
        free(atom);
        return 0;
    }
    
    /* Get next atom ID */
    LOCK_ACQUIRE(as->lock);
    atom_id = as->next_id++;
    LOCK_RELEASE(as->lock);
    
    /* Initialize atom */
    atom->id = atom_id;
    atom->type = type;
    atom->flags = 0;
    atom->tv = tv_default();
    atom->av = av_default();
    atom->incoming = 0;
    atom->refcount = 1;
    atom->data = data;
    atom->datalen = sizeof(NodeData);
    
    /* Add to hash table */
    hash = atom_hash(atom_id);
    
    LOCK_ACQUIRE(as->lock);
    
    entry->atom_id = atom_id;
    entry->atom = atom;
    entry->next = as->buckets[hash];
    as->buckets[hash] = entry;
    
    as->atom_count++;
    as->node_count++;
    
    LOCK_RELEASE(as->lock);
    
    return atom_id;
}

/*
 * Add a link to the AtomSpace
 */
uint32_t atomspace_add_link(AtomSpace *as, uint16_t type, uint32_t *outgoing, uint32_t arity)
{
    AtomTableEntry *entry;
    Atom *atom;
    LinkData *data;
    uint32_t atom_id, hash;
    uint32_t i;
    
    if (as == NULL || outgoing == NULL || arity == 0) {
        return 0;
    }
    
    /* Allocate atom structure */
    atom = (Atom*)calloc(1, sizeof(Atom));
    if (atom == NULL) {
        return 0;
    }
    
    /* Allocate link data */
    data = (LinkData*)calloc(1, sizeof(LinkData));
    if (data == NULL) {
        free(atom);
        return 0;
    }
    
    /* Allocate outgoing array */
    data->outgoing = (uint32_t*)calloc(arity, sizeof(uint32_t));
    if (data->outgoing == NULL) {
        free(data);
        free(atom);
        return 0;
    }
    
    /* Copy outgoing atoms */
    for (i = 0; i < arity; i++) {
        data->outgoing[i] = outgoing[i];
    }
    data->arity = arity;
    
    /* Allocate hash table entry */
    entry = (AtomTableEntry*)calloc(1, sizeof(AtomTableEntry));
    if (entry == NULL) {
        free(data->outgoing);
        free(data);
        free(atom);
        return 0;
    }
    
    /* Get next atom ID */
    LOCK_ACQUIRE(as->lock);
    atom_id = as->next_id++;
    LOCK_RELEASE(as->lock);
    
    /* Initialize atom */
    atom->id = atom_id;
    atom->type = type;
    atom->flags = 0;
    atom->tv = tv_default();
    atom->av = av_default();
    atom->incoming = 0;
    atom->refcount = 1;
    atom->data = data;
    atom->datalen = sizeof(LinkData);
    
    /* Add to hash table */
    hash = atom_hash(atom_id);
    
    LOCK_ACQUIRE(as->lock);
    
    entry->atom_id = atom_id;
    entry->atom = atom;
    entry->next = as->buckets[hash];
    as->buckets[hash] = entry;
    
    as->atom_count++;
    as->link_count++;
    
    LOCK_RELEASE(as->lock);
    
    /* Update incoming counts for outgoing atoms */
    for (i = 0; i < arity; i++) {
        Atom *target = atomspace_get_atom(as, outgoing[i]);
        if (target != NULL) {
            target->incoming++;
        }
    }
    
    return atom_id;
}

/*
 * Remove an atom from the AtomSpace
 */
void atomspace_remove_atom(AtomSpace *as, uint32_t atom_id)
{
    AtomTableEntry *entry, *prev;
    uint32_t hash;
    
    if (as == NULL || atom_id == 0) {
        return;
    }
    
    hash = atom_hash(atom_id);
    
    LOCK_ACQUIRE(as->lock);
    
    prev = NULL;
    entry = as->buckets[hash];
    
    while (entry != NULL) {
        if (entry->atom_id == atom_id) {
            /* Remove from list */
            if (prev == NULL) {
                as->buckets[hash] = entry->next;
            } else {
                prev->next = entry->next;
            }
            
            /* Update counts */
            if (entry->atom->type < ATOM_TYPE_LINK) {
                as->node_count--;
            } else {
                as->link_count--;
            }
            as->atom_count--;
            
            /* Free atom data */
            if (entry->atom->data != NULL) {
                if (entry->atom->type < ATOM_TYPE_LINK) {
                    NodeData *nd = (NodeData*)entry->atom->data;
                    free(nd->name);
                } else {
                    LinkData *ld = (LinkData*)entry->atom->data;
                    free(ld->outgoing);
                }
                free(entry->atom->data);
            }
            free(entry->atom);
            free(entry);
            
            LOCK_RELEASE(as->lock);
            return;
        }
        prev = entry;
        entry = entry->next;
    }
    
    LOCK_RELEASE(as->lock);
}

/*
 * Truth Value Operations
 */
TruthValue tv_default(void)
{
    TruthValue tv;
    tv.strength = 0.0f;
    tv.confidence = 0.0f;
    tv.count = 0;
    return tv;
}

TruthValue tv_create(float strength, float confidence)
{
    TruthValue tv;
    tv.strength = strength;
    tv.confidence = confidence;
    tv.count = 1;
    return tv;
}

/*
 * Attention Value Operations
 */
AttentionValue av_default(void)
{
    AttentionValue av;
    av.sti = 0;
    av.lti = 0;
    av.vlti = 0;
    return av;
}

AttentionValue av_create(int16_t sti, int16_t lti, int16_t vlti)
{
    AttentionValue av;
    av.sti = sti;
    av.lti = lti;
    av.vlti = vlti;
    return av;
}

/*
 * Get atom name (for nodes)
 */
const char* atomspace_get_name(AtomSpace *as, uint32_t atom_id)
{
    Atom *atom = atomspace_get_atom(as, atom_id);
    if (atom == NULL || atom->type >= ATOM_TYPE_LINK) {
        return NULL;
    }
    NodeData *nd = (NodeData*)atom->data;
    return nd->name;
}

/*
 * Get atom type
 */
uint16_t atomspace_get_type(AtomSpace *as, uint32_t atom_id)
{
    Atom *atom = atomspace_get_atom(as, atom_id);
    if (atom == NULL) {
        return 0;
    }
    return atom->type;
}

/*
 * Set truth value
 */
void atomspace_set_tv(AtomSpace *as, uint32_t atom_id, TruthValue tv)
{
    Atom *atom = atomspace_get_atom(as, atom_id);
    if (atom != NULL) {
        atom->tv = tv;
    }
}

/*
 * Get truth value
 */
TruthValue atomspace_get_tv(AtomSpace *as, uint32_t atom_id)
{
    Atom *atom = atomspace_get_atom(as, atom_id);
    if (atom != NULL) {
        return atom->tv;
    }
    return tv_default();
}

/*
 * Set attention value
 */
void atomspace_set_av(AtomSpace *as, uint32_t atom_id, AttentionValue av)
{
    Atom *atom = atomspace_get_atom(as, atom_id);
    if (atom != NULL) {
        atom->av = av;
    }
}

/*
 * Get attention value
 */
AttentionValue atomspace_get_av(AtomSpace *as, uint32_t atom_id)
{
    Atom *atom = atomspace_get_atom(as, atom_id);
    if (atom != NULL) {
        return atom->av;
    }
    return av_default();
}

/*
 * Print AtomSpace statistics
 */
void atomspace_print_stats(AtomSpace *as)
{
    if (as == NULL) {
        return;
    }
    
    printf("AtomSpace Statistics:\n");
    printf("  Total atoms: %u\n", as->atom_count);
    printf("  Nodes: %u\n", as->node_count);
    printf("  Links: %u\n", as->link_count);
    printf("  Next ID: %u\n", as->next_id);
}

/*
 * Convert atom type to string
 */
const char* atom_type_to_string(uint16_t type)
{
    switch (type) {
    case ATOM_TYPE_NODE:        return "Node";
    case ATOM_TYPE_CONCEPT:     return "ConceptNode";
    case ATOM_TYPE_PREDICATE:   return "PredicateNode";
    case ATOM_TYPE_VARIABLE:    return "VariableNode";
    case ATOM_TYPE_NUMBER:      return "NumberNode";
    case ATOM_TYPE_LINK:        return "Link";
    case ATOM_TYPE_INHERITANCE: return "InheritanceLink";
    case ATOM_TYPE_SIMILARITY:  return "SimilarityLink";
    case ATOM_TYPE_EVALUATION:  return "EvaluationLink";
    case ATOM_TYPE_LIST:        return "ListLink";
    case ATOM_TYPE_AND:         return "AndLink";
    case ATOM_TYPE_OR:          return "OrLink";
    case ATOM_TYPE_NOT:         return "NotLink";
    case ATOM_TYPE_IMPLICATION: return "ImplicationLink";
    default:                    return "UnknownType";
    }
}
