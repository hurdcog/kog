# Cognitive Services API Reference

## Overview

This document provides the API reference for the HurdCog Cognitive Services layer. These services enable Hurd components to integrate with the OpenCog cognitive architecture.

## Table of Contents

1. [Core API](#core-api)
2. [Cognitive Process Management](#cognitive-process-management)
3. [Cognitive Memory Management](#cognitive-memory-management)
4. [Cognitive IPC Routing](#cognitive-ipc-routing)
5. [Pattern Learning](#pattern-learning)
6. [Decision Support](#decision-support)
7. [Examples](#examples)

## Core API

### Initialization

#### `hurd_cognitive_init()`

Initialize the cognitive OS layer. Must be called before any other cognitive API.

```c
#include <cogkernel/hurd-atomspace-bridge.h>

error_t hurd_cognitive_init(void);
```

**Returns**: `0` on success, error code on failure

**Example**:
```c
error_t err = hurd_cognitive_init();
if (err) {
    fprintf(stderr, "Failed to initialize cognitive layer: %s\n", 
            strerror(err));
    return err;
}
```

#### `hurd_cognitive_register()`

Register a component with the cognitive system.

```c
error_t hurd_cognitive_register(const char *component_name);
```

**Parameters**:
- `component_name`: Unique name for this component

**Returns**: `0` on success, error code on failure

**Example**:
```c
err = hurd_cognitive_register("my-translator");
```

### Event Reporting

#### `hurd_cognitive_add_event()`

Report an event to the cognitive system for learning and monitoring.

```c
typedef enum {
    EVENT_REQUEST_HANDLED,
    EVENT_ERROR_OCCURRED,
    EVENT_RESOURCE_ALLOCATED,
    EVENT_RESOURCE_RELEASED,
    EVENT_STATE_CHANGED,
    EVENT_PERFORMANCE_METRIC
} event_type_t;

typedef struct {
    event_type_t type;
    const char *component;
    void *data;
    size_t data_size;
    struct timeval timestamp;
} event_t;

error_t hurd_cognitive_add_event(event_t *event);
```

**Example**:
```c
event_t event = {
    .type = EVENT_REQUEST_HANDLED,
    .component = "auth-server",
    .data = &request_info,
    .data_size = sizeof(request_info)
};
gettimeofday(&event.timestamp, NULL);

hurd_cognitive_add_event(&event);
```

### State Updates

#### `hurd_cognitive_update_state()`

Update component state in the cognitive system.

```c
typedef struct {
    const char *component;
    const char *state_key;
    double value;
    double confidence;  /* 0.0 to 1.0 */
} state_update_t;

error_t hurd_cognitive_update_state(state_update_t *update);
```

**Example**:
```c
state_update_t update = {
    .component = "proc-server",
    .state_key = "active_processes",
    .value = get_process_count(),
    .confidence = 1.0
};

hurd_cognitive_update_state(&update);
```

## Cognitive Process Management

### Process Scheduling Support

#### `cognitive_schedule_query()`

Query the cognitive system for scheduling recommendations.

```c
typedef struct {
    process_t process;
    double priority_score;    /* Recommended priority (0.0-1.0) */
    double urgency_score;     /* Urgency assessment (0.0-1.0) */
    double resource_score;    /* Resource need prediction (0.0-1.0) */
    double confidence;        /* Confidence in recommendation */
} cognitive_schedule_decision_t;

error_t cognitive_schedule_query(process_t process, 
                                   cognitive_schedule_decision_t *decision);
```

**Example**:
```c
cognitive_schedule_decision_t decision;
error_t err = cognitive_schedule_query(proc, &decision);

if (!err && decision.confidence > 0.7) {
    /* Use cognitive recommendation */
    set_process_priority(proc, decision.priority_score);
}
```

#### `cognitive_process_predict_behavior()`

Predict process behavior based on learned patterns.

```c
typedef struct {
    double predicted_cpu_usage;      /* 0.0-1.0 */
    double predicted_memory_usage;   /* MB */
    double predicted_io_rate;        /* operations/sec */
    double prediction_confidence;    /* 0.0-1.0 */
} process_behavior_prediction_t;

error_t cognitive_process_predict_behavior(
    process_t process,
    process_behavior_prediction_t *prediction);
```

## Cognitive Memory Management

### Page Management

#### `cognitive_page_predict_access()`

Predict likelihood of page access in near future.

```c
typedef struct {
    void *page_addr;
    double access_probability;    /* 0.0-1.0 */
    useconds_t predicted_time;    /* Microseconds until access */
    double confidence;
} page_access_prediction_t;

error_t cognitive_page_predict_access(
    void *page_addr,
    page_access_prediction_t *prediction);
```

**Example**:
```c
/* Use for smart prefetching */
page_access_prediction_t pred;
err = cognitive_page_predict_access(page, &pred);

if (!err && pred.access_probability > 0.8 && 
    pred.predicted_time < 1000) {
    /* High probability of access soon - prefetch */
    prefetch_page(page);
}
```

#### `cognitive_memory_optimize_allocation()`

Get cognitive recommendation for memory allocation.

```c
typedef struct {
    size_t requested_size;
    const char *requesting_component;
    void *suggested_address;      /* NULL if no preference */
    int suggested_flags;          /* Suggested allocation flags */
    double confidence;
} memory_allocation_recommendation_t;

error_t cognitive_memory_optimize_allocation(
    size_t size,
    memory_allocation_recommendation_t *recommendation);
```

## Cognitive IPC Routing

### Message Routing Optimization

#### `cognitive_ipc_suggest_route()`

Get optimal routing suggestion for IPC message.

```c
typedef struct {
    mach_port_t source_port;
    mach_port_t dest_port;
    mach_port_t suggested_intermediate;  /* For indirect routing */
    size_t suggested_buffer_size;
    int suggested_priority;
    double confidence;
} ipc_route_suggestion_t;

error_t cognitive_ipc_suggest_route(
    mach_port_t source,
    mach_port_t dest,
    ipc_route_suggestion_t *suggestion);
```

**Example**:
```c
ipc_route_suggestion_t suggestion;
err = cognitive_ipc_suggest_route(src_port, dst_port, &suggestion);

if (!err && suggestion.confidence > 0.75) {
    /* Use cognitive routing suggestion */
    if (suggestion.suggested_intermediate != MACH_PORT_NULL) {
        /* Route through intermediate for better performance */
        route_through(suggestion.suggested_intermediate);
    }
    
    /* Adjust buffer size based on prediction */
    allocate_buffer(suggestion.suggested_buffer_size);
}
```

#### `cognitive_ipc_predict_load()`

Predict IPC load for capacity planning.

```c
typedef struct {
    mach_port_t port;
    double predicted_msg_rate;        /* messages/sec */
    double predicted_avg_size;        /* bytes */
    struct timeval prediction_window; /* Time window for prediction */
    double confidence;
} ipc_load_prediction_t;

error_t cognitive_ipc_predict_load(
    mach_port_t port,
    ipc_load_prediction_t *prediction);
```

## Pattern Learning

### Register Patterns

#### `cognitive_pattern_register()`

Register a pattern for the system to learn and recognize.

```c
typedef enum {
    PATTERN_TEMPORAL,      /* Time-based patterns */
    PATTERN_SPATIAL,       /* Space/location patterns */
    PATTERN_CAUSAL,        /* Cause-effect patterns */
    PATTERN_BEHAVIORAL     /* Behavior patterns */
} pattern_type_t;

typedef struct {
    pattern_type_t type;
    const char *name;
    const char *description;
    void *pattern_data;
    size_t data_size;
} pattern_descriptor_t;

error_t cognitive_pattern_register(pattern_descriptor_t *pattern);
```

**Example**:
```c
/* Register a temporal pattern to learn */
pattern_descriptor_t pattern = {
    .type = PATTERN_TEMPORAL,
    .name = "evening-load-spike",
    .description = "Increased load during evening hours",
    .pattern_data = &temporal_data,
    .data_size = sizeof(temporal_data)
};

cognitive_pattern_register(&pattern);
```

### Query Patterns

#### `cognitive_pattern_query()`

Query if a learned pattern matches current situation.

```c
typedef struct {
    const char *pattern_name;
    double match_score;      /* 0.0-1.0 */
    double confidence;
    void *match_data;        /* Additional match information */
} pattern_match_result_t;

error_t cognitive_pattern_query(
    const char *pattern_name,
    pattern_match_result_t *result);
```

## Decision Support

### Request Decision

#### `cognitive_decision_request()`

Request a decision from the cognitive system.

```c
typedef struct {
    const char *decision_context;
    const char *question;
    void *context_data;
    size_t context_data_size;
} decision_request_t;

typedef struct {
    const char *decision;
    double confidence;
    const char *reasoning;    /* Explanation */
    void *supporting_data;
} decision_result_t;

error_t cognitive_decision_request(
    decision_request_t *request,
    decision_result_t *result);
```

**Example**:
```c
/* Ask cognitive system whether to restart a server */
decision_request_t req = {
    .decision_context = "server-health",
    .question = "should_restart_server",
    .context_data = &server_metrics,
    .context_data_size = sizeof(server_metrics)
};

decision_result_t result;
err = cognitive_decision_request(&req, &result);

if (!err && result.confidence > 0.8) {
    if (strcmp(result.decision, "yes") == 0) {
        /* Cognitive system recommends restart */
        log_info("Restarting server based on cognitive decision: %s",
                 result.reasoning);
        restart_server();
    }
}

/* Free result data */
cognitive_decision_free(&result);
```

### Optimization Suggestions

#### `hurd_cognitive_get_optimization()`

Request optimization suggestions for a component.

```c
typedef enum {
    COMP_PROCESS_MANAGER,
    COMP_MEMORY_MANAGER,
    COMP_IPC_ROUTER,
    COMP_TRANSLATOR,
    COMP_SERVER
} component_type_t;

typedef struct {
    const char *optimization_type;
    const char *description;
    void *parameters;
    size_t param_size;
    double expected_improvement;  /* Estimated benefit */
    double confidence;
} optimization_t;

error_t hurd_cognitive_get_optimization(
    component_type_t component,
    optimization_t *opt);
```

**Example**:
```c
optimization_t opt;
err = hurd_cognitive_get_optimization(COMP_MEMORY_MANAGER, &opt);

if (!err && opt.confidence > 0.75) {
    log_info("Cognitive optimization suggested: %s", 
             opt.description);
    log_info("Expected improvement: %.2f%%", 
             opt.expected_improvement * 100);
    
    /* Apply optimization */
    apply_optimization(&opt);
}
```

## Examples

### Complete Example: Cognitive Translator

```c
/* my-cognitive-translator.c */
#include <hurd.h>
#include <cogkernel/hurd-atomspace-bridge.h>

static error_t
init_cognitive_translator(void)
{
    error_t err;
    
    /* Initialize cognitive interface */
    err = hurd_cognitive_init();
    if (err)
        return err;
    
    /* Register this translator */
    err = hurd_cognitive_register("my-translator");
    if (err)
        return err;
    
    log_info("Cognitive translator initialized");
    return 0;
}

static error_t
handle_read_request(void *buffer, size_t size)
{
    error_t err;
    struct timeval start, end;
    
    gettimeofday(&start, NULL);
    
    /* Perform the read */
    err = do_actual_read(buffer, size);
    
    gettimeofday(&end, NULL);
    
    /* Report performance to cognitive system */
    event_t event = {
        .type = EVENT_PERFORMANCE_METRIC,
        .component = "my-translator",
        .timestamp = end
    };
    
    double latency = (end.tv_sec - start.tv_sec) * 1000000 +
                     (end.tv_usec - start.tv_usec);
    event.data = &latency;
    event.data_size = sizeof(latency);
    
    hurd_cognitive_add_event(&event);
    
    /* Update state */
    state_update_t state = {
        .component = "my-translator",
        .state_key = "bytes_read",
        .value = size,
        .confidence = 1.0
    };
    hurd_cognitive_update_state(&state);
    
    return err;
}

static void
periodic_optimization(void)
{
    optimization_t opt;
    error_t err;
    
    /* Query for optimization suggestions */
    err = hurd_cognitive_get_optimization(COMP_TRANSLATOR, &opt);
    
    if (!err && opt.confidence > 0.7) {
        log_info("Applying cognitive optimization: %s", 
                 opt.description);
        
        /* Apply suggested optimization */
        if (strcmp(opt.optimization_type, "buffer_size") == 0) {
            size_t *new_size = (size_t *)opt.parameters;
            set_buffer_size(*new_size);
        }
        else if (strcmp(opt.optimization_type, "cache_policy") == 0) {
            adjust_cache_policy(opt.parameters);
        }
    }
}

int
main(int argc, char **argv)
{
    error_t err;
    
    /* Initialize */
    err = init_cognitive_translator();
    if (err) {
        fprintf(stderr, "Failed to initialize: %s\n", strerror(err));
        return 1;
    }
    
    /* Main loop */
    while (1) {
        /* Handle requests */
        handle_requests();
        
        /* Periodically check for optimizations */
        static int counter = 0;
        if (++counter % 1000 == 0) {
            periodic_optimization();
        }
    }
    
    return 0;
}
```

### Example: Predictive Memory Management

```c
/* cognitive-memory-manager.c */
#include <cogkernel/hurd-atomspace-bridge.h>

static void
smart_prefetch(vm_address_t base_addr, size_t size)
{
    error_t err;
    size_t pages = size / vm_page_size;
    
    for (size_t i = 0; i < pages; i++) {
        void *page = (void *)(base_addr + i * vm_page_size);
        
        /* Predict access probability */
        page_access_prediction_t pred;
        err = cognitive_page_predict_access(page, &pred);
        
        if (!err && pred.access_probability > 0.7 &&
            pred.predicted_time < 5000) {  /* 5ms */
            /* High probability of access soon - prefetch */
            vm_prefetch(page, vm_page_size);
            
            /* Report prefetch decision */
            event_t event = {
                .type = EVENT_PERFORMANCE_METRIC,
                .component = "memory-manager"
            };
            double prob = pred.access_probability;
            event.data = &prob;
            event.data_size = sizeof(prob);
            hurd_cognitive_add_event(&event);
        }
    }
}

static error_t
cognitive_page_replacement(vm_address_t addr, vm_address_t *victim)
{
    error_t err;
    page_access_prediction_t pred;
    double min_probability = 1.0;
    vm_address_t best_victim = 0;
    
    /* Find page least likely to be accessed soon */
    for_each_candidate_page(addr, candidate) {
        err = cognitive_page_predict_access(candidate, &pred);
        
        if (!err && pred.access_probability < min_probability) {
            min_probability = pred.access_probability;
            best_victim = candidate;
        }
    }
    
    *victim = best_victim;
    return 0;
}
```

## Error Handling

All cognitive API functions return `error_t`. Common error codes:

- `0`: Success
- `ENOMEM`: Memory allocation failed
- `ENOTCONN`: Cognitive system not initialized
- `EINVAL`: Invalid parameter
- `ETIMEDOUT`: Operation timed out
- `ENOENT`: Requested entity not found in AtomSpace

Always check return values and handle errors appropriately.

## Performance Considerations

1. **Caching**: Cache cognitive query results when appropriate
2. **Async Operations**: Use asynchronous queries for non-critical decisions
3. **Batch Updates**: Batch state updates when possible
4. **Confidence Thresholds**: Use appropriate confidence thresholds for your use case
5. **Fallbacks**: Always have fallback behavior when cognitive system unavailable

## Thread Safety

All cognitive API functions are thread-safe. However, for best performance:

- Minimize contention on shared cognitive resources
- Use thread-local caching where appropriate
- Consider using separate cognitive contexts for independent components

## Debugging

Enable cognitive debug logging:

```c
hurd_cognitive_set_debug_level(COGNITIVE_DEBUG_VERBOSE);
```

Query AtomSpace state:

```c
hurd_cognitive_dump_state("component-name", "/tmp/cognitive-state.txt");
```

## See Also

- [OpenCog Hurd Integration](OPENCOG_HURD_INTEGRATION.md)
- [Cognitive Kernel Implementation](../cogkernel/README.md)
- [AtomSpace API](../cogkernel/atomspace/README.md)

---

**Version**: 1.0  
**Last Updated**: October 2025
