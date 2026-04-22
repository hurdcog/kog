/**
 * @file cogkapi_hurdcog.h
 * @brief CogKAPI Hurd-Cog (GNU Hurd Multi-Server) Platform Implementation
 * 
 * Platform-specific implementation mapping CogKAPI calls to Hurd server primitives.
 * 
 * @author Manus AI / cogpy
 * @version 1.0.0
 */

#ifndef COGKAPI_HURDCOG_H
#define COGKAPI_HURDCOG_H

#include <cogkapi/cogkapi.h>

#ifdef COGKAPI_PLATFORM_HURDCOG

#include <hurd.h>
#include <hurd/hurd_types.h>
#include <hurd/process.h>
#include <hurd/io.h>
#include <mach/mach.h>

/*============================================================================
 * Hurd Server Primitive Mappings
 *============================================================================*/

/**
 * CogKAPI to Hurd Server Mapping Table:
 * 
 * | CogKAPI Function           | Hurd Server Primitive(s)                   |
 * |----------------------------|-------------------------------------------|
 * | cog_context_create         | proc_create + translator setup            |
 * | cog_context_destroy        | proc_terminate                            |
 * | cog_atomspace_create       | store_create + file_create (translator)   |
 * | cog_atomspace_destroy      | store_destroy                             |
 * | cog_node_create            | Send message to store server translator   |
 * | cog_link_create            | Send message to store server translator   |
 * | cog_atom_remove            | Send message to store server translator   |
 * | cog_event_port_create      | port_create                               |
 * | cog_ipc_send_atom          | ipc_send on Hurd port                     |
 * | cog_event_receive          | ipc_receive on Hurd port                  |
 * 
 * Hurd-Cog Architecture:
 * - AtomSpace implemented as a translator (file system server)
 * - Each AtomSpace is a virtual file system node
 * - Atoms are accessed via file-like operations
 * - Cognitive IPC uses Hurd port mechanism (built on Mach)
 */

/*============================================================================
 * Internal Structures
 *============================================================================*/

/** @brief Hurd context internal structure */
typedef struct hurdcog_context {
    cog_handle_t handle;
    pid_t hurd_pid;
    process_t proc_port;
    mach_port_t control_port;
    auth_t auth_port;
    uint32_t flags;
} hurdcog_context_t;

/** @brief Hurd AtomSpace internal structure (translator-based) */
typedef struct hurdcog_atomspace {
    cog_handle_t handle;
    cog_context_t owner_context;
    file_t translator_port;      /* Port to AtomSpace translator */
    char translator_path[256];   /* Path to translator node */
    char name[256];
    /* Store backend */
    store_t backing_store;
} hurdcog_atomspace_t;

/** @brief Hurd event port internal structure */
typedef struct hurdcog_event_port {
    cog_handle_t handle;
    cog_context_t owner_context;
    mach_port_t receive_port;
    mach_port_t send_port;
} hurdcog_event_port_t;

/** @brief AtomSpace translator operations */
typedef struct atomspace_translator_ops {
    /* Read atom by handle */
    error_t (*read_atom)(file_t port, cog_atom_t handle, void* out_data, size_t* size);
    /* Write atom */
    error_t (*write_atom)(file_t port, const char* type, const char* name, cog_atom_t* out_handle);
    /* Create link */
    error_t (*create_link)(file_t port, const char* type, const cog_atom_t* targets, size_t num, cog_atom_t* out_handle);
    /* Remove atom */
    error_t (*remove_atom)(file_t port, cog_atom_t handle);
    /* Query atoms */
    error_t (*query_atoms)(file_t port, const char* pattern, cog_atom_t* results, size_t max, size_t* count);
} atomspace_translator_ops_t;

/*============================================================================
 * Platform-Specific Initialization
 *============================================================================*/

/**
 * @brief Initialize Hurd-Cog platform layer.
 * @return COG_STATUS_SUCCESS on success
 */
cog_status_t hurdcog_init(void);

/**
 * @brief Shutdown Hurd-Cog platform layer.
 * @return COG_STATUS_SUCCESS on success
 */
cog_status_t hurdcog_shutdown(void);

/*============================================================================
 * Hurd Server Bridge Functions
 *============================================================================*/

/**
 * @brief Create Hurd process for cognitive context.
 */
cog_status_t hurdcog_create_process(
    process_t parent_proc,
    hurdcog_context_t* out_context
);

/**
 * @brief Setup AtomSpace translator at given path.
 */
cog_status_t hurdcog_setup_atomspace_translator(
    const char* path,
    file_t* out_translator_port
);

/**
 * @brief Send RPC to AtomSpace translator.
 */
cog_status_t hurdcog_translator_rpc(
    file_t translator_port,
    int operation,
    const void* in_data,
    size_t in_size,
    void* out_data,
    size_t* out_size
);

/*============================================================================
 * Translator Operations
 *============================================================================*/

/** @brief Operation codes for AtomSpace translator RPC */
typedef enum atomspace_translator_op {
    ATOMSPACE_OP_CREATE_NODE = 1,
    ATOMSPACE_OP_CREATE_LINK = 2,
    ATOMSPACE_OP_REMOVE_ATOM = 3,
    ATOMSPACE_OP_GET_ATOM = 4,
    ATOMSPACE_OP_SET_TV = 5,
    ATOMSPACE_OP_GET_TV = 6,
    ATOMSPACE_OP_QUERY = 7,
    ATOMSPACE_OP_GET_COUNT = 8
} atomspace_translator_op_t;

/**
 * @brief Create AtomSpace backing store.
 */
cog_status_t hurdcog_create_backing_store(
    const char* path,
    size_t initial_size,
    store_t* out_store
);

/**
 * @brief Register AtomSpace translator with Hurd.
 */
cog_status_t hurdcog_register_translator(
    const char* path,
    mach_port_t control_port
);

/*============================================================================
 * Cognitive Translator Server Interface
 *============================================================================*/

/**
 * @brief Start the AtomSpace translator server.
 * 
 * This runs the translator server loop, handling incoming requests
 * for AtomSpace operations.
 */
cog_status_t hurdcog_atomspace_server_loop(
    mach_port_t server_port,
    hurdcog_atomspace_t* atomspace
);

#endif /* COGKAPI_PLATFORM_HURDCOG */

#endif /* COGKAPI_HURDCOG_H */
