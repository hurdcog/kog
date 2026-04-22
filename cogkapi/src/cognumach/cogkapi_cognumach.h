/**
 * @file cogkapi_cognumach.h
 * @brief CogKAPI CogNumach (GNU Mach Microkernel) Platform Implementation
 * 
 * Platform-specific implementation mapping CogKAPI calls to Mach kernel primitives.
 * 
 * @author Manus AI / cogpy
 * @version 1.0.0
 */

#ifndef COGKAPI_COGNUMACH_H
#define COGKAPI_COGNUMACH_H

#include <cogkapi/cogkapi.h>

#ifdef COGKAPI_PLATFORM_COGNUMACH

#include <mach/mach.h>
#include <mach/mach_types.h>
#include <mach/port.h>
#include <mach/message.h>
#include <mach/vm_map.h>

/*============================================================================
 * Mach Kernel Primitive Mappings
 *============================================================================*/

/**
 * CogKAPI to Mach Kernel Mapping Table:
 * 
 * | CogKAPI Function           | Mach Kernel Primitive(s)                   |
 * |----------------------------|-------------------------------------------|
 * | cog_context_create         | task_create + mach_port_allocate          |
 * | cog_context_destroy        | task_terminate                            |
 * | cog_atomspace_create       | mach_vm_allocate + memory_object_create   |
 * | cog_atomspace_destroy      | mach_vm_deallocate                        |
 * | cog_node_create            | Write to VM region (red-black tree)       |
 * | cog_link_create            | Write to VM region (red-black tree)       |
 * | cog_atom_remove            | Remove from VM region tree                |
 * | cog_event_port_create      | mach_port_allocate(RECEIVE)               |
 * | cog_ipc_send_atom          | mach_msg(MACH_MSG_TYPE_MOVE_SEND)         |
 * | cog_event_receive          | mach_msg(MACH_RCV_MSG)                    |
 */

/*============================================================================
 * Internal Structures
 *============================================================================*/

/** @brief Mach context internal structure */
typedef struct cognumach_context {
    cog_handle_t handle;
    task_t mach_task;
    mach_port_t control_port;
    mach_port_t event_port;
    uint32_t flags;
} cognumach_context_t;

/** @brief Mach AtomSpace internal structure */
typedef struct cognumach_atomspace {
    cog_handle_t handle;
    cog_context_t owner_context;
    vm_address_t vm_base;
    vm_size_t vm_size;
    memory_object_t memory_object;
    char name[256];
    /* Red-black tree for atom storage */
    struct rb_root atom_tree;
} cognumach_atomspace_t;

/** @brief Mach event port internal structure */
typedef struct cognumach_event_port {
    cog_handle_t handle;
    cog_context_t owner_context;
    mach_port_t receive_right;
    mach_port_t send_right;
} cognumach_event_port_t;

/** @brief Mach message structure for cognitive IPC */
typedef struct cognumach_atom_message {
    mach_msg_header_t header;
    mach_msg_body_t body;
    mach_msg_ool_descriptor_t atom_data;
    cog_atom_t source_atom;
    cog_atom_t target_atom;
    cog_truth_value_t truth_value;
} cognumach_atom_message_t;

/*============================================================================
 * Platform-Specific Initialization
 *============================================================================*/

/**
 * @brief Initialize CogNumach platform layer.
 * @return COG_STATUS_SUCCESS on success
 */
cog_status_t cognumach_init(void);

/**
 * @brief Shutdown CogNumach platform layer.
 * @return COG_STATUS_SUCCESS on success
 */
cog_status_t cognumach_shutdown(void);

/*============================================================================
 * Mach Kernel Bridge Functions
 *============================================================================*/

/**
 * @brief Create Mach task for cognitive context.
 */
cog_status_t cognumach_create_task(
    task_t parent_task,
    cognumach_context_t* out_context
);

/**
 * @brief Allocate VM region for AtomSpace.
 */
cog_status_t cognumach_allocate_atomspace_vm(
    task_t task,
    vm_size_t size,
    vm_address_t* out_address
);

/**
 * @brief Send Mach message for cognitive IPC.
 */
cog_status_t cognumach_send_mach_msg(
    mach_port_t port,
    const cognumach_atom_message_t* msg
);

/**
 * @brief Receive Mach message from event port.
 */
cog_status_t cognumach_receive_mach_msg(
    mach_port_t port,
    mach_msg_timeout_t timeout,
    cognumach_atom_message_t* out_msg
);

/*============================================================================
 * Memory Object Operations (for shared AtomSpaces)
 *============================================================================*/

/**
 * @brief Create memory object for AtomSpace backing.
 */
cog_status_t cognumach_create_memory_object(
    vm_size_t size,
    memory_object_t* out_object
);

/**
 * @brief Map memory object into task address space.
 */
cog_status_t cognumach_map_memory_object(
    task_t task,
    memory_object_t object,
    vm_address_t* out_address
);

#endif /* COGKAPI_PLATFORM_COGNUMACH */

#endif /* COGKAPI_COGNUMACH_H */
