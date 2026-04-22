/*
 * Mach 5 Research: IPC Performance Benchmarking Framework
 * 
 * This header defines the interface for benchmarking various IPC mechanisms
 * in GNU Mach to identify performance bottlenecks and opportunities for
 * optimization in next-generation implementations.
 */

#ifndef _MACH5_IPC_BENCHMARK_H_
#define _MACH5_IPC_BENCHMARK_H_

#include <mach/mach.h>
#include <mach/message.h>
#include <mach/port.h>
#include <sys/time.h>

/*
 * Benchmark Configuration
 */
#define IPC_BENCHMARK_MAX_ITERATIONS    10000
#define IPC_BENCHMARK_MAX_MESSAGE_SIZE  65536
#define IPC_BENCHMARK_MIN_MESSAGE_SIZE  64

/*
 * Benchmark Test Types
 */
typedef enum {
    IPC_BENCH_LATENCY,          /* Round-trip latency measurement */
    IPC_BENCH_THROUGHPUT,       /* Messages per second */
    IPC_BENCH_BANDWIDTH,        /* Bytes per second */
    IPC_BENCH_SCALABILITY,      /* Performance vs. number of tasks */
    IPC_BENCH_MEMORY_USAGE      /* Memory overhead analysis */
} ipc_benchmark_type_t;

/*
 * Benchmark Message Sizes
 */
typedef enum {
    IPC_MSG_TINY    = 64,       /* Small control messages */
    IPC_MSG_SMALL   = 256,      /* Typical IPC messages */
    IPC_MSG_MEDIUM  = 4096,     /* Page-sized messages */
    IPC_MSG_LARGE   = 16384,    /* Multi-page messages */
    IPC_MSG_HUGE    = 65536     /* Large data transfers */
} ipc_message_size_t;

/*
 * Benchmark Results Structure
 */
typedef struct {
    ipc_benchmark_type_t type;
    ipc_message_size_t message_size;
    unsigned int iterations;
    
    /* Timing Results */
    struct timeval start_time;
    struct timeval end_time;
    unsigned long total_microseconds;
    
    /* Performance Metrics */
    double average_latency_us;      /* Average latency in microseconds */
    double messages_per_second;     /* Throughput in messages/sec */
    double bytes_per_second;        /* Bandwidth in bytes/sec */
    double cpu_utilization;         /* CPU usage percentage */
    
    /* Memory Metrics */
    unsigned long memory_allocated; /* Bytes allocated for test */
    unsigned long memory_peak;      /* Peak memory usage */
    unsigned int page_faults;       /* Number of page faults */
    
    /* Error Statistics */
    unsigned int failed_sends;      /* Failed send operations */
    unsigned int failed_receives;   /* Failed receive operations */
    unsigned int timeouts;          /* Message timeouts */
} ipc_benchmark_result_t;

/*
 * Benchmark Context
 */
typedef struct {
    mach_port_t server_port;        /* Server receive port */
    mach_port_t client_port;        /* Client receive port */
    mach_port_t reply_port;         /* Reply port for responses */
    
    task_t server_task;             /* Server task */
    task_t client_task;             /* Client task */
    
    void *message_buffer;           /* Pre-allocated message buffer */
    mach_msg_size_t buffer_size;    /* Buffer size */
    
    unsigned int num_iterations;    /* Number of test iterations */
    ipc_benchmark_type_t test_type; /* Type of benchmark to run */
} ipc_benchmark_context_t;

/*
 * Function Prototypes
 */

/* Benchmark Initialization */
kern_return_t ipc_benchmark_init(ipc_benchmark_context_t *context,
                                ipc_benchmark_type_t type,
                                unsigned int iterations);

kern_return_t ipc_benchmark_cleanup(ipc_benchmark_context_t *context);

/* Core Benchmark Functions */
kern_return_t ipc_benchmark_latency(ipc_benchmark_context_t *context,
                                   ipc_message_size_t msg_size,
                                   ipc_benchmark_result_t *result);

kern_return_t ipc_benchmark_throughput(ipc_benchmark_context_t *context,
                                      ipc_message_size_t msg_size,
                                      ipc_benchmark_result_t *result);

kern_return_t ipc_benchmark_bandwidth(ipc_benchmark_context_t *context,
                                     ipc_message_size_t msg_size,
                                     ipc_benchmark_result_t *result);

kern_return_t ipc_benchmark_scalability(ipc_benchmark_context_t *context,
                                       unsigned int num_tasks,
                                       ipc_benchmark_result_t *result);

/* Utility Functions */
void ipc_benchmark_print_results(const ipc_benchmark_result_t *result);
void ipc_benchmark_save_results(const ipc_benchmark_result_t *result,
                                const char *filename);

double ipc_benchmark_calculate_latency(const struct timeval *start,
                                      const struct timeval *end,
                                      unsigned int iterations);

/* Memory Analysis */
kern_return_t ipc_benchmark_memory_analysis(ipc_benchmark_context_t *context,
                                           ipc_benchmark_result_t *result);

/* Modern IPC Mechanism Prototypes */
kern_return_t ipc_benchmark_zero_copy(ipc_benchmark_context_t *context,
                                     ipc_message_size_t msg_size,
                                     ipc_benchmark_result_t *result);

kern_return_t ipc_benchmark_shared_memory(ipc_benchmark_context_t *context,
                                         ipc_message_size_t msg_size,
                                         ipc_benchmark_result_t *result);

kern_return_t ipc_benchmark_async_ipc(ipc_benchmark_context_t *context,
                                     ipc_message_size_t msg_size,
                                     ipc_benchmark_result_t *result);

/*
 * Benchmark Comparison Framework
 */
typedef struct {
    ipc_benchmark_result_t traditional_ipc;
    ipc_benchmark_result_t zero_copy_ipc;
    ipc_benchmark_result_t shared_memory_ipc;
    ipc_benchmark_result_t async_ipc;
    
    /* Improvement Metrics */
    double latency_improvement;     /* Percentage improvement */
    double throughput_improvement;  /* Percentage improvement */
    double memory_efficiency;      /* Memory usage reduction */
} ipc_benchmark_comparison_t;

kern_return_t ipc_benchmark_compare_mechanisms(ipc_benchmark_context_t *context,
                                              ipc_message_size_t msg_size,
                                              ipc_benchmark_comparison_t *comparison);

void ipc_benchmark_print_comparison(const ipc_benchmark_comparison_t *comparison);

#endif /* _MACH5_IPC_BENCHMARK_H_ */