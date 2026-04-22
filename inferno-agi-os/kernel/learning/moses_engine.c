/*
 * MOSES Evolutionary Learning Engine Implementation
 * Meta-Optimizing Semantic Evolutionary Search
 *
 * Full implementation of program evolution with:
 * - Tournament selection
 * - Subtree crossover
 * - Point mutation
 * - Fitness evaluation
 * - Complexity penalty
 *
 * Copyright (c) 2026 OpenCog Inferno Project
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "moses_engine.h"

/* ========================================================================
 * Random Number Generation
 * ======================================================================== */

static int g_moses_seeded = 0;

static void moses_seed_rng(void)
{
    if (!g_moses_seeded) {
        srand((unsigned int)time(NULL));
        g_moses_seeded = 1;
    }
}

static float moses_random_float(void)
{
    return (float)rand() / (float)RAND_MAX;
}

static uint32_t moses_random_int(uint32_t max)
{
    if (max == 0) return 0;
    return (uint32_t)rand() % max;
}

/* ========================================================================
 * Program Tree Operations
 * ======================================================================== */

ProgNode* prog_create_node(ProgNodeType type)
{
    ProgNode *node = (ProgNode *)calloc(1, sizeof(ProgNode));
    if (!node) return NULL;
    node->type = type;
    return node;
}

ProgNode* prog_create_const(float value)
{
    ProgNode *node = prog_create_node(PROG_NODE_CONST);
    if (node) node->value = value;
    return node;
}

ProgNode* prog_create_input(uint32_t idx)
{
    ProgNode *node = prog_create_node(PROG_NODE_INPUT);
    if (node) node->input_idx = idx;
    return node;
}

void prog_destroy(ProgNode *node)
{
    uint32_t i;
    if (!node) return;
    for (i = 0; i < node->child_count; i++) {
        prog_destroy(node->children[i]);
    }
    free(node);
}

ProgNode* prog_copy(ProgNode *node)
{
    ProgNode *copy;
    uint32_t i;

    if (!node) return NULL;

    copy = (ProgNode *)calloc(1, sizeof(ProgNode));
    if (!copy) return NULL;

    copy->type = node->type;
    copy->value = node->value;
    copy->input_idx = node->input_idx;
    copy->child_count = node->child_count;
    copy->depth = node->depth;

    for (i = 0; i < node->child_count; i++) {
        copy->children[i] = prog_copy(node->children[i]);
    }

    return copy;
}

/**
 * Evaluate a program tree with given inputs
 */
float prog_evaluate(ProgNode *node, float *inputs, uint32_t input_count)
{
    float left, right;

    if (!node) return 0.0f;

    switch (node->type) {
    case PROG_NODE_CONST:
        return node->value;

    case PROG_NODE_INPUT:
        if (node->input_idx < input_count) {
            return inputs[node->input_idx];
        }
        return 0.0f;

    case PROG_NODE_TRUE:
        return 1.0f;

    case PROG_NODE_FALSE:
        return 0.0f;

    case PROG_NODE_AND:
        if (node->child_count < 2) return 0.0f;
        left = prog_evaluate(node->children[0], inputs, input_count);
        right = prog_evaluate(node->children[1], inputs, input_count);
        return (left > 0.5f && right > 0.5f) ? 1.0f : 0.0f;

    case PROG_NODE_OR:
        if (node->child_count < 2) return 0.0f;
        left = prog_evaluate(node->children[0], inputs, input_count);
        right = prog_evaluate(node->children[1], inputs, input_count);
        return (left > 0.5f || right > 0.5f) ? 1.0f : 0.0f;

    case PROG_NODE_NOT:
        if (node->child_count < 1) return 0.0f;
        left = prog_evaluate(node->children[0], inputs, input_count);
        return (left > 0.5f) ? 0.0f : 1.0f;

    case PROG_NODE_IF:
        if (node->child_count < 3) return 0.0f;
        left = prog_evaluate(node->children[0], inputs, input_count);
        if (left > 0.5f) {
            return prog_evaluate(node->children[1], inputs, input_count);
        } else {
            return prog_evaluate(node->children[2], inputs, input_count);
        }

    case PROG_NODE_PLUS:
        if (node->child_count < 2) return 0.0f;
        left = prog_evaluate(node->children[0], inputs, input_count);
        right = prog_evaluate(node->children[1], inputs, input_count);
        return left + right;

    case PROG_NODE_TIMES:
        if (node->child_count < 2) return 0.0f;
        left = prog_evaluate(node->children[0], inputs, input_count);
        right = prog_evaluate(node->children[1], inputs, input_count);
        return left * right;

    case PROG_NODE_SIN:
        if (node->child_count < 1) return 0.0f;
        left = prog_evaluate(node->children[0], inputs, input_count);
        return sinf(left);

    case PROG_NODE_LOG:
        if (node->child_count < 1) return 0.0f;
        left = prog_evaluate(node->children[0], inputs, input_count);
        return (left > 0.0f) ? logf(left) : -10.0f;

    case PROG_NODE_EXP:
        if (node->child_count < 1) return 0.0f;
        left = prog_evaluate(node->children[0], inputs, input_count);
        if (left > 20.0f) left = 20.0f;  /* Prevent overflow */
        return expf(left);

    default:
        return 0.0f;
    }
}

