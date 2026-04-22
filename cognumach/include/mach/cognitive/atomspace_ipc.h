/*
 * AtomSpace IPC Integration for Cognumach
 * Provides IPC primitives for cognitive operations
 */

#ifndef _MACH_COGNITIVE_ATOMSPACE_IPC_H_
#define _MACH_COGNITIVE_ATOMSPACE_IPC_H_

#include <mach/message.h>
#include <mach/port.h>

/* Cognitive IPC message types */
#define MACH_COGNITIVE_MSG_ATOM_CREATE    1000
#define MACH_COGNITIVE_MSG_ATOM_QUERY     1001
#define MACH_COGNITIVE_MSG_ATOM_UPDATE    1002
#define MACH_COGNITIVE_MSG_ATOM_DELETE    1003
#define MACH_COGNITIVE_MSG_PLN_REASON     1004
#define MACH_COGNITIVE_MSG_ECAN_ALLOCATE  1005

/* Cognitive port rights */
typedef mach_port_t cognitive_port_t;

/* Cognitive message structure */
typedef struct {
    mach_msg_header_t header;
    mach_msg_type_t type;
    int cognitive_op;
    int atom_id;
    int data_size;
    char data[1024];
} cognitive_msg_t;

/* Function declarations */
kern_return_t cognitive_port_allocate(
    ipc_space_t space,
    cognitive_port_t *port);

kern_return_t cognitive_msg_send(
    cognitive_port_t port,
    cognitive_msg_t *msg,
    mach_msg_timeout_t timeout);

kern_return_t cognitive_msg_receive(
    cognitive_port_t port,
    cognitive_msg_t *msg,
    mach_msg_timeout_t timeout);

#endif /* _MACH_COGNITIVE_ATOMSPACE_IPC_H_ */
