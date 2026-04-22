/*
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University.
 * Copyright (c) 1993,1994 The University of Utah and
 * the Computer Systems Laboratory (CSL).
 * All rights reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON, THE UNIVERSITY OF UTAH AND CSL ALLOW FREE USE OF
 * THIS SOFTWARE IN ITS "AS IS" CONDITION, AND DISCLAIM ANY LIABILITY
 * OF ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF
 * THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 */
/*
 *	File:	kern/ipc_kobject.h
 *	Author:	Rich Draves
 *	Date:	1989
 *
 *	Declarations for letting a port represent a kernel object.
 */

#ifndef	_KERN_IPC_KOBJECT_H_
#define _KERN_IPC_KOBJECT_H_

#include <mach/machine/vm_types.h>
#include <ipc/ipc_types.h>
#include <ipc/ipc_kmsg.h>

typedef vm_offset_t ipc_kobject_t;

#define	IKO_NULL	((ipc_kobject_t) 0)

typedef unsigned int ipc_kobject_type_t;

/*
 *	Define types of kernel objects that use IPC.
 *	These values are used to identify kernel objects in IPC messages.
 *	Phase 1.1: Document all magic numbers with clear meanings
 */

/* IPC Kernel Object Types */
#define	IKOT_NONE		0	/* No kernel object - invalid/null reference */
#define IKOT_THREAD		1	/* Thread object - represents a kernel thread */
#define	IKOT_TASK		2	/* Task object - represents a task/process */
#define	IKOT_HOST		3	/* Host object - represents the host system */
#define	IKOT_HOST_PRIV		4	/* Privileged host object - host with admin rights */
#define	IKOT_PROCESSOR		5	/* Processor object - represents a CPU */
#define	IKOT_PSET		6	/* Processor set object - group of processors */
#define	IKOT_PSET_NAME		7	/* Processor set name port - naming right */
#define	IKOT_PAGER		8	/* Memory pager object - handles page faults */
#define	IKOT_PAGING_REQUEST	9	/* Paging request object - pending page operation */
#define	IKOT_DEVICE		10	/* Device object - hardware device interface */
#define	IKOT_XMM_OBJECT		11	/* External memory manager object */
#define	IKOT_XMM_PAGER		12	/* XMM pager object - external pager */
#define	IKOT_XMM_KERNEL		13	/* XMM kernel object - kernel memory manager */
#define	IKOT_XMM_REPLY		14	/* XMM reply object - memory manager reply */
#define	IKOT_PAGER_TERMINATING	15	/* Terminating pager - being destroyed */
#define IKOT_PAGING_NAME	16	/* Paging name object - memory object name */
#define IKOT_HOST_SECURITY	17	/* Host security object - security operations */
#define	IKOT_LEDGER		18	/* Ledger object - resource accounting */
#define IKOT_MASTER_DEVICE	19	/* Master device object - device master port */
#define IKOT_ACT		20	/* Activation object - thread activation */
#define IKOT_SUBSYSTEM		21	/* Subsystem object - RPC subsystem */
#define IKOT_IO_DONE_QUEUE	22	/* I/O done queue - completed I/O operations */
#define IKOT_SEMAPHORE		23	/* Semaphore object - synchronization primitive */
#define IKOT_LOCK_SET		24	/* Lock set object - group of locks */
#define IKOT_CLOCK		25	/* Clock object - time services */
#define IKOT_CLOCK_CTRL		26	/* Clock control object - clock management */
#define	IKOT_PAGER_PROXY	27	/* Pager proxy object - proxy for remote pager */
			/*	   << new entries here	*/
#define	IKOT_UNKNOWN		28	/* magic catchall - unknown object type */
#define	IKOT_MAX_TYPE		29	/* # of IKOT_ types - must be last */
 /* Please keep ipc/ipc_object.c:ikot_print_array up to date	*/

#define is_ipc_kobject(ikot)	(ikot != IKOT_NONE)

/*
 *	Define types of kernel objects that use page lists instead
 *	of entry lists for copyin of out of line memory.
 */

#define ipc_kobject_vm_page_list(ikot) 			\
	((ikot == IKOT_PAGING_REQUEST) || (ikot == IKOT_DEVICE))

#define ipc_kobject_vm_page_steal(ikot)	(ikot == IKOT_PAGING_REQUEST)

/* Initialize kernel server dispatch table */
/* XXX
extern void mig_init(void);
*/

/* Dispatch a kernel server function */
extern ipc_kmsg_t ipc_kobject_server(
	ipc_kmsg_t	request);

/* Make a port represent a kernel object of the given type */
extern void ipc_kobject_set(
	ipc_port_t		port,
	ipc_kobject_t		kobject,
	ipc_kobject_type_t	type);

/* Release any kernel object resources associated with a port */
extern void ipc_kobject_destroy(
	ipc_port_t		port);

/* Deliver notifications to kobjects that care about them */
extern boolean_t ipc_kobject_notify (
	mach_msg_header_t	*request_header,
	mach_msg_header_t	*reply_header);

#define	null_conversion(port)	(port)

#endif	/* _KERN_IPC_KOBJECT_H_ */
