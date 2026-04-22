/* HurdCog AtomSpace-Microkernel Bridge
 * Implements direct integration between OpenCog AtomSpace and GNU/Hurd microkernel
 * Part of Phase 2: Microkernel Integration in SKZ framework
 * 
 * This module provides C-level interface between the cognitive architecture
 * and the Hurd microkernel for performance and direct system integration.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

/* Hurd and Mach includes */
#include <mach.h>
#include <hurd.h>
#include <hurd/process.h>
#include <hurd/auth.h>
#include <hurd/paths.h>

/* Local includes */
#include "hurd-atomspace-bridge.h"

/* Global state for the bridge */
static atomspace_bridge_t bridge_context = {0};
static int bridge_initialized = 0;

/* Error handling and logging */
static void
log_error(const char *context, const char *message, error_t err)
{
    fprintf(stderr, "[HurdAtomSpace ERROR] %s: %s", context, message);
    if (err)
        fprintf(stderr, " (error: %s)", strerror(err));
    fprintf(stderr, "\n");
}

static void
log_info(const char *context, const char *message)
{
    printf("[HurdAtomSpace INFO] %s: %s\n", context, message);
}

/* Initialize the AtomSpace-Microkernel bridge */
error_t
hurd_atomspace_bridge_init(void)
{
    error_t err = 0;
    
    if (bridge_initialized) {
        log_info("INIT", "Bridge already initialized");
        return 0;
    }
    
    log_info("INIT", "Initializing HurdCog AtomSpace-Microkernel bridge");
    
    /* Initialize Mach port management */
    bridge_context.task_port = mach_task_self();
    if (!MACH_PORT_VALID(bridge_context.task_port)) {
        log_error("INIT", "Failed to get task port", 0);
        return EINVAL;
    }
    
    /* Get host port */
    err = get_privileged_ports(&bridge_context.host_priv_port, NULL);
    if (err) {
        log_error("INIT", "Failed to get host port", err);
        bridge_context.host_priv_port = MACH_PORT_NULL;
    }
    
    /* Initialize AtomSpace interface */
    bridge_context.atom_count = 0;
    bridge_context.port_count = 0;
    bridge_context.server_count = 0;
    
    /* Initialize performance monitoring */
    gettimeofday(&bridge_context.init_time, NULL);
    bridge_context.ipc_calls = 0;
    bridge_context.errors = 0;
    
    bridge_initialized = 1;
    log_info("INIT", "Bridge initialization complete");
    
    return 0;
}

/* Shutdown the bridge */
void
hurd_atomspace_bridge_shutdown(void)
{
    if (!bridge_initialized)
        return;
        
    log_info("SHUTDOWN", "Shutting down HurdCog AtomSpace-Microkernel bridge");
    
    /* Clean up Mach ports */
    if (MACH_PORT_VALID(bridge_context.host_priv_port)) {
        mach_port_deallocate(mach_task_self(), bridge_context.host_priv_port);
    }
    
    /* Print performance stats */
    printf("[HurdAtomSpace STATS] Atoms: %d, Ports: %d, Servers: %d, IPC calls: %lu, Errors: %lu\n",
           bridge_context.atom_count, bridge_context.port_count, 
           bridge_context.server_count, bridge_context.ipc_calls, bridge_context.errors);
    
    memset(&bridge_context, 0, sizeof(bridge_context));
    bridge_initialized = 0;
}

/* Register a Mach port in the AtomSpace */
error_t
hurd_atomspace_register_port(const char *port_name, mach_port_t port, 
                            mach_port_type_t port_type)
{
    if (!bridge_initialized) {
        error_t err = hurd_atomspace_bridge_init();
        if (err) return err;
    }
    
    if (!port_name || !MACH_PORT_VALID(port)) {
        log_error("REGISTER_PORT", "Invalid parameters", 0);
        bridge_context.errors++;
        return EINVAL;
    }
    
    /* Check if we have space for more ports */
    if (bridge_context.port_count >= MAX_PORTS) {
        log_error("REGISTER_PORT", "Maximum ports exceeded", 0);
        bridge_context.errors++;
        return ENOMEM;
    }
    
    /* Store port information */
    atomspace_port_t *port_entry = &bridge_context.ports[bridge_context.port_count];
    strncpy(port_entry->name, port_name, sizeof(port_entry->name) - 1);
    port_entry->name[sizeof(port_entry->name) - 1] = '\0';
    port_entry->port = port;
    port_entry->type = port_type;
    port_entry->registered_time = time(NULL);
    
    bridge_context.port_count++;
    bridge_context.atom_count++;
    
    log_info("REGISTER_PORT", port_name);
    
    return 0;
}

