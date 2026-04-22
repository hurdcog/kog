/*
 * Copyright (c) 2024 GNU Mach project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * Memory compression implementation for GNU Mach
 * Provides compressed memory storage to extend effective memory capacity
 */

#include <string.h>
#include <kern/assert.h>
#include <kern/debug.h>
#include <kern/kalloc.h>
#include <kern/slab.h>
#include <kern/lock.h>
#include <kern/queue.h>
#include <mach/vm_param.h>
#include <vm/vm_compress.h>
#include <vm/vm_page.h>
#include <vm/vm_kern.h>

/* Global compression statistics */
static unsigned int total_compressed_pages = 0;
static unsigned int total_decompressed_pages = 0;
static unsigned int compression_ratio_sum = 0;  /* For average calculation */

/* Slab cache for compressed page entries */
static struct kmem_cache compressed_page_cache;

/* Global lock for compression statistics */
static simple_lock_data_t compress_stats_lock;

/* Hash table for compressed page lookup */
#define VM_COMPRESS_HASH_SIZE 512
static queue_head_t compress_hash_buckets[VM_COMPRESS_HASH_SIZE];
static simple_lock_data_t compress_hash_locks[VM_COMPRESS_HASH_SIZE];

/*
 * Simple hash function for compressed page lookup
 */
static inline unsigned int
vm_compress_hash(vm_object_t object, vm_offset_t offset)
{
	unsigned long hash = (unsigned long)object ^ (offset >> PAGE_SHIFT);
	return hash % VM_COMPRESS_HASH_SIZE;
}

/*
 * Simple compression algorithm (RLE-like for demonstration)
 * In a production system, this would use zlib, lz4, or similar
 */
static vm_size_t
simple_compress(const void *src, vm_size_t src_len, void *dst, vm_size_t dst_len)
{
	const unsigned char *input = (const unsigned char *)src;
	unsigned char *output = (unsigned char *)dst;
	vm_size_t in_pos = 0, out_pos = 0;
	
	if (dst_len < src_len / 2) {
		/* Not enough space for worst-case compression */
		return 0;
	}
	
	while (in_pos < src_len && out_pos < dst_len - 2) {
		unsigned char current = input[in_pos];
		vm_size_t run_length = 1;
		
		/* Count consecutive identical bytes */
		while (in_pos + run_length < src_len && 
		       input[in_pos + run_length] == current &&
		       run_length < 255) {
			run_length++;
		}
		
		if (run_length > 3 || current == 0) {
			/* Use RLE encoding */
			output[out_pos++] = 0xFF;  /* Escape marker */
			output[out_pos++] = (unsigned char)run_length;
			output[out_pos++] = current;
		} else {
			/* Direct copy */
			for (vm_size_t i = 0; i < run_length && out_pos < dst_len; i++) {
				output[out_pos++] = current;
			}
		}
		
		in_pos += run_length;
	}
	
	return (out_pos < dst_len) ? out_pos : 0;
}

/*
 * Simple decompression algorithm
 */
static boolean_t
simple_decompress(const void *src, vm_size_t src_len, void *dst, vm_size_t dst_len)
{
	const unsigned char *input = (const unsigned char *)src;
	unsigned char *output = (unsigned char *)dst;
	vm_size_t in_pos = 0, out_pos = 0;
	
	while (in_pos < src_len && out_pos < dst_len) {
		if (input[in_pos] == 0xFF && in_pos + 2 < src_len) {
			/* RLE encoded run */
			unsigned char run_length = input[in_pos + 1];
			unsigned char value = input[in_pos + 2];
			
			if (out_pos + run_length > dst_len) {
				return FALSE;  /* Output buffer overflow */
			}
			
			for (unsigned char i = 0; i < run_length; i++) {
				output[out_pos++] = value;
			}
			in_pos += 3;
		} else {
			/* Direct copy */
			output[out_pos++] = input[in_pos++];
		}
	}
	
	return (out_pos == dst_len);
}

/*
 * Initialize the memory compression subsystem
 */
void
vm_compress_init(void)
{
	int i;
	
	kmem_cache_init(&compressed_page_cache, "compressed_page",
			sizeof(struct vm_compressed_page), 0, NULL, 0);
	simple_lock_init(&compress_stats_lock);
	
	/* Initialize hash table */
	for (i = 0; i < VM_COMPRESS_HASH_SIZE; i++) {
		queue_init(&compress_hash_buckets[i]);
		simple_lock_init(&compress_hash_locks[i]);
	}
}

/*
 * Compress a page and store it
 */
