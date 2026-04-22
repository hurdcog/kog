/*
 * Attention Allocation Portable Implementation
 * 
 * Cross-platform implementation of cognitive attention allocation:
 * - Short-term importance (STI) management
 * - Long-term importance (LTI) management
 * - Attentional focus maintenance
 * - Importance spreading
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

/* Forward declarations from atomspace */
typedef struct AtomSpace AtomSpace;
typedef struct AttentionValue {
    int16_t sti;
    int16_t lti;
    int16_t vlti;
} AttentionValue;

extern AtomSpace* get_global_atomspace(void);
extern void atomspace_set_av(AtomSpace *as, uint32_t atom_id, AttentionValue av);
extern AttentionValue atomspace_get_av(AtomSpace *as, uint32_t atom_id);

/* Attention bank configuration */
#define DEFAULT_STI_FUNDS 10000
#define DEFAULT_LTI_FUNDS 10000
#define DEFAULT_AF_BOUNDARY 100
#define MAX_ATTENTIONAL_FOCUS 1000

/* Attentional Focus entry */
typedef struct AFEntry {
    uint32_t atom_id;
    int16_t sti;
    struct AFEntry *next;
} AFEntry;

/* Attention Bank structure */
typedef struct AttentionBank {
    AtomSpace *atomspace;
    
    /* STI/LTI funds */
    int32_t sti_funds;
    int32_t lti_funds;
    
    /* Attentional focus */
    int16_t af_boundary;
    AFEntry *af_head;
    uint32_t af_size;
    uint32_t af_max_size;
    
    /* Importance spreading */
    float spread_threshold;
    float spread_decay;
    
    /* Statistics */
    uint32_t stimulations;
    uint32_t spreads;
    
    LOCK_TYPE lock;
} AttentionBank;

/* Global attention bank */
static AttentionBank *global_attention = NULL;

/*
 * Create a new attention bank
 */
AttentionBank* attention_create(void)
{
    AttentionBank *ab;
    
    ab = (AttentionBank*)calloc(1, sizeof(AttentionBank));
    if (ab == NULL) {
        return NULL;
    }
    
    ab->atomspace = get_global_atomspace();
    ab->sti_funds = DEFAULT_STI_FUNDS;
    ab->lti_funds = DEFAULT_LTI_FUNDS;
    ab->af_boundary = DEFAULT_AF_BOUNDARY;
    ab->af_head = NULL;
    ab->af_size = 0;
    ab->af_max_size = MAX_ATTENTIONAL_FOCUS;
    ab->spread_threshold = 0.1f;
    ab->spread_decay = 0.5f;
    ab->stimulations = 0;
    ab->spreads = 0;
    
    LOCK_INIT(ab->lock);
    
    return ab;
}

/*
 * Destroy an attention bank
 */
void attention_destroy(AttentionBank *ab)
{
    AFEntry *entry, *next;
    
    if (ab == NULL) {
        return;
    }
    
    /* Free attentional focus list */
    entry = ab->af_head;
    while (entry != NULL) {
        next = entry->next;
        free(entry);
        entry = next;
    }
    
    LOCK_DESTROY(ab->lock);
    free(ab);
}

/*
 * Initialize the Attention subsystem
 */
void attention_init(void)
{
    printf("Attention: Initializing portable module\n");
    
    global_attention = attention_create();
    if (global_attention == NULL) {
        fprintf(stderr, "Attention: Failed to create attention bank\n");
        return;
    }
    
    printf("Attention: Portable module initialized\n");
    printf("  STI Funds: %d\n", global_attention->sti_funds);
    printf("  LTI Funds: %d\n", global_attention->lti_funds);
    printf("  AF Boundary: %d\n", global_attention->af_boundary);
}

/*
 * Shutdown the Attention subsystem
 */
