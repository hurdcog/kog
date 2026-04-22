implement URE;

#
# Unified Rule Engine Module for Limbo
#
# Provides high-level interface to the URE kernel service.
# Supports forward chaining, backward chaining, and rule management
# via the /dev/ure device.
#
# Copyright (C) 2026 OpenCog Community
# Licensed under AGPL-3.0
#

include "sys.m";
    sys: Sys;

include "draw.m";

URE: module {
    PATH: con "/dis/inferno-kernel/ure.dis";

    init: fn();

    # Inference result
    Result: adt {
        atom_id:    int;
        strength:   real;
        confidence: real;

        to_string: fn(r: self ref Result): string;
    };

    # Forward chaining from a source atom
    forward_chain: fn(source_id: int, max_depth: int): list of ref Result;

    # Backward chaining from a goal atom
    backward_chain: fn(goal_id: int, max_depth: int): ref Result;

    # Add a rule to the rule base
    add_rule: fn(rule_id: int): int;

    # Get URE status
    status: fn(): string;

    # Get active rules
    get_rules: fn(): string;
};

init()
{
    sys = load Sys Sys->PATH;
}

Result.to_string(r: self ref Result): string
{
    return sys->sprint("Atom(%d) <%.3f, %.3f>", r.atom_id, r.strength, r.confidence);
}

forward_chain(source_id: int, max_depth: int): list of ref Result
{
    fd := sys->open("/dev/ure/forward", Sys->ORDWR);
    if(fd == nil)
        return nil;

    req := sys->sprint("%d %d", source_id, max_depth);
    buf := array of byte req;
    sys->write(fd, buf, len buf);

    rbuf := array[8192] of byte;
    n := sys->read(fd, rbuf, len rbuf);
    sys->close(fd);

    if(n <= 0)
        return nil;

    # Parse response: "atom_id strength confidence\n" per result
    results: list of ref Result = nil;
    s := string rbuf[0:n];
    lines := splitlines(s);
    for(ls := lines; ls != nil; ls = tl ls) {
        line := hd ls;
        if(len line == 0)
            continue;
        r := ref Result;
        # Simple parsing: first field is atom_id
        r.atom_id    = int line;
        r.strength   = 0.5;
        r.confidence = 0.0;
        results = r :: results;
    }

    return results;
}

backward_chain(goal_id: int, max_depth: int): ref Result
{
    fd := sys->open("/dev/ure/backward", Sys->ORDWR);
    if(fd == nil)
        return nil;

    req := sys->sprint("%d %d", goal_id, max_depth);
    buf := array of byte req;
    sys->write(fd, buf, len buf);

    rbuf := array[512] of byte;
    n := sys->read(fd, rbuf, len rbuf);
    sys->close(fd);

    if(n <= 0)
        return nil;

    r := ref Result;
    r.atom_id    = goal_id;
    r.strength   = 0.5;
    r.confidence = 0.0;
    return r;
}

add_rule(rule_id: int): int
{
    fd := sys->open("/dev/ure/ctl", Sys->OWRITE);
    if(fd == nil)
        return -1;

    cmd := sys->sprint("addrule %d", rule_id);
    buf := array of byte cmd;
    sys->write(fd, buf, len buf);
    sys->close(fd);

    return 0;
}

status(): string
{
    fd := sys->open("/dev/ure/status", Sys->OREAD);
    if(fd == nil)
        return "unavailable";

    buf := array[512] of byte;
    n := sys->read(fd, buf, len buf);
    sys->close(fd);

    if(n <= 0)
        return "";

    return string buf[0:n];
}

get_rules(): string
{
    fd := sys->open("/dev/ure/rules", Sys->OREAD);
    if(fd == nil)
        return "";

    buf := array[4096] of byte;
    n := sys->read(fd, buf, len buf);
    sys->close(fd);

    if(n <= 0)
        return "";

    return string buf[0:n];
}

# Helper: split string on newlines
splitlines(s: string): list of string
{
    lines: list of string = nil;
    start := 0;
    for(i := 0; i < len s; i++) {
        if(s[i] == '\n') {
            lines = s[start:i] :: lines;
            start = i + 1;
        }
    }
    if(start < len s)
        lines = s[start:] :: lines;
    return lines;
}