/**
 * Count nodes in a program tree (complexity measure)
 */
static uint32_t prog_node_count(ProgNode *node)
{
    uint32_t count = 1;
    uint32_t i;
    if (!node) return 0;
    for (i = 0; i < node->child_count; i++) {
        count += prog_node_count(node->children[i]);
    }
    return count;
}

float prog_complexity(ProgNode *node)
{
    return (float)prog_node_count(node);
}

/**
 * Get the depth of a program tree
 */
static uint32_t prog_depth(ProgNode *node)
{
    uint32_t max_child = 0;
    uint32_t i;
    if (!node) return 0;
    for (i = 0; i < node->child_count; i++) {
        uint32_t d = prog_depth(node->children[i]);
        if (d > max_child) max_child = d;
    }
    return 1 + max_child;
}

void prog_print(ProgNode *node)
{
    if (!node) {
        printf("nil");
        return;
    }

    switch (node->type) {
    case PROG_NODE_CONST:
        printf("%.2f", node->value);
        break;
    case PROG_NODE_INPUT:
        printf("$%u", node->input_idx);
        break;
    case PROG_NODE_TRUE:
        printf("true");
        break;
    case PROG_NODE_FALSE:
        printf("false");
        break;
    case PROG_NODE_AND:
        printf("(and ");
        prog_print(node->children[0]);
        printf(" ");
        prog_print(node->children[1]);
        printf(")");
        break;
    case PROG_NODE_OR:
        printf("(or ");
        prog_print(node->children[0]);
        printf(" ");
        prog_print(node->children[1]);
        printf(")");
        break;
    case PROG_NODE_NOT:
        printf("(not ");
        prog_print(node->children[0]);
        printf(")");
        break;
    case PROG_NODE_PLUS:
        printf("(+ ");
        prog_print(node->children[0]);
        printf(" ");
        prog_print(node->children[1]);
        printf(")");
        break;
    case PROG_NODE_TIMES:
        printf("(* ");
        prog_print(node->children[0]);
        printf(" ");
        prog_print(node->children[1]);
        printf(")");
        break;
    default:
        printf("?");
        break;
    }
}

/* ========================================================================
 * Random Program Generation
 * ======================================================================== */

static ProgNode* generate_random_program(uint32_t max_depth, uint32_t num_inputs)
{
    ProgNode *node;

    moses_seed_rng();

    /* At max depth, generate terminal nodes */
    if (max_depth <= 1) {
        if (moses_random_float() < 0.5f) {
            return prog_create_const(moses_random_float() * 2.0f - 1.0f);
        } else {
            return prog_create_input(moses_random_int(num_inputs > 0 ? num_inputs : 1));
        }
    }

    /* Choose a random node type */
    uint32_t choice = moses_random_int(8);

    switch (choice) {
    case 0: /* AND */
        node = prog_create_node(PROG_NODE_AND);
        node->children[0] = generate_random_program(max_depth - 1, num_inputs);
        node->children[1] = generate_random_program(max_depth - 1, num_inputs);
        node->child_count = 2;
        break;
    case 1: /* OR */
        node = prog_create_node(PROG_NODE_OR);
        node->children[0] = generate_random_program(max_depth - 1, num_inputs);
        node->children[1] = generate_random_program(max_depth - 1, num_inputs);
        node->child_count = 2;
        break;
    case 2: /* PLUS */
        node = prog_create_node(PROG_NODE_PLUS);
        node->children[0] = generate_random_program(max_depth - 1, num_inputs);
        node->children[1] = generate_random_program(max_depth - 1, num_inputs);
        node->child_count = 2;
        break;
    case 3: /* TIMES */
        node = prog_create_node(PROG_NODE_TIMES);
        node->children[0] = generate_random_program(max_depth - 1, num_inputs);
        node->children[1] = generate_random_program(max_depth - 1, num_inputs);
        node->child_count = 2;
        break;
    case 4: /* NOT */
        node = prog_create_node(PROG_NODE_NOT);
        node->children[0] = generate_random_program(max_depth - 1, num_inputs);
        node->child_count = 1;
        break;
    case 5: /* IF */
        node = prog_create_node(PROG_NODE_IF);
        node->children[0] = generate_random_program(max_depth - 1, num_inputs);
        node->children[1] = generate_random_program(max_depth - 1, num_inputs);
        node->children[2] = generate_random_program(max_depth - 1, num_inputs);
        node->child_count = 3;
        break;
    default: /* Terminal */
        if (moses_random_float() < 0.5f) {
            node = prog_create_const(moses_random_float() * 2.0f - 1.0f);
        } else {
            node = prog_create_input(moses_random_int(num_inputs > 0 ? num_inputs : 1));
        }
        break;
    }

    return node;
}

