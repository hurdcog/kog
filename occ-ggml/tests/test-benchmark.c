/**
 * OCC-GGML Benchmarks
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "occ-ggml.h"
#include "occ-atomspace.h"
#include "occ-propagation.h"

static double get_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

void benchmark_atomspace_creation(void) {
    printf("\n=== AtomSpace Creation Benchmark ===\n");
    
    int sizes[] = {1000, 10000, 100000};
    int n_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int i = 0; i < n_sizes; i++) {
        int size = sizes[i];
        
        double start = get_time_ms();
        struct occ_atomspace* as = occ_atomspace_new(size, 64, false);
        double create_time = get_time_ms() - start;
        
        // Add nodes
        start = get_time_ms();
        for (int j = 0; j < size / 10; j++) {
            char name[32];
            snprintf(name, sizeof(name), "Node_%d", j);
            occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, name, NULL);
        }
        double add_time = get_time_ms() - start;
        
        printf("  Size %6d: create=%.2fms, add %d nodes=%.2fms\n",
               size, create_time, size / 10, add_time);
        
        occ_atomspace_free(as);
    }
}

void benchmark_propagation(void) {
    printf("\n=== Propagation Benchmark ===\n");
    
    int sizes[] = {100, 200, 300};
    int n_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int i = 0; i < n_sizes; i++) {
        int n = sizes[i];
        
        // Create AtomSpace with extra capacity for links
        struct occ_atomspace* as = occ_atomspace_new(n * 2, 32, true);
        if (!as) continue;
        
        // Add nodes
        int64_t* nodes = (int64_t*)malloc(n * sizeof(int64_t));
        for (int j = 0; j < n; j++) {
            char name[32];
            snprintf(name, sizeof(name), "N%d", j);
            nodes[j] = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, name, NULL);
        }
        
        // Add a small number of random edges
        int n_edges = n / 4;
        for (int j = 0; j < n_edges; j++) {
            int64_t src = nodes[rand() % n];
            int64_t dst = nodes[rand() % n];
            if (src != dst) {
                int64_t out[] = {src, dst};
                occ_atomspace_add_link(as, OCC_ATOM_SIMILARITY_LINK, out, 2, NULL);
            }
        }
        
        // Benchmark propagation
        struct occ_propagation_state* prop = occ_propagation_new(as);
        struct occ_propagation_params params = OCC_DEFAULT_PROPAGATION_PARAMS;
        params.decay_rate = 0.8f;
        params.max_hops = 5;
        
        int n_trials = 10;
        double total_time = 0.0;
        int total_iterations = 0;
        
        for (int t = 0; t < n_trials; t++) {
            occ_propagation_reset(prop);
            int64_t source = nodes[rand() % n];
            
            double start = get_time_ms();
            occ_propagate_from_atom(prop, source, 1.0f, &params);
            total_time += get_time_ms() - start;
            total_iterations += prop->total_iterations;
        }
        
        printf("  N=%4d: avg=%.2fms, iterations=%.1f, active=%d\n",
               n, total_time / n_trials, (float)total_iterations / n_trials,
               prop->n_active_atoms);
        
        free(nodes);
        occ_propagation_free(prop);
        occ_atomspace_free(as);
    }
}

void benchmark_matrix_multiply(void) {
    printf("\n=== Matrix Multiply Benchmark ===\n");
    
    int sizes[] = {64, 128, 256, 512, 1024};
    int n_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int i = 0; i < n_sizes; i++) {
        int n = sizes[i];
        
        struct occ_context* ctx = occ_init(n * n * 4 * 10);
        
        struct occ_tensor* a = occ_new_tensor_2d(ctx, OCC_TYPE_F32, n, n);
        struct occ_tensor* b = occ_new_tensor_2d(ctx, OCC_TYPE_F32, n, n);
        
        // Initialize with random data
        float* pa = (float*)a->data;
        float* pb = (float*)b->data;
        for (int j = 0; j < n * n; j++) {
            pa[j] = (float)rand() / RAND_MAX;
            pb[j] = (float)rand() / RAND_MAX;
        }
        
        // Create computation graph
        struct occ_tensor* c = occ_mul_mat(ctx, a, b);
        struct occ_cgraph* graph = occ_new_graph(ctx);
        occ_build_forward(graph, c);
        
        // Benchmark
        int n_trials = 5;
        double total_time = 0.0;
        
        for (int t = 0; t < n_trials; t++) {
            double start = get_time_ms();
            occ_compute(graph);
            total_time += get_time_ms() - start;
        }
        
        double avg_time = total_time / n_trials;
        double gflops = (2.0 * n * n * n) / (avg_time * 1e6);
        
        printf("  %4dx%4d: %.2fms (%.2f GFLOPS)\n", n, n, avg_time, gflops);
        
        occ_free(ctx);
    }
}

void benchmark_oscillator_dynamics(void) {
    printf("\n=== Oscillator Dynamics Benchmark ===\n");
    
    int sizes[] = {50, 100, 150};
    int n_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int i = 0; i < n_sizes; i++) {
        int n = sizes[i];
        
        // Extra capacity for links
        struct occ_atomspace* as = occ_atomspace_new(n * 3, 32, true);
        if (!as) continue;
        
        // Create ring topology
        int64_t* nodes = (int64_t*)malloc(n * sizeof(int64_t));
        for (int j = 0; j < n; j++) {
            char name[32];
            snprintf(name, sizeof(name), "N%d", j);
            nodes[j] = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, name, NULL);
        }
        
        for (int j = 0; j < n; j++) {
            int64_t out[] = {nodes[j], nodes[(j + 1) % n]};
            occ_atomspace_add_link(as, OCC_ATOM_SIMILARITY_LINK, out, 2, NULL);
        }
        
        struct occ_propagation_state* prop = occ_propagation_new(as);
        
        struct occ_oscillator_params osc_params = {
            .frequency = 1.0f,
            .damping = 0.5f,
            .coupling = 0.3f,
            .phase_noise = 0.1f,
            .n_harmonics = 1
        };
        
        occ_oscillator_init(prop, &osc_params);
        
        // Inject activation
        struct occ_propagation_params params = OCC_DEFAULT_PROPAGATION_PARAMS;
        occ_propagate_from_atom(prop, nodes[0], 1.0f, &params);
        
        // Benchmark oscillator steps
        int n_steps = 100;
        double start = get_time_ms();
        
        for (int s = 0; s < n_steps; s++) {
            occ_oscillator_step(prop, &osc_params, 0.1f);
        }
        
        double total_time = get_time_ms() - start;
        
        printf("  N=%4d: %d steps in %.2fms (%.2f steps/ms)\n",
               n, n_steps, total_time, n_steps / total_time);
        
        free(nodes);
        occ_propagation_free(prop);
        occ_atomspace_free(as);
    }
}

void benchmark_memory_usage(void) {
    printf("\n=== Memory Usage Estimate ===\n");
    
    int sizes[] = {1000, 10000, 100000};
    int n_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int i = 0; i < n_sizes; i++) {
        int n = sizes[i];
        int embed_dim = 64;
        
        // Calculate memory usage
        size_t embeddings = n * embed_dim * sizeof(float);
        size_t types = n * sizeof(int32_t);
        size_t tv = n * 2 * sizeof(float) + n * sizeof(int64_t);
        size_t av = n * 3 * sizeof(float);
        size_t activation = n * sizeof(float);
        size_t dense_adj = (size_t)n * n * sizeof(float);
        
        size_t total_sparse = embeddings + types + tv + av + activation;
        size_t total_dense = total_sparse + dense_adj;
        
        printf("  N=%6d:\n", n);
        printf("    Embeddings:  %8.2f MB\n", embeddings / (1024.0 * 1024.0));
        printf("    Sparse total: %7.2f MB\n", total_sparse / (1024.0 * 1024.0));
        printf("    Dense total:  %7.2f MB\n", total_dense / (1024.0 * 1024.0));
    }
}

int main(void) {
    printf("OCC-GGML Benchmarks\n");
    printf("===================\n");
    
    srand((unsigned int)time(NULL));
    
    benchmark_atomspace_creation();
    benchmark_propagation();
    benchmark_matrix_multiply();
    benchmark_oscillator_dynamics();
    benchmark_memory_usage();
    
    printf("\nBenchmarks completed.\n");
    return 0;
}
