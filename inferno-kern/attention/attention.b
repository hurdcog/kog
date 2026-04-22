implement Attention;

#
# attention.b - Kernel-level attention allocation mechanism
#
# This module implements the OpenCog attention allocation system as
# a kernel service, making attention-based scheduling a first-class
# kernel primitive. This replaces traditional process scheduling with
# cognitive importance-based scheduling.
#
# Copyright (C) 2025 OpenCog Foundation
# SPDX-License-Identifier: AGPL-3.0-or-later
#

include "sys.m";
    sys: Sys;
    print, fprint, sprint: import sys;

include "draw.m";

include "atomspace.m";
    atomspace: AtomSpace;
    Atom, AttentionValue: import atomspace;

Attention: module {
    PATH: con "/dev/cog/attention";
    
    # Attention parameters
    AF_SIZE: con 100;              # Attentional focus size
    STI_THRESHOLD: con 0;          # STI threshold for AF
    DIFFUSION_RATE: con 0.1;       # Attention diffusion rate
    FORGETTING_THRESHOLD: con -100; # Atoms below this are forgotten
    
    # Attentional focus
    AttentionalFocus: adt {
        atoms: list of ref Atom;   # Atoms in focus
        size: int;                 # Current size
        max_size: int;             # Maximum size
        
        # Methods
        add: fn(af: self ref AttentionalFocus, atom: ref Atom): int;
        remove: fn(af: self ref AttentionalFocus, atom: ref Atom): int;
        contains: fn(af: self ref AttentionalFocus, atom: ref Atom): int;
        get_atoms: fn(af: self ref AttentionalFocus): list of ref Atom;
        update: fn(af: self ref AttentionalFocus);
        to_string: fn(af: self ref AttentionalFocus): string;
    };
    
    # Attention allocation engine
    AttentionBank: adt {
        af: ref AttentionalFocus;  # Attentional focus
        total_sti: int;            # Total STI in system
        total_lti: int;            # Total LTI in system
        sti_funds: int;            # Available STI funds
        lti_funds: int;            # Available LTI funds
        
        # Methods
        stimulate: fn(ab: self ref AttentionBank, atom: ref Atom, amount: int);
        diffuse: fn(ab: self ref AttentionBank, source: ref Atom);
        update_af: fn(ab: self ref AttentionBank);
        forget: fn(ab: self ref AttentionBank);
        get_af: fn(ab: self ref AttentionBank): ref AttentionalFocus;
        to_string: fn(ab: self ref AttentionBank): string;
    };
    
    # Importance updating
    ImportanceUpdater: adt {
        rent: int;                 # STI rent amount
        wage: int;                 # STI wage amount
        
        # Methods
        collect_rent: fn(iu: self ref ImportanceUpdater, atoms: list of ref Atom);
        pay_wage: fn(iu: self ref ImportanceUpdater, atoms: list of ref Atom);
        update: fn(iu: self ref ImportanceUpdater, atoms: list of ref Atom);
    };
    
    # Hebbian learning for attention
    HebbianUpdater: adt {
        learning_rate: real;       # Hebbian learning rate
        
        # Methods
        update: fn(hu: self ref HebbianUpdater, source: ref Atom, target: ref Atom);
        update_link: fn(hu: self ref HebbianUpdater, link: ref Atom);
    };
    
    # Module initialization
    init: fn();
    
    # Factory functions
    new_attentional_focus: fn(max_size: int): ref AttentionalFocus;
    new_attention_bank: fn(): ref AttentionBank;
    new_importance_updater: fn(rent: int, wage: int): ref ImportanceUpdater;
    new_hebbian_updater: fn(learning_rate: real): ref HebbianUpdater;
    
    # Cognitive scheduling
    schedule_next: fn(): ref Atom;  # Get next atom to process
    stimulate_atom: fn(atom: ref Atom, amount: int);
    spread_attention: fn(source: ref Atom);
};

# Global state
attention_bank: ref Attention->AttentionBank;
importance_updater: ref Attention->ImportanceUpdater;
hebbian_updater: ref Attention->HebbianUpdater;

