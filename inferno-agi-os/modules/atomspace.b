implement AtomSpaceKernel;

#
# AtomSpace Kernel Service for Inferno AGI OS
# Provides hypergraph knowledge representation as a kernel-level service
#
# Copyright (C) 2025 OpenCog Foundation
# Licensed under AGPL-3.0
#

include "sys.m";
	sys: Sys;
	print, fprint, sprint: import sys;

include "draw.m";

include "styx.m";
	styx: Styx;
	Tmsg, Rmsg: import styx;

include "styxservers.m";
	styxservers: Styxservers;
	Styxserver, Navigator, Fid: import styxservers;
	Enotfound, Enotdir, Eperm: import styxservers;

include "daytime.m";
	daytime: Daytime;

AtomSpaceKernel: module {
	PATH: con "/dis/cognitive/atomspace.dis";
	
	init: fn(nil: ref Draw->Context, args: list of string);
};

#
# Atom Data Structures
#

Atom: adt {
	id: big;              # Unique atom identifier
	atype: string;        # Atom type (ConceptNode, PredicateNode, etc.)
	name: string;         # Atom name/value
	tv: ref TruthValue;   # Truth value
	av: ref AttentionValue; # Attention value
	incoming: list of ref Link; # Incoming links
	timestamp: int;       # Creation timestamp
	
	# Methods
	text: fn(a: self ref Atom): string;
	equals: fn(a: self ref Atom, b: ref Atom): int;
};

Link: adt {
	atom: ref Atom;       # Base atom
	outgoing: array of ref Atom; # Outgoing set
	
	# Methods
	text: fn(l: self ref Link): string;
};

TruthValue: adt {
	strength: real;       # Probability/strength (0.0 to 1.0)
	confidence: real;     # Confidence (0.0 to 1.0)
	
	# Methods
	text: fn(tv: self ref TruthValue): string;
	merge: fn(tv: self ref TruthValue, other: ref TruthValue): ref TruthValue;
};

AttentionValue: adt {
	sti: int;            # Short-term importance
	lti: int;            # Long-term importance
	vlti: int;           # Very long-term importance
	
	# Methods
	text: fn(av: self ref AttentionValue): string;
};

#
# AtomSpace - The hypergraph database
#

AtomSpace: adt {
	atoms: array of ref Atom;  # All atoms
	atomcount: int;            # Number of atoms
	maxatoms: int;             # Maximum atoms
	typeindex: ref TypeIndex;  # Type-based indexing
	nameindex: ref NameIndex;  # Name-based indexing
	
	# Methods
	create: fn(): ref AtomSpace;
	add_atom: fn(as: self ref AtomSpace, atype: string, name: string): ref Atom;
	add_link: fn(as: self ref AtomSpace, ltype: string, outgoing: array of ref Atom): ref Link;
	get_atom: fn(as: self ref AtomSpace, id: big): ref Atom;
	remove_atom: fn(as: self ref AtomSpace, id: big): int;
	get_atoms_by_type: fn(as: self ref AtomSpace, atype: string): list of ref Atom;
	get_atoms_by_name: fn(as: self ref AtomSpace, name: string): list of ref Atom;
	pattern_match: fn(as: self ref AtomSpace, pattern: ref Pattern): list of ref Atom;
	size: fn(as: self ref AtomSpace): int;
};

TypeIndex: adt {
	types: array of list of ref Atom;
	
	create: fn(): ref TypeIndex;
	add: fn(ti: self ref TypeIndex, atype: string, atom: ref Atom);
	get: fn(ti: self ref TypeIndex, atype: string): list of ref Atom;
};

NameIndex: adt {
	names: array of list of ref Atom;
	
	create: fn(): ref NameIndex;
	add: fn(ni: self ref NameIndex, name: string, atom: ref Atom);
	get: fn(ni: self ref NameIndex, name: string): list of ref Atom;
};

Pattern: adt {
	ptype: string;        # Pattern type
	variables: list of string; # Pattern variables
	clauses: list of ref Atom; # Pattern clauses
	
	matches: fn(p: self ref Pattern, atom: ref Atom): int;
};

#
# Global AtomSpace instance
#

atomspace: ref AtomSpace;
nextid: big = 1;

#
# Namespace tree for /cognitive/atomspace
#

Qroot, Qctl, Qatoms, Qatom, Qtypes, Qtype, Qqueries, Qquery: con iota;

Nav: module {
	init: fn(srv: ref Styxserver);
};

nav: Nav;

#
# Initialization
#

