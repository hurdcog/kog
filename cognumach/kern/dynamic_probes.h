/*
 * Dynamic Probe Management for GNU Mach
 * Runtime probe insertion and removal capabilities
 * 
 * Copyright (c) 2024 Cognu Mach Contributors
 * All rights reserved.
 */

#ifndef _KERN_DYNAMIC_PROBES_H_
#define _KERN_DYNAMIC_PROBES_H_

#include <kern/dtrace.h>
#include <kern/kern_types.h>

/*
 * Dynamic probe configuration
 */
typedef struct dynamic_probe_config {
    const char *module_name;       /* Module to instrument */
    const char *function_name;     /* Function to instrument */  
    const char *probe_name;        /* User-defined probe name */
    dtrace_probe_type_t type;      /* Type of probe */
    vm_offset_t address;           /* Memory address (if known) */
    boolean_t enabled;             /* Initially enabled? */
} dynamic_probe_config_t;

/*
 * Dynamic probe status
 */
typedef struct dynamic_probe_status {
    uint32_t probe_id;             /* Assigned probe ID */
    boolean_t active;              /* Currently active? */
    uint64_t fire_count;           /* Times fired since creation */
    uint64_t error_count;          /* Error count */
    vm_offset_t actual_address;    /* Resolved address */
} dynamic_probe_status_t;

/*
 * Function prototypes for dynamic probe management
 */

/* Initialize dynamic probe subsystem */
void dynamic_probes_init(void);

/* Create a new dynamic probe */
uint32_t dynamic_probe_create(const dynamic_probe_config_t *config);

/* Remove a dynamic probe */
boolean_t dynamic_probe_destroy(uint32_t probe_id);

/* List all dynamic probes */
uint32_t dynamic_probe_list(dynamic_probe_status_t *probes, uint32_t max_probes);

/* Enable/disable dynamic probe by address */
boolean_t dynamic_probe_enable_at_address(vm_offset_t address, const char *name);
boolean_t dynamic_probe_disable_at_address(vm_offset_t address);

/* Resolve function name to address */
vm_offset_t dynamic_probe_resolve_symbol(const char *module, const char *function);

/* Install probe at specific address (low-level) */
boolean_t dynamic_probe_install_at_address(vm_offset_t address, uint32_t probe_id);
boolean_t dynamic_probe_remove_from_address(vm_offset_t address);

#endif /* _KERN_DYNAMIC_PROBES_H_ */