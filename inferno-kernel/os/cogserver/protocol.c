/*
 * protocol.c - Cognitive 9P Protocol Extensions
 *
 * Implements 9P-based cognitive protocol extensions (9P-Cog).
 * Extends the Styx protocol with atom transfer and reasoning operations.
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "../port/error.h"

#include "protocol.h"

/*
 * Cognitive 9P message types
 *
 * Standard 9P messages: Tversion, Tattach, Twalk, Topen, Tread, Twrite, Tclunk
 * Extended cognitive messages:
 *   Tcogquery  / Rcogquery  - AtomSpace pattern query
 *   Tcogatom   / Rcogatom   - Single atom retrieval
 *   Tcogreason / Rcogreason - Trigger reasoning and retrieve results
 *   Tcoglearn  / Rcoglearn  - Submit training examples and retrieve model
 */

enum {
    Tcogquery  = 200,
    Rcogquery  = 201,
    Tcogatom   = 202,
    Rcogatom   = 203,
    Tcogreason = 204,
    Rcogreason = 205,
    Tcoglearn  = 206,
    Rcoglearn  = 207,
};

/*
 * Serialize an atom to a byte buffer for network transfer
 * Format: id[4] type[2] namelen[2] name[namelen] tvs[4] tvc[4] avsti[2] avlti[2]
 */
int
serialize_atom(ulong id, ushort type, char *name,
               float tvs, float tvc, short avsti, short avlti,
               uchar *buf, int maxlen)
{
    int namelen, total;
    uchar *p;

    namelen = name ? strlen(name) : 0;
    total = 4 + 2 + 2 + namelen + 4 + 4 + 2 + 2;
    if(total > maxlen)
        return -1;

    p = buf;
    PBIT32(p, (ulong)id);   p += 4;
    PBIT16(p, type);         p += 2;
    PBIT16(p, (ushort)namelen); p += 2;
    if(namelen > 0) {
        memmove(p, name, namelen);
        p += namelen;
    }
    /* Encode floats as fixed-point (scale by 1e6) */
    PBIT32(p, (ulong)(tvs * 1000000.0f)); p += 4;
    PBIT32(p, (ulong)(tvc * 1000000.0f)); p += 4;
    PBIT16(p, (ushort)avsti); p += 2;
    PBIT16(p, (ushort)avlti); p += 2;

    return total;
}

/*
 * Deserialize an atom from a byte buffer
 * Returns number of bytes consumed, or -1 on error
 */
int
deserialize_atom(uchar *buf, int buflen,
                 ulong *id, ushort *type, char **name,
                 float *tvs, float *tvc, short *avsti, short *avlti)
{
    uchar *p;
    int namelen;

    if(buflen < 14)
        return -1;

    p = buf;
    *id   = GBIT32(p); p += 4;
    *type = GBIT16(p); p += 2;
    namelen = GBIT16(p); p += 2;

    if(buflen < 14 + namelen)
        return -1;

    if(namelen > 0) {
        *name = malloc(namelen + 1);
        if(*name == nil)
            return -1;
        memmove(*name, p, namelen);
        (*name)[namelen] = 0;
    } else {
        *name = nil;
    }
    p += namelen;

    *tvs  = (float)GBIT32(p) / 1000000.0f; p += 4;
    *tvc  = (float)GBIT32(p) / 1000000.0f; p += 4;
    *avsti = (short)GBIT16(p); p += 2;
    *avlti = (short)GBIT16(p); p += 2;

    return (int)(p - buf);
}

/*
 * Handle a Tcogquery message
 * Pattern-matches against the local AtomSpace and returns matching atoms.
 */
int
handle_cogquery(uchar *tag, uchar *pattern, int patlen,
                uchar *outbuf, int outmax)
{
    ulong results[256];
    int n, i, total;
    uchar *p;
    USED(tag); USED(patlen); USED(pattern);

    /* Simple type-0 wildcard query for now */
    n = atomquery(0, results, nelem(results));
    if(n < 0) n = 0;

    p = outbuf;
    total = 0;
    PBIT16(p, (ushort)n); p += 2; total += 2;

    for(i = 0; i < n && total + 20 < outmax; i++) {
        float tvs, tvc;
        if(atomgettv(results[i], &tvs, &tvc) < 0) { tvs = 0.5f; tvc = 0.0f; }
        int nb = serialize_atom(results[i], 0, nil, tvs, tvc, 0, 0, p, outmax - total);
        if(nb < 0) break;
        p += nb;
        total += nb;
    }

    return total;
}

/*
 * Handle a Tcogatom message
 * Returns the full representation of a single atom by ID.
 */
int
handle_cogatom(ulong atom_id, uchar *outbuf, int outmax)
{
    float tvs, tvc;
    if(atomgettv(atom_id, &tvs, &tvc) < 0)
        return -1;
    return serialize_atom(atom_id, 0, nil, tvs, tvc, 0, 0, outbuf, outmax);
}