init(nil: ref Draw->Context, args: list of string)
{
	sys = load Sys Sys->PATH;
	styx = load Styx Styx->PATH;
	styxservers = load Styxservers Styxservers->PATH;
	daytime = load Daytime Daytime->PATH;
	
	if (styx == nil || styxservers == nil || daytime == nil) {
		sys->fprint(sys->fildes(2), "atomspace: cannot load required modules\n");
		raise "fail:load";
	}
	
	styx->init();
	styxservers->init(styx);
	daytime->init();
	
	# Create global AtomSpace
	atomspace = AtomSpace.create();
	
	# Initialize with some basic types
	init_atom_types();
	
	# Start Styx server
	sys->pctl(Sys->NEWPGRP, nil);
	
	navch := chan of ref Styxservers->Navop;
	spawn navigator(navch);
	
	(tchan, srv) := Styxserver.new(sys->fildes(0), Navigator.new(navch), big Qroot);
	
	while ((gm := <-tchan) != nil) {
		pick m := gm {
		Readerror =>
			sys->fprint(sys->fildes(2), "atomspace: read error: %s\n", m.error);
			exit;
		}
	}
}

#
# Initialize basic atom types
#

init_atom_types()
{
	# Node types
	atomspace.add_atom("Node", "");
	atomspace.add_atom("ConceptNode", "");
	atomspace.add_atom("PredicateNode", "");
	atomspace.add_atom("VariableNode", "");
	atomspace.add_atom("TypeNode", "");
	
	# Link types
	atomspace.add_atom("Link", "");
	atomspace.add_atom("InheritanceLink", "");
	atomspace.add_atom("SimilarityLink", "");
	atomspace.add_atom("ImplicationLink", "");
	atomspace.add_atom("AndLink", "");
	atomspace.add_atom("OrLink", "");
	atomspace.add_atom("NotLink", "");
	atomspace.add_atom("EvaluationLink", "");
	atomspace.add_atom("ExecutionLink", "");
	atomspace.add_atom("ListLink", "");
}

#
# Navigator - handles namespace traversal
#

navigator(navch: chan of ref Styxservers->Navop)
{
	while ((m := <-navch) != nil) {
		pick n := m {
		Stat =>
			n.reply <-= (stat(n.path, n.name), nil);
		Walk =>
			n.reply <-= (walk(n.path, n.name), nil);
		Readdir =>
			n.reply <-= (readdir(n.path, n.offset, n.count), nil);
		}
	}
}

stat(path: big, name: string): (ref Sys->Dir, string)
{
	case int path {
	Qroot =>
		return (dir(".", Sys->DMDIR|8r555, path), nil);
	Qctl =>
		return (dir("ctl", 8r666, path), nil);
	Qatoms =>
		return (dir("atoms", Sys->DMDIR|8r555, path), nil);
	Qtypes =>
		return (dir("types", Sys->DMDIR|8r555, path), nil);
	Qqueries =>
		return (dir("queries", Sys->DMDIR|8r555, path), nil);
	* =>
		return (nil, Enotfound);
	}
}

walk(path: big, name: string): (big, string)
{
	case int path {
	Qroot =>
		if (name == "ctl")
			return (big Qctl, nil);
		if (name == "atoms")
			return (big Qatoms, nil);
		if (name == "types")
			return (big Qtypes, nil);
		if (name == "queries")
			return (big Qqueries, nil);
	}
	return (big 0, Enotfound);
}

readdir(path: big, offset: int, count: int): (array of ref Sys->Dir, string)
{
	case int path {
	Qroot =>
		dirs := array[] of {
			dir("ctl", 8r666, big Qctl),
			dir("atoms", Sys->DMDIR|8r555, big Qatoms),
			dir("types", Sys->DMDIR|8r555, big Qtypes),
			dir("queries", Sys->DMDIR|8r555, big Qqueries),
		};
		return (dirs[offset:], nil);
	Qatoms =>
		# Return list of atoms
		dirs := array[atomspace.atomcount] of ref Sys->Dir;
		for (i := 0; i < atomspace.atomcount; i++) {
			atom := atomspace.atoms[i];
			if (atom != nil)
				dirs[i] = dir(sprint("%bd", atom.id), 8r444, big (Qatom + int atom.id));
		}
		return (dirs[offset:], nil);
	* =>
		return (nil, Enotdir);
	}
}

