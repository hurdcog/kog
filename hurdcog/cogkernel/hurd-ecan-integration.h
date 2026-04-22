/* hurd-ecan-integration.h - ECAN Attention Economics for GNU Hurd Resource Management
 * 
 * Provides C API for integrating ECAN attention allocation with Hurd servers
 * to solve the fundamental resource attribution problem.
 * 
 * Copyright (C) 2025 Free Software Foundation, Inc.
 *
 * This file is part of the GNU Hurd.
 *
 * The GNU Hurd is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 *
 * The GNU Hurd is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the GNU Hurd.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _HURD_ECAN_INTEGRATION_H
#define _HURD_ECAN_INTEGRATION_H

#include <errno.h>
#include <sys/types.h>

/* Conditionally include Mach headers if available */
#ifdef __GNU__
#include <mach/mach_types.h>
#else
/* Define types for non-Hurd systems */
typedef unsigned int mach_port_t;
typedef int error_t;
#endif

/* ECAN Attention Economics Configuration */

#define ECAN_DEFAULT_TOTAL_FUNDS    10000
#define ECAN_DEFAULT_FOCUS_THRESHOLD 100
#define ECAN_DEFAULT_WAGE_RATE      0.1    /* 10% of activity as wage */
#define ECAN_DEFAULT_RENT_RATE      0.05   /* 5% of STI as rent */
#define ECAN_DEFAULT_SPREAD_RATE    0.2    /* 20% spreading */

/* Attention Value Structure */
typedef struct {
    double sti;   /* Short-term importance (current resources) */
    double lti;   /* Long-term importance (historical activity) */
    double vlti;  /* Very long-term importance (structural) */
} attention_value_t;

/* Client Activity Types */
typedef enum {
    ACTIVITY_READ = 10,
    ACTIVITY_WRITE = 20,
    ACTIVITY_CREATE = 15,
    ACTIVITY_DELETE = 10,
    ACTIVITY_RENAME = 5,
    ACTIVITY_STAT = 2,
    ACTIVITY_OPEN = 5,
    ACTIVITY_CLOSE = 2,
    ACTIVITY_NETWORK_SEND = 10,
    ACTIVITY_NETWORK_RECV = 8,
} activity_type_t;

/* Resource Types */
typedef enum {
    RESOURCE_MEMORY,
    RESOURCE_CPU,
    RESOURCE_IO,
    RESOURCE_NETWORK,
    RESOURCE_FILE_DESCRIPTOR,
} resource_type_t;

/* ==================== Core ECAN Functions ==================== */

/* Initialize ECAN attention bank for Hurd resource management */
error_t hurd_ecan_init(void);

/* Shutdown ECAN system */
void hurd_ecan_shutdown(void);

/* ==================== Client Registration ==================== */

/* Register a client (process) with ECAN attention bank */
error_t hurd_ecan_register_client(mach_port_t client_port, 
                                   pid_t pid);

/* Unregister client when process exits */
error_t hurd_ecan_unregister_client(mach_port_t client_port);

/* ==================== Activity Tracking (Wages) ==================== */

/* Record client activity for wage calculation
 * 
 * Should be called after completing any operation on behalf of a client.
 * The activity_value determines the cognitive wage awarded.
 * 
 * Example:
 *   hurd_ecan_record_activity(client_port, ACTIVITY_WRITE, 1);
 */
error_t hurd_ecan_record_activity(mach_port_t client_port,
                                   activity_type_t activity,
                                   size_t quantity);

/* Batch record multiple activities */
error_t hurd_ecan_record_activities(mach_port_t client_port,
                                    activity_type_t *activities,
                                    size_t *quantities,
                                    size_t count);

/* Apply cognitive wages to all active clients */
error_t hurd_ecan_apply_wages(void);

/* ==================== Resource Tracking (Rent) ==================== */

/* Charge rent for resource usage
 * 
 * Should be called when allocating resources on behalf of a client.
 * Rent will be deducted from client's attention (STI).
 * 
 * Example:
 *   hurd_ecan_charge_rent(client_port, RESOURCE_MEMORY, 4096);
 */