/* ========================================================================
 * Genetic Operators
 * ======================================================================== */

/**
 * Collect all nodes in a tree into a flat array
 */
static uint32_t collect_nodes(ProgNode *node, ProgNode **array, uint32_t max)
{
    uint32_t count = 0;
    uint32_t i;

    if (!node || count >= max) return 0;

    array[count++] = node;

    for (i = 0; i < node->child_count && count < max; i++) {
        count += collect_nodes(node->children[i], &array[count], max - count);
    }

    return count;
}

/**
 * Subtree crossover: Replace a random subtree in parent1 with a subtree from parent2
 */
ProgNode* moses_crossover(ProgNode *parent1, ProgNode *parent2)
{
    ProgNode *child;
    ProgNode *nodes1[256], *nodes2[256];
    uint32_t count1, count2;
    uint32_t pick1, pick2;

    if (!parent1 || !parent2) return NULL;

    /* Copy parent1 as the child */
    child = prog_copy(parent1);
    if (!child) return NULL;

    /* Collect all nodes from both trees */
    count1 = collect_nodes(child, nodes1, 256);
    count2 = collect_nodes(parent2, nodes2, 256);

    if (count1 < 2 || count2 < 1) return child;

    /* Pick a random non-root node in child */
    pick1 = 1 + moses_random_int(count1 - 1);
    /* Pick a random node in parent2 */
    pick2 = moses_random_int(count2);

    /* Find the parent of the picked node in child and replace */
    ProgNode *target = nodes1[pick1];
    ProgNode *replacement = prog_copy(nodes2[pick2]);

    if (!replacement) return child;

    /* Find and replace in parent */
    uint32_t i, j;
    for (i = 0; i < count1; i++) {
        for (j = 0; j < nodes1[i]->child_count; j++) {
            if (nodes1[i]->children[j] == target) {
                prog_destroy(target);
                nodes1[i]->children[j] = replacement;
                return child;
            }
        }
    }

    /* If we couldn't find the parent, just return the copy */
    prog_destroy(replacement);
    return child;
}

/**
 * Point mutation: Randomly change a node's type or value
 */
void moses_mutate(ProgNode *program, float rate)
{
    uint32_t i;

    if (!program) return;

    if (moses_random_float() < rate) {
        /* Mutate this node */
        if (program->type == PROG_NODE_CONST) {
            /* Perturb constant value */
            program->value += (moses_random_float() - 0.5f) * 0.5f;
        } else if (program->type == PROG_NODE_INPUT) {
            /* Change input index */
            program->input_idx = moses_random_int(4);
        } else if (program->child_count == 2) {
            /* Swap binary operator */
            ProgNodeType ops[] = { PROG_NODE_AND, PROG_NODE_OR,
                                   PROG_NODE_PLUS, PROG_NODE_TIMES };
            program->type = ops[moses_random_int(4)];
        }
    }

    /* Recurse into children */
    for (i = 0; i < program->child_count; i++) {
        moses_mutate(program->children[i], rate);
    }
}

/**
 * Tournament selection
 */
Individual* moses_tournament_select(MOSESEngine *engine)
{
    Individual *best = NULL;
    uint32_t i;

    for (i = 0; i < engine->config.tournament_size; i++) {
        uint32_t idx = moses_random_int(engine->pop_size);
        Individual *candidate = &engine->population[idx];

        if (!best || candidate->score > best->score) {
            best = candidate;
        }
    }

    return best;
}

/* ========================================================================
 * MOSES Engine Implementation
 * ======================================================================== */

MOSESConfig moses_default_config(void)
{
    MOSESConfig config;
    config.max_generations = 100;
    config.population_size = 256;
    config.mutation_rate = MOSES_DEFAULT_MUTATION_RATE;
    config.crossover_rate = MOSES_DEFAULT_CROSSOVER_RATE;
    config.complexity_penalty = 0.01f;
    config.tournament_size = MOSES_TOURNAMENT_SIZE;
    config.target_fitness = 0.99f;
    config.max_evals = 100000;
    config.elitism = 2;
    return config;
}

MOSESEngine* moses_engine_create(AtomSpace *as, MOSESConfig *config)
{
    MOSESEngine *engine;
    uint32_t i;

    engine = (MOSESEngine *)calloc(1, sizeof(MOSESEngine));
    if (!engine) return NULL;

    engine->atomspace = as;
    engine->config = config ? *config : moses_default_config();
    engine->next_id = 1;

    /* Allocate population */
    engine->pop_size = engine->config.population_size;
    if (engine->pop_size > MOSES_MAX_POPULATION) {
        engine->pop_size = MOSES_MAX_POPULATION;
    }

    engine->population = (Individual *)calloc(engine->pop_size, sizeof(Individual));
    if (!engine->population) {
        free(engine);
        return NULL;
    }

    /* Initialize population with random programs */
    moses_seed_rng();
    for (i = 0; i < engine->pop_size; i++) {
        engine->population[i].program = generate_random_program(4, 4);
        engine->population[i].fitness = 0.0f;
        engine->population[i].complexity = 0.0f;
        engine->population[i].score = 0.0f;
        engine->population[i].generation = 0;
        engine->population[i].id = engine->next_id++;
        engine->population[i].evaluated = 0;
    }

    memset(&engine->stats, 0, sizeof(MOSESStats));

    printf("MOSES Engine: Created with population %u, max generations %u\n",
           engine->pop_size, engine->config.max_generations);

    return engine;
}

void moses_engine_destroy(MOSESEngine *engine)
{
    uint32_t i;

    if (!engine) return;

    if (engine->population) {
        for (i = 0; i < engine->pop_size; i++) {
            prog_destroy(engine->population[i].program);
        }
        free(engine->population);
    }

    printf("MOSES Engine: Destroyed (total evaluations: %u)\n",
           engine->stats.total_evaluations);

    free(engine);
}

void moses_set_fitness(MOSESEngine *engine, MOSESFitnessFunc func, void *data)
{
    if (!engine) return;
    engine->fitness_func = func;
    engine->fitness_data = data;
}

/**
 * Evaluate fitness for all unevaluated individuals
 */
static void evaluate_population(MOSESEngine *engine)
{
    uint32_t i;

    if (!engine->fitness_func) return;

    for (i = 0; i < engine->pop_size; i++) {
        Individual *ind = &engine->population[i];

        if (!ind->evaluated && ind->program) {
            ind->fitness = engine->fitness_func(ind->program,
                                                 engine->fitness_data);
            ind->complexity = prog_complexity(ind->program) *
                              engine->config.complexity_penalty;
            ind->score = ind->fitness - ind->complexity;
            ind->evaluated = 1;
            engine->stats.total_evaluations++;
        }
    }
}

/**
 * Sort population by score (descending)
 */
static int compare_individuals(const void *a, const void *b)
{
    const Individual *ia = (const Individual *)a;
    const Individual *ib = (const Individual *)b;
    if (ib->score > ia->score) return 1;
    if (ib->score < ia->score) return -1;
    return 0;
}

static void sort_population(MOSESEngine *engine)
{
    qsort(engine->population, engine->pop_size, sizeof(Individual),
          compare_individuals);
}

/**
 * Update statistics after evaluation
 */
static void update_stats(MOSESEngine *engine)
{
    uint32_t i;
    float sum = 0.0f;

    engine->stats.best_fitness = engine->population[0].fitness;
    engine->stats.best_score = engine->population[0].score;
    engine->stats.worst_fitness = engine->population[engine->pop_size - 1].fitness;

    for (i = 0; i < engine->pop_size; i++) {
        sum += engine->population[i].fitness;
    }
    engine->stats.avg_fitness = sum / (float)engine->pop_size;
}

int moses_evolve_generation(MOSESEngine *engine)
{
    Individual *new_pop;
    uint32_t i;
    uint32_t elitism;

    if (!engine) return -1;

    /* Evaluate current population */
    evaluate_population(engine);
    sort_population(engine);
    update_stats(engine);

    engine->stats.generation++;

    /* Check termination */
    if (engine->stats.best_fitness >= engine->config.target_fitness) {
        printf("MOSES: Target fitness reached (%.4f >= %.4f)\n",
               engine->stats.best_fitness, engine->config.target_fitness);
        return 1;  /* Done */
    }

    if (engine->stats.total_evaluations >= engine->config.max_evals) {
        printf("MOSES: Max evaluations reached (%u)\n",
               engine->stats.total_evaluations);
        return 1;  /* Done */
    }

    /* Create new population */
    new_pop = (Individual *)calloc(engine->pop_size, sizeof(Individual));
    if (!new_pop) return -1;

    /* Elitism: keep best individuals */
    elitism = (uint32_t)engine->config.elitism;
    if (elitism > engine->pop_size) elitism = engine->pop_size;

    for (i = 0; i < elitism; i++) {
        new_pop[i].program = prog_copy(engine->population[i].program);
        new_pop[i].fitness = engine->population[i].fitness;
        new_pop[i].complexity = engine->population[i].complexity;
        new_pop[i].score = engine->population[i].score;
        new_pop[i].generation = engine->stats.generation;
        new_pop[i].id = engine->next_id++;
        new_pop[i].evaluated = 1;
    }

    /* Fill rest with offspring */
    for (i = elitism; i < engine->pop_size; i++) {
        Individual *parent1 = moses_tournament_select(engine);
        Individual *parent2 = moses_tournament_select(engine);

        if (moses_random_float() < engine->config.crossover_rate) {
            new_pop[i].program = moses_crossover(parent1->program,
                                                  parent2->program);
        } else {
            new_pop[i].program = prog_copy(parent1->program);
        }

        /* Mutate */
        moses_mutate(new_pop[i].program, engine->config.mutation_rate);

        new_pop[i].generation = engine->stats.generation;
        new_pop[i].id = engine->next_id++;
        new_pop[i].evaluated = 0;
    }

    /* Replace old population */
    for (i = 0; i < engine->pop_size; i++) {
        prog_destroy(engine->population[i].program);
    }
    free(engine->population);
    engine->population = new_pop;

    return 0;
}

int moses_run(MOSESEngine *engine, uint32_t max_generations)
{
    uint32_t gen;
    int result;

    if (!engine) return -1;

    printf("MOSES: Starting evolution (max %u generations)\n", max_generations);

    for (gen = 0; gen < max_generations; gen++) {
        result = moses_evolve_generation(engine);

        if (gen % 10 == 0 || result != 0) {
            printf("MOSES: Gen %u - best=%.4f avg=%.4f evals=%u\n",
                   engine->stats.generation,
                   engine->stats.best_fitness,
                   engine->stats.avg_fitness,
                   engine->stats.total_evaluations);
        }

        if (result != 0) break;
    }

    printf("MOSES: Evolution complete after %u generations\n",
           engine->stats.generation);

    return 0;
}

Individual* moses_get_best(MOSESEngine *engine)
{
    if (!engine || engine->pop_size == 0) return NULL;

    evaluate_population(engine);
    sort_population(engine);

    return &engine->population[0];
}

void moses_print_stats(MOSESEngine *engine)
{
    if (!engine) return;

    printf("\nMOSES Engine Statistics:\n");
    printf("  Generation: %u\n", engine->stats.generation);
    printf("  Total evaluations: %u\n", engine->stats.total_evaluations);
    printf("  Best fitness: %.4f\n", engine->stats.best_fitness);
    printf("  Average fitness: %.4f\n", engine->stats.avg_fitness);
    printf("  Worst fitness: %.4f\n", engine->stats.worst_fitness);
    printf("  Best score (fitness - complexity): %.4f\n", engine->stats.best_score);
    printf("  Population size: %u\n", engine->pop_size);
    printf("  Mutation rate: %.4f\n", engine->config.mutation_rate);
    printf("  Crossover rate: %.4f\n", engine->config.crossover_rate);

    Individual *best = moses_get_best(engine);
    if (best && best->program) {
        printf("  Best program: ");
        prog_print(best->program);
        printf("\n");
    }
}
