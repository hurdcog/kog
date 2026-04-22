/*
 * Mach 5 Research: Enhanced Capability System Prototype
 * 
 * This header defines an enhanced capability system that extends GNU Mach's
 * port-based capabilities with fine-grained permissions, delegation controls,
 * and security auditing features for next-generation access control.
 */

#ifndef _MACH5_ENHANCED_CAPABILITIES_H_
#define _MACH5_ENHANCED_CAPABILITIES_H_

#include <mach/mach.h>
#include <mach/port.h>
#include <sys/time.h>

/*
 * Enhanced Capability Configuration
 */
#define ECAP_MAX_PERMISSIONS    64      /* Maximum permission bits */
#define ECAP_MAX_DELEGATION_DEPTH 8     /* Maximum delegation chain depth */
#define ECAP_MAX_AUDIT_ENTRIES  1000    /* Maximum audit log entries */

/*
 * Fine-Grained Permission Flags
 */
typedef enum {
    /* Basic IPC Permissions */
    ECAP_PERM_SEND          = (1 << 0),     /* Send messages */
    ECAP_PERM_RECEIVE       = (1 << 1),     /* Receive messages */
    ECAP_PERM_SEND_ONCE     = (1 << 2),     /* Send-once right */
    
    /* Extended IPC Permissions */
    ECAP_PERM_SEND_TIMEOUT  = (1 << 3),     /* Send with timeout */
    ECAP_PERM_RECEIVE_TIMEOUT = (1 << 4),   /* Receive with timeout */
    ECAP_PERM_MSG_PRIORITY  = (1 << 5),     /* Set message priority */
    
    /* Memory Permissions */
    ECAP_PERM_VM_READ       = (1 << 8),     /* Read memory */
    ECAP_PERM_VM_WRITE      = (1 << 9),     /* Write memory */
    ECAP_PERM_VM_EXECUTE    = (1 << 10),    /* Execute memory */
    ECAP_PERM_VM_MAP        = (1 << 11),    /* Map memory */
    ECAP_PERM_VM_UNMAP      = (1 << 12),    /* Unmap memory */
    
    /* Task Management Permissions */
    ECAP_PERM_TASK_CREATE   = (1 << 16),    /* Create tasks */
    ECAP_PERM_TASK_DESTROY  = (1 << 17),    /* Destroy tasks */
    ECAP_PERM_TASK_SUSPEND  = (1 << 18),    /* Suspend tasks */
    ECAP_PERM_TASK_RESUME   = (1 << 19),    /* Resume tasks */
    ECAP_PERM_TASK_INFO     = (1 << 20),    /* Get task information */
    
    /* Thread Management Permissions */
    ECAP_PERM_THREAD_CREATE = (1 << 24),    /* Create threads */
    ECAP_PERM_THREAD_DESTROY = (1 << 25),   /* Destroy threads */
    ECAP_PERM_THREAD_CONTROL = (1 << 26),   /* Control thread execution */
    
    /* Capability Management Permissions */
    ECAP_PERM_CAP_DELEGATE  = (1 << 32),    /* Delegate capabilities */
    ECAP_PERM_CAP_REVOKE    = (1 << 33),    /* Revoke capabilities */
    ECAP_PERM_CAP_AUDIT     = (1 << 34),    /* Audit capability usage */
    
    /* Administrative Permissions */
    ECAP_PERM_ADMIN_ALL     = (1ULL << 63)  /* Administrative override */
} ecap_permission_t;

/*
 * Capability Delegation Control
 */
typedef struct {
    unsigned int    max_depth;          /* Maximum delegation depth */
    unsigned int    current_depth;      /* Current depth in chain */
    struct timeval  expiration_time;    /* When capability expires */
    task_t          delegator_task;     /* Task that delegated this capability */
    uint64_t        delegation_id;      /* Unique delegation identifier */
} ecap_delegation_info_t;

/*
 * Enhanced Capability Structure
 */
typedef struct {
    /* Core Capability Data */
    mach_port_t         port;               /* Associated port */
    uint64_t            permissions;        /* Permission bit mask */
    task_t              owner_task;         /* Owning task */
    
    /* Delegation Control */
    ecap_delegation_info_t delegation;      /* Delegation information */
    
    /* Security Context */
    uint32_t            security_label;     /* Security label/level */
    unsigned int        reference_count;    /* Reference count */
    boolean_t           is_transferable;    /* Can be transferred */
    
    /* Audit Information */
    struct timeval      creation_time;      /* When capability was created */
    struct timeval      last_use_time;      /* Last time capability was used */
    unsigned long       use_count;          /* Number of times used */
    
    /* Capability Identifier */
    uint64_t            capability_id;      /* Unique capability ID */
} ecap_capability_t;

/*
 * Capability Audit Entry
 */
typedef struct {
    uint64_t            capability_id;      /* Capability used */
    task_t              task;               /* Task that used capability */
    ecap_permission_t   permission_used;    /* Specific permission used */
    struct timeval      timestamp;          /* When operation occurred */
    kern_return_t       result;             /* Operation result */
    char                operation[64];      /* Description of operation */
} ecap_audit_entry_t;

/*
 * Capability System Context
 */
typedef struct {
    /* Capability Storage */
    ecap_capability_t   *capabilities;      /* Array of capabilities */
    unsigned int        capability_count;   /* Number of capabilities */
    unsigned int        max_capabilities;   /* Maximum capabilities */
    
    /* Audit System */
    ecap_audit_entry_t  *audit_log;         /* Audit log entries */
    unsigned int        audit_count;        /* Number of audit entries */
    unsigned int        audit_max;          /* Maximum audit entries */
    
    /* System Configuration */
    boolean_t           audit_enabled;      /* Whether auditing is enabled */
    boolean_t           strict_delegation;  /* Strict delegation checking */
    unsigned int        default_expiration; /* Default expiration time (seconds) */
} ecap_system_context_t;

