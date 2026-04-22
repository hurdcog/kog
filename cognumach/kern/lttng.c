/*
 * LTTng-style tracing implementation for GNU Mach
 * Copyright (C) 2024 Free Software Foundation, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <mach/lttng.h>
#include <kern/printf.h>
#include <kern/cpu_number.h>
#include <kern/thread.h>
#include <kern/task.h>
#include <kern/lock.h>
#include <stdarg.h>
#include <string.h>

/* Internal trace buffer implementation */
struct mach_trace_buffer_impl {
	struct mach_trace_event events[MACH_TRACE_BUF_SIZE];
	volatile uint32_t write_pos;
	volatile uint32_t read_pos;
	simple_lock_irq_data_t lock;
	boolean_t enabled;
	uint32_t dropped_events;
} __attribute__((aligned(64)));

static struct mach_trace_buffer_impl mach_trace_buf_impl;
struct mach_trace_buffer_impl *mach_trace_buf_ptr = &mach_trace_buf_impl;

/* Global tracing enabled flag */
boolean_t mach_tracing_enabled = FALSE;

/* Statistics */
static uint32_t total_events_generated = 0;
static uint32_t total_events_dropped = 0;

/*
 * Initialize the Mach tracing subsystem
 */
void
mach_trace_init(void)
{
	/* Initialize the trace buffer implementation */
	memset(&mach_trace_buf_impl, 0, sizeof(mach_trace_buf_impl));
	simple_lock_init_irq(&mach_trace_buf_impl.lock);
	mach_trace_buf_impl.enabled = FALSE;
	
	/* Reset statistics */
	total_events_generated = 0;
	total_events_dropped = 0;
	
	printf("Mach tracing initialized (buffer size: %d events)\n", 
	       MACH_TRACE_BUF_SIZE);
}

/*
 * Enable or disable tracing globally
 */
void
mach_trace_enable(boolean_t enable)
{
	spl_t spl;
	
	spl = simple_lock_irq(&mach_trace_buf_impl.lock);
	
	if (enable && !mach_tracing_enabled) {
		/* Clear buffer when enabling */
		mach_trace_buf_impl.write_pos = 0;
		mach_trace_buf_impl.read_pos = 0;
		mach_trace_buf_impl.dropped_events = 0;
		mach_trace_buf_impl.enabled = TRUE;
		mach_tracing_enabled = TRUE;
		printf("Mach tracing enabled\n");
	} else if (!enable && mach_tracing_enabled) {
		mach_tracing_enabled = FALSE;
		mach_trace_buf_impl.enabled = FALSE;
		printf("Mach tracing disabled\n");
	}
	
	simple_unlock_irq(spl, &mach_trace_buf_impl.lock);
}

/*
 * Check if tracing is currently enabled
 */
boolean_t
mach_trace_is_enabled(void)
{
	return mach_tracing_enabled;
}

/*
 * Get current high-resolution timestamp
 */
static inline uint64_t
mach_trace_timestamp(void)
{
	/* Use a simple counter for now - could be enhanced later */
	static uint64_t counter = 0;
	return ++counter;
}

/*
 * Get current task ID safely
 */
static uint32_t
mach_trace_get_task_id(void)
{
	thread_t thread = current_thread();
	if (thread && thread->task)
		return (uint32_t)(uintptr_t)thread->task;
	return 0;
}

/*
 * Get current thread ID safely  
 */
static uint32_t
mach_trace_get_thread_id(void)
{
	thread_t thread = current_thread();
	if (thread)
		return (uint32_t)(uintptr_t)thread;
	return 0;
}

/*
 * Add a trace event to the buffer
 * This function must be fast and lock-free when possible
 */
void
mach_trace_event(mach_trace_category_t category, 
                 mach_trace_level_t level,
                 uint32_t event_id,
                 const char *fmt, ...)
{
	struct mach_trace_event *event;
	uint64_t timestamp;
	uint32_t pos, next_pos;
	va_list args;
	spl_t spl;
	
	/* Quick check without locking */
	if (!mach_tracing_enabled || !mach_trace_buf_impl.enabled)
		return;
		
	/* Get timestamp early to minimize latency */
	timestamp = mach_trace_timestamp();
	
	/* Increment total events counter */
	total_events_generated++;
	
	/* Lock buffer for modification */
	spl = simple_lock_irq(&mach_trace_buf_impl.lock);
	
	/* Double-check enabled status */
	if (!mach_trace_buf_impl.enabled) {
		simple_unlock_irq(spl, &mach_trace_buf_impl.lock);
		return;
	}
	
	/* Get current write position */
	pos = mach_trace_buf_impl.write_pos;
	next_pos = (pos + 1) % MACH_TRACE_BUF_SIZE;
	
	/* Check for buffer overflow */
	if (next_pos == mach_trace_buf_impl.read_pos) {
		/* Buffer full - drop event */
		mach_trace_buf_impl.dropped_events++;
		total_events_dropped++;
		simple_unlock_irq(spl, &mach_trace_buf_impl.lock);
		return;
	}
	
	/* Fill in the event */
	event = &mach_trace_buf_impl.events[pos];
	event->timestamp_hi = (uint32_t)(timestamp >> 32);
	event->timestamp_lo = (uint32_t)(timestamp & 0xFFFFFFFF);
	event->category = (uint16_t)category;
	event->level = (uint16_t)level;
	event->event_id = event_id;
	event->cpu_id = cpu_number();
	event->task_id = mach_trace_get_task_id();
	event->thread_id = mach_trace_get_thread_id();
	
	/* Format the event data */
	if (fmt) {
		va_start(args, fmt);
		vsnprintf(event->data, sizeof(event->data) - 1, fmt, args);
		va_end(args);
		event->data[sizeof(event->data) - 1] = '\0';
	} else {
		event->data[0] = '\0';
	}
	
	/* Update write position */
	mach_trace_buf_impl.write_pos = next_pos;
	
	simple_unlock_irq(spl, &mach_trace_buf_impl.lock);
}

