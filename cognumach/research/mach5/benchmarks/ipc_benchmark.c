/*
 * Mach 5 Research: IPC Performance Benchmarking Framework Implementation
 * 
 * This implementation provides benchmarking capabilities to measure and analyze
 * IPC performance in GNU Mach, identifying bottlenecks and opportunities for
 * next-generation IPC mechanism optimization.
 */

#include "ipc_benchmark.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <mach/mach_error.h>

/*
 * Initialize benchmark context
 */
kern_return_t 
ipc_benchmark_init(ipc_benchmark_context_t *context,
                   ipc_benchmark_type_t type,
                   unsigned int iterations)
{
    kern_return_t kr;
    
    if (!context) {
        return KERN_INVALID_ARGUMENT;
    }
    
    memset(context, 0, sizeof(*context));
    
    /* Allocate ports for benchmarking */
    kr = mach_port_allocate(mach_task_self(),
                           MACH_PORT_RIGHT_RECEIVE,
                           &context->server_port);
    if (kr != KERN_SUCCESS) {
        printf("Failed to allocate server port: %s\n", mach_error_string(kr));
        return kr;
    }
    
    kr = mach_port_allocate(mach_task_self(),
                           MACH_PORT_RIGHT_RECEIVE,
                           &context->client_port);
    if (kr != KERN_SUCCESS) {
        printf("Failed to allocate client port: %s\n", mach_error_string(kr));
        mach_port_deallocate(mach_task_self(), context->server_port);
        return kr;
    }
    
    kr = mach_port_allocate(mach_task_self(),
                           MACH_PORT_RIGHT_RECEIVE,
                           &context->reply_port);
    if (kr != KERN_SUCCESS) {
        printf("Failed to allocate reply port: %s\n", mach_error_string(kr));
        mach_port_deallocate(mach_task_self(), context->server_port);
        mach_port_deallocate(mach_task_self(), context->client_port);
        return kr;
    }
    
    /* Allocate message buffer */
    context->buffer_size = IPC_BENCHMARK_MAX_MESSAGE_SIZE;
    context->message_buffer = malloc(context->buffer_size);
    if (!context->message_buffer) {
        printf("Failed to allocate message buffer\n");
        ipc_benchmark_cleanup(context);
        return KERN_RESOURCE_SHORTAGE;
    }
    
    context->num_iterations = iterations;
    context->test_type = type;
    context->server_task = mach_task_self();
    context->client_task = mach_task_self();
    
    return KERN_SUCCESS;
}

/*
 * Cleanup benchmark context
 */
kern_return_t 
ipc_benchmark_cleanup(ipc_benchmark_context_t *context)
{
    if (!context) {
        return KERN_INVALID_ARGUMENT;
    }
    
    if (context->server_port != MACH_PORT_NULL) {
        mach_port_deallocate(mach_task_self(), context->server_port);
    }
    
    if (context->client_port != MACH_PORT_NULL) {
        mach_port_deallocate(mach_task_self(), context->client_port);
    }
    
    if (context->reply_port != MACH_PORT_NULL) {
        mach_port_deallocate(mach_task_self(), context->reply_port);
    }
    
    if (context->message_buffer) {
        free(context->message_buffer);
        context->message_buffer = NULL;
    }
    
    memset(context, 0, sizeof(*context));
    return KERN_SUCCESS;
}

/*
 * Calculate latency from timing measurements
 */
double 
ipc_benchmark_calculate_latency(const struct timeval *start,
                               const struct timeval *end,
                               unsigned int iterations)
{
    if (!start || !end || iterations == 0) {
        return 0.0;
    }
    
    unsigned long start_us = start->tv_sec * 1000000 + start->tv_usec;
    unsigned long end_us = end->tv_sec * 1000000 + end->tv_usec;
    unsigned long total_us = end_us - start_us;
    
    return (double)total_us / (double)iterations;
}

/*
 * Benchmark IPC latency (round-trip time)
 */
