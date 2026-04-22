/*
 * HurdCog Compiler Explorer JIT Infrastructure - Configuration
 * Part of Phase 2: Microkernel Integration - SKZ Framework
 * 
 * Copyright (C) 2025 HurdCog Cognitive Architecture Project
 * Licensed under the GNU General Public License v3.0
 */

#ifndef HURDCOG_JIT_CONFIG_H
#define HURDCOG_JIT_CONFIG_H

/* Build-time configuration */
#define HURDCOG_JIT_VERSION_MAJOR   1
#define HURDCOG_JIT_VERSION_MINOR   0
#define HURDCOG_JIT_VERSION_PATCH   0

/* Default configuration values */
#define JIT_DEFAULT_OPT_LEVEL       2
#define JIT_DEFAULT_TARGET_ARCH     "native"
#define JIT_DEFAULT_LOG_LEVEL       1
#define JIT_MAX_SOURCE_SIZE         (1024 * 1024)  /* 1MB */
#define JIT_MAX_FUNCTIONS           256
#define JIT_COMPILE_TIMEOUT_MS      5000

/* Environment variable names */
#define JIT_ENV_ENABLE              "HURDCOG_JIT_ENABLE"
#define JIT_ENV_OPTIMIZE            "HURDCOG_JIT_OPTIMIZE"  
#define JIT_ENV_LOG_LEVEL           "HURDCOG_JIT_LOG_LEVEL"
#define JIT_ENV_TARGET_ARCH         "HURDCOG_JIT_TARGET_ARCH"
#define JIT_ENV_LOG_FILE            "HURDCOG_JIT_LOG_FILE"
#define JIT_ENV_COGNITIVE_HINTS     "HURDCOG_JIT_COGNITIVE_HINTS"

/* Cognitive integration settings */
#define JIT_COGNITIVE_HINT_MAX      64
#define JIT_COGNITIVE_PATTERN_MAX   32
#define JIT_PERFORMANCE_SAMPLE_SIZE 100

/* SKZ Framework compatibility */
#define SKZ_AGENT_JIT_TYPE          "jit_compiler"
#define SKZ_JIT_NAMESPACE           "hurdcog.compiler_explorer.jit"
#define SKZ_JIT_LOG_PREFIX          "[JIT] "

/* OJS Installation compatibility paths */
#define OJS_CONFIG_PATH             "/etc/hurdcog/ojs.conf"
#define OJS_RUNTIME_PATH            "/usr/local/lib/ojs"

/* Feature flags (can be disabled at build time) */
#ifndef JIT_ENABLE_LLVM
#define JIT_ENABLE_LLVM             1
#endif

#ifndef JIT_ENABLE_COGNITIVE
#define JIT_ENABLE_COGNITIVE        1  
#endif

#ifndef JIT_ENABLE_PERFORMANCE_MONITORING
#define JIT_ENABLE_PERFORMANCE_MONITORING  1
#endif

#ifndef JIT_ENABLE_LOGGING
#define JIT_ENABLE_LOGGING          1
#endif

/* Internal implementation limits */
#define JIT_INTERNAL_BUFFER_SIZE    8192
#define JIT_MAX_ERROR_MESSAGE       512
#define JIT_MAX_SYMBOL_NAME         256
#define JIT_HASH_TABLE_SIZE         1024

#endif /* HURDCOG_JIT_CONFIG_H */