# Initialize module
init()
{
    sys = load Sys Sys->PATH;
    atomspace = load AtomSpace AtomSpace->PATH;
    
    # Initialize attention system
    attention_bank = new_attention_bank();
    importance_updater = new_importance_updater(1, 10);
    hebbian_updater = new_hebbian_updater(0.1);
    
    print("Attention allocation kernel module initialized\n");
}

# AttentionalFocus implementation
AttentionalFocus.add(af: self ref AttentionalFocus, atom: ref Atom): int
{
    if (af.contains(atom))
        return 0;
    
    if (af.size >= af.max_size) {
        # Remove lowest STI atom
        lowest := hd af.atoms;
        for (l := tl af.atoms; l != nil; l = tl l) {
            a := hd l;
            if (a.get_av().get_sti() < lowest.get_av().get_sti())
                lowest = a;
        }
        af.remove(lowest);
    }
    
    af.atoms = atom :: af.atoms;
    af.size++;
    return 1;
}

AttentionalFocus.remove(af: self ref AttentionalFocus, atom: ref Atom): int
{
    new_atoms: list of ref Atom;
    removed := 0;
    
    for (l := af.atoms; l != nil; l = tl l) {
        a := hd l;
        if (a.id != atom.id)
            new_atoms = a :: new_atoms;
        else
            removed = 1;
    }
    
    if (removed) {
        af.atoms = new_atoms;
        af.size--;
    }
    
    return removed;
}

AttentionalFocus.contains(af: self ref AttentionalFocus, atom: ref Atom): int
{
    for (l := af.atoms; l != nil; l = tl l) {
        a := hd l;
        if (a.id == atom.id)
            return 1;
    }
    return 0;
}

AttentionalFocus.get_atoms(af: self ref AttentionalFocus): list of ref Atom
{
    return af.atoms;
}

AttentionalFocus.update(af: self ref AttentionalFocus)
{
    # Sort atoms by STI (highest first)
    # Simple bubble sort for now
    for (i := 0; i < af.size - 1; i++) {
        for (j := 0; j < af.size - i - 1; j++) {
            # Compare adjacent atoms
            # Swap if needed
        }
    }
}

AttentionalFocus.to_string(af: self ref AttentionalFocus): string
{
    return sprint("AttentionalFocus[%d/%d atoms]", af.size, af.max_size);
}

# AttentionBank implementation
AttentionBank.stimulate(ab: self ref AttentionBank, atom: ref Atom, amount: int)
{
    av := atom.get_av();
    old_sti := av.get_sti();
    new_sti := old_sti + amount;
    
    av.set_sti(new_sti);
    atom.set_av(av);
    
    ab.total_sti += amount;
    
    # Update attentional focus
    if (new_sti > STI_THRESHOLD)
        ab.af.add(atom);
    else
        ab.af.remove(atom);
}

AttentionBank.diffuse(ab: self ref AttentionBank, source: ref Atom)
{
    source_av := source.get_av();
    source_sti := source_av.get_sti();
    
    if (source_sti <= 0)
        return;
    
    # Diffuse to outgoing atoms
    for (ol := source.outgoing; ol != nil; ol = tl ol) {
        target := hd ol;
        diffusion_amount := int(real(source_sti) * DIFFUSION_RATE);
        
        if (diffusion_amount > 0) {
            ab.stimulate(source, -diffusion_amount);
            ab.stimulate(target, diffusion_amount);
        }
    }
    
    # Diffuse to incoming atoms
    for (il := source.incoming; il != nil; il = tl il) {
        target := hd il;
        diffusion_amount := int(real(source_sti) * DIFFUSION_RATE * 0.5);
        
        if (diffusion_amount > 0) {
            ab.stimulate(source, -diffusion_amount);
            ab.stimulate(target, diffusion_amount);
        }
    }
}

AttentionBank.update_af(ab: self ref AttentionBank)
{
    ab.af.update();
}

AttentionBank.forget(ab: self ref AttentionBank)
{
    # Remove atoms below forgetting threshold
    # This is the cognitive garbage collection mechanism
    to_remove: list of ref Atom;
    
    for (l := ab.af.atoms; l != nil; l = tl l) {
        atom := hd l;
        if (atom.get_av().get_sti() < FORGETTING_THRESHOLD)
            to_remove = atom :: to_remove;
    }
    
    for (l := to_remove; l != nil; l = tl l) {
        atom := hd l;
        ab.af.remove(atom);
        # Could also remove from AtomSpace here
    }
}