kern_return_t 
ipc_benchmark_latency(ipc_benchmark_context_t *context,
                     ipc_message_size_t msg_size,
                     ipc_benchmark_result_t *result)
{
    if (!context || !result) {
        return KERN_INVALID_ARGUMENT;
    }
    
    memset(result, 0, sizeof(*result));
    result->type = IPC_BENCH_LATENCY;
    result->message_size = msg_size;
    result->iterations = context->num_iterations;
    
    /* Create test message */
    mach_msg_header_t *msg = (mach_msg_header_t *)context->message_buffer;
    msg->msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_MAKE_SEND,
                                   MACH_MSG_TYPE_MAKE_SEND_ONCE);
    msg->msgh_size = sizeof(mach_msg_header_t) + msg_size;
    msg->msgh_remote_port = context->server_port;
    msg->msgh_local_port = context->reply_port;
    msg->msgh_id = 1000;
    
    /* Start timing */
    gettimeofday(&result->start_time, NULL);
    
    /* Perform latency test */
    unsigned int i;
    for (i = 0; i < context->num_iterations; i++) {
        kern_return_t kr;
        
        /* Send message (non-blocking) */
        kr = mach_msg(msg,
                     MACH_SEND_MSG | MACH_SEND_TIMEOUT,
                     msg->msgh_size,
                     0,
                     MACH_PORT_NULL,
                     1000,  /* 1 second timeout */
                     MACH_PORT_NULL);
        
        if (kr != KERN_SUCCESS) {
            result->failed_sends++;
            if (kr == MACH_SEND_TIMED_OUT) {
                result->timeouts++;
            }
            continue;
        }
        
        /* Simulate server processing and reply */
        usleep(1); /* Minimal processing delay */
        
        /* Receive reply */
        kr = mach_msg(msg,
                     MACH_RCV_MSG | MACH_RCV_TIMEOUT,
                     0,
                     msg->msgh_size,
                     context->reply_port,
                     1000,  /* 1 second timeout */
                     MACH_PORT_NULL);
        
        if (kr != KERN_SUCCESS) {
            result->failed_receives++;
            if (kr == MACH_RCV_TIMED_OUT) {
                result->timeouts++;
            }
        }
    }
    
    /* End timing */
    gettimeofday(&result->end_time, NULL);
    
    /* Calculate metrics */
    result->average_latency_us = ipc_benchmark_calculate_latency(
        &result->start_time, &result->end_time, context->num_iterations);
    
    result->messages_per_second = 1000000.0 / result->average_latency_us;
    result->bytes_per_second = result->messages_per_second * msg_size;
    
    result->total_microseconds = 
        (result->end_time.tv_sec - result->start_time.tv_sec) * 1000000 +
        (result->end_time.tv_usec - result->start_time.tv_usec);
    
    return KERN_SUCCESS;
}

/*
 * Benchmark IPC throughput (messages per second)
 */
kern_return_t 
ipc_benchmark_throughput(ipc_benchmark_context_t *context,
                        ipc_message_size_t msg_size,
                        ipc_benchmark_result_t *result)
{
    if (!context || !result) {
        return KERN_INVALID_ARGUMENT;
    }
    
    memset(result, 0, sizeof(*result));
    result->type = IPC_BENCH_THROUGHPUT;
    result->message_size = msg_size;
    result->iterations = context->num_iterations;
    
    /* Create test message */
    mach_msg_header_t *msg = (mach_msg_header_t *)context->message_buffer;
    msg->msgh_bits = MACH_MSGH_BITS_REMOTE(MACH_MSG_TYPE_MAKE_SEND);
    msg->msgh_size = sizeof(mach_msg_header_t) + msg_size;
    msg->msgh_remote_port = context->server_port;
    msg->msgh_local_port = MACH_PORT_NULL;
    msg->msgh_id = 2000;
    
    /* Start timing */
    gettimeofday(&result->start_time, NULL);
    
    /* Perform throughput test */
    unsigned int successful_sends = 0;
    unsigned int i;
    for (i = 0; i < context->num_iterations; i++) {
        kern_return_t kr = mach_msg(msg,
                                   MACH_SEND_MSG | MACH_SEND_TIMEOUT,
                                   msg->msgh_size,
                                   0,
                                   MACH_PORT_NULL,
                                   100,  /* Short timeout for throughput test */
                                   MACH_PORT_NULL);
        
        if (kr == KERN_SUCCESS) {
            successful_sends++;
        } else {
            result->failed_sends++;
            if (kr == MACH_SEND_TIMED_OUT) {
                result->timeouts++;
            }
        }
    }
    
    /* End timing */
    gettimeofday(&result->end_time, NULL);
    
    /* Calculate metrics */
    result->total_microseconds = 
        (result->end_time.tv_sec - result->start_time.tv_sec) * 1000000 +
        (result->end_time.tv_usec - result->start_time.tv_usec);
    
    if (result->total_microseconds > 0) {
        result->messages_per_second = 
            (double)successful_sends * 1000000.0 / (double)result->total_microseconds;
        result->bytes_per_second = result->messages_per_second * msg_size;
    }
    
    result->average_latency_us = 
        (double)result->total_microseconds / (double)successful_sends;
    
    return KERN_SUCCESS;
}

/*
 * Benchmark memory usage patterns
 */
