/*
 * AtomSpace Kernel Module Implementation
 * 
 * Core implementation of the AtomSpace hypergraph database as an Inferno
 * kernel module. This provides the foundational cognitive data structure
 * for the OpenCog Inferno AGI operating system.
 */

#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "atomspace.h"

/* Global AtomSpace instance */
static AtomSpace *global_atomspace = nil;

/* Hash table implementation for atom storage */
#define ATOM_TABLE_SIZE 65536

typedef struct AtomTableEntry {
    u32int atom_id;
    Atom *atom;
    struct AtomTableEntry *next;
} AtomTableEntry;

typedef struct AtomTable {
    AtomTableEntry *buckets[ATOM_TABLE_SIZE];
    Lock lock;
} AtomTable;

/*
 * Initialize the AtomSpace kernel module
 * Called during kernel initialization
 */
void
atomspace_init(void)
{
    print("AtomSpace: Initializing kernel module\n");
    
    global_atomspace = atomspace_create();
    if (global_atomspace == nil) {
        panic("AtomSpace: Failed to create global atomspace");
    }
    
    print("AtomSpace: Kernel module initialized\n");
}

/*
 * Shutdown the AtomSpace kernel module
 * Called during kernel shutdown
 */
void
atomspace_shutdown(void)
{
    print("AtomSpace: Shutting down kernel module\n");
    
    if (global_atomspace != nil) {
        atomspace_destroy(global_atomspace);
        global_atomspace = nil;
    }
    
    print("AtomSpace: Kernel module shutdown complete\n");
}

/*
 * Create a new AtomSpace instance
 */
AtomSpace*
atomspace_create(void)
{
    AtomSpace *as;
    AtomTable *table;
    int i;
    
    as = mallocz(sizeof(AtomSpace), 1);
    if (as == nil) {
        return nil;
    }
    
    table = mallocz(sizeof(AtomTable), 1);
    if (table == nil) {
        free(as);
        return nil;
    }
    
    /* Initialize hash table buckets */
    for (i = 0; i < ATOM_TABLE_SIZE; i++) {
        table->buckets[i] = nil;
    }
    
    as->next_id = 1;
    as->atom_count = 0;
    as->node_count = 0;
    as->link_count = 0;
    as->atom_table = table;
    as->name_index = nil;  /* TODO: Implement name index */
    as->type_index = nil;  /* TODO: Implement type index */
    
    return as;
}

/*
 * Destroy an AtomSpace instance
 */
void
atomspace_destroy(AtomSpace *as)
{
    AtomTable *table;
    AtomTableEntry *entry, *next;
    int i;
    
    if (as == nil) {
        return;
    }
    
    table = (AtomTable*)as->atom_table;
    
    /* Free all atoms in the hash table */
    for (i = 0; i < ATOM_TABLE_SIZE; i++) {
        entry = table->buckets[i];
        while (entry != nil) {
            next = entry->next;
            
            /* Free atom data */
            if (entry->atom != nil) {
                if (entry->atom->data != nil) {
                    free(entry->atom->data);
                }
                free(entry->atom);
            }
            
            free(entry);
            entry = next;
        }
    }
    
    free(table);
    free(as);
}

/*
 * Hash function for atom IDs
 */
static u32int
atom_hash(u32int atom_id)
{
    return atom_id % ATOM_TABLE_SIZE;
}

/*
 * Get an atom by ID
 */
Atom*
atomspace_get_atom(AtomSpace *as, u32int atom_id)
{
    AtomTable *table;
    AtomTableEntry *entry;
    u32int hash;
    
    if (as == nil || atom_id == 0) {
        return nil;
    }
    
    table = (AtomTable*)as->atom_table;
    hash = atom_hash(atom_id);
    
    lock(&table->lock);
    
    entry = table->buckets[hash];
    while (entry != nil) {
        if (entry->atom_id == atom_id) {
            unlock(&table->lock);
            return entry->atom;
        }
        entry = entry->next;
    }
    
    unlock(&table->lock);
    return nil;
}

/*
 * Add a node to the AtomSpace
 */
u32int
atomspace_add_node(AtomSpace *as, u16int type, char *name)
{
    AtomTable *table;
    AtomTableEntry *entry;
    Atom *atom;
    NodeData *data;
    u32int atom_id, hash;
    
    if (as == nil || name == nil) {
        return 0;
    }
    
    /* Allocate atom structure */
    atom = mallocz(sizeof(Atom), 1);
    if (atom == nil) {
        return 0;
    }
    
    /* Allocate node data */
    data = mallocz(sizeof(NodeData), 1);
    if (data == nil) {
        free(atom);
        return 0;
    }
    
    data->name = strdup(name);
    if (data->name == nil) {
        free(data);
        free(atom);
        return 0;
    }
    
    /* Allocate hash table entry */
    entry = mallocz(sizeof(AtomTableEntry), 1);
    if (entry == nil) {
        free(data->name);
        free(data);
        free(atom);
        return 0;
    }
    
    /* Get next atom ID */
    lock(&as->lock);
    atom_id = as->next_id++;
    unlock(&as->lock);
    
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
    table = (AtomTable*)as->atom_table;
    hash = atom_hash(atom_id);
    
    lock(&table->lock);
    
    entry->atom_id = atom_id;
    entry->atom = atom;
    entry->next = table->buckets[hash];
    table->buckets[hash] = entry;
    
    unlock(&table->lock);
    
    /* Update statistics */
    lock(&as->lock);
    as->atom_count++;
    as->node_count++;
    unlock(&as->lock);
    
    return atom_id;
}

