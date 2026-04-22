/* simple-cognitive-server.c
 * 
 * A simple demonstration server that uses cognitive services
 * to optimize its operation.
 * 
 * This example shows how a Hurd server can integrate with the
 * OpenCog cognitive architecture to:
 * - Learn from its operation patterns
 * - Adapt to changing loads
 * - Make intelligent decisions about resource allocation
 * 
 * Build:
 *   gcc -o simple-cognitive-server simple-cognitive-server.c \
 *       -lhurd-atomspace-bridge -lpthread
 * 
 * Run:
 *   ./simple-cognitive-server
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>

/* Include cognitive interface */
#include "../hurd-atomspace-bridge.h"

/* Server configuration */
#define SERVER_NAME "simple-cognitive-server"
#define MAX_REQUESTS 1000
#define OPTIMIZATION_INTERVAL 10  /* seconds */

/* Server statistics */
typedef struct {
    unsigned long requests_handled;
    unsigned long requests_failed;
    double avg_response_time;
    double current_load;
    pthread_mutex_t lock;
} server_stats_t;

static server_stats_t stats = {
    .requests_handled = 0,
    .requests_failed = 0,
    .avg_response_time = 0.0,
    .current_load = 0.0,
    .lock = PTHREAD_MUTEX_INITIALIZER
};

/* Server configuration that can be optimized */
typedef struct {
    size_t buffer_size;
    int thread_pool_size;
    int cache_size;
    double timeout;
} server_config_t;

static server_config_t config = {
    .buffer_size = 4096,
    .thread_pool_size = 4,
    .cache_size = 100,
    .timeout = 5.0
};

/* Forward declarations */
static int init_cognitive_server(void);
static void handle_request(void);
static void update_cognitive_state(void);
static void apply_cognitive_optimizations(void);
static void *optimization_thread(void *arg);
static void report_event(const char *event_type, double value);

/*
 * Initialize the cognitive server
 */
static int
init_cognitive_server(void)
{
    error_t err;
    
    printf("=== Simple Cognitive Server ===\n");
    printf("Initializing cognitive interface...\n");
    
    /* Initialize cognitive OS layer */
    err = hurd_cognitive_init();
    if (err) {
        fprintf(stderr, "Failed to initialize cognitive layer: %s\n",
                strerror(err));
        fprintf(stderr, "NOTE: Running without cognitive features\n");
        return -1;
    }
    
    /* Register this server with cognitive system */
    err = hurd_cognitive_register(SERVER_NAME);
    if (err) {
        fprintf(stderr, "Failed to register with cognitive system: %s\n",
                strerror(err));
        return -1;
    }
    
    printf("✓ Cognitive interface initialized\n");
    printf("✓ Server registered as '%s'\n", SERVER_NAME);
    
    /* Report initial configuration */
    state_update_t state = {
        .component = SERVER_NAME,
        .state_key = "buffer_size",
        .value = (double)config.buffer_size,
        .confidence = 1.0
    };
    hurd_cognitive_update_state(&state);
    
    state.state_key = "thread_pool_size";
    state.value = (double)config.thread_pool_size;
    hurd_cognitive_update_state(&state);
    
    printf("✓ Initial state reported to cognitive system\n");
    
    return 0;
}

/*
 * Simulate handling a request
 */
static void
handle_request(void)
{
    struct timeval start, end;
    double response_time;
    int success;
    
    gettimeofday(&start, NULL);
    
    /* Simulate work (random between 10-100 ms) */
    usleep((rand() % 90000) + 10000);
    
    /* Simulate occasional failures (5% chance) */
    success = (rand() % 100) >= 5;
    
    gettimeofday(&end, NULL);
    
    /* Calculate response time in milliseconds */
    response_time = ((end.tv_sec - start.tv_sec) * 1000.0 +
                     (end.tv_usec - start.tv_usec) / 1000.0);
    
    /* Update statistics */
    pthread_mutex_lock(&stats.lock);
    
    if (success) {
        stats.requests_handled++;
        
        /* Update average response time */
        if (stats.avg_response_time == 0.0) {
            stats.avg_response_time = response_time;
        } else {
            stats.avg_response_time = 
                (stats.avg_response_time * 0.9) + (response_time * 0.1);
        }
        
        /* Report to cognitive system */
        report_event("request_handled", response_time);
        
    } else {
        stats.requests_failed++;
        report_event("request_failed", 1.0);
    }
    
    /* Calculate current load */
    stats.current_load = (double)stats.requests_handled / 
                         (stats.requests_handled + stats.requests_failed);
    
    pthread_mutex_unlock(&stats.lock);
}

/*
 * Report an event to the cognitive system
 */
static void
report_event(const char *event_type, double value)
{
    event_t event = {
        .type = EVENT_PERFORMANCE_METRIC,
        .component = SERVER_NAME,
        .data = &value,
        .data_size = sizeof(value)
    };
    
    gettimeofday(&event.timestamp, NULL);
    
    /* Report to cognitive system (ignore errors for simplicity) */
    hurd_cognitive_add_event(&event);
}

/*
 * Update cognitive system with current state
 */
