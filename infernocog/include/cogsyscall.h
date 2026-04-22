/**
 * Cognitive System Calls for InfernoCog
 * 
 * This header defines the system call interface for cognitive operations
 * in the InfernoCog kernel. These system calls provide user-space programs
 * with access to kernel-level cognitive services.
 *
 * Copyright (C) 2025 OpenCog Foundation
 * License: AGPL-3.0
 */

#ifndef _COGSYSCALL_H_
#define _COGSYSCALL_H_

#include <sys/types.h>

/* Cognitive system call numbers */
#define SYS_COG_BASE            1000

/* AtomSpace operations */
#define SYS_ATOM_CREATE         (SYS_COG_BASE + 0)
#define SYS_ATOM_DELETE         (SYS_COG_BASE + 1)
#define SYS_ATOM_GET            (SYS_COG_BASE + 2)
#define SYS_ATOM_SET_TV         (SYS_COG_BASE + 3)
#define SYS_ATOM_GET_TV         (SYS_COG_BASE + 4)
#define SYS_ATOM_SET_AV         (SYS_COG_BASE + 5)
#define SYS_ATOM_GET_AV         (SYS_COG_BASE + 6)
#define SYS_ATOM_GET_INCOMING   (SYS_COG_BASE + 7)
#define SYS_ATOM_GET_OUTGOING   (SYS_COG_BASE + 8)

/* Pattern matching operations */
#define SYS_PATTERN_COMPILE     (SYS_COG_BASE + 10)
#define SYS_PATTERN_MATCH       (SYS_COG_BASE + 11)
#define SYS_PATTERN_EXECUTE     (SYS_COG_BASE + 12)
#define SYS_PATTERN_WATCH       (SYS_COG_BASE + 13)

/* Attention operations */
#define SYS_ATTENTION_STIMULATE (SYS_COG_BASE + 20)
#define SYS_ATTENTION_SPREAD    (SYS_COG_BASE + 21)
#define SYS_ATTENTION_DECAY     (SYS_COG_BASE + 22)
#define SYS_ATTENTION_GET_FOCUS (SYS_COG_BASE + 23)
#define SYS_ATTENTION_UPDATE    (SYS_COG_BASE + 24)

/* PLN inference operations */
#define SYS_PLN_DEDUCE          (SYS_COG_BASE + 30)
#define SYS_PLN_INDUCE          (SYS_COG_BASE + 31)
#define SYS_PLN_ABDUCE          (SYS_COG_BASE + 32)
#define SYS_PLN_MODUS_PONENS    (SYS_COG_BASE + 33)
#define SYS_PLN_FORWARD_CHAIN   (SYS_COG_BASE + 34)
#define SYS_PLN_BACKWARD_CHAIN  (SYS_COG_BASE + 35)

/* URE operations */
#define SYS_URE_INFER           (SYS_COG_BASE + 40)
#define SYS_URE_ADD_RULE        (SYS_COG_BASE + 41)
#define SYS_URE_REMOVE_RULE     (SYS_COG_BASE + 42)

/* MOSES operations */
#define SYS_MOSES_EVOLVE        (SYS_COG_BASE + 50)
#define SYS_MOSES_OPTIMIZE      (SYS_COG_BASE + 51)

/* Atom types */
typedef enum {
    ATOM_NODE = 1,
    ATOM_LINK = 2,
    
    /* Node types */
    CONCEPT_NODE = 10,
    PREDICATE_NODE = 11,
    VARIABLE_NODE = 12,
    NUMBER_NODE = 13,
    
    /* Link types */
    INHERITANCE_LINK = 20,
    EVALUATION_LINK = 21,
    SIMILARITY_LINK = 22,
    IMPLICATION_LINK = 23,
    AND_LINK = 24,
    OR_LINK = 25,
    NOT_LINK = 26
} atom_type_t;

/* Truth value structure */
typedef struct {
    float strength;      /* Probability [0.0, 1.0] */
    float confidence;    /* Confidence [0.0, 1.0] */
    float count;         /* Evidence count */
} truth_value_t;

/* Attention value structure */
typedef struct {
    short sti;          /* Short-term importance */
    short lti;          /* Long-term importance */
    unsigned short vlti; /* Very long-term importance */
} attention_value_t;

/* Atom handle type */
typedef int atom_handle_t;

/* Pattern handle type */
typedef int pattern_handle_t;

/* Atom creation */
atom_handle_t cog_atom_create(atom_type_t type, const char *name, 
                              truth_value_t *tv);

