/*
 * Attention Allocation Interface
 * Header file for ECAN attention services
 */

#ifndef _ATTENTION_H_
#define _ATTENTION_H_

/* Forward declarations */
typedef struct AtomSpace AtomSpace;
typedef struct Atom Atom;

/* Attention allocator structure */
typedef struct AttentionAllocator {
    AtomSpace *atomspace;
    s32int sti_funds;           /* Short-term importance funds */
    s32int lti_funds;           /* Long-term importance funds */
    u32int af_size;             /* Attentional focus size */
    s16int af_boundary;         /* STI threshold for AF */
    int spreading_enabled;      /* Importance spreading enabled */
    int forgetting_enabled;     /* Forgetting enabled */
    u32int *af_atoms;           /* Atoms in attentional focus */
    u32int af_count;            /* Number of atoms in AF */
    Lock lock;
} AttentionAllocator;

/* Attention statistics */
typedef struct AttentionStats {
    s32int sti_funds;
    s32int lti_funds;
    u32int af_size;
    u32int af_count;
    s16int af_boundary;
} AttentionStats;

/* Initialization */
void attention_init(void);

/* Attention allocator management */
AttentionAllocator* attention_create(void);
void attention_destroy(AttentionAllocator *aa);

/* Attention operations */
int attention_stimulate(AttentionAllocator *aa, u32int atom_id, s16int amount);
void attention_spread_importance(AttentionAllocator *aa, u32int source_atom);
void attention_forget(AttentionAllocator *aa, u32int atom_id);
void attention_rent_collection(AttentionAllocator *aa);

/* Attentional focus */
void attention_update_af(AttentionAllocator *aa);
int attention_get_af(AttentionAllocator *aa, u32int *atoms, u32int max_atoms);
void attention_set_af_size(AttentionAllocator *aa, u32int size);

/* Configuration */
void attention_set_sti_funds(AttentionAllocator *aa, s32int funds);
void attention_set_lti_funds(AttentionAllocator *aa, s32int funds);

/* Statistics */
void attention_get_stats(AttentionAllocator *aa, AttentionStats *stats);
void print_attention_stats(AttentionAllocator *aa);

/* Main loop */
void attention_main_loop(void);

/* Utility */
AtomSpace* get_global_atomspace(void);

#endif /* _ATTENTION_H_ */
