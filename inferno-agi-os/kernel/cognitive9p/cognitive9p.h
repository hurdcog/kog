/*
 * Cognitive 9P Filesystem Interface
 * Header file for cognitive resource access through 9P protocol
 */

#ifndef _COGNITIVE9P_H_
#define _COGNITIVE9P_H_

/* Forward declarations */
typedef struct CogFs CogFs;
typedef struct AtomSpace AtomSpace;

/* Initialization */
void cognitive9p_init(void);

/* File operations */
long cognitive9p_read(CogFs *file, void *buf, long n, vlong offset);
long cognitive9p_write(CogFs *file, void *buf, long n, vlong offset);

/* Atom operations through filesystem */
u32int cognitive9p_create_atom(char *type, char *name);
long cognitive9p_query_atoms(char *pattern, void *buf, long n);

/* Utility functions */
AtomSpace* get_global_atomspace(void);
void print_cogfs_tree(void);

#endif /* _COGNITIVE9P_H_ */