kern_return_t
vm_page_compress(vm_object_t object, vm_offset_t offset, vm_page_t page)
{
	struct vm_compressed_page *comp_page;
	unsigned int hash;
	vm_size_t compressed_size;
	void *page_data, *compressed_data;
	
	assert(object != VM_OBJECT_NULL);
	assert(page != VM_PAGE_NULL);
	
	/* Allocate compressed page structure */
	comp_page = (struct vm_compressed_page *)kmem_cache_alloc(&compressed_page_cache);
	if (comp_page == NULL) {
		return KERN_RESOURCE_SHORTAGE;
	}
	
	/* Allocate temporary compression buffer */
	compressed_data = (void *)kalloc(PAGE_SIZE);
	if (compressed_data == NULL) {
		kmem_cache_free(&compressed_page_cache, (vm_offset_t)comp_page);
		return KERN_RESOURCE_SHORTAGE;
	}
	
	/* Get page data */
	page_data = (void *)phystokv(page->phys_addr);
	
	/* Compress the page */
	compressed_size = simple_compress(page_data, PAGE_SIZE, 
					 compressed_data, PAGE_SIZE);
	
	if (compressed_size == 0 || compressed_size >= PAGE_SIZE) {
		/* Compression failed or not beneficial */
		kfree((vm_offset_t)compressed_data, PAGE_SIZE);
		kmem_cache_free(&compressed_page_cache, (vm_offset_t)comp_page);
		return KERN_FAILURE;
	}
	
	/* Allocate final compressed data storage */
	comp_page->compressed_data = (void *)kalloc(compressed_size);
	if (comp_page->compressed_data == NULL) {
		kfree((vm_offset_t)compressed_data, PAGE_SIZE);
		kmem_cache_free(&compressed_page_cache, (vm_offset_t)comp_page);
		return KERN_RESOURCE_SHORTAGE;
	}
	
	/* Copy compressed data */
	memcpy(comp_page->compressed_data, compressed_data, compressed_size);
	kfree((vm_offset_t)compressed_data, PAGE_SIZE);
	
	/* Initialize compressed page structure */
	comp_page->object = object;
	comp_page->offset = offset;
	comp_page->compressed_size = compressed_size;
	comp_page->access_time = 1;
	
	/* Add to hash table */
	hash = vm_compress_hash(object, offset);
	simple_lock(&compress_hash_locks[hash]);
	queue_enter(&compress_hash_buckets[hash], comp_page, 
		   struct vm_compressed_page *, hash_link);
	simple_unlock(&compress_hash_locks[hash]);
	
	/* Update statistics */
	simple_lock(&compress_stats_lock);
	total_compressed_pages++;
	compression_ratio_sum += (PAGE_SIZE * 100) / compressed_size;
	simple_unlock(&compress_stats_lock);
	
	return KERN_SUCCESS;
}

/*
 * Find and decompress a page
 */
kern_return_t
vm_page_decompress(vm_object_t object, vm_offset_t offset, vm_page_t page)
{
	struct vm_compressed_page *comp_page;
	unsigned int hash;
	void *page_data;
	boolean_t found = FALSE;
	
	assert(object != VM_OBJECT_NULL);
	assert(page != VM_PAGE_NULL);
	
	/* Look up in hash table */
	hash = vm_compress_hash(object, offset);
	simple_lock(&compress_hash_locks[hash]);
	
	queue_iterate(&compress_hash_buckets[hash], comp_page,
		     struct vm_compressed_page *, hash_link) {
		if (comp_page->object == object && comp_page->offset == offset) {
			/* Remove from hash table */
			queue_remove(&compress_hash_buckets[hash], comp_page,
				    struct vm_compressed_page *, hash_link);
			found = TRUE;
			break;
		}
	}
	
	simple_unlock(&compress_hash_locks[hash]);
	
	if (!found) {
		return KERN_FAILURE;
	}
	
	/* Get page data */
	page_data = (void *)phystokv(page->phys_addr);
	
	/* Decompress the page */
	if (!simple_decompress(comp_page->compressed_data, comp_page->compressed_size,
			      page_data, PAGE_SIZE)) {
		/* Decompression failed - should not happen */
		panic("vm_page_decompress: decompression failed");
	}
	
	/* Free compressed data */
	kfree((vm_offset_t)comp_page->compressed_data, comp_page->compressed_size);
	kmem_cache_free(&compressed_page_cache, (vm_offset_t)comp_page);
	
	/* Update statistics */
	simple_lock(&compress_stats_lock);
	total_decompressed_pages++;
	simple_unlock(&compress_stats_lock);
	
	return KERN_SUCCESS;
}

/*
 * Remove a compressed page without decompressing
 */
kern_return_t
vm_page_compress_remove(vm_object_t object, vm_offset_t offset)
{
	struct vm_compressed_page *comp_page;
	unsigned int hash;
	boolean_t found = FALSE;
	
	/* Look up in hash table */
	hash = vm_compress_hash(object, offset);
	simple_lock(&compress_hash_locks[hash]);
	
	queue_iterate(&compress_hash_buckets[hash], comp_page,
		     struct vm_compressed_page *, hash_link) {
		if (comp_page->object == object && comp_page->offset == offset) {
			/* Remove from hash table */
			queue_remove(&compress_hash_buckets[hash], comp_page,
				    struct vm_compressed_page *, hash_link);
			found = TRUE;
			break;
		}
	}
	
	simple_unlock(&compress_hash_locks[hash]);
	
	if (!found) {
		return KERN_FAILURE;
	}
	
	/* Free compressed data */
	kfree((vm_offset_t)comp_page->compressed_data, comp_page->compressed_size);
	kmem_cache_free(&compressed_page_cache, (vm_offset_t)comp_page);
	
	return KERN_SUCCESS;
}

/*
 * Get compression statistics
 */
void
vm_compress_get_stats(struct vm_compress_stats *stats)
{
	simple_lock(&compress_stats_lock);
	stats->compressed_pages = total_compressed_pages;
	stats->decompressed_pages = total_decompressed_pages;
	stats->average_compression_ratio = 
		(total_compressed_pages > 0) ? 
		(compression_ratio_sum / total_compressed_pages) : 0;
	simple_unlock(&compress_stats_lock);
}