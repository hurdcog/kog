/* hurd-ecan-integration.c - ECAN Attention Economics Implementation for GNU Hurd
 * 
 * Implements the C API for ECAN attention allocation integrated with Hurd servers.
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

#include "hurd-ecan-integration.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/* Internal state structure */
typedef struct {
    int initialized;
    pthread_mutex_t lock;
    double wage_rate;
    double rent_rate;
    double spread_rate;
    size_t focus_threshold;
    size_t client_count;
    /* In full implementation, would maintain hash tables of clients */
} ecan_state_t;

static ecan_state_t global_ecan_state = {
    .initialized = 0,
    .wage_rate = ECAN_DEFAULT_WAGE_RATE,
    .rent_rate = ECAN_DEFAULT_RENT_RATE,
    .spread_rate = ECAN_DEFAULT_SPREAD_RATE,
    .focus_threshold = ECAN_DEFAULT_FOCUS_THRESHOLD,
    .client_count = 0,
};

/* ==================== Core ECAN Functions ==================== */

error_t hurd_ecan_init(void) {
    if (global_ecan_state.initialized) {
        return 0; /* Already initialized */
    }
    
    pthread_mutex_init(&global_ecan_state.lock, NULL);
    global_ecan_state.initialized = 1;
    
    /* In full implementation, would:
     * - Initialize Guile interpreter
     * - Load ECAN Scheme modules
     * - Create global attention bank
     * - Start attention cycle thread
     */
    
    return 0;
}

void hurd_ecan_shutdown(void) {
    if (!global_ecan_state.initialized) {
        return;
    }
    
    pthread_mutex_lock(&global_ecan_state.lock);
    
    /* In full implementation, would:
     * - Stop attention cycle thread
     * - Cleanup client tracking
     * - Shutdown Guile interpreter
     */
    
    global_ecan_state.initialized = 0;
    pthread_mutex_unlock(&global_ecan_state.lock);
    pthread_mutex_destroy(&global_ecan_state.lock);
}

/* ==================== Client Registration ==================== */

error_t hurd_ecan_register_client(mach_port_t client_port, pid_t pid) {
    if (!global_ecan_state.initialized) {
        return EINVAL;
    }
    
    pthread_mutex_lock(&global_ecan_state.lock);
    
    /* In full implementation, would:
     * - Create atom for client in atomspace
     * - Initialize attention value (STI=100, LTI=50, VLTI=25)
     * - Add to attention bank
     * - Map client_port -> atom
     */
    
    global_ecan_state.client_count++;
    
    pthread_mutex_unlock(&global_ecan_state.lock);
    return 0;
}

error_t hurd_ecan_unregister_client(mach_port_t client_port) {
    if (!global_ecan_state.initialized) {
        return EINVAL;
    }
    
    pthread_mutex_lock(&global_ecan_state.lock);
    
    /* In full implementation, would:
     * - Remove client from attention bank
     * - Cleanup atomspace entries
     * - Remove port mapping
     */
    
    if (global_ecan_state.client_count > 0) {
        global_ecan_state.client_count--;
    }
    
    pthread_mutex_unlock(&global_ecan_state.lock);
    return 0;
}

/* ==================== Activity Tracking (Wages) ==================== */

error_t hurd_ecan_record_activity(mach_port_t client_port,
                                   activity_type_t activity,
                                   size_t quantity) {
    if (!global_ecan_state.initialized) {
        return EINVAL;
    }
    
    /* In full implementation, would:
     * - Look up client atom from port
     * - Calculate activity value = activity * quantity
     * - Queue for wage application
     * - Call: (attention-bank-apply-wages! bank `((client activity-value)))
     */
    
    return 0;
}

error_t hurd_ecan_record_activities(mach_port_t client_port,
                                    activity_type_t *activities,
                                    size_t *quantities,
                                    size_t count) {
    if (!global_ecan_state.initialized || !activities || !quantities) {
        return EINVAL;
    }
    
    /* Batch record activities */
    for (size_t i = 0; i < count; i++) {
        error_t err = hurd_ecan_record_activity(client_port, 
                                               activities[i], 
                                               quantities[i]);
        if (err) return err;
    }
    
    return 0;
}

error_t hurd_ecan_apply_wages(void) {
    if (!global_ecan_state.initialized) {
        return EINVAL;
    }
    
    /* In full implementation, would:
     * - Call: (attention-bank-apply-wages! *global-attention-bank* activities)
     * - Update all client attention values
     */
    
    return 0;
}

