/*
 * cogserver.h - CogServer Kernel Module Header
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#pragma once

void cogserverinit(void);
int  cogserverhandle_query(char *pattern, char *outbuf, int outmax);

extern Dev cogserverdevtab;
