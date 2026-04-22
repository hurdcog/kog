/*
 * Virtual Dynamic Shared Object (VDSO) Support for GNU Mach
 * 
 * Copyright (c) 2024 Cognu Mach Contributors
 * All rights reserved.
 * 
 * This provides fast user-space callable implementations of frequently
 * used system calls to optimize performance.
 */

#ifndef _KERN_VDSO_H_
#define _KERN_VDSO_H_

#include <mach/mach_types.h>
#include <mach/vm_param.h> 
#include <kern/kern_types.h>

/*
 * VDSO Configuration Constants
 */
#define VDSO_PAGE_SIZE		PAGE_SIZE
#define VDSO_MAX_SYMBOLS	16
#define VDSO_VERSION		1

/*
 * VDSO Symbol Types - functions exposed in the VDSO
 */
typedef enum {
    VDSO_SYM_GETTIMEOFDAY = 0,
    VDSO_SYM_CLOCK_GETTIME,
    VDSO_SYM_TIME,
    VDSO_SYM_GETPID,
    VDSO_SYM_COUNT /* Must be last */
} vdso_symbol_type_t;

/*
 * VDSO Symbol Table Entry
 */
typedef struct vdso_symbol {
    const char *name;           /* Symbol name */
    vm_offset_t offset;         /* Offset within VDSO page */
    boolean_t available;        /* Whether this symbol is available */
} vdso_symbol_t;

/*
 * VDSO Page Header - placed at the beginning of the VDSO page
 */
typedef struct vdso_header {
    uint32_t magic;             /* Magic number for validation */
    uint32_t version;           /* VDSO version */
    uint32_t symbol_count;      /* Number of available symbols */
    uint32_t reserved;          /* Reserved for future use */
    vdso_symbol_t symbols[VDSO_MAX_SYMBOLS];
} vdso_header_t;

/*
 * VDSO State Management
 */
typedef struct vdso_state {
    vm_offset_t vdso_page;      /* Kernel address of VDSO page */
    vm_size_t vdso_size;        /* Size of VDSO mapping */
    boolean_t initialized;      /* Whether VDSO is initialized */
    vdso_header_t *header;      /* Pointer to VDSO header */
} vdso_state_t;

/*
 * VDSO Magic Number
 */
#define VDSO_MAGIC 0x564453C0  /* "VDSO" */

/*
 * VDSO Function Prototypes
 */

/* Initialization */
extern void vdso_init(void);
extern kern_return_t vdso_bootstrap(void);

/* Memory Management */
extern kern_return_t vdso_map_into_task(task_t task, vm_offset_t *address);
extern void vdso_unmap_from_task(task_t task);

/* Symbol Management */
extern vm_offset_t vdso_lookup_symbol(vdso_symbol_type_t symbol);
extern kern_return_t vdso_add_symbol(vdso_symbol_type_t type, 
                                     const char *name, 
                                     vm_offset_t offset);

/* Fast Path Implementations */
extern int vdso_gettimeofday(time_value_t *tv);
extern int vdso_clock_gettime(int clock_id, time_value_t *tp);
extern long vdso_time(long *t);
extern int vdso_getpid(void);

/* State Access */
extern vdso_state_t *vdso_get_state(void);
extern boolean_t vdso_is_available(void);

/*
 * Architecture-specific functions (implemented in arch-specific files)
 */
extern void vdso_arch_init(void);
extern kern_return_t vdso_arch_setup_page(vm_offset_t page);

/*
 * VDSO User-visible Constants
 * These are used by user-space to locate and use VDSO functions
 */
#define VDSO_USER_BASE  0x7ffff000UL  /* Default user-space VDSO address */

#endif /* _KERN_VDSO_H_ */