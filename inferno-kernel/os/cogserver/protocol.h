/*
 * protocol.h - 9P-Cog Protocol Header
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#pragma once

/* Cognitive 9P message handlers */
int handle_cogquery(uchar *tag, uchar *pattern, int patlen,
                    uchar *outbuf, int outmax);
int handle_cogatom(ulong atom_id, uchar *outbuf, int outmax);

/* Atom serialization */
int serialize_atom(ulong id, ushort type, char *name,
                   float tvs, float tvc, short avsti, short avlti,
                   uchar *buf, int maxlen);
int deserialize_atom(uchar *buf, int buflen,
                     ulong *id, ushort *type, char **name,
                     float *tvs, float *tvc, short *avsti, short *avlti);
