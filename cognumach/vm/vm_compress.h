/*
 * Copyright (c) 2024 GNU Mach project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * Memory compression interface for GNU Mach
 */

#ifndef _VM_COMPRESS_H_
#define _VM_COMPRESS_H_

#include <mach/boolean.h>
#include <vm/vm_types.h>
#include <vm/vm_object.h>
#include <vm/vm_page.h>
#include <kern/queue.h>
#include <kern/lock.h>

/*
 * Compressed page structure
 */
struct vm_compressed_page {
	queue_chain_t		hash_link;	/* Hash table linkage */
	vm_object_t		object;		/* Source object */
	vm_offset_t		offset;		/* Offset within object */
	void			*compressed_data;	/* Compressed page data */
	vm_size_t		compressed_size;	/* Size of compressed data */
	unsigned int		access_time;	/* For LRU eviction */
};

/*
 * Compression statistics
 */
struct vm_compress_stats {
	unsigned int		compressed_pages;	/* Pages compressed */
	unsigned int		decompressed_pages;	/* Pages decompressed */
	unsigned int		average_compression_ratio;  /* Percentage */
};

/*
 * Function prototypes
 */

/* Initialize compression subsystem */
void vm_compress_init(void);

/* Compress a page and store it */
kern_return_t vm_page_compress(vm_object_t object, vm_offset_t offset, vm_page_t page);

/* Find and decompress a page */
kern_return_t vm_page_decompress(vm_object_t object, vm_offset_t offset, vm_page_t page);

/* Remove a compressed page without decompressing */
kern_return_t vm_page_compress_remove(vm_object_t object, vm_offset_t offset);

/* Get compression statistics */
void vm_compress_get_stats(struct vm_compress_stats *stats);

#endif /* _VM_COMPRESS_H_ */