void attention_shutdown(void)
{
    printf("Attention: Shutting down portable module\n");
    
    if (global_attention != NULL) {
        attention_destroy(global_attention);
        global_attention = NULL;
    }
    
    printf("Attention: Portable module shutdown complete\n");
}

/*
 * Get the global attention bank
 */
AttentionBank* get_global_attention(void)
{
    return global_attention;
}

/*
 * Stimulate an atom (increase its STI)
 */
int attention_stimulate(AttentionBank *ab, uint32_t atom_id, int16_t amount)
{
    AttentionValue av;
    int32_t new_sti;
    
    if (ab == NULL || atom_id == 0) {
        return -1;
    }
    
    LOCK_ACQUIRE(ab->lock);
    
    /* Check if we have enough funds */
    if (ab->sti_funds < amount) {
        LOCK_RELEASE(ab->lock);
        return -1;
    }
    
    /* Get current attention value */
    av = atomspace_get_av(ab->atomspace, atom_id);
    
    /* Calculate new STI */
    new_sti = av.sti + amount;
    if (new_sti > 32767) new_sti = 32767;
    if (new_sti < -32768) new_sti = -32768;
    
    av.sti = (int16_t)new_sti;
    
    /* Update attention value */
    atomspace_set_av(ab->atomspace, atom_id, av);
    
    /* Deduct from funds */
    ab->sti_funds -= amount;
    ab->stimulations++;
    
    /* Update attentional focus if needed */
    if (av.sti >= ab->af_boundary) {
        attention_update_focus(ab, atom_id, av.sti);
    }
    
    LOCK_RELEASE(ab->lock);
    
    return 0;
}

/*
 * Update the attentional focus
 */
void attention_update_focus(AttentionBank *ab, uint32_t atom_id, int16_t sti)
{
    AFEntry *entry, *prev, *new_entry;
    
    if (ab == NULL || atom_id == 0) {
        return;
    }
    
    /* Check if atom is already in focus */
    prev = NULL;
    entry = ab->af_head;
    while (entry != NULL) {
        if (entry->atom_id == atom_id) {
            /* Update STI and reposition */
            entry->sti = sti;
            
            /* Remove from current position */
            if (prev == NULL) {
                ab->af_head = entry->next;
            } else {
                prev->next = entry->next;
            }
            
            /* Reinsert in sorted position */
            attention_insert_focus(ab, entry);
            return;
        }
        prev = entry;
        entry = entry->next;
    }
    
    /* Create new entry */
    new_entry = (AFEntry*)calloc(1, sizeof(AFEntry));
    if (new_entry == NULL) {
        return;
    }
    
    new_entry->atom_id = atom_id;
    new_entry->sti = sti;
    new_entry->next = NULL;
    
    /* Insert in sorted position */
    attention_insert_focus(ab, new_entry);
    ab->af_size++;
    
    /* Trim focus if too large */
    while (ab->af_size > ab->af_max_size) {
        attention_remove_lowest_focus(ab);
    }
}

/*
 * Insert entry into attentional focus (sorted by STI descending)
 */
void attention_insert_focus(AttentionBank *ab, AFEntry *entry)
{
    AFEntry *current, *prev;
    
    if (ab->af_head == NULL || entry->sti > ab->af_head->sti) {
        entry->next = ab->af_head;
        ab->af_head = entry;
        return;
    }
    
    prev = ab->af_head;
    current = ab->af_head->next;
    
    while (current != NULL && current->sti >= entry->sti) {
        prev = current;
        current = current->next;
    }
    
    entry->next = current;
    prev->next = entry;
}

/*
 * Remove lowest STI atom from attentional focus
 */
void attention_remove_lowest_focus(AttentionBank *ab)
{
    AFEntry *entry, *prev;
    
    if (ab->af_head == NULL) {
        return;
    }
    
    /* Find last entry */
    prev = NULL;
    entry = ab->af_head;
    while (entry->next != NULL) {
        prev = entry;
        entry = entry->next;
    }
    
    /* Remove last entry */
    if (prev == NULL) {
        ab->af_head = NULL;
    } else {
        prev->next = NULL;
    }
    
    free(entry);
    ab->af_size--;
}