static void
update_cognitive_state(void)
{
    state_update_t state;
    
    pthread_mutex_lock(&stats.lock);
    
    /* Update request count */
    state.component = SERVER_NAME;
    state.state_key = "requests_handled";
    state.value = (double)stats.requests_handled;
    state.confidence = 1.0;
    hurd_cognitive_update_state(&state);
    
    /* Update average response time */
    state.state_key = "avg_response_time";
    state.value = stats.avg_response_time;
    state.confidence = 1.0;
    hurd_cognitive_update_state(&state);
    
    /* Update load */
    state.state_key = "current_load";
    state.value = stats.current_load;
    state.confidence = 1.0;
    hurd_cognitive_update_state(&state);
    
    pthread_mutex_unlock(&stats.lock);
}

/*
 * Apply cognitive optimizations
 */
static void
apply_cognitive_optimizations(void)
{
    optimization_t opt;
    error_t err;
    
    /* Query cognitive system for optimization suggestions */
    err = hurd_cognitive_get_optimization(COMP_SERVER, &opt);
    
    if (err) {
        /* No optimization available or error */
        return;
    }
    
    if (opt.confidence < 0.7) {
        /* Not confident enough in suggestion */
        printf("⚠ Cognitive suggestion confidence too low (%.2f)\n",
               opt.confidence);
        return;
    }
    
    printf("\n📊 Cognitive Optimization Suggested:\n");
    printf("   Type: %s\n", opt.optimization_type);
    printf("   Description: %s\n", opt.description);
    printf("   Expected improvement: %.1f%%\n", 
           opt.expected_improvement * 100);
    printf("   Confidence: %.2f\n", opt.confidence);
    
    /* Apply optimization based on type */
    if (strcmp(opt.optimization_type, "buffer_size") == 0) {
        size_t new_size = *(size_t *)opt.parameters;
        printf("   → Adjusting buffer size: %zu → %zu bytes\n",
               config.buffer_size, new_size);
        config.buffer_size = new_size;
    }
    else if (strcmp(opt.optimization_type, "thread_pool") == 0) {
        int new_pool_size = *(int *)opt.parameters;
        printf("   → Adjusting thread pool: %d → %d threads\n",
               config.thread_pool_size, new_pool_size);
        config.thread_pool_size = new_pool_size;
    }
    else if (strcmp(opt.optimization_type, "cache_size") == 0) {
        int new_cache_size = *(int *)opt.parameters;
        printf("   → Adjusting cache size: %d → %d entries\n",
               config.cache_size, new_cache_size);
        config.cache_size = new_cache_size;
    }
    
    printf("   ✓ Optimization applied\n\n");
}

/*
 * Background thread for periodic optimization checks
 */
static void *
optimization_thread(void *arg)
{
    (void)arg;  /* Unused */
    
    while (1) {
        sleep(OPTIMIZATION_INTERVAL);
        
        /* Update state in cognitive system */
        update_cognitive_state();
        
        /* Check for and apply optimizations */
        apply_cognitive_optimizations();
    }
    
    return NULL;
}

/*
 * Print server statistics
 */
static void
print_statistics(void)
{
    pthread_mutex_lock(&stats.lock);
    
    printf("\n📊 Server Statistics:\n");
    printf("   Requests handled: %lu\n", stats.requests_handled);
    printf("   Requests failed:  %lu\n", stats.requests_failed);
    printf("   Average response: %.2f ms\n", stats.avg_response_time);
    printf("   Current load:     %.2f%%\n", stats.current_load * 100);
    printf("   Buffer size:      %zu bytes\n", config.buffer_size);
    printf("   Thread pool:      %d threads\n", config.thread_pool_size);
    printf("   Cache size:       %d entries\n", config.cache_size);
    
    pthread_mutex_unlock(&stats.lock);
}

/*
 * Main server loop
 */
int
main(int argc, char **argv)
{
    pthread_t opt_thread;
    int cognitive_enabled = 1;
    
    /* Initialize random seed */
    srand(time(NULL));
    
    /* Initialize cognitive server */
    if (init_cognitive_server() < 0) {
        cognitive_enabled = 0;
    }
    
    if (cognitive_enabled) {
        /* Start optimization thread */
        if (pthread_create(&opt_thread, NULL, optimization_thread, NULL) != 0) {
            fprintf(stderr, "Warning: Failed to start optimization thread\n");
        }
    }
    
    printf("\n🚀 Server started. Processing requests...\n");
    printf("   (Cognitive features: %s)\n\n", 
           cognitive_enabled ? "ENABLED" : "DISABLED");
    
    /* Main request handling loop */
    for (int i = 0; i < MAX_REQUESTS; i++) {
        handle_request();
        
        /* Print statistics every 100 requests */
        if ((i + 1) % 100 == 0) {
            print_statistics();
        }
        
        /* Small delay between requests */
        usleep(1000);
    }
    
    /* Final statistics */
    printf("\n=== Final Statistics ===\n");
    print_statistics();
    
    if (cognitive_enabled) {
        printf("\n✓ Cognitive learning data has been recorded\n");
        printf("✓ System will use this data for future optimizations\n");
    }
    
    printf("\n🏁 Server shutdown complete\n");
    
    return 0;
}