/* Atom deletion */
int cog_atom_delete(atom_handle_t handle);

/* Atom retrieval */
atom_handle_t cog_atom_get(atom_type_t type, const char *name);

/* Truth value operations */
int cog_atom_set_tv(atom_handle_t handle, truth_value_t *tv);
int cog_atom_get_tv(atom_handle_t handle, truth_value_t *tv);

/* Attention value operations */
int cog_atom_set_av(atom_handle_t handle, attention_value_t *av);
int cog_atom_get_av(atom_handle_t handle, attention_value_t *av);

/* Atom relationships */
int cog_atom_get_incoming(atom_handle_t handle, atom_handle_t *atoms, 
                          int max_count);
int cog_atom_get_outgoing(atom_handle_t handle, atom_handle_t *atoms, 
                          int max_count);

/* Pattern matching */
pattern_handle_t cog_pattern_compile(const char *pattern_expr);
int cog_pattern_match(pattern_handle_t pattern, atom_handle_t *results, 
                     int max_results);
int cog_pattern_execute(pattern_handle_t pattern);

/* Attention operations */
int cog_attention_stimulate(atom_handle_t handle, int amount);
int cog_attention_spread(atom_handle_t source, float rate);
int cog_attention_decay(float rate);
int cog_attention_get_focus(atom_handle_t *atoms, int max_count);
int cog_attention_update(void);

/* PLN inference */
atom_handle_t cog_pln_deduce(atom_handle_t premise1, atom_handle_t premise2);
atom_handle_t cog_pln_induce(atom_handle_t *instances, int count);
atom_handle_t cog_pln_abduce(atom_handle_t implication, atom_handle_t observation);
atom_handle_t cog_pln_modus_ponens(atom_handle_t implication, atom_handle_t antecedent);

/* Chaining inference */
int cog_pln_forward_chain(atom_handle_t *premises, int premise_count,
                         atom_handle_t *conclusions, int max_conclusions,
                         int max_steps);
int cog_pln_backward_chain(atom_handle_t goal, atom_handle_t *proofs,
                          int max_proofs, int max_steps);

/* URE operations */
int cog_ure_infer(atom_handle_t *rules, int rule_count,
                 atom_handle_t target, atom_handle_t *results,
                 int max_results, int max_steps);

/* MOSES operations */
atom_handle_t cog_moses_evolve(const char *fitness_function,
                              int population_size, int generations);

/* Cognitive process management */
typedef int cogproc_t;

cogproc_t cog_proc_create(const char *name, void (*entry)(void *), void *arg);
int cog_proc_wait(cogproc_t proc);
int cog_proc_kill(cogproc_t proc);
int cog_proc_get_sti(cogproc_t proc);
int cog_proc_set_sti(cogproc_t proc, int sti);

/* Cognitive IPC */
typedef int cogchan_t;

cogchan_t cog_chan_create(int buffer_size);
int cog_chan_send(cogchan_t chan, atom_handle_t atom);
atom_handle_t cog_chan_recv(cogchan_t chan);
int cog_chan_close(cogchan_t chan);

/* Distributed cognition */
typedef struct {
    char addr[256];     /* Node address */
    int port;           /* Port number */
} cognode_t;

int cog_export_atomspace(cognode_t *node);
int cog_import_atomspace(cognode_t *node);
int cog_sync_atomspace(cognode_t *node);

/* Cognitive statistics */
typedef struct {
    int atom_count;
    int node_count;
    int link_count;
    int af_size;
    int total_sti;
    int total_lti;
    int inference_count;
    int pattern_match_count;
} cog_stats_t;

int cog_get_stats(cog_stats_t *stats);

/* Error codes */
#define COG_OK              0
#define COG_ERROR          -1
#define COG_INVALID_HANDLE -2
#define COG_INVALID_TYPE   -3
#define COG_NO_MEMORY      -4
#define COG_NOT_FOUND      -5
#define COG_TIMEOUT        -6

/* Cognitive file system paths */
#define COG_FS_ROOT         "/cog"
#define COG_FS_ATOMSPACE    "/cog/atomspace"
#define COG_FS_ATTENTION    "/cog/attention"
#define COG_FS_INFERENCE    "/cog/inference"
#define COG_FS_PATTERN      "/cog/pattern"
#define COG_FS_LEARNING     "/cog/learning"
#define COG_FS_PERCEPTION   "/cog/perception"
#define COG_FS_ACTION       "/cog/action"
#define COG_FS_META         "/cog/meta"

#endif /* _COGSYSCALL_H_ */
