/*
 * AtomSpace Filesystem Header
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#ifndef ATOMFS_H
#define ATOMFS_H

#include <stdint.h>
#include <stddef.h>

/* Initialize/shutdown */
int  atomfs_init(void);
void atomfs_shutdown(void);

/* Atom operations */
uint32_t atomfs_create(const char *type_str, const char *name,
                       float strength, float confidence);
int      atomfs_read(uint32_t atom_id, char *buf, size_t maxlen);
int      atomfs_delete(uint32_t atom_id);

/* Statistics and queries */
int      atomfs_stats(char *buf, size_t maxlen);
int      atomfs_query(const char *type_filter, uint32_t *ids, uint32_t max_results);

/* Truth and attention values */
int      atomfs_set_tv(uint32_t atom_id, float strength, float confidence);
int      atomfs_set_av(uint32_t atom_id, int16_t sti, int16_t lti);

#endif /* ATOMFS_H */