error_t hurd_ecan_charge_rent(mach_port_t client_port,
                              resource_type_t resource,
                              size_t amount);

/* Collect attention rent from all clients */
error_t hurd_ecan_collect_rent(void);

/* ==================== Client Credit & Limits ==================== */

/* Check if client has sufficient attention credit for operation
 * 
 * Use this to proactively prevent resource exhaustion.
 * Returns 0 if client has credit, ENOSPC if not.
 * 
 * Example:
 *   if (hurd_ecan_check_client_credit(client_port, 1024*1024) != 0) {
 *       return ENOSPC;  // Reject request
 *   }
 */
error_t hurd_ecan_check_client_credit(mach_port_t client_port,
                                      size_t required_resources);

/* Get dynamic resource limit for client based on attention
 * 
 * Returns the maximum resources this client should be allowed to use
 * based on their attention economics (STI/LTI balance).
 */
size_t hurd_ecan_get_client_limit(mach_port_t client_port,
                                  resource_type_t resource);

/* Get client's current attention value */
error_t hurd_ecan_get_client_attention(mach_port_t client_port,
                                       attention_value_t *av);

/* ==================== Rogue Client Detection ==================== */

/* Detect rogue clients (high resource, low activity)
 * 
 * Returns list of client ports that should be killed.
 * Caller must free the returned array.
 */
error_t hurd_ecan_get_rogue_clients(mach_port_t **rogues,
                                    size_t *count);

/* Check if a specific client is rogue */
int hurd_ecan_is_client_rogue(mach_port_t client_port);

/* ==================== ECAN Cycle Management ==================== */

/* Execute one complete ECAN attention cycle
 * 
 * Performs:
 *   1. Apply wages for recorded activities
 *   2. Collect rent from resource usage
 *   3. Detect rogue clients
 *   4. Update attention economics
 * 
 * Should be called periodically (e.g., every 60 seconds)
 */
error_t hurd_ecan_cycle(void);

/* ==================== Economics Monitoring ==================== */

/* Get attention economics summary */
typedef struct {
    size_t total_funds;
    size_t focus_threshold;
    double total_sti;
    double total_lti;
    size_t client_count;
    double wage_rate;
    double rent_rate;
    double spread_rate;
    size_t history_length;
} ecan_economics_t;

error_t hurd_ecan_get_economics(ecan_economics_t *economics);

/* ==================== Distributed Attention ==================== */

/* Synchronize attention state with remote Hurd node */
error_t hurd_ecan_sync_node(const char *node_id);

/* Broadcast attention event to cluster */
error_t hurd_ecan_broadcast_event(const char *event_type,
                                  const void *event_data,
                                  size_t data_size);

/* ==================== Configuration ==================== */

/* Set ECAN economic parameters */
error_t hurd_ecan_set_wage_rate(double rate);
error_t hurd_ecan_set_rent_rate(double rate);
error_t hurd_ecan_set_spread_rate(double rate);
error_t hurd_ecan_set_focus_threshold(size_t threshold);

/* Get current configuration */
error_t hurd_ecan_get_wage_rate(double *rate);
error_t hurd_ecan_get_rent_rate(double *rate);
error_t hurd_ecan_get_spread_rate(double *rate);
error_t hurd_ecan_get_focus_threshold(size_t *threshold);

/* ==================== Convenience Macros ==================== */

/* Record activity with default quantity */
#define HURD_ECAN_RECORD(client, activity) \
    hurd_ecan_record_activity((client), (activity), 1)

/* Check and reject if no credit */
#define HURD_ECAN_CHECK_OR_FAIL(client, size) \
    do { \
        if (hurd_ecan_check_client_credit((client), (size)) != 0) { \
            return ENOSPC; \
        } \
    } while (0)

/* Charge rent for memory allocation */
#define HURD_ECAN_CHARGE_MEMORY(client, size) \
    hurd_ecan_charge_rent((client), RESOURCE_MEMORY, (size))

#endif /* _HURD_ECAN_INTEGRATION_H */
