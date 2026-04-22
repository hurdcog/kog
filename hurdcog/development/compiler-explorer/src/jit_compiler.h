/*
 * HurdCog Compiler Explorer JIT Infrastructure
 * Part of Phase 2: Microkernel Integration - SKZ Framework
 * 
 * Copyright (C) 2025 HurdCog Cognitive Architecture Project
 * Licensed under the GNU General Public License v3.0
 */

#ifndef HURDCOG_JIT_COMPILER_H
#define HURDCOG_JIT_COMPILER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* JIT Compiler Configuration */
typedef struct {
    int optimization_level;      /* 0-3, optimization level */
    bool enable_cognitive;       /* Enable cognitive analysis */
    bool enable_logging;         /* Enable detailed logging */
    const char* target_arch;     /* Target architecture (default: host) */
    const char* log_file;        /* Log file path (NULL for stderr) */
} jit_config_t;

/* JIT Compilation Context */
typedef struct jit_context jit_context_t;

/* JIT Error Codes */
typedef enum {
    JIT_SUCCESS = 0,
    JIT_ERROR_INIT = -1,
    JIT_ERROR_COMPILE = -2,
    JIT_ERROR_EXECUTE = -3,
    JIT_ERROR_INVALID_CODE = -4,
    JIT_ERROR_OUT_OF_MEMORY = -5,
    JIT_ERROR_UNSUPPORTED = -6
} jit_error_t;

/* JIT Compilation Statistics */
typedef struct {
    uint64_t compile_time_us;    /* Compilation time in microseconds */
    uint64_t code_size_bytes;    /* Generated code size */
    uint32_t optimization_passes; /* Number of optimization passes */
    uint32_t cognitive_hints;    /* Number of cognitive optimization hints */
} jit_stats_t;

/**
 * Initialize JIT compiler infrastructure
 * @param config Configuration parameters (NULL for defaults)
 * @return JIT context on success, NULL on failure
 */
jit_context_t* jit_init(const jit_config_t* config);

/**
 * Compile source code using JIT
 * @param ctx JIT context
 * @param source_code C source code string
 * @param entry_function Name of entry function
 * @param stats Compilation statistics output (optional)
 * @return JIT_SUCCESS on success, error code on failure
 */
jit_error_t jit_compile(jit_context_t* ctx, 
                        const char* source_code,
                        const char* entry_function,
                        jit_stats_t* stats);

/**
 * Execute JIT compiled function
 * @param ctx JIT context
 * @param function_name Function to execute
 * @param args Function arguments (implementation-defined)
 * @param result Return value (implementation-defined)
 * @return JIT_SUCCESS on success, error code on failure
 */
jit_error_t jit_execute(jit_context_t* ctx,
                        const char* function_name,
                        void* args,
                        void* result);

/**
 * Apply cognitive optimization hints
 * @param ctx JIT context
 * @param hint_type Optimization hint type
 * @param hint_data Hint-specific data
 * @return JIT_SUCCESS on success, error code on failure
 */
jit_error_t jit_apply_cognitive_hint(jit_context_t* ctx,
                                     const char* hint_type,
                                     const void* hint_data);

/**
 * Get performance statistics
 * @param ctx JIT context
 * @param stats Statistics output
 * @return JIT_SUCCESS on success, error code on failure
 */
jit_error_t jit_get_stats(jit_context_t* ctx, jit_stats_t* stats);

/**
 * Get error message for last error
 * @param ctx JIT context
 * @return Human-readable error message
 */
const char* jit_get_error(jit_context_t* ctx);

/**
 * Cleanup and destroy JIT context
 * @param ctx JIT context to destroy
 */
void jit_destroy(jit_context_t* ctx);

/**
 * Check if JIT is available on this platform
 * @return true if JIT is available, false otherwise
 */
bool jit_is_available(void);

/**
 * Get JIT compiler version information
 * @return Version string
 */
const char* jit_get_version(void);

#ifdef __cplusplus
}
#endif

#endif /* HURDCOG_JIT_COMPILER_H */