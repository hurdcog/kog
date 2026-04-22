/*
 * styx-cog.h - 9P-Cog Protocol Extensions Header
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#pragma once

int cogquery_send(char *addr, char *pattern, unsigned char *outbuf, int outmax);
int cogquery_recv(unsigned char *buf, int buflen, unsigned char *outbuf, int outmax);
int cogmount(char *addr, char *localpath);