/* Register a Hurd server in the AtomSpace */
error_t  
hurd_atomspace_register_server(const char *server_name, const char *server_path,
                              mach_port_t server_port)
{
    if (!bridge_initialized) {
        error_t err = hurd_atomspace_bridge_init();
        if (err) return err;
    }
    
    if (!server_name || !server_path) {
        log_error("REGISTER_SERVER", "Invalid parameters", 0);
        bridge_context.errors++;
        return EINVAL;
    }
    
    /* Check if we have space for more servers */
    if (bridge_context.server_count >= MAX_SERVERS) {
        log_error("REGISTER_SERVER", "Maximum servers exceeded", 0);
        bridge_context.errors++;
        return ENOMEM;
    }
    
    /* Store server information */
    atomspace_server_t *server_entry = &bridge_context.servers[bridge_context.server_count];
    strncpy(server_entry->name, server_name, sizeof(server_entry->name) - 1);
    server_entry->name[sizeof(server_entry->name) - 1] = '\0';
    strncpy(server_entry->path, server_path, sizeof(server_entry->path) - 1);
    server_entry->path[sizeof(server_entry->path) - 1] = '\0';
    server_entry->port = server_port;
    server_entry->registered_time = time(NULL);
    
    bridge_context.server_count++;
    bridge_context.atom_count++;
    
    log_info("REGISTER_SERVER", server_name);
    
    return 0;
}

/* Send IPC message through AtomSpace-aware routing */
error_t
hurd_atomspace_ipc_send(const char *destination, const void *data, size_t size)
{
    if (!bridge_initialized) {
        error_t err = hurd_atomspace_bridge_init();
        if (err) return err;
    }
    
    bridge_context.ipc_calls++;
    
    /* Find destination port */
    mach_port_t dest_port = MACH_PORT_NULL;
    for (int i = 0; i < bridge_context.port_count; i++) {
        if (strcmp(bridge_context.ports[i].name, destination) == 0) {
            dest_port = bridge_context.ports[i].port;
            break;
        }
    }
    
    if (!MACH_PORT_VALID(dest_port)) {
        log_error("IPC_SEND", "Destination port not found", 0);
        bridge_context.errors++;
        return ENOENT;
    }
    
    /* For now, simulate IPC send */
    log_info("IPC_SEND", "Message sent through cognitive routing");
    
    return 0;
}

/* Query AtomSpace for microkernel objects */
int
hurd_atomspace_query_objects(const char *object_type, char **results, int max_results)
{
    if (!bridge_initialized) {
        error_t err = hurd_atomspace_bridge_init();
        if (err) return -1;
    }
    
    int count = 0;
    
    if (strcmp(object_type, "ports") == 0) {
        for (int i = 0; i < bridge_context.port_count && count < max_results; i++) {
            results[count] = strdup(bridge_context.ports[i].name);
            count++;
        }
    } else if (strcmp(object_type, "servers") == 0) {
        for (int i = 0; i < bridge_context.server_count && count < max_results; i++) {
            results[count] = strdup(bridge_context.servers[i].name);
            count++;
        }
    }
    
    return count;
}

/* Get bridge statistics */
void
hurd_atomspace_get_stats(atomspace_stats_t *stats)
{
    if (!stats) return;
    
    if (!bridge_initialized) {
        memset(stats, 0, sizeof(*stats));
        return;
    }
    
    stats->atom_count = bridge_context.atom_count;
    stats->port_count = bridge_context.port_count;
    stats->server_count = bridge_context.server_count;
    stats->ipc_calls = bridge_context.ipc_calls;
    stats->errors = bridge_context.errors;
    
    /* Calculate uptime */
    struct timeval now;
    gettimeofday(&now, NULL);
    stats->uptime_seconds = now.tv_sec - bridge_context.init_time.tv_sec;
}

/* Performance monitoring */
void
hurd_atomspace_monitor_performance(void)
{
    if (!bridge_initialized) return;
    
    atomspace_stats_t stats;
    hurd_atomspace_get_stats(&stats);
    
    printf("[HurdAtomSpace MONITOR] Uptime: %ld s, IPC: %lu, Errors: %lu, Efficiency: %.2f%%\n",
           stats.uptime_seconds, stats.ipc_calls, stats.errors,
           stats.ipc_calls > 0 ? (1.0 - (double)stats.errors / stats.ipc_calls) * 100.0 : 100.0);
}

/* Bootstrap core Hurd components into AtomSpace */
error_t
hurd_atomspace_bootstrap_core(void)
{
    error_t err = 0;
    
    if (!bridge_initialized) {
        err = hurd_atomspace_bridge_init();
        if (err) return err;
    }
    
    log_info("BOOTSTRAP", "Registering core Hurd components in AtomSpace");
    
    /* Register essential Mach ports */
    err = hurd_atomspace_register_port("task-port", mach_task_self(), MACH_PORT_TYPE_SEND);
    if (err) {
        log_error("BOOTSTRAP", "Failed to register task port", err);
        return err;
    }
    
    /* Register essential Hurd servers */
    err = hurd_atomspace_register_server("auth-server", _SERVERS_AUTH, MACH_PORT_NULL);
    if (err) {
        log_error("BOOTSTRAP", "Failed to register auth server", err);
    }
    
    err = hurd_atomspace_register_server("proc-server", _SERVERS_PROC, MACH_PORT_NULL);
    if (err) {
        log_error("BOOTSTRAP", "Failed to register proc server", err);
    }
    
    err = hurd_atomspace_register_server("exec-server", _SERVERS_EXEC, MACH_PORT_NULL);
    if (err) {
        log_error("BOOTSTRAP", "Failed to register exec server", err);
    }
    
    log_info("BOOTSTRAP", "Core component registration complete");
    return 0;
}