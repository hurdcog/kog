/*
 * ure.h - URE Kernel Module Header
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#pragma once

void  ureinit(void);
int   ureforward(ulong source_id, int max_depth, ulong *results, int maxresults);
float urebackward(ulong goal_id, int max_depth, float *confidence_out);

extern Dev uredevtab;
