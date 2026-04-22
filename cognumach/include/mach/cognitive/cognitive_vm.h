/*
 * Cognitive VM Extensions for Cognumach
 * Provides VM primitives optimized for AtomSpace operations
 */

#ifndef _MACH_COGNITIVE_VM_H_
#define _MACH_COGNITIVE_VM_H_

#include <mach/vm_types.h>
#include <mach/vm_prot.h>

/* Cognitive memory regions */
#define VM_REGION_ATOMSPACE     0x1000
#define VM_REGION_COGNITIVE     0x2000

/* Cognitive VM attributes */
typedef struct {
    vm_address_t base_address;
    vm_size_t size;
    vm_prot_t protection;
    int cognitive_flags;
    int atom_count;
    int link_count;
} cognitive_vm_region_t;

/* Function declarations */
kern_return_t cognitive_vm_allocate(
    vm_map_t map,
    vm_address_t *address,
    vm_size_t size,
    int flags);

kern_return_t cognitive_vm_deallocate(
    vm_map_t map,
    vm_address_t address,
    vm_size_t size);

kern_return_t cognitive_vm_protect(
    vm_map_t map,
    vm_address_t address,
    vm_size_t size,
    vm_prot_t protection);

kern_return_t cognitive_vm_region_info(
    vm_map_t map,
    vm_address_t address,
    cognitive_vm_region_t *info);

#endif /* _MACH_COGNITIVE_VM_H_ */
