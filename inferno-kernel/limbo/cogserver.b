implement CogServer;

#
# CogServer Module for Limbo
#
# Provides high-level interface to the CogServer kernel service.
# Manages sessions, handles cognitive protocol messages, and bridges
# kernel AtomSpace to network-accessible operations.
#
# Copyright (C) 2026 OpenCog Community
# Licensed under AGPL-3.0
#

include "sys.m";
    sys: Sys;

include "draw.m";

include "string.m";
    str: String;

CogServer: module {
    PATH: con "/dis/inferno-kernel/cogserver.dis";

    init: fn();

    # Session management
    Session: adt {
        id:     int;
        node:   string;     # Remote node address

        send:   fn(s: self ref Session, req: string): string;
        close:  fn(s: self ref Session);
    };

    # Connect to a remote CogServer node
    connect: fn(addr: string): ref Session;

    # Send a cognitive query to the local CogServer
    query: fn(pattern: string): string;

    # Create an atom via CogServer
    create_atom: fn(atype: int, name: string): int;

    # Set truth value via CogServer
    set_tv: fn(id: int, strength: real, confidence: real): int;

    # Get CogServer status
    status: fn(): string;
};

init()
{
    sys = load Sys Sys->PATH;
    str = load String String->PATH;
}

connect(addr: string): ref Session
{
    # Open connection to remote CogServer via /net
    fd := sys->dial("tcp!" + addr + "!17001", nil);
    if(fd == nil)
        return nil;

    s := ref Session;
    s.id = 1;
    s.node = addr;
    return s;
}

Session.send(s: self ref Session, req: string): string
{
    # Write request to /dev/cogserver/request
    fd := sys->open("/dev/cogserver/request", Sys->ORDWR);
    if(fd == nil)
        return "error: cannot open cogserver request";

    buf := array of byte req;
    sys->write(fd, buf, len buf);

    rbuf := array[4096] of byte;
    n := sys->read(fd, rbuf, len rbuf);
    sys->close(fd);

    if(n <= 0)
        return "";

    return string rbuf[0:n];
}

Session.close(s: self ref Session)
{
    # Close session resources
    USED(s);
}

query(pattern: string): string
{
    fd := sys->open("/dev/cogserver/request", Sys->ORDWR);
    if(fd == nil)
        return "error: cogserver unavailable";

    req := "0 " + pattern;   # Type 0 = query
    buf := array of byte req;
    sys->write(fd, buf, len buf);

    rbuf := array[4096] of byte;
    n := sys->read(fd, rbuf, len rbuf);
    sys->close(fd);

    if(n <= 0)
        return "";

    return string rbuf[0:n];
}

create_atom(atype: int, name: string): int
{
    fd := sys->open("/dev/cogserver/request", Sys->OWRITE);
    if(fd == nil)
        return -1;

    req := sys->sprint("1 %d %s", atype, name);
    buf := array of byte req;
    sys->write(fd, buf, len buf);
    sys->close(fd);

    return 0;
}

set_tv(id: int, strength: real, confidence: real): int
{
    fd := sys->open("/dev/cogserver/request", Sys->OWRITE);
    if(fd == nil)
        return -1;

    req := sys->sprint("3 %d %f %f", id, strength, confidence);
    buf := array of byte req;
    sys->write(fd, buf, len buf);
    sys->close(fd);

    return 0;
}

status(): string
{
    fd := sys->open("/dev/cogserver/status", Sys->OREAD);
    if(fd == nil)
        return "unavailable";

    buf := array[512] of byte;
    n := sys->read(fd, buf, len buf);
    sys->close(fd);

    if(n <= 0)
        return "";

    return string buf[0:n];
}