/*
 * Add a link to the AtomSpace
 */
u32int
atomspace_add_link(AtomSpace *as, u16int type, u32int *outgoing, u32int arity)
{
    AtomTable *table;
    AtomTableEntry *entry;
    Atom *atom;
    LinkData *data;
    u32int atom_id, hash;
    int i;
    
    if (as == nil || outgoing == nil || arity == 0) {
        return 0;
    }
    
    /* Allocate atom structure */
    atom = mallocz(sizeof(Atom), 1);
    if (atom == nil) {
        return 0;
    }
    
    /* Allocate link data */
    data = mallocz(sizeof(LinkData), 1);
    if (data == nil) {
        free(atom);
        return 0;
    }
    
    /* Allocate outgoing array */
    data->outgoing = mallocz(arity * sizeof(u32int), 1);
    if (data->outgoing == nil) {
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
    entry = mallocz(sizeof(AtomTableEntry), 1);
    if (entry == nil) {
        free(data->outgoing);
        free(data);
        free(atom);
        return 0;
    }
    
    /* Get next atom ID */
    lock(&as->lock);
    atom_id = as->next_id++;
    unlock(&as->lock);
    
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
    table = (AtomTable*)as->atom_table;
    hash = atom_hash(atom_id);
    
    lock(&table->lock);
    
    entry->atom_id = atom_id;
    entry->atom = atom;
    entry->next = table->buckets[hash];
    table->buckets[hash] = entry;
    
    unlock(&table->lock);
    
    /* Update incoming counts for outgoing atoms */
    for (i = 0; i < arity; i++) {
        Atom *target = atomspace_get_atom(as, outgoing[i]);
        if (target != nil) {
            target->incoming++;
        }
    }
    
    /* Update statistics */
    lock(&as->lock);
    as->atom_count++;
    as->link_count++;
    unlock(&as->lock);
    
    return atom_id;
}

/*
 * System call: Create an atom
 */
int
atom_create(u16int type, void *data, u32int datalen)
{
    /* TODO: Validate user pointer */
    /* TODO: Copy data from user space */
    /* TODO: Call appropriate atomspace function */
    return -1;  /* Not implemented */
}

/*
 * System call: Create a link
 */
int
atom_link(u16int type, u32int *atoms, u32int count)
{
    /* TODO: Validate user pointer */
    /* TODO: Copy atoms array from user space */
    /* TODO: Call atomspace_add_link */
    return -1;  /* Not implemented */
}

/*
 * Truth Value Operations
 */
TruthValue
tv_default(void)
{
    TruthValue tv;
    tv.strength = 0.0;
    tv.confidence = 0.0;
    tv.count = 0;
    return tv;
}

TruthValue
tv_create(float strength, float confidence)
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
AttentionValue
av_default(void)
{
    AttentionValue av;
    av.sti = 0;
    av.lti = 0;
    av.vlti = 0;
    return av;
}

AttentionValue
av_create(s16int sti, s16int lti, s16int vlti)
{
    AttentionValue av;
    av.sti = sti;
    av.lti = lti;
    av.vlti = vlti;
    return av;
}

/*
 * Utility: Convert atom type to string
 */
char*
atom_type_to_string(u16int type)
{
    switch (type) {
    case ATOM_TYPE_NODE:
        return "Node";
    case ATOM_TYPE_CONCEPT:
        return "ConceptNode";
    case ATOM_TYPE_PREDICATE:
        return "PredicateNode";
    case ATOM_TYPE_LINK:
        return "Link";
    case ATOM_TYPE_INHERITANCE:
        return "InheritanceLink";
    case ATOM_TYPE_SIMILARITY:
        return "SimilarityLink";
    default:
        return "UnknownType";
    }
}

/*
 * Print AtomSpace statistics
 */
void
print_atomspace_stats(AtomSpace *as)
{
    if (as == nil) {
        return;
    }
    
    print("AtomSpace Statistics:\n");
    print("  Total atoms: %ud\n", as->atom_count);
    print("  Nodes: %ud\n", as->node_count);
    print("  Links: %ud\n", as->link_count);
    print("  Next ID: %ud\n", as->next_id);
}