kern_return_t 
ipc_benchmark_memory_analysis(ipc_benchmark_context_t *context,
                             ipc_benchmark_result_t *result)
{
    if (!context || !result) {
        return KERN_INVALID_ARGUMENT;
    }
    
    memset(result, 0, sizeof(*result));
    result->type = IPC_BENCH_MEMORY_USAGE;
    
    /* Get initial memory statistics */
    task_basic_info_data_t initial_info;
    mach_msg_type_number_t count = TASK_BASIC_INFO_COUNT;
    
    kern_return_t kr = task_info(mach_task_self(),
                                TASK_BASIC_INFO,
                                (task_info_t)&initial_info,
                                &count);
    
    if (kr != KERN_SUCCESS) {
        printf("Failed to get initial task info: %s\n", mach_error_string(kr));
        return kr;
    }
    
    result->memory_allocated = initial_info.resident_size;
    
    /* Perform memory-intensive IPC operations */
    unsigned int i;
    for (i = 0; i < 1000; i++) {
        void *temp_buffer = malloc(IPC_BENCHMARK_MAX_MESSAGE_SIZE);
        if (temp_buffer) {
            memset(temp_buffer, 0xAA, IPC_BENCHMARK_MAX_MESSAGE_SIZE);
            free(temp_buffer);
        }
    }
    
    /* Get final memory statistics */
    task_basic_info_data_t final_info;
    count = TASK_BASIC_INFO_COUNT;
    
    kr = task_info(mach_task_self(),
                  TASK_BASIC_INFO,
                  (task_info_t)&final_info,
                  &count);
    
    if (kr != KERN_SUCCESS) {
        printf("Failed to get final task info: %s\n", mach_error_string(kr));
        return kr;
    }
    
    result->memory_peak = final_info.resident_size;
    result->page_faults = final_info.pageins;
    
    return KERN_SUCCESS;
}

/*
 * Print benchmark results
 */
void 
ipc_benchmark_print_results(const ipc_benchmark_result_t *result)
{
    if (!result) {
        return;
    }
    
    printf("\n=== IPC Benchmark Results ===\n");
    
    const char *type_name;
    switch (result->type) {
        case IPC_BENCH_LATENCY:     type_name = "Latency"; break;
        case IPC_BENCH_THROUGHPUT:  type_name = "Throughput"; break;
        case IPC_BENCH_BANDWIDTH:   type_name = "Bandwidth"; break;
        case IPC_BENCH_SCALABILITY: type_name = "Scalability"; break;
        case IPC_BENCH_MEMORY_USAGE: type_name = "Memory Usage"; break;
        default:                    type_name = "Unknown"; break;
    }
    
    printf("Test Type: %s\n", type_name);
    printf("Message Size: %u bytes\n", result->message_size);
    printf("Iterations: %u\n", result->iterations);
    printf("Total Time: %lu microseconds\n", result->total_microseconds);
    
    if (result->average_latency_us > 0) {
        printf("Average Latency: %.2f microseconds\n", result->average_latency_us);
    }
    
    if (result->messages_per_second > 0) {
        printf("Messages/sec: %.2f\n", result->messages_per_second);
    }
    
    if (result->bytes_per_second > 0) {
        printf("Bandwidth: %.2f bytes/sec (%.2f MB/sec)\n", 
               result->bytes_per_second,
               result->bytes_per_second / (1024.0 * 1024.0));
    }
    
    if (result->memory_allocated > 0) {
        printf("Memory Allocated: %lu bytes\n", result->memory_allocated);
    }
    
    if (result->memory_peak > 0) {
        printf("Peak Memory: %lu bytes\n", result->memory_peak);
    }
    
    if (result->failed_sends > 0) {
        printf("Failed Sends: %u\n", result->failed_sends);
    }
    
    if (result->failed_receives > 0) {
        printf("Failed Receives: %u\n", result->failed_receives);
    }
    
    if (result->timeouts > 0) {
        printf("Timeouts: %u\n", result->timeouts);
    }
    
    printf("==============================\n\n");
}

/*
 * Save benchmark results to file
 */
void 
ipc_benchmark_save_results(const ipc_benchmark_result_t *result,
                          const char *filename)
{
    if (!result || !filename) {
        return;
    }
    
    FILE *fp = fopen(filename, "a");
    if (!fp) {
        printf("Failed to open results file: %s\n", strerror(errno));
        return;
    }
    
    fprintf(fp, "%d,%u,%u,%lu,%.2f,%.2f,%.2f,%lu,%lu,%u,%u,%u,%u\n",
            result->type,
            result->message_size,
            result->iterations,
            result->total_microseconds,
            result->average_latency_us,
            result->messages_per_second,
            result->bytes_per_second,
            result->memory_allocated,
            result->memory_peak,
            result->page_faults,
            result->failed_sends,
            result->failed_receives,
            result->timeouts);
    
    fclose(fp);
}