/*
 * Function Prototypes
 */

/* System Initialization */
kern_return_t ecap_system_init(ecap_system_context_t *context);
kern_return_t ecap_system_cleanup(ecap_system_context_t *context);

/* Capability Creation and Management */
kern_return_t ecap_create_capability(ecap_system_context_t *context,
                                    mach_port_t port,
                                    uint64_t permissions,
                                    task_t owner_task,
                                    ecap_capability_t **capability);

kern_return_t ecap_destroy_capability(ecap_system_context_t *context,
                                     uint64_t capability_id);

kern_return_t ecap_find_capability(ecap_system_context_t *context,
                                  uint64_t capability_id,
                                  ecap_capability_t **capability);

/* Permission Checking */
kern_return_t ecap_check_permission(ecap_system_context_t *context,
                                   uint64_t capability_id,
                                   ecap_permission_t permission,
                                   task_t requesting_task);

boolean_t ecap_has_permission(const ecap_capability_t *capability,
                             ecap_permission_t permission);

/* Capability Delegation */
kern_return_t ecap_delegate_capability(ecap_system_context_t *context,
                                      uint64_t source_capability_id,
                                      task_t target_task,
                                      uint64_t restricted_permissions,
                                      unsigned int expiration_seconds,
                                      uint64_t *new_capability_id);

kern_return_t ecap_revoke_delegation(ecap_system_context_t *context,
                                    uint64_t capability_id,
                                    boolean_t recursive);

kern_return_t ecap_check_delegation_validity(const ecap_capability_t *capability);

/* Capability Transfer */
kern_return_t ecap_transfer_capability(ecap_system_context_t *context,
                                      uint64_t capability_id,
                                      task_t source_task,
                                      task_t dest_task);

/* Audit System */
kern_return_t ecap_audit_operation(ecap_system_context_t *context,
                                  uint64_t capability_id,
                                  task_t task,
                                  ecap_permission_t permission,
                                  const char *operation,
                                  kern_return_t result);

kern_return_t ecap_get_audit_log(ecap_system_context_t *context,
                                task_t task,
                                ecap_audit_entry_t **entries,
                                unsigned int *count);

void ecap_print_audit_entry(const ecap_audit_entry_t *entry);

/* Security Policy Enforcement */
kern_return_t ecap_set_security_policy(ecap_system_context_t *context,
                                      boolean_t strict_mode,
                                      boolean_t audit_all,
                                      unsigned int default_expiration);

kern_return_t ecap_enforce_security_label(ecap_system_context_t *context,
                                         task_t task,
                                         uint32_t required_label);

/* Capability Introspection */
kern_return_t ecap_list_capabilities(ecap_system_context_t *context,
                                    task_t task,
                                    uint64_t **capability_ids,
                                    unsigned int *count);

kern_return_t ecap_get_capability_info(ecap_system_context_t *context,
                                      uint64_t capability_id,
                                      ecap_capability_t *info);

void ecap_print_capability(const ecap_capability_t *capability);

/* Performance and Statistics */
typedef struct {
    unsigned long   total_checks;          /* Total permission checks */
    unsigned long   successful_checks;     /* Successful checks */
    unsigned long   failed_checks;         /* Failed checks */
    unsigned long   delegations_created;   /* Delegations created */
    unsigned long   delegations_revoked;   /* Delegations revoked */
    double          avg_check_time_us;     /* Average check time */
} ecap_statistics_t;

kern_return_t ecap_get_statistics(ecap_system_context_t *context,
                                 ecap_statistics_t *stats);

void ecap_print_statistics(const ecap_statistics_t *stats);

/*
 * Advanced Capability Features
 */

/* Capability Composition */
kern_return_t ecap_compose_capabilities(ecap_system_context_t *context,
                                       uint64_t *capability_ids,
                                       unsigned int count,
                                       uint64_t *composed_capability_id);

/* Conditional Capabilities */
typedef boolean_t (*ecap_condition_func_t)(task_t task, void *context);

kern_return_t ecap_create_conditional_capability(ecap_system_context_t *context,
                                                 mach_port_t port,
                                                 uint64_t permissions,
                                                 ecap_condition_func_t condition,
                                                 void *condition_context,
                                                 uint64_t *capability_id);

/* Capability Templates */
typedef struct {
    char            name[64];               /* Template name */
    uint64_t        default_permissions;    /* Default permission set */
    unsigned int    default_expiration;     /* Default expiration time */
    boolean_t       transferable;           /* Whether transferable */
} ecap_template_t;

kern_return_t ecap_create_template(ecap_system_context_t *context,
                                  const char *name,
                                  uint64_t permissions,
                                  ecap_template_t **template);

kern_return_t ecap_create_from_template(ecap_system_context_t *context,
                                       const ecap_template_t *template,
                                       mach_port_t port,
                                       task_t owner_task,
                                       uint64_t *capability_id);

/*
 * Research and Evaluation Functions
 */

/* Formal Verification Support */
kern_return_t ecap_generate_formal_model(ecap_system_context_t *context,
                                        const char *model_file);

kern_return_t ecap_verify_security_properties(ecap_system_context_t *context,
                                             boolean_t *properties_verified);

/* Performance Analysis */
kern_return_t ecap_benchmark_permission_checks(ecap_system_context_t *context,
                                              unsigned int iterations,
                                              double *avg_latency_us);

/* Compatibility Analysis */
kern_return_t ecap_analyze_compatibility(ecap_system_context_t *context,
                                        mach_port_t traditional_port,
                                        boolean_t *is_compatible);

#endif /* _MACH5_ENHANCED_CAPABILITIES_H_ */