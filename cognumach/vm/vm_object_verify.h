/*
 * VM Object Verification and Consistency Checks
 * Copyright (c) 2024 GNU Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _VM_VM_OBJECT_VERIFY_H_
#define _VM_VM_OBJECT_VERIFY_H_

#include <mach/kern_return.h>
#include <mach/boolean.h>

/* Forward declarations */
typedef struct vm_object *vm_object_t;

/*
 * VM object memory statistics structure.
 */
typedef struct vm_object_memory_stats {
    unsigned long resident_pages;    /* Number of resident pages */
    unsigned long wired_pages;       /* Number of wired pages */
    unsigned long active_pages;      /* Number of active pages */
    unsigned long inactive_pages;    /* Number of inactive pages */
    unsigned long dirty_pages;       /* Number of dirty pages */
    unsigned long referenced_pages;  /* Number of referenced pages */
    vm_size_t memory_size;          /* Total memory size in bytes */
} vm_object_memory_stats_t;

/*
 * VM object verification functions.
 */
extern boolean_t vm_object_verify_resident_count(vm_object_t object);
extern void vm_object_increment_resident_count(vm_object_t object);
extern void vm_object_decrement_resident_count(vm_object_t object);
extern kern_return_t vm_object_get_memory_stats(vm_object_t object, vm_object_memory_stats_t *stats);
extern void vm_object_verify_all_counts(void);

#endif /* _VM_VM_OBJECT_VERIFY_H_ */