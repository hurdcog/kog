/*
 * Copyright (c) 2024 GNU Mach project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * Block-level cache implementation for GNU Mach
 */

#include <string.h>
#include <kern/assert.h>
#include <kern/debug.h>
#include <kern/kalloc.h>
#include <kern/slab.h>
#include <kern/queue.h>
#include <mach/vm_param.h>
#include <vm/vm_block_cache.h>
#include <vm/vm_page.h>
#include <vm/vm_object.h>
#include <vm/vm_kern.h>

/* Global block cache statistics */
static unsigned int total_cache_hits = 0;
static unsigned int total_cache_misses = 0;
static unsigned int total_blocks_cached = 0;

/* Slab cache for block cache entries */
static struct kmem_cache block_cache_entry_cache;
static struct kmem_cache block_cache_cache;

/* Global lock for cache statistics */
static simple_lock_data_t block_cache_stats_lock;

/*
 * Initialize the block cache subsystem
 */
void
vm_block_cache_init(void)
{
	kmem_cache_init(&block_cache_entry_cache, "block_cache_entry",
			sizeof(struct block_cache_entry), 0, NULL, 0);
	kmem_cache_init(&block_cache_cache, "block_cache", 
			sizeof(struct block_cache), 0, NULL, 0);
	simple_lock_init(&block_cache_stats_lock);
}

/*
 * Create a new block cache for the specified memory object
 */
block_cache_t
block_cache_create(vm_object_t object, vm_size_t block_size)
{
	block_cache_t cache;
	int i;
	
	assert(object != VM_OBJECT_NULL);
	assert(block_size >= BLOCK_CACHE_MIN_BLOCK_SIZE);
	assert(block_size <= BLOCK_CACHE_MAX_BLOCK_SIZE);
	assert((block_size & (block_size - 1)) == 0); /* Power of 2 */
	
	cache = (block_cache_t)kmem_cache_alloc(&block_cache_cache);
	if (cache == NULL)
		return NULL;
	
	/* Initialize cache structure */
	cache->block_size = block_size;
	cache->max_blocks = BLOCK_CACHE_DEFAULT_BLOCKS;
	cache->total_blocks = 0;
	cache->hits = 0;
	cache->misses = 0;
	cache->reads = 0;
	cache->writes = 0;
	cache->object = object;
	
	/* Initialize hash table */
	for (i = 0; i < BLOCK_CACHE_HASH_SIZE; i++) {
		queue_init(&cache->hash_buckets[i]);
	}
	
	/* Initialize LRU queue and block list */
	queue_init(&cache->lru_queue);
	queue_init(&cache->block_list);
	
	/* Initialize lock */
	simple_lock_init(&cache->lock);
	
	return cache;
}

/*
 * Destroy a block cache and free all associated resources
 */
void
block_cache_destroy(block_cache_t cache)
{
	block_cache_entry_t entry;
	
	assert(cache != NULL);
	
	simple_lock(&cache->lock);
	
	/* Free all cached blocks */
	entry = (block_cache_entry_t)queue_first(&cache->block_list);
	while (!queue_end(&cache->block_list, (queue_entry_t)entry)) {
		block_cache_entry_t next_entry = (block_cache_entry_t)queue_next(&entry->object_link);
		
		queue_remove(&cache->block_list, entry, 
			    block_cache_entry_t, object_link);
		queue_remove(&cache->lru_queue, entry,
			    block_cache_entry_t, lru_link);
		queue_remove(&cache->hash_buckets[block_cache_hash(entry->block_offset)],
			    entry, block_cache_entry_t, hash_link);
		
		/* Free associated pages if any */
		if (entry->pages != NULL) {
			kfree((vm_offset_t)entry->pages, entry->page_count * sizeof(vm_page_t));
		}
		
		kmem_cache_free(&block_cache_entry_cache, (vm_offset_t)entry);
		entry = next_entry;
	}
	
	simple_unlock(&cache->lock);
	kmem_cache_free(&block_cache_cache, (vm_offset_t)cache);
}

/*
 * Find a block cache entry for the given offset
 */
static block_cache_entry_t
block_cache_lookup(block_cache_t cache, vm_offset_t block_offset)
{
	block_cache_entry_t entry;
	unsigned int hash = block_cache_hash(block_offset);
	
	assert(simple_lock_taken(&cache->lock));
	
	queue_iterate(&cache->hash_buckets[hash], entry, 
		     block_cache_entry_t, hash_link) {
		if (entry->block_offset == block_offset) {
			/* Update access time for LRU - use simple counter */
			entry->access_time++;
			entry->access_frequency++;
			
			/* Move to front of LRU queue */
			queue_remove(&cache->lru_queue, entry,
				    block_cache_entry_t, lru_link);
			queue_enter_first(&cache->lru_queue, entry,
					  block_cache_entry_t, lru_link);
			
			return entry;
		}
	}
	
	return NULL;
}