/* ==================== Resource Tracking (Rent) ==================== */

error_t hurd_ecan_charge_rent(mach_port_t client_port,
                              resource_type_t resource,
                              size_t amount) {
    if (!global_ecan_state.initialized) {
        return EINVAL;
    }
    
    /* In full implementation, would:
     * - Calculate rent = amount * rent_rate
     * - Update client STI: STI += amount (will be rented later)
     * - Record resource allocation
     */
    
    return 0;
}

error_t hurd_ecan_collect_rent(void) {
    if (!global_ecan_state.initialized) {
        return EINVAL;
    }
    
    /* In full implementation, would:
     * - Call: (attention-bank-collect-rent! *global-attention-bank*)
     * - Deduct rent from all clients
     * - Return rent to global funds pool
     */
    
    return 0;
}

/* ==================== Client Credit & Limits ==================== */

error_t hurd_ecan_check_client_credit(mach_port_t client_port,
                                      size_t required_resources) {
    if (!global_ecan_state.initialized) {
        return EINVAL;
    }
    
    /* In full implementation, would:
     * - Get client attention value
     * - Check: client_sti >= required_resources * cost_factor
     * - Return 0 if sufficient, ENOSPC if not
     */
    
    /* Stub: always allow (until full implementation) */
    return 0;
}

size_t hurd_ecan_get_client_limit(mach_port_t client_port,
                                  resource_type_t resource) {
    if (!global_ecan_state.initialized) {
        return 0;
    }
    
    /* In full implementation, would:
     * - Get client attention value (STI, LTI)
     * - Calculate: base_limit + (LTI * bonus) - (STI * penalty)
     * - Return dynamic limit
     */
    
    /* Stub: return reasonable default */
    return 1024 * 1024 * 1024; /* 1GB default */
}

error_t hurd_ecan_get_client_attention(mach_port_t client_port,
                                       attention_value_t *av) {
    if (!global_ecan_state.initialized || !av) {
        return EINVAL;
    }
    
    /* In full implementation, would:
     * - Look up client in attention bank
     * - Return current STI/LTI/VLTI values
     */
    
    /* Stub: return default values */
    av->sti = 100.0;
    av->lti = 50.0;
    av->vlti = 25.0;
    
    return 0;
}

/* ==================== Rogue Client Detection ==================== */

error_t hurd_ecan_get_rogue_clients(mach_port_t **rogues, size_t *count) {
    if (!global_ecan_state.initialized || !rogues || !count) {
        return EINVAL;
    }
    
    /* In full implementation, would:
     * - Scan all clients in attention bank
     * - Identify: high STI (resources) + low LTI (activity)
     * - Build list of rogue client ports
     */
    
    /* Stub: no rogues detected */
    *rogues = NULL;
    *count = 0;
    
    return 0;
}

int hurd_ecan_is_client_rogue(mach_port_t client_port) {
    if (!global_ecan_state.initialized) {
        return 0;
    }
    
    /* In full implementation, would:
     * - Get client attention value
     * - Check: STI > 500 && LTI < 50
     * - Return 1 if rogue, 0 if not
     */
    
    return 0; /* Stub: not rogue */
}

/* ==================== ECAN Cycle Management ==================== */

error_t hurd_ecan_cycle(void) {
    if (!global_ecan_state.initialized) {
        return EINVAL;
    }
    
    pthread_mutex_lock(&global_ecan_state.lock);
    
    /* In full implementation, would:
     * 1. Apply wages for recorded activities
     * 2. Collect rent from all clients
     * 3. Detect rogue clients
     * 4. Apply spreading activation
     * 5. Update economics history
     */
    
    pthread_mutex_unlock(&global_ecan_state.lock);
    
    return 0;
}

/* ==================== Economics Monitoring ==================== */

