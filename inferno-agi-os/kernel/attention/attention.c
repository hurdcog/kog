/*
 * Attention Allocation Kernel Module
 * 
 * Implements ECAN (Economic Attention Network) as a kernel service:
 * - STI (Short-Term Importance) management
 * - LTI (Long-Term Importance) management
 * - Attentional focus control
 * - Importance spreading
 */

#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "../atomspace/atomspace.h"
#include "attention.h"

/* Global attention allocator instance */
static AttentionAllocator *global_attention = nil;

/* ECAN parameters */
#define DEFAULT_STI_FUNDS 100000
#define DEFAULT_LTI_FUNDS 100000
#define DEFAULT_AF_SIZE 100
#define MIN_STI_THRESHOLD 10
#define SPREADING_FACTOR 0.1

/*
 * Initialize the Attention Allocation kernel module
 */
void
attention_init(void)
{
    print("Attention: Initializing kernel module\n");
    
    global_attention = attention_create();
    if (global_attention == nil) {
        panic("Attention: Failed to create attention allocator");
    }
    
    print("Attention: Kernel module initialized\n");
    print("  STI Funds: %d\n", global_attention->sti_funds);
    print("  LTI Funds: %d\n", global_attention->lti_funds);
    print("  AF Size: %d\n", global_attention->af_size);
}

/*
 * Create a new attention allocator instance
 */
AttentionAllocator*
attention_create(void)
{
    AttentionAllocator *aa;
    
    aa = mallocz(sizeof(AttentionAllocator), 1);
    if (aa == nil) {
        return nil;
    }
    
    aa->atomspace = get_global_atomspace();
    aa->sti_funds = DEFAULT_STI_FUNDS;
    aa->lti_funds = DEFAULT_LTI_FUNDS;
    aa->af_size = DEFAULT_AF_SIZE;
    aa->af_boundary = MIN_STI_THRESHOLD;
    aa->spreading_enabled = 1;
    aa->forgetting_enabled = 1;
    aa->af_atoms = nil;
    aa->af_count = 0;
    
    return aa;
}

/*
 * Destroy an attention allocator instance
 */
void
attention_destroy(AttentionAllocator *aa)
{
    if (aa == nil) {
        return;
    }
    
    if (aa->af_atoms != nil) {
        free(aa->af_atoms);
    }
    
    free(aa);
}

/*
 * Stimulate an atom (increase its STI)
 */
int
attention_stimulate(AttentionAllocator *aa, u32int atom_id, s16int amount)
{
    Atom *atom;
    s16int old_sti, new_sti;
    
    if (aa == nil || atom_id == 0) {
        return -1;
    }
    
    atom = atomspace_get_atom(aa->atomspace, atom_id);
    if (atom == nil) {
        return -1;
    }
    
    lock(&aa->lock);
    
    old_sti = atom->av.sti;
    new_sti = old_sti + amount;
    
    /* Check if we have enough STI funds */
    if (amount > 0 && aa->sti_funds < amount) {
        /* Not enough funds, use what we have */
        amount = aa->sti_funds;
        new_sti = old_sti + amount;
    }
    
    /* Update atom's STI */
    atom->av.sti = new_sti;
    
    /* Update STI funds */
    aa->sti_funds -= amount;
    
    unlock(&aa->lock);
    
    /* Update attentional focus if needed */
    attention_update_af(aa);
    
    return 0;
}

/*
 * Rent collection: Gradually decrease STI of atoms not in focus
 */
void
attention_rent_collection(AttentionAllocator *aa)
{
    /* TODO: Implement rent collection */
    /* Iterate through all atoms and decrease STI */
    /* Collect rent back to STI funds */
    
    print("Attention: Rent collection (not yet implemented)\n");
}

/*
 * Importance spreading: Spread importance along links
 */
void
attention_spread_importance(AttentionAllocator *aa, u32int source_atom)
{
    Atom *source;
    LinkData *link_data;
    Atom *target;
    s16int spread_amount;
    int i;
    
    if (aa == nil || source_atom == 0 || !aa->spreading_enabled) {
        return;
    }
    
    source = atomspace_get_atom(aa->atomspace, source_atom);
    if (source == nil) {
        return;
    }
    
    /* Calculate spread amount */
    spread_amount = (s16int)(source->av.sti * SPREADING_FACTOR);
    
    if (spread_amount <= 0) {
        return;
    }
    
    /* If this is a link, spread to outgoing atoms */
    if (source->flags & ATOM_FLAG_LINK) {
        link_data = (LinkData*)source->data;
        
        for (i = 0; i < link_data->arity; i++) {
            target = atomspace_get_atom(aa->atomspace, link_data->outgoing[i]);
            if (target != nil) {
                attention_stimulate(aa, target->id, spread_amount / link_data->arity);
            }
        }
    }
}

