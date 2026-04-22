implement Action;

#
# Action Application for Inferno Kernel AGI
#
# Translates high-attention action atoms from the kernel AtomSpace
# into motor commands / speech / planning outputs.
# Writes actions to /dev/action for execution.
#
# Copyright (C) 2026 OpenCog Community
# Licensed under AGPL-3.0
#

include "sys.m";
    sys: Sys;

include "draw.m";

Action: module {
    init: fn(ctxt: ref Draw->Context, args: list of string);
};

include "atomspace_kern.m";
    atomspace: AtomSpaceKern;
    Atom, TruthValue, AttentionValue: import atomspace;

# Action types
ACTION_MOTOR  := 0;
ACTION_SPEECH := 1;
ACTION_PLAN   := 2;

init(ctxt: ref Draw->Context, args: list of string)
{
    sys = load Sys Sys->PATH;
    atomspace = load AtomSpaceKern AtomSpaceKern->PATH;
    if(atomspace == nil) {
        sys->print("action: cannot load AtomSpaceKern\n");
        return;
    }
    atomspace->init(ctxt, nil);

    sys->print("Action application started\n");

    # Open action device
    action_fd := sys->open("/dev/action", Sys->OWRITE);
    if(action_fd == nil)
        sys->print("action: /dev/action unavailable, logging only\n");

    # Main action loop
    for(;;) {
        # Query AtomSpace for high-attention action atoms
        actions := atomspace->global_atomspace.get_atoms_by_type("ActionNode");
        if(actions == nil) {
            sys->sleep(100);
            continue;
        }

        for(alist := actions; alist != nil; alist = tl alist) {
            atom := hd alist;
            av   := atom.get_av();
            if(av == nil || av.sti < 100)
                continue;

            execute_action(atom, action_fd);

            # Decay STI after execution
            av.sti = int(real av.sti * 0.5);
            atom.set_av(av);
        }

        sys->sleep(50);
    }
}

execute_action(atom: ref Atom, fd: ref Sys->FD)
{
    name := atom.get_name();
    tv   := atom.get_tv();

    if(tv == nil || tv.strength < 0.5)
        return;

    cmd := name;

    if(fd != nil) {
        buf := array of byte cmd;
        sys->write(fd, buf, len buf);
    }

    sys->print("action: execute '%s' <%.2f, %.2f>\n",
        name, tv.strength, tv.confidence);
}