dir(name: string, perm: int, qid: big): ref Sys->Dir
{
	d := ref sys->zerodir;
	d.name = name;
	d.uid = "agi";
	d.gid = "agi";
	d.qid.path = qid;
	if (perm & Sys->DMDIR)
		d.qid.qtype = Sys->QTDIR;
	else
		d.qid.qtype = Sys->QTFILE;
	d.mode = perm;
	d.atime = d.mtime = daytime->now();
	return d;
}

#
# AtomSpace implementation
#

AtomSpace.create(): ref AtomSpace
{
	as := ref AtomSpace;
	as.maxatoms = 1000000;  # 1 million atoms
	as.atoms = array[as.maxatoms] of ref Atom;
	as.atomcount = 0;
	as.typeindex = TypeIndex.create();
	as.nameindex = NameIndex.create();
	return as;
}

AtomSpace.add_atom(as: self ref AtomSpace, atype: string, name: string): ref Atom
{
	if (as.atomcount >= as.maxatoms)
		return nil;
	
	atom := ref Atom;
	atom.id = nextid++;
	atom.atype = atype;
	atom.name = name;
	atom.tv = ref TruthValue(1.0, 1.0);  # Default TV
	atom.av = ref AttentionValue(0, 0, 0); # Default AV
	atom.incoming = nil;
	atom.timestamp = daytime->now();
	
	as.atoms[as.atomcount++] = atom;
	as.typeindex.add(atype, atom);
	if (name != "")
		as.nameindex.add(name, atom);
	
	return atom;
}

AtomSpace.add_link(as: self ref AtomSpace, ltype: string, outgoing: array of ref Atom): ref Link
{
	atom := as.add_atom(ltype, "");
	if (atom == nil)
		return nil;
	
	link := ref Link;
	link.atom = atom;
	link.outgoing = outgoing;
	
	# Update incoming sets
	for (i := 0; i < len outgoing; i++) {
		if (outgoing[i] != nil)
			outgoing[i].incoming = link :: outgoing[i].incoming;
	}
	
	return link;
}

AtomSpace.get_atom(as: self ref AtomSpace, id: big): ref Atom
{
	for (i := 0; i < as.atomcount; i++) {
		if (as.atoms[i] != nil && as.atoms[i].id == id)
			return as.atoms[i];
	}
	return nil;
}

AtomSpace.size(as: self ref AtomSpace): int
{
	return as.atomcount;
}

#
# Atom methods
#

Atom.text(a: self ref Atom): string
{
	return sprint("(%s '%s' %s %s)", a.atype, a.name, a.tv.text(), a.av.text());
}

Atom.equals(a: self ref Atom, b: ref Atom): int
{
	if (a == nil || b == nil)
		return 0;
	return a.id == b.id;
}

#
# TruthValue methods
#

TruthValue.text(tv: self ref TruthValue): string
{
	return sprint("(stv %.3f %.3f)", tv.strength, tv.confidence);
}

TruthValue.merge(tv: self ref TruthValue, other: ref TruthValue): ref TruthValue
{
	# Simple averaging for now
	s := (tv.strength + other.strength) / 2.0;
	c := (tv.confidence + other.confidence) / 2.0;
	return ref TruthValue(s, c);
}

#
# AttentionValue methods
#

AttentionValue.text(av: self ref AttentionValue): string
{
	return sprint("(av %d %d %d)", av.sti, av.lti, av.vlti);
}

#
# TypeIndex implementation
#

TypeIndex.create(): ref TypeIndex
{
	ti := ref TypeIndex;
	ti.types = array[1000] of list of ref Atom;
	return ti;
}

TypeIndex.add(ti: self ref TypeIndex, atype: string, atom: ref Atom)
{
	h := hash(atype) % len ti.types;
	ti.types[h] = atom :: ti.types[h];
}

TypeIndex.get(ti: self ref TypeIndex, atype: string): list of ref Atom
{
	h := hash(atype) % len ti.types;
	return ti.types[h];
}

#
# NameIndex implementation
#

NameIndex.create(): ref NameIndex
{
	ni := ref NameIndex;
	ni.names = array[1000] of list of ref Atom;
	return ni;
}

NameIndex.add(ni: self ref NameIndex, name: string, atom: ref Atom)
{
	h := hash(name) % len ni.names;
	ni.names[h] = atom :: ni.names[h];
}

NameIndex.get(ni: self ref NameIndex, name: string): list of ref Atom
{
	h := hash(name) % len ni.names;
	return ni.names[h];
}

#
# Utility functions
#

hash(s: string): int
{
	h := 0;
	for (i := 0; i < len s; i++)
		h = h * 31 + s[i];
	if (h < 0)
		h = -h;
	return h;
}