/*
 * Update the attentional focus (AF)
 * AF contains atoms with highest STI values
 */
void
attention_update_af(AttentionAllocator *aa)
{
    /* TODO: Implement AF update */
    /* 1. Get all atoms from atomspace */
    /* 2. Sort by STI */
    /* 3. Take top N atoms */
    /* 4. Update af_atoms array */
    
    /* For now, just update the count */
    lock(&aa->lock);
    /* aa->af_count = ... */
    unlock(&aa->lock);
}

/*
 * Get atoms in attentional focus
 */
int
attention_get_af(AttentionAllocator *aa, u32int *atoms, u32int max_atoms)
{
    int count;
    
    if (aa == nil || atoms == nil) {
        return -1;
    }
    
    lock(&aa->lock);
    
    count = aa->af_count;
    if (count > max_atoms) {
        count = max_atoms;
    }
    
    if (aa->af_atoms != nil && count > 0) {
        memmove(atoms, aa->af_atoms, count * sizeof(u32int));
    }
    
    unlock(&aa->lock);
    
    return count;
}

/*
 * Forgetting: Move atoms from STI to LTI
 */
void
attention_forget(AttentionAllocator *aa, u32int atom_id)
{
    Atom *atom;
    s16int sti_amount;
    
    if (aa == nil || atom_id == 0 || !aa->forgetting_enabled) {
        return;
    }
    
    atom = atomspace_get_atom(aa->atomspace, atom_id);
    if (atom == nil) {
        return;
    }
    
    lock(&aa->lock);
    
    /* Transfer STI to LTI */
    sti_amount = atom->av.sti;
    atom->av.sti = 0;
    atom->av.lti += sti_amount / 10;  /* 10% conversion rate */
    
    /* Return STI to funds */
    aa->sti_funds += sti_amount;
    
    unlock(&aa->lock);
}

/*
 * Set attentional focus size
 */
void
attention_set_af_size(AttentionAllocator *aa, u32int size)
{
    if (aa == nil) {
        return;
    }
    
    lock(&aa->lock);
    aa->af_size = size;
    unlock(&aa->lock);
    
    /* Reallocate AF atoms array if needed */
    if (aa->af_atoms != nil) {
        free(aa->af_atoms);
    }
    
    aa->af_atoms = mallocz(size * sizeof(u32int), 1);
    
    /* Update AF */
    attention_update_af(aa);
}

/*
 * Set STI funds
 */
void
attention_set_sti_funds(AttentionAllocator *aa, s32int funds)
{
    if (aa == nil) {
        return;
    }
    
    lock(&aa->lock);
    aa->sti_funds = funds;
    unlock(&aa->lock);
}

/*
 * Set LTI funds
 */
void
attention_set_lti_funds(AttentionAllocator *aa, s32int funds)
{
    if (aa == nil) {
        return;
    }
    
    lock(&aa->lock);
    aa->lti_funds = funds;
    unlock(&aa->lock);
}

/*
 * Get attention allocator statistics
 */
void
attention_get_stats(AttentionAllocator *aa, AttentionStats *stats)
{
    if (aa == nil || stats == nil) {
        return;
    }
    
    lock(&aa->lock);
    
    stats->sti_funds = aa->sti_funds;
    stats->lti_funds = aa->lti_funds;
    stats->af_size = aa->af_size;
    stats->af_count = aa->af_count;
    stats->af_boundary = aa->af_boundary;
    
    unlock(&aa->lock);
}

/*
 * Print attention allocator statistics
 */
void
print_attention_stats(AttentionAllocator *aa)
{
    AttentionStats stats;
    
    if (aa == nil) {
        return;
    }
    
    attention_get_stats(aa, &stats);
    
    print("Attention Allocator Statistics:\n");
    print("  STI Funds: %d\n", stats.sti_funds);
    print("  LTI Funds: %d\n", stats.lti_funds);
    print("  AF Size: %ud\n", stats.af_size);
    print("  AF Count: %ud\n", stats.af_count);
    print("  AF Boundary: %d\n", stats.af_boundary);
    print("  Spreading: %s\n", aa->spreading_enabled ? "enabled" : "disabled");
    print("  Forgetting: %s\n", aa->forgetting_enabled ? "enabled" : "disabled");
}

/*
 * Attention allocation main loop (runs as kernel process)
 */
void
attention_main_loop(void)
{
    AttentionAllocator *aa = global_attention;
    
    if (aa == nil) {
        return;
    }
    
    print("Attention: Starting main loop\n");
    
    for (;;) {
        /* Update attentional focus */
        attention_update_af(aa);
        
        /* Perform rent collection */
        attention_rent_collection(aa);
        
        /* Sleep for a while */
        tsleep(&up->sleep, return0, 0, 1000);  /* 1 second */
    }
}
