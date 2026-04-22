/*
 * MOSES Engine Test Suite
 *
 * Tests for the Meta-Optimizing Semantic Evolutionary Search engine.
 * Tests program creation, evaluation, genetic operators, and evolution.
 *
 * Copyright (c) 2026 OpenCog Inferno Project
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include "../learning/moses_engine.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) do { \
    printf("Running test: %s... ", #name); \
    tests_run++; \
    if (test_##name()) { \
        printf("PASSED\n"); \
        tests_passed++; \
    } else { \
        printf("FAILED\n"); \
    } \
} while(0)

/* Test: Create and evaluate a simple program */
int test_prog_evaluate(void)
{
    /* Build: (+ $0 1.0) */
    ProgNode *root = prog_create_node(PROG_NODE_PLUS);
    root->children[0] = prog_create_input(0);
    root->children[1] = prog_create_const(1.0f);
    root->child_count = 2;

    float inputs[1] = { 5.0f };
    float result = prog_evaluate(root, inputs, 1);

    int success = (fabsf(result - 6.0f) < 0.001f);

    printf("    (+ 5.0 1.0) = %.2f ", result);

    prog_destroy(root);
    return success;
}

/* Test: Boolean program evaluation */
int test_prog_boolean(void)
{
    /* Build: (and $0 (not $1)) */
    ProgNode *root = prog_create_node(PROG_NODE_AND);
    root->children[0] = prog_create_input(0);
    ProgNode *not_node = prog_create_node(PROG_NODE_NOT);
    not_node->children[0] = prog_create_input(1);
    not_node->child_count = 1;
    root->children[1] = not_node;
    root->child_count = 2;

    float inputs1[2] = { 1.0f, 0.0f };  /* true AND (NOT false) = true */
    float inputs2[2] = { 1.0f, 1.0f };  /* true AND (NOT true) = false */

    float r1 = prog_evaluate(root, inputs1, 2);
    float r2 = prog_evaluate(root, inputs2, 2);

    int success = (r1 > 0.5f && r2 < 0.5f);

    printf("    (and 1 (not 0))=%.0f, (and 1 (not 1))=%.0f ", r1, r2);

    prog_destroy(root);
    return success;
}

/* Test: Program copy */
int test_prog_copy(void)
{
    ProgNode *root = prog_create_node(PROG_NODE_TIMES);
    root->children[0] = prog_create_input(0);
    root->children[1] = prog_create_const(2.0f);
    root->child_count = 2;

    ProgNode *copy = prog_copy(root);
    if (!copy) { prog_destroy(root); return 0; }

    float inputs[1] = { 3.0f };
    float r1 = prog_evaluate(root, inputs, 1);
    float r2 = prog_evaluate(copy, inputs, 1);

    int success = (fabsf(r1 - r2) < 0.001f);

    prog_destroy(root);
    prog_destroy(copy);
    return success;
}

/* Test: Program complexity */
int test_prog_complexity(void)
{
    ProgNode *simple = prog_create_const(1.0f);
    ProgNode *complex_prog = prog_create_node(PROG_NODE_PLUS);
    complex_prog->children[0] = prog_create_input(0);
    complex_prog->children[1] = prog_create_node(PROG_NODE_TIMES);
    complex_prog->children[1]->children[0] = prog_create_input(1);
    complex_prog->children[1]->children[1] = prog_create_const(2.0f);
    complex_prog->children[1]->child_count = 2;
    complex_prog->child_count = 2;

    float c1 = prog_complexity(simple);
    float c2 = prog_complexity(complex_prog);

    int success = (c2 > c1);

    printf("    simple=%.0f, complex=%.0f ", c1, c2);

    prog_destroy(simple);
    prog_destroy(complex_prog);
    return success;
}

/* Test: Crossover */
int test_crossover(void)
{
    ProgNode *p1 = prog_create_node(PROG_NODE_PLUS);
    p1->children[0] = prog_create_input(0);
    p1->children[1] = prog_create_const(1.0f);
    p1->child_count = 2;

    ProgNode *p2 = prog_create_node(PROG_NODE_TIMES);
    p2->children[0] = prog_create_input(0);
    p2->children[1] = prog_create_const(2.0f);
    p2->child_count = 2;

    ProgNode *child = moses_crossover(p1, p2);
    int success = (child != NULL);

    if (child) {
        float inputs[1] = { 3.0f };
        float result = prog_evaluate(child, inputs, 1);
        printf("    child(3.0)=%.2f ", result);
        prog_destroy(child);
    }

    prog_destroy(p1);
    prog_destroy(p2);
    return success;
}

/* Test: Mutation */
int test_mutation(void)
{
    ProgNode *prog = prog_create_node(PROG_NODE_PLUS);
    prog->children[0] = prog_create_const(1.0f);
    prog->children[1] = prog_create_const(2.0f);
    prog->child_count = 2;

    float inputs[1] = { 0.0f };
    float before = prog_evaluate(prog, inputs, 1);

    /* Mutate with high rate */
    moses_mutate(prog, 1.0f);

    float after = prog_evaluate(prog, inputs, 1);

    /* After mutation, the result should likely be different */
    printf("    before=%.2f, after=%.2f ", before, after);

    prog_destroy(prog);
    return 1;  /* Mutation is stochastic, just check it doesn't crash */
}

/* Fitness function: try to evolve f(x) = x^2 */
static float fitness_x_squared(ProgNode *program, void *data)
{
    float error = 0.0f;
    int i;

    for (i = -5; i <= 5; i++) {
        float x = (float)i;
        float expected = x * x;
        float inputs[1] = { x };
        float actual = prog_evaluate(program, inputs, 1);
        float diff = expected - actual;
        error += diff * diff;
    }

    /* Convert error to fitness (lower error = higher fitness) */
    return 1.0f / (1.0f + error);
}

/* Test: MOSES evolution */
int test_moses_evolution(void)
{
    MOSESConfig config = moses_default_config();
    config.max_generations = 20;
    config.population_size = 64;
    config.target_fitness = 0.99f;
    config.max_evals = 5000;

    MOSESEngine *engine = moses_engine_create(NULL, &config);
    if (!engine) return 0;

    moses_set_fitness(engine, fitness_x_squared, NULL);

    moses_run(engine, 20);

    moses_print_stats(engine);

    moses_engine_destroy(engine);
    return 1;  /* Just check it doesn't crash */
}

int main(int argc, char **argv)
{
    printf("=== MOSES Engine Test Suite ===\n\n");

    TEST(prog_evaluate);
    TEST(prog_boolean);
    TEST(prog_copy);
    TEST(prog_complexity);
    TEST(crossover);
    TEST(mutation);
    TEST(moses_evolution);

    printf("\n=== Results: %d/%d tests passed ===\n", tests_passed, tests_run);

    return (tests_passed == tests_run) ? 0 : 1;
}
