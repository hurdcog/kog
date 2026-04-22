/*
 * Header file for printf type functions.
 * Copyright (C) 2006, 2007 Free Software Foundation.
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
/*
 *     String handling functions.
 *
 */

#ifndef _MACH_SA_SYS_PRINTF_H_
#define _MACH_SA_SYS_PRINTF_H_

#include <sys/types.h>
#include <stdarg.h>
#include <mach/boolean.h>
#include <mach/time_value.h>

/* Console timestamp configuration */
extern boolean_t console_timestamps_enabled;
extern void console_timestamp_init(void);
extern void console_print_timestamp(void);

/* Timestamp format options */
typedef enum {
    TIMESTAMP_FORMAT_RELATIVE,   /* [seconds.milliseconds] from boot */
    TIMESTAMP_FORMAT_UPTIME,     /* [uptime] absolute */
    TIMESTAMP_FORMAT_SIMPLE,     /* [sss.mmm] simple format */
    TIMESTAMP_FORMAT_PRECISE     /* [sss.mmm.uuu] with microseconds */
} console_timestamp_format_t;

/* Configuration functions for timestamp behavior */
extern void console_timestamp_enable(boolean_t enable);
extern boolean_t console_timestamp_is_enabled(void);
extern void console_timestamp_set_format(console_timestamp_format_t format);
extern console_timestamp_format_t console_timestamp_get_format(void);
extern void console_timestamp_get_boot_time(time_value64_t *boot_time);

extern void _doprnt (const char *fmt,
		     va_list argp, 
		     void (*putc)(char, vm_offset_t), 
		     int radix, 
		     vm_offset_t putc_arg);

extern void printnum (unsigned long long u, int base,
                      void (*putc)(char, vm_offset_t),
                      vm_offset_t putc_arg);

extern int sprintf (char *buf, const char *fmt, ...)
	__attribute__ ((format (printf, 2, 3)));
extern int snprintf (char *buf, size_t size, const char *fmt, ...)
	__attribute__ ((format (printf, 3, 4)));
extern int vsnprintf (char *buf, size_t size, const char *fmt, va_list args)
	__attribute__ ((format (printf, 3, 0)));


extern int printf (const char *fmt, ...)
	__attribute__ ((format (printf, 1, 2)));

#define printf_once(fmt, ...)			\
	MACRO_BEGIN				\
	static int __once = 0;			\
	if (!__once) {				\
		printf(fmt, ##__VA_ARGS__);	\
		__once = 1;			\
	}					\
	MACRO_END

extern int indent;
extern void iprintf (const char *fmt, ...);

extern int vprintf(const char *fmt, va_list listp);

extern void safe_gets (char *str, int maxlen);

/* LTTng-style tracing integration */
#ifdef CONFIG_MACH_TRACING
#include <mach/lttng.h>

/* Enhanced printf with automatic tracing */
#define printf_trace(level, fmt, ...) \
	do { \
		printf(fmt, ##__VA_ARGS__); \
		if (mach_tracing_enabled) \
			mach_trace_event(MACH_TRACE_KERN, level, \
			                 MACH_TRACE_EVENT_KERN_BASE + 10, \
			                 fmt, ##__VA_ARGS__); \
	} while (0)

/* Tracing-aware panic that logs to trace buffer */
#define panic_trace(fmt, ...) \
	do { \
		if (mach_tracing_enabled) \
			mach_trace_event(MACH_TRACE_KERN, MACH_TRACE_LEVEL_EMERG, \
			                 MACH_TRACE_EVENT_KERN_BASE + 2, \
			                 fmt, ##__VA_ARGS__); \
		panic(fmt, ##__VA_ARGS__); \
	} while (0)

/* Function declarations for tracing system */
extern void mach_trace_print_stats(void);
extern void mach_trace_early_init(void);

#else /* !CONFIG_MACH_TRACING */

#define printf_trace(level, fmt, ...) printf(fmt, ##__VA_ARGS__)
#define panic_trace(fmt, ...) panic(fmt, ##__VA_ARGS__)

#endif /* CONFIG_MACH_TRACING */

#endif /* _MACH_SA_SYS_PRINTF_H_ */

