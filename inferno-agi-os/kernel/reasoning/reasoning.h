/*
 * Reasoning Engine Interface
 * Header file for cognitive reasoning services
 */

#ifndef _REASONING_H_
#define _REASONING_H_

/* Forward declarations */
typedef struct AtomSpace AtomSpace;

/* Rule types */
typedef enum {
    RULE_TYPE_PLN,      /* Probabilistic Logic Networks */
    RULE_TYPE_URE,      /* Unified Rule Engine */
    RULE_TYPE_PATTERN,  /* Pattern matching */
    RULE_TYPE_CUSTOM    /* Custom rules */
} RuleType;

/* Rule function signature */
typedef u32int (*RuleFunc)(AtomSpace *as, u32int *premises, u32int count);

/* Rule structure */
typedef struct Rule {
    char *name;
    RuleType type;
    RuleFunc func;
    int enabled;
    float priority;
    u32int application_count;
    struct Rule *next;
} Rule;

/* Reasoning engine structure */
typedef struct ReasoningEngine {
    AtomSpace *atomspace;
    Rule *rules;
    u32int rule_count;
    u32int inference_count;
    u32int max_iterations;
    Lock lock;
} ReasoningEngine;

/* Initialization */
void reasoning_init(void);

/* Reasoning engine management */
ReasoningEngine* reasoning_create(void);
void reasoning_destroy(ReasoningEngine *re);

/* Rule management */
int reasoning_add_rule(ReasoningEngine *re, char *name, RuleType type, RuleFunc func);

/* Inference operations */
int reasoning_infer(ReasoningEngine *re, u32int *premises, u32int premise_count, 
                   u32int *conclusions, u32int max_conclusions);
int reasoning_forward_chain(ReasoningEngine *re, u32int *initial_atoms, u32int count, int max_steps);
int reasoning_backward_chain(ReasoningEngine *re, u32int goal, u32int *evidence, u32int max_evidence);

/* Pattern matching */
int reasoning_pattern_match(ReasoningEngine *re, u32int pattern, u32int *matches, u32int max_matches);

/* PLN functions */
void pln_init(ReasoningEngine *re);
u32int pln_deduction_rule(AtomSpace *as, u32int *premises, u32int count);
u32int pln_induction_rule(AtomSpace *as, u32int *premises, u32int count);
u32int pln_abduction_rule(AtomSpace *as, u32int *premises, u32int count);
u32int pln_modus_ponens_rule(AtomSpace *as, u32int *premises, u32int count);

/* URE functions */
void ure_init(ReasoningEngine *re);
u32int ure_variable_instantiation(AtomSpace *as, u32int *premises, u32int count);
u32int ure_unification(AtomSpace *as, u32int *premises, u32int count);

/* Utility functions */
void print_reasoning_stats(ReasoningEngine *re);

#endif /* _REASONING_H_ */