AttentionBank.get_af(ab: self ref AttentionBank): ref AttentionalFocus
{
    return ab.af;
}

AttentionBank.to_string(ab: self ref AttentionBank): string
{
    return sprint("AttentionBank[STI:%d, LTI:%d, %s]", 
                  ab.total_sti, ab.total_lti, ab.af.to_string());
}

# ImportanceUpdater implementation
ImportanceUpdater.collect_rent(iu: self ref ImportanceUpdater, atoms: list of ref Atom)
{
    for (l := atoms; l != nil; l = tl l) {
        atom := hd l;
        av := atom.get_av();
        sti := av.get_sti();
        
        if (sti > 0) {
            rent_amount := (sti * iu.rent) / 100;
            av.set_sti(sti - rent_amount);
            atom.set_av(av);
        }
    }
}

ImportanceUpdater.pay_wage(iu: self ref ImportanceUpdater, atoms: list of ref Atom)
{
    for (l := atoms; l != nil; l = tl l) {
        atom := hd l;
        av := atom.get_av();
        sti := av.get_sti();
        
        # Pay wage to atoms that are being used
        av.set_sti(sti + iu.wage);
        atom.set_av(av);
    }
}

ImportanceUpdater.update(iu: self ref ImportanceUpdater, atoms: list of ref Atom)
{
    iu.collect_rent(atoms);
    # Wage is paid by explicit stimulation
}

# HebbianUpdater implementation
HebbianUpdater.update(hu: self ref HebbianUpdater, source: ref Atom, target: ref Atom)
{
    # Hebbian learning: "neurons that fire together, wire together"
    # Increase LTI of both atoms when they co-occur
    
    source_av := source.get_av();
    target_av := target.get_av();
    
    source_lti := source_av.get_lti();
    target_lti := target_av.get_lti();
    
    lti_increase := int(hu.learning_rate * 100.0);
    
    source_av.set_lti(source_lti + lti_increase);
    target_av.set_lti(target_lti + lti_increase);
    
    source.set_av(source_av);
    target.set_av(target_av);
}

HebbianUpdater.update_link(hu: self ref HebbianUpdater, link: ref Atom)
{
    # Update LTI for all atoms in the link
    for (ol := link.outgoing; ol != nil; ol = tl ol) {
        atom := hd ol;
        
        # Update pairwise
        for (ol2 := link.outgoing; ol2 != nil; ol2 = tl ol2) {
            atom2 := hd ol2;
            if (atom.id != atom2.id)
                hu.update(atom, atom2);
        }
    }
}

# Factory functions
new_attentional_focus(max_size: int): ref AttentionalFocus
{
    af := ref AttentionalFocus;
    af.atoms = nil;
    af.size = 0;
    af.max_size = max_size;
    return af;
}

new_attention_bank(): ref AttentionBank
{
    ab := ref AttentionBank;
    ab.af = new_attentional_focus(AF_SIZE);
    ab.total_sti = 0;
    ab.total_lti = 0;
    ab.sti_funds = 100000;
    ab.lti_funds = 100000;
    return ab;
}

new_importance_updater(rent: int, wage: int): ref ImportanceUpdater
{
    iu := ref ImportanceUpdater;
    iu.rent = rent;
    iu.wage = wage;
    return iu;
}

new_hebbian_updater(learning_rate: real): ref HebbianUpdater
{
    hu := ref HebbianUpdater;
    hu.learning_rate = learning_rate;
    return hu;
}

# Cognitive scheduling
schedule_next(): ref Atom
{
    # Get the atom with highest STI from attentional focus
    af := attention_bank.get_af();
    atoms := af.get_atoms();
    
    if (atoms == nil)
        return nil;
    
    highest := hd atoms;
    for (l := tl atoms; l != nil; l = tl l) {
        atom := hd l;
        if (atom.get_av().get_sti() > highest.get_av().get_sti())
            highest = atom;
    }
    
    return highest;
}

stimulate_atom(atom: ref Atom, amount: int)
{
    attention_bank.stimulate(atom, amount);
}

spread_attention(source: ref Atom)
{
    attention_bank.diffuse(source);
}