error_t hurd_ecan_get_economics(ecan_economics_t *economics) {
    if (!global_ecan_state.initialized || !economics) {
        return EINVAL;
    }
    
    pthread_mutex_lock(&global_ecan_state.lock);
    
    /* In full implementation, would:
     * - Call: (attention-bank-get-economics *global-attention-bank*)
     * - Convert Scheme alist to C struct
     */
    
    /* Stub: return current state */
    economics->total_funds = ECAN_DEFAULT_TOTAL_FUNDS;
    economics->focus_threshold = global_ecan_state.focus_threshold;
    economics->total_sti = 100.0 * global_ecan_state.client_count;
    economics->total_lti = 50.0 * global_ecan_state.client_count;
    economics->client_count = global_ecan_state.client_count;
    economics->wage_rate = global_ecan_state.wage_rate;
    economics->rent_rate = global_ecan_state.rent_rate;
    economics->spread_rate = global_ecan_state.spread_rate;
    economics->history_length = 0;
    
    pthread_mutex_unlock(&global_ecan_state.lock);
    
    return 0;
}

/* ==================== Distributed Attention ==================== */

error_t hurd_ecan_sync_node(const char *node_id) {
    if (!global_ecan_state.initialized || !node_id) {
        return EINVAL;
    }
    
    /* In full implementation, would:
     * - Call: (distributed-attention-sync! network node-id)
     * - Exchange attention state with remote node
     * - Merge economics
     */
    
    return 0;
}

error_t hurd_ecan_broadcast_event(const char *event_type,
                                  const void *event_data,
                                  size_t data_size) {
    if (!global_ecan_state.initialized || !event_type) {
        return EINVAL;
    }
    
    /* In full implementation, would:
     * - Call: (distributed-attention-broadcast! network event)
     * - Send to all nodes in mesh topology
     */
    
    return 0;
}

/* ==================== Configuration ==================== */

error_t hurd_ecan_set_wage_rate(double rate) {
    if (!global_ecan_state.initialized || rate < 0.0 || rate > 1.0) {
        return EINVAL;
    }
    
    pthread_mutex_lock(&global_ecan_state.lock);
    global_ecan_state.wage_rate = rate;
    pthread_mutex_unlock(&global_ecan_state.lock);
    
    return 0;
}

error_t hurd_ecan_set_rent_rate(double rate) {
    if (!global_ecan_state.initialized || rate < 0.0 || rate > 1.0) {
        return EINVAL;
    }
    
    pthread_mutex_lock(&global_ecan_state.lock);
    global_ecan_state.rent_rate = rate;
    pthread_mutex_unlock(&global_ecan_state.lock);
    
    return 0;
}

error_t hurd_ecan_set_spread_rate(double rate) {
    if (!global_ecan_state.initialized || rate < 0.0 || rate > 1.0) {
        return EINVAL;
    }
    
    pthread_mutex_lock(&global_ecan_state.lock);
    global_ecan_state.spread_rate = rate;
    pthread_mutex_unlock(&global_ecan_state.lock);
    
    return 0;
}

error_t hurd_ecan_set_focus_threshold(size_t threshold) {
    if (!global_ecan_state.initialized) {
        return EINVAL;
    }
    
    pthread_mutex_lock(&global_ecan_state.lock);
    global_ecan_state.focus_threshold = threshold;
    pthread_mutex_unlock(&global_ecan_state.lock);
    
    return 0;
}

error_t hurd_ecan_get_wage_rate(double *rate) {
    if (!global_ecan_state.initialized || !rate) {
        return EINVAL;
    }
    
    pthread_mutex_lock(&global_ecan_state.lock);
    *rate = global_ecan_state.wage_rate;
    pthread_mutex_unlock(&global_ecan_state.lock);
    
    return 0;
}

error_t hurd_ecan_get_rent_rate(double *rate) {
    if (!global_ecan_state.initialized || !rate) {
        return EINVAL;
    }
    
    pthread_mutex_lock(&global_ecan_state.lock);
    *rate = global_ecan_state.rent_rate;
    pthread_mutex_unlock(&global_ecan_state.lock);
    
    return 0;
}

error_t hurd_ecan_get_spread_rate(double *rate) {
    if (!global_ecan_state.initialized || !rate) {
        return EINVAL;
    }
    
    pthread_mutex_lock(&global_ecan_state.lock);
    *rate = global_ecan_state.spread_rate;
    pthread_mutex_unlock(&global_ecan_state.lock);
    
    return 0;
}

error_t hurd_ecan_get_focus_threshold(size_t *threshold) {
    if (!global_ecan_state.initialized || !threshold) {
        return EINVAL;
    }
    
    pthread_mutex_lock(&global_ecan_state.lock);
    *threshold = global_ecan_state.focus_threshold;
    pthread_mutex_unlock(&global_ecan_state.lock);
    
    return 0;
}
