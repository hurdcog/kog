/*
 * MOSES Evolutionary Learning Engine
 * Meta-Optimizing Semantic Evolutionary Search
 *
 * Implements program evolution using:
 * - Population management with fitness evaluation
 * - Genetic operators: crossover, mutation, selection
 * - Knob turning for program optimization
 * - Representation building for combo programs
 *
 * Copyright (c) 2026 OpenCog Inferno Project
 * Licensed under AGPL-3.0
 */

#ifndef _MOSES_ENGINE_H_
#define _MOSES_ENGINE_H_

#include <stdint.h>

/* Maximum population size */
#define MOSES_MAX_POPULATION 4096

/* Maximum program tree depth */
#define MOSES_MAX_TREE_DEPTH 16

/* Maximum knobs per representation */
#define MOSES_MAX_KNOBS 256

/* Default mutation rate */
#define MOSES_DEFAULT_MUTATION_RATE 0.05f

/* Default crossover rate */
#define MOSES_DEFAULT_CROSSOVER_RATE 0.7f

/* Tournament selection size */
#define MOSES_TOURNAMENT_SIZE 7

/* ========================================================================
 * Program Representation
 * ======================================================================== */

/* Program node types (combo language) */
typedef enum {
    PROG_NODE_AND = 1,
    PROG_NODE_OR,
    PROG_NODE_NOT,
    PROG_NODE_IF,
    PROG_NODE_PLUS,
    PROG_NODE_TIMES,
    PROG_NODE_SIN,
    PROG_NODE_LOG,
    PROG_NODE_EXP,
    PROG_NODE_CONST,
    PROG_NODE_INPUT,
    PROG_NODE_TRUE,
    PROG_NODE_FALSE,
    PROG_NODE_MAX
} ProgNodeType;

/* Program tree node */
typedef struct ProgNode {
    ProgNodeType type;
    float value;                /* For CONST nodes */
    uint32_t input_idx;         /* For INPUT nodes */
    struct ProgNode *children[4]; /* Child nodes */
    uint32_t child_count;
    uint32_t depth;
} ProgNode;

/* Individual in the population */
typedef struct Individual {
    ProgNode *program;          /* Root of program tree */
    float fitness;              /* Evaluated fitness score */
    float complexity;           /* Program complexity penalty */
    float score;                /* fitness - complexity */
    uint32_t generation;        /* Generation when created */
    uint32_t id;                /* Unique ID */
    int evaluated;              /* Whether fitness has been computed */
} Individual;

/* Knob for program optimization */
typedef struct Knob {
    uint32_t node_index;        /* Index of node to modify */
    ProgNodeType original_type; /* Original node type */
    ProgNodeType alternatives[4]; /* Alternative node types */
    uint32_t alt_count;
    float original_value;
    float value_range[2];       /* Min/max for continuous knobs */
} Knob;

/* Representation (exemplar + knobs) */
typedef struct Representation {
    ProgNode *exemplar;         /* Base program */
    Knob knobs[MOSES_MAX_KNOBS];
    uint32_t knob_count;
} Representation;

/* ========================================================================
 * MOSES Engine
 * ======================================================================== */

/* Fitness function type */
typedef float (*MOSESFitnessFunc)(ProgNode *program, void *user_data);

/* MOSES configuration */
typedef struct MOSESConfig {
    uint32_t max_generations;
    uint32_t population_size;
    float mutation_rate;
    float crossover_rate;
    float complexity_penalty;
    uint32_t tournament_size;
    float target_fitness;
    uint32_t max_evals;
    int elitism;                /* Keep best N individuals */
} MOSESConfig;

/* MOSES statistics */
typedef struct MOSESStats {
    uint32_t generation;
    uint32_t total_evaluations;
    float best_fitness;
    float avg_fitness;
    float worst_fitness;
    float best_score;
    uint32_t population_diversity;
} MOSESStats;

/* Forward declaration */
typedef struct AtomSpace AtomSpace;

/* MOSES Engine */
typedef struct MOSESEngine {
    AtomSpace *atomspace;
    MOSESConfig config;
    MOSESStats stats;
    Individual *population;
    uint32_t pop_size;
    uint32_t next_id;
    MOSESFitnessFunc fitness_func;
    void *fitness_data;
    Representation *current_rep;
} MOSESEngine;

/* ========================================================================
 * API Functions
 * ======================================================================== */

/* Engine lifecycle */
MOSESEngine* moses_engine_create(AtomSpace *as, MOSESConfig *config);
void moses_engine_destroy(MOSESEngine *engine);

/* Configuration */
MOSESConfig moses_default_config(void);

/* Fitness function */
void moses_set_fitness(MOSESEngine *engine, MOSESFitnessFunc func, void *data);

/* Evolution */
int moses_evolve_generation(MOSESEngine *engine);
int moses_run(MOSESEngine *engine, uint32_t max_generations);
Individual* moses_get_best(MOSESEngine *engine);

/* Program operations */
ProgNode* prog_create_node(ProgNodeType type);
ProgNode* prog_create_const(float value);
ProgNode* prog_create_input(uint32_t idx);
void prog_destroy(ProgNode *node);
ProgNode* prog_copy(ProgNode *node);
float prog_evaluate(ProgNode *node, float *inputs, uint32_t input_count);
float prog_complexity(ProgNode *node);
void prog_print(ProgNode *node);

/* Genetic operators */
ProgNode* moses_crossover(ProgNode *parent1, ProgNode *parent2);
void moses_mutate(ProgNode *program, float rate);
Individual* moses_tournament_select(MOSESEngine *engine);

/* Statistics */
void moses_print_stats(MOSESEngine *engine);

#endif /* _MOSES_ENGINE_H_ */
