/*
 * HurdCog Compiler Explorer JIT Infrastructure - Public API
 * Part of Phase 2: Microkernel Integration - SKZ Framework
 * 
 * This is the public interface for the HurdCog JIT compiler infrastructure.
 * Include this header to use JIT compilation features in your applications.
 * 
 * Copyright (C) 2025 HurdCog Cognitive Architecture Project
 * Licensed under the GNU General Public License v3.0
 */

#ifndef HURDCOG_JIT_H
#define HURDCOG_JIT_H

/* Include the main JIT compiler interface */
#include "../src/jit_compiler.h"

/* 
 * Public API convenience macros and helpers
 */

/* Default JIT configuration for microkernel development */
#define HURDCOG_JIT_DEFAULT_CONFIG() \
    { \
        .optimization_level = 2, \
        .enable_cognitive = true, \
        .enable_logging = true, \
        .target_arch = "native", \
        .log_file = NULL \
    }

/* Microkernel-specific JIT configuration */
#define HURDCOG_JIT_MICROKERNEL_CONFIG() \
    { \
        .optimization_level = 3, \
        .enable_cognitive = true, \
        .enable_logging = false, \
        .target_arch = "native", \
        .log_file = "/var/log/hurdcog-jit.log" \
    }

/* 
 * Convenience functions for common use cases
 */

/**
 * Quick JIT compile and execute for simple functions
 * @param source_code C source code string
 * @param function_name Function to compile and execute
 * @param result Execution result (optional)
 * @return JIT_SUCCESS on success, error code on failure
 */
static inline jit_error_t hurdcog_jit_quick_execute(const char* source_code,
                                                     const char* function_name,
                                                     void* result) {
    jit_context_t* ctx = jit_init(NULL);
    if (!ctx) return JIT_ERROR_INIT;
    
    jit_error_t compile_result = jit_compile(ctx, source_code, function_name, NULL);
    if (compile_result != JIT_SUCCESS) {
        jit_destroy(ctx);
        return compile_result;
    }
    
    jit_error_t exec_result = jit_execute(ctx, function_name, NULL, result);
    jit_destroy(ctx);
    
    return exec_result;
}

/**
 * Initialize JIT with microkernel-optimized settings
 * @return JIT context on success, NULL on failure
 */
static inline jit_context_t* hurdcog_jit_init_microkernel(void) {
    jit_config_t config = HURDCOG_JIT_MICROKERNEL_CONFIG();
    return jit_init(&config);
}

#endif /* HURDCOG_JIT_H */