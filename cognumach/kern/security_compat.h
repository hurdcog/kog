/*
 * Simplified compatibility definitions for security framework testing
 */

#ifndef _SECURITY_COMPAT_H_
#define _SECURITY_COMPAT_H_

#include <stdint.h>
#include <stddef.h>

/* Basic types */
typedef int boolean_t;
typedef int kern_return_t;

/* Basic constants */
#define TRUE  1
#define FALSE 0
#define KERN_SUCCESS 0
#define KERN_INVALID_ARGUMENT 1
#define KERN_INVALID_ADDRESS 2
#define KERN_PROTECTION_FAILURE 3

/* Simple lock structure */
struct simple_lock {
    volatile int lock_data;
};

typedef struct simple_lock simple_lock_data_t;

#define decl_simple_lock_data(class, name) class simple_lock_data_t name

/* Time value structure */
typedef struct time_value {
    int seconds;
    int microseconds;
} time_value_t;

/* String functions */
extern int strncmp(const char *, const char *, size_t);
extern char *strncpy(char *, const char *, size_t);
extern size_t strlen(const char *);

#endif /* _SECURITY_COMPAT_H_ */