/*
 * Spread importance from an atom to its neighbors
 */
int attention_spread(AttentionBank *ab, uint32_t atom_id)
{
    /* TODO: Implement importance spreading */
    /* This requires access to link structure in AtomSpace */
    
    if (ab == NULL || atom_id == 0) {
        return -1;
    }
    
    ab->spreads++;
    
    return 0;
}

/*
 * Get atoms in attentional focus
 */
int attention_get_focus(AttentionBank *ab, uint32_t *atoms, uint32_t max_atoms)
{
    AFEntry *entry;
    uint32_t count = 0;
    
    if (ab == NULL || atoms == NULL) {
        return -1;
    }
    
    LOCK_ACQUIRE(ab->lock);
    
    entry = ab->af_head;
    while (entry != NULL && count < max_atoms) {
        atoms[count++] = entry->atom_id;
        entry = entry->next;
    }
    
    LOCK_RELEASE(ab->lock);
    
    return count;
}

/*
 * Check if an atom is in attentional focus
 */
int attention_in_focus(AttentionBank *ab, uint32_t atom_id)
{
    AFEntry *entry;
    
    if (ab == NULL || atom_id == 0) {
        return 0;
    }
    
    LOCK_ACQUIRE(ab->lock);
    
    entry = ab->af_head;
    while (entry != NULL) {
        if (entry->atom_id == atom_id) {
            LOCK_RELEASE(ab->lock);
            return 1;
        }
        entry = entry->next;
    }
    
    LOCK_RELEASE(ab->lock);
    return 0;
}

/*
 * Set attentional focus boundary
 */
void attention_set_af_boundary(AttentionBank *ab, int16_t boundary)
{
    if (ab == NULL) {
        return;
    }
    
    LOCK_ACQUIRE(ab->lock);
    ab->af_boundary = boundary;
    LOCK_RELEASE(ab->lock);
}

/*
 * Get attentional focus boundary
 */
int16_t attention_get_af_boundary(AttentionBank *ab)
{
    if (ab == NULL) {
        return 0;
    }
    return ab->af_boundary;
}

/*
 * Set STI funds
 */
void attention_set_sti_funds(AttentionBank *ab, int32_t funds)
{
    if (ab == NULL) {
        return;
    }
    
    LOCK_ACQUIRE(ab->lock);
    ab->sti_funds = funds;
    LOCK_RELEASE(ab->lock);
}

/*
 * Get STI funds
 */
int32_t attention_get_sti_funds(AttentionBank *ab)
{
    if (ab == NULL) {
        return 0;
    }
    return ab->sti_funds;
}

/*
 * Set LTI funds
 */
void attention_set_lti_funds(AttentionBank *ab, int32_t funds)
{
    if (ab == NULL) {
        return;
    }
    
    LOCK_ACQUIRE(ab->lock);
    ab->lti_funds = funds;
    LOCK_RELEASE(ab->lock);
}

/*
 * Get LTI funds
 */
int32_t attention_get_lti_funds(AttentionBank *ab)
{
    if (ab == NULL) {
        return 0;
    }
    return ab->lti_funds;
}

/*
 * Print attention bank statistics
 */
void attention_print_stats(AttentionBank *ab)
{
    if (ab == NULL) {
        return;
    }
    
    printf("Attention Bank Statistics:\n");
    printf("  STI Funds: %d\n", ab->sti_funds);
    printf("  LTI Funds: %d\n", ab->lti_funds);
    printf("  AF Boundary: %d\n", ab->af_boundary);
    printf("  AF Size: %u / %u\n", ab->af_size, ab->af_max_size);
    printf("  Total Stimulations: %u\n", ab->stimulations);
    printf("  Total Spreads: %u\n", ab->spreads);
}