/*
 * Print trace buffer statistics
 */
void
mach_trace_print_stats(void)
{
	uint32_t buffered_events;
	spl_t spl;
	
	spl = simple_lock_irq(&mach_trace_buf_impl.lock);
	
	/* Calculate number of buffered events */
	if (mach_trace_buf_impl.write_pos >= mach_trace_buf_impl.read_pos) {
		buffered_events = mach_trace_buf_impl.write_pos - mach_trace_buf_impl.read_pos;
	} else {
		buffered_events = MACH_TRACE_BUF_SIZE - 
		                 (mach_trace_buf_impl.read_pos - mach_trace_buf_impl.write_pos);
	}
	
	printf("Mach Tracing Statistics:\n");
	printf("  Status: %s\n", mach_tracing_enabled ? "Enabled" : "Disabled");
	printf("  Buffer size: %d events\n", MACH_TRACE_BUF_SIZE);
	printf("  Buffered events: %u\n", buffered_events);
	printf("  Events generated: %u\n", total_events_generated);
	printf("  Events dropped: %u\n", total_events_dropped);
	printf("  Buffer dropped: %u\n", mach_trace_buf_impl.dropped_events);
	
	simple_unlock_irq(spl, &mach_trace_buf_impl.lock);
}

/*
 * Define some basic trace points for key kernel subsystems
 */

/* IPC tracepoints */
MACH_TRACEPOINT_DEFINE(IPC, msg_send, MACH_TRACE_LEVEL_INFO, 
                       MACH_TRACE_EVENT_IPC_BASE + 1,
                       "IPC message send");

MACH_TRACEPOINT_DEFINE(IPC, msg_receive, MACH_TRACE_LEVEL_INFO,
                       MACH_TRACE_EVENT_IPC_BASE + 2, 
                       "IPC message receive");

MACH_TRACEPOINT_DEFINE(IPC, port_allocate, MACH_TRACE_LEVEL_DEBUG,
                       MACH_TRACE_EVENT_IPC_BASE + 3,
                       "IPC port allocate");

/* Scheduler tracepoints */
MACH_TRACEPOINT_DEFINE(SCHED, thread_switch, MACH_TRACE_LEVEL_DEBUG,
                       MACH_TRACE_EVENT_SCHED_BASE + 1,
                       "Thread context switch");

MACH_TRACEPOINT_DEFINE(SCHED, thread_create, MACH_TRACE_LEVEL_INFO,
                       MACH_TRACE_EVENT_SCHED_BASE + 2,
                       "Thread creation");

MACH_TRACEPOINT_DEFINE(SCHED, thread_terminate, MACH_TRACE_LEVEL_INFO,
                       MACH_TRACE_EVENT_SCHED_BASE + 3,
                       "Thread termination");

/* VM tracepoints */
MACH_TRACEPOINT_DEFINE(VM, page_fault, MACH_TRACE_LEVEL_DEBUG,
                       MACH_TRACE_EVENT_VM_BASE + 1,
                       "Page fault");

MACH_TRACEPOINT_DEFINE(VM, map_enter, MACH_TRACE_LEVEL_DEBUG,
                       MACH_TRACE_EVENT_VM_BASE + 2,
                       "VM map entry");

/* Kernel tracepoints */
MACH_TRACEPOINT_DEFINE(KERN, startup, MACH_TRACE_LEVEL_INFO,
                       MACH_TRACE_EVENT_KERN_BASE + 1,
                       "Kernel startup");

MACH_TRACEPOINT_DEFINE(KERN, panic, MACH_TRACE_LEVEL_EMERG,
                       MACH_TRACE_EVENT_KERN_BASE + 2,
                       "Kernel panic");

/* Debug tracepoints */
MACH_TRACEPOINT_DEFINE(DEBUG, assertion_failed, MACH_TRACE_LEVEL_ERR,
                       MACH_TRACE_EVENT_DEBUG_BASE + 1,
                       "Assertion failed");

MACH_TRACEPOINT_DEFINE(DEBUG, warning, MACH_TRACE_LEVEL_WARNING,
                       MACH_TRACE_EVENT_DEBUG_BASE + 2,
                       "Debug warning");

/*
 * Initialize tracing early in the boot process
 */
void
mach_trace_early_init(void)
{
	/* Basic initialization without printf to avoid recursion */
	memset(&mach_trace_buf_impl, 0, sizeof(mach_trace_buf_impl));
	simple_lock_init_irq(&mach_trace_buf_impl.lock);
	mach_trace_buf_impl.enabled = FALSE;
	mach_tracing_enabled = FALSE;
}