/*
 * Allocate a new block cache entry
 */
static block_cache_entry_t
block_cache_entry_alloc(block_cache_t cache, vm_offset_t block_offset)
{
	block_cache_entry_t entry;
	unsigned int hash;
	unsigned int pages_needed;
	
	assert(simple_lock_taken(&cache->lock));
	
	/* Check if we need to evict an entry first */
	if (cache->total_blocks >= cache->max_blocks) {
		/* Evict least recently used clean block */
		block_cache_entry_t lru_entry;
		
		/* Find a clean block to evict from the end of LRU queue */
		/* Simple iteration from end using queue_last and queue_prev */
		if (!queue_empty(&cache->lru_queue)) {
			lru_entry = (block_cache_entry_t)queue_last(&cache->lru_queue);
			
			while (!queue_end(&cache->lru_queue, (queue_entry_t)lru_entry)) {
				if (lru_entry->state == BLOCK_CACHE_CLEAN && 
				    lru_entry->ref_count == 0) {
					/* Remove from all queues and hash table */
					queue_remove(&cache->lru_queue, lru_entry,
						    block_cache_entry_t, lru_link);
					queue_remove(&cache->block_list, lru_entry,
						    block_cache_entry_t, object_link);
					hash = block_cache_hash(lru_entry->block_offset);
					queue_remove(&cache->hash_buckets[hash], lru_entry,
						    block_cache_entry_t, hash_link);
					
					/* Free associated pages */
					if (lru_entry->pages != NULL) {
						kfree((vm_offset_t)lru_entry->pages, 
						     lru_entry->page_count * sizeof(vm_page_t));
					}
					
					kmem_cache_free(&block_cache_entry_cache, (vm_offset_t)lru_entry);
					cache->total_blocks--;
					break;
				}
				lru_entry = (block_cache_entry_t)queue_prev(&lru_entry->lru_link);
			}
		}
		
		/* If still at capacity, allocation will fail */
		if (cache->total_blocks >= cache->max_blocks) {
			return NULL;
		}
	}
	
	/* Allocate new entry */
	entry = (block_cache_entry_t)kmem_cache_alloc(&block_cache_entry_cache);
	if (entry == NULL)
		return NULL;
	
	/* Initialize entry */
	memset(entry, 0, sizeof(*entry));
	entry->object = cache->object;
	entry->block_offset = block_offset;
	entry->block_size = cache->block_size;
	entry->state = BLOCK_CACHE_CLEAN;
	entry->ref_count = 0;
	entry->access_time = 1;  /* Simple counter instead of ticks */
	entry->access_frequency = 1;
	
	/* Calculate number of pages needed for this block */
	pages_needed = (cache->block_size + PAGE_SIZE - 1) / PAGE_SIZE;
	entry->page_count = pages_needed;
	
		/* Allocate page array */
		if (pages_needed > 0) {
			entry->pages = (vm_page_t *)kalloc(pages_needed * sizeof(vm_page_t));
			if (entry->pages == NULL) {
				kmem_cache_free(&block_cache_entry_cache, (vm_offset_t)entry);
				return NULL;
			}
		memset(entry->pages, 0, pages_needed * sizeof(vm_page_t));
	}
	
	simple_lock_init(&entry->lock);
	
	/* Add to hash table */
	hash = block_cache_hash(block_offset);
	queue_enter(&cache->hash_buckets[hash], entry, 
		   block_cache_entry_t, hash_link);
	
	/* Add to LRU queue (at front, since it's most recently accessed) */
	queue_enter_first(&cache->lru_queue, entry, 
			 block_cache_entry_t, lru_link);
	
	/* Add to object block list */
	queue_enter(&cache->block_list, entry, 
		   block_cache_entry_t, object_link);
	
	cache->total_blocks++;
	
	return entry;
}

/*
 * Read data from the block cache
 */
