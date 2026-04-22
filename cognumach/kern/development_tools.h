/*
 * Advanced Development Tools Kernel Interface
 * Copyright (C) 2024 Free Software Foundation, Inc.
 */

#ifndef _KERN_DEVELOPMENT_TOOLS_H_
#define _KERN_DEVELOPMENT_TOOLS_H_

#include <mach/kern_return.h>
#include <mach/mach_types.h>

/*
 * Initialize the development tools subsystem
 */
extern void development_tools_init(void);

/*
 * Valgrind interface functions (RPC wrappers)
 */
extern kern_return_t valgrind_enable_rpc(host_t host, boolean_t enable);
extern kern_return_t valgrind_track_alloc_rpc(host_t host, vm_address_t address, vm_size_t size, vm_address_t caller_pc);
extern kern_return_t valgrind_track_free_rpc(host_t host, vm_address_t address);
extern kern_return_t valgrind_make_defined_rpc(host_t host, vm_address_t address, vm_size_t size);
extern kern_return_t valgrind_make_undefined_rpc(host_t host, vm_address_t address, vm_size_t size);
extern kern_return_t valgrind_make_noaccess_rpc(host_t host, vm_address_t address, vm_size_t size);
extern kern_return_t valgrind_check_memory_rpc(host_t host, vm_address_t address, vm_size_t size, boolean_t *is_defined, boolean_t *is_addressable);
extern kern_return_t valgrind_client_request_rpc(host_t host, uint32_t request, vm_address_t arg1, vm_address_t arg2, vm_address_t arg3, vm_address_t arg4, vm_address_t arg5, kern_return_t *result);

/*
 * System debugging interface functions (RPC wrappers)  
 */
extern kern_return_t system_debug_enable_rpc(host_t host, uint32_t subsystem, uint32_t level);
extern kern_return_t system_debug_event_rpc(host_t host, uint32_t subsystem, uint32_t event_type, vm_address_t address, const char *context);
extern kern_return_t system_debug_get_stats_rpc(host_t host, uint32_t subsystem, natural_t *stats_count, pointer_t *stats);
extern kern_return_t system_debug_set_tracking_rpc(host_t host, boolean_t enable);
extern kern_return_t system_debug_dump_state_rpc(host_t host);

/*
 * Security analysis interface functions (RPC wrappers)
 */
extern kern_return_t security_monitor_enable_rpc(host_t host, boolean_t enable);
extern kern_return_t security_event_report_rpc(host_t host, uint32_t event_type, vm_address_t address, const char *context);
extern kern_return_t security_get_statistics_rpc(host_t host, natural_t *stats_count, pointer_t *stats);
extern kern_return_t security_buffer_guard_init_rpc(host_t host, vm_address_t buffer_addr, vm_size_t buffer_size, uint32_t *guard_id);
extern kern_return_t security_buffer_guard_check_rpc(host_t host, uint32_t guard_id, boolean_t *is_valid);
extern kern_return_t security_cfi_validate_rpc(host_t host, vm_address_t return_addr, vm_address_t expected_addr, boolean_t *is_valid);
extern kern_return_t security_reset_statistics_rpc(host_t host);

#endif /* _KERN_DEVELOPMENT_TOOLS_H_ */