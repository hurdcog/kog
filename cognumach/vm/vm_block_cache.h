/*
 * Copyright (c) 2024 GNU Mach project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * Block-level cache layer for GNU Mach
 * 
 * This module provides a block-aware caching layer that sits between
 * the memory object interface and the page management system, optimizing
 * for block-oriented I/O operations while maintaining full compatibility
 * with the existing page-based memory object model.
 */

#ifndef _VM_BLOCK_CACHE_H_
#define _VM_BLOCK_CACHE_H_

#include <mach/boolean.h>
#include <vm/vm_types.h>
#include <kern/queue.h>
#include <kern/lock.h>
#include <vm/vm_object.h>
#include <vm/vm_page.h>

/*
 * Block cache configuration
 */
#define BLOCK_CACHE_MIN_BLOCK_SIZE	512		/* Minimum block size */
#define BLOCK_CACHE_MAX_BLOCK_SIZE	65536		/* Maximum block size */
#define BLOCK_CACHE_DEFAULT_BLOCKS	1024		/* Default cache size in blocks */
#define BLOCK_CACHE_HASH_SIZE		256		/* Hash table size */

/*
 * Block cache entry states
 */
typedef enum {
	BLOCK_CACHE_CLEAN = 0,		/* Block is clean, matches storage */
	BLOCK_CACHE_DIRTY = 1,		/* Block has been modified */
	BLOCK_CACHE_READING = 2,	/* Block is being read from storage */
	BLOCK_CACHE_WRITING = 3,	/* Block is being written to storage */
	BLOCK_CACHE_ERROR = 4		/* I/O error occurred */
} block_cache_state_t;

/*
 * Block cache entry
 * 
 * Represents a cached block that may span multiple pages.
 * Maintains both block-level metadata and links to constituent pages.
 */
struct block_cache_entry {
	queue_chain_t	hash_link;	/* Hash table linkage */
	queue_chain_t	lru_link;	/* LRU queue linkage */
	queue_chain_t	object_link;	/* Per-object linkage */
	
	vm_object_t	object;		/* Associated memory object */
	vm_offset_t	block_offset;	/* Block offset within object */
	vm_size_t	block_size;	/* Size of this block */
	
	block_cache_state_t state;	/* Current block state */
	
	unsigned int	ref_count;	/* Reference count */
	unsigned int	access_time;	/* For LRU replacement */
	unsigned int	access_frequency; /* Access frequency counter */
	
	boolean_t	read_ahead_hint; /* Block was fetched via read-ahead */
	boolean_t	write_cluster_hint; /* Block should be clustered for write */
	
	/* Page management */
	unsigned int	page_count;	/* Number of pages in this block */
	vm_page_t	*pages;		/* Array of constituent pages */
	
	/* Synchronization */
	simple_lock_data_t lock;	/* Per-block lock */
	int		waiters;	/* Number of waiters on this block */
};

typedef struct block_cache_entry *block_cache_entry_t;

/*
 * Block cache
 * 
 * Main data structure managing the block cache for a memory object.
 * Each memory object that benefits from block caching has an associated
 * block cache structure.
 */
struct block_cache {
	/* Configuration */
	vm_size_t	block_size;	/* Block size for this cache */
	unsigned int	max_blocks;	/* Maximum blocks to cache */
	
	/* Statistics */
	unsigned int	total_blocks;	/* Current number of cached blocks */
	unsigned int	hits;		/* Cache hits */
	unsigned int	misses;		/* Cache misses */
	unsigned int	reads;		/* Read operations */
	unsigned int	writes;		/* Write operations */
	
	/* Hash table for block lookup */
	queue_head_t	hash_buckets[BLOCK_CACHE_HASH_SIZE];
	
	/* LRU queue for replacement policy */
	queue_head_t	lru_queue;
	
	/* Per-object block list */
	queue_head_t	block_list;
	
	/* Synchronization */
	simple_lock_data_t lock;	/* Cache-wide lock */
	
	/* Associated memory object */
	vm_object_t	object;
};

typedef struct block_cache *block_cache_t;

/*
 * Block cache operations
 */

/* System initialization */
void vm_block_cache_init(void);

/* Cache initialization and cleanup */
block_cache_t block_cache_create(vm_object_t object, vm_size_t block_size);
void block_cache_destroy(block_cache_t cache);

/* Block operations */
kern_return_t block_cache_read(block_cache_t cache, vm_offset_t offset,
			      vm_size_t size, void **data);
kern_return_t block_cache_write(block_cache_t cache, vm_offset_t offset,
			       vm_size_t size, void *data);
kern_return_t block_cache_flush(block_cache_t cache, vm_offset_t offset,
			       vm_size_t size);

/* Cache management */
void block_cache_set_read_ahead(block_cache_t cache, boolean_t enable);
void block_cache_set_write_clustering(block_cache_t cache, boolean_t enable);
void block_cache_memory_pressure(block_cache_t cache, vm_size_t target_free);

/* Statistics and debugging */
void block_cache_get_stats(block_cache_t cache, 
			  unsigned int *hits, unsigned int *misses,
			  unsigned int *total_blocks);

/*
 * Integration with existing memory object system
 */
kern_return_t vm_object_enable_block_cache(vm_object_t object, 
					   vm_size_t block_size);
void vm_object_disable_block_cache(vm_object_t object);
boolean_t vm_object_has_block_cache(vm_object_t object);

/*
 * Page-block bridge operations
 * 
 * These functions handle the translation between page-based memory
 * object operations and block-based cache operations.
 */
kern_return_t vm_page_to_block_read(vm_object_t object, vm_offset_t offset,
				   vm_page_t *pages, unsigned int page_count);
kern_return_t vm_page_to_block_write(vm_object_t object, vm_offset_t offset,
				    vm_page_t *pages, unsigned int page_count);

/* Internal utilities */
static inline unsigned int
block_cache_hash(vm_offset_t offset)
{
	return (offset >> PAGE_SHIFT) % BLOCK_CACHE_HASH_SIZE;
}

static inline boolean_t
block_cache_offset_aligned(vm_offset_t offset, vm_size_t block_size)
{
	return (offset & (block_size - 1)) == 0;
}

static inline vm_size_t
block_cache_round_to_block(vm_size_t size, vm_size_t block_size)
{
	return (size + block_size - 1) & ~(block_size - 1);
}

#endif /* _VM_BLOCK_CACHE_H_ */