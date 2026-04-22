/*
 * Learning Filesystem (learnfs)
 *
 * Exposes MOSES evolutionary learning and URE rule engine
 * as a 9P filesystem.
 *
 * Filesystem layout:
 *   /learning/
 *     moses/
 *       evolve     - Write fitness function + params, read best program
 *       population - Read current population stats
 *       params     - Read/write MOSES parameters
 *     ure/
 *       rules      - Directory of URE rules
 *       engine     - Write query, read results
 *       config     - Read/write URE configuration
 *     stats        - Read combined learning statistics
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

/* MOSES parameters */
typedef struct MosesParams {
    int    population_size;
    int    max_generations;
    float  mutation_rate;
    float  crossover_rate;
    int    tournament_size;
    float  fitness_threshold;
} MosesParams;

/* MOSES population member */
typedef struct MosesMember {
    char    program[512];
    float   fitness;
    int     generation;
    int     active;
} MosesMember;

/* URE rule */
typedef struct URERule {
    char    name[64];
    char    pattern[256];
    char    action[256];
    float   weight;
    int     active;
} URERule;

/* LearnFS state */
typedef struct LearnFS {
    MosesParams    moses_params;
    MosesMember    population[256];
    int            pop_count;
    int            generation;
    URERule        ure_rules[64];
    int            ure_rule_count;
    int            total_evaluations;
    int            initialized;
} LearnFS;

static LearnFS g_learnfs;

/*
 * Initialize the learning filesystem
 */
int
learnfs_init(void)
{
    memset(&g_learnfs, 0, sizeof(LearnFS));
    g_learnfs.initialized = 1;

    /* Default MOSES parameters */
    g_learnfs.moses_params.population_size = 100;
    g_learnfs.moses_params.max_generations = 50;
    g_learnfs.moses_params.mutation_rate = 0.1f;
    g_learnfs.moses_params.crossover_rate = 0.7f;
    g_learnfs.moses_params.tournament_size = 5;
    g_learnfs.moses_params.fitness_threshold = 0.95f;

    /* Register default URE rules */
    struct { const char *name; const char *pattern; const char *action; float weight; } defaults[] = {
        {"deduction",     "InheritanceLink($A,$B), InheritanceLink($B,$C)",
                          "InheritanceLink($A,$C)", 1.0f},
        {"modus_ponens",  "ImplicationLink($A,$B), $A",
                          "$B", 0.9f},
        {"and_intro",     "$A, $B",
                          "AndLink($A,$B)", 0.8f},
        {"or_intro",      "$A",
                          "OrLink($A,$B)", 0.7f},
    };

    for (int i = 0; i < 4; i++) {
        strncpy(g_learnfs.ure_rules[i].name, defaults[i].name, 63);
        strncpy(g_learnfs.ure_rules[i].pattern, defaults[i].pattern, 255);
        strncpy(g_learnfs.ure_rules[i].action, defaults[i].action, 255);
        g_learnfs.ure_rules[i].weight = defaults[i].weight;
        g_learnfs.ure_rules[i].active = 1;
    }
    g_learnfs.ure_rule_count = 4;

    return 0;
}

/*
 * Shutdown the learning filesystem
 */
void
learnfs_shutdown(void)
{
    g_learnfs.initialized = 0;
}

/*
 * Handle read from /learning/moses/params
 */
int
learnfs_moses_params(char *buf, size_t maxlen)
{
    if (!g_learnfs.initialized)
        return -1;

    return snprintf(buf, maxlen,
        "MOSES Parameters\n"
        "  Population size: %d\n"
        "  Max generations: %d\n"
        "  Mutation rate: %.3f\n"
        "  Crossover rate: %.3f\n"
        "  Tournament size: %d\n"
        "  Fitness threshold: %.3f\n"
        "  Current generation: %d\n"
        "  Current population: %d\n",
        g_learnfs.moses_params.population_size,
        g_learnfs.moses_params.max_generations,
        g_learnfs.moses_params.mutation_rate,
        g_learnfs.moses_params.crossover_rate,
        g_learnfs.moses_params.tournament_size,
        g_learnfs.moses_params.fitness_threshold,
        g_learnfs.generation,
        g_learnfs.pop_count);
}

/*
 * Handle read from /learning/ure/rules
 */
int
learnfs_ure_rules(char *buf, size_t maxlen)
{
    if (!g_learnfs.initialized)
        return -1;

    int offset = 0;
    offset += snprintf(buf + offset, maxlen - offset, "URE Rules (%d active)\n",
                       g_learnfs.ure_rule_count);

    for (int i = 0; i < g_learnfs.ure_rule_count; i++) {
        if (g_learnfs.ure_rules[i].active) {
            offset += snprintf(buf + offset, maxlen - offset,
                "  %s (weight: %.2f)\n"
                "    Pattern: %s\n"
                "    Action:  %s\n",
                g_learnfs.ure_rules[i].name,
                g_learnfs.ure_rules[i].weight,
                g_learnfs.ure_rules[i].pattern,
                g_learnfs.ure_rules[i].action);
        }
    }

    return offset;
}

/*
 * Handle read from /learning/stats
 */
int
learnfs_stats(char *buf, size_t maxlen)
{
    if (!g_learnfs.initialized)
        return -1;

    float best_fitness = -1.0f;
    for (int i = 0; i < g_learnfs.pop_count; i++) {
        if (g_learnfs.population[i].active &&
            g_learnfs.population[i].fitness > best_fitness)
            best_fitness = g_learnfs.population[i].fitness;
    }

    return snprintf(buf, maxlen,
        "Learning Filesystem Statistics\n"
        "  MOSES generation: %d\n"
        "  Population size: %d\n"
        "  Best fitness: %.4f\n"
        "  Total evaluations: %d\n"
        "  URE rules: %d\n",
        g_learnfs.generation,
        g_learnfs.pop_count,
        best_fitness,
        g_learnfs.total_evaluations,
        g_learnfs.ure_rule_count);
}