kern_return_t
block_cache_read(block_cache_t cache, vm_offset_t offset, 
		vm_size_t size, void **data)
{
	vm_offset_t block_offset;
	block_cache_entry_t entry;
	kern_return_t result = KERN_SUCCESS;
	
	assert(cache != NULL);
	assert(data != NULL);
	
	/* Align to block boundaries */
	block_offset = offset & ~(cache->block_size - 1);
	
	simple_lock(&cache->lock);
	
	/* Look up block in cache */
	entry = block_cache_lookup(cache, block_offset);
	if (entry != NULL) {
		/* Cache hit */
		cache->hits++;
		simple_lock(&block_cache_stats_lock);
		total_cache_hits++;
		simple_unlock(&block_cache_stats_lock);
		
		simple_lock(&entry->lock);
		entry->ref_count++;
		simple_unlock(&entry->lock);
		
		simple_unlock(&cache->lock);
		
		/* TODO: Return data from cached block */
		/* For now, just indicate success */
		*data = NULL;
		
		simple_lock(&entry->lock);
		entry->ref_count--;
		simple_unlock(&entry->lock);
		
	} else {
		/* Cache miss - need to allocate and read from storage */
		cache->misses++;
		simple_lock(&block_cache_stats_lock);
		total_cache_misses++;
		simple_unlock(&block_cache_stats_lock);
		
		entry = block_cache_entry_alloc(cache, block_offset);
		if (entry == NULL) {
			simple_unlock(&cache->lock);
			return KERN_RESOURCE_SHORTAGE;
		}
		
		simple_lock(&entry->lock);
		entry->ref_count++;
		entry->state = BLOCK_CACHE_READING;
		simple_unlock(&entry->lock);
		
		simple_unlock(&cache->lock);
		
		/* TODO: Initiate read from storage device */
		/* This would integrate with the existing page-based I/O system */
		
		/* For now, simulate successful read */
		simple_lock(&entry->lock);
		entry->state = BLOCK_CACHE_CLEAN;
		entry->ref_count--;
		simple_unlock(&entry->lock);
		
		*data = NULL;
	}
	
	cache->reads++;
	return result;
}

/*
 * Write data to the block cache
 */
kern_return_t
block_cache_write(block_cache_t cache, vm_offset_t offset,
		 vm_size_t size, void *data)
{
	vm_offset_t block_offset;
	block_cache_entry_t entry;
	kern_return_t result = KERN_SUCCESS;
	
	assert(cache != NULL);
	
	/* Align to block boundaries */
	block_offset = offset & ~(cache->block_size - 1);
	
	simple_lock(&cache->lock);
	
	/* Look up or create block entry */
	entry = block_cache_lookup(cache, block_offset);
	if (entry == NULL) {
		entry = block_cache_entry_alloc(cache, block_offset);
		if (entry == NULL) {
			simple_unlock(&cache->lock);
			return KERN_RESOURCE_SHORTAGE;
		}
	}
	
	simple_lock(&entry->lock);
	entry->ref_count++;
	entry->state = BLOCK_CACHE_DIRTY;
	simple_unlock(&entry->lock);
	
	simple_unlock(&cache->lock);
	
	/* TODO: Update block data */
	/* This would modify the cached block data and mark pages as dirty */
	
	simple_lock(&entry->lock);
	entry->ref_count--;
	simple_unlock(&entry->lock);
	
	cache->writes++;
	return result;
}

/*
 * Flush dirty blocks to storage
 */
kern_return_t
block_cache_flush(block_cache_t cache, vm_offset_t offset, vm_size_t size)
{
	block_cache_entry_t entry;
	vm_offset_t end_offset = offset + size;
	
	assert(cache != NULL);
	
	simple_lock(&cache->lock);
	
	/* Iterate through all blocks that intersect the flush range */
	queue_iterate(&cache->block_list, entry, 
		     block_cache_entry_t, object_link) {
		vm_offset_t entry_end = entry->block_offset + entry->block_size;
		
		/* Check if block intersects with flush range */
		if (entry->block_offset < end_offset && entry_end > offset) {
			simple_lock(&entry->lock);
			if (entry->state == BLOCK_CACHE_DIRTY) {
				entry->state = BLOCK_CACHE_WRITING;
				/* TODO: Initiate write to storage */
				entry->state = BLOCK_CACHE_CLEAN;
			}
			simple_unlock(&entry->lock);
		}
	}
	
	simple_unlock(&cache->lock);
	return KERN_SUCCESS;
}

/*
 * Get cache statistics
 */
void
block_cache_get_stats(block_cache_t cache, unsigned int *hits,
		     unsigned int *misses, unsigned int *total_blocks)
{
	assert(cache != NULL);
	
	simple_lock(&cache->lock);
	if (hits) *hits = cache->hits;
	if (misses) *misses = cache->misses;
	if (total_blocks) *total_blocks = cache->total_blocks;
	simple_unlock(&cache->lock);
}