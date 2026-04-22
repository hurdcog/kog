implement AtomSpaceKern;

# Inferno Kernel Module: AtomSpace Hypergraph Service
# Provides kernel-level hypergraph storage and operations
# All cognitive knowledge represented as atoms in this space

include "sys.m";
	sys: Sys;
	print, fprint, sprint: import sys;

include "draw.m";

include "string.m";
	str: String;

include "bufio.m";
	bufio: Bufio;
	Iobuf: import bufio;

AtomSpaceKern: module {
	PATH: con "/dis/inferno-kernel/atomspace_kern.dis";
	
	# Atom types
	NODE: con 1;
	LINK: con 2;
	
	# Core atom structure
	Atom: adt {
		id: int;              # Unique atom identifier
		atype: int;           # Atom type (NODE or LINK)
		name: string;         # Atom name (for nodes)
		tv: ref TruthValue;   # Truth value
		av: ref AttentionValue; # Attention value
		incoming: list of ref Atom; # Incoming links
		outgoing: list of ref Atom; # Outgoing links (for links)
		
		# Methods
		get_type: fn(a: self ref Atom): string;
		get_name: fn(a: self ref Atom): string;
		get_tv: fn(a: self ref Atom): ref TruthValue;
		get_av: fn(a: self ref Atom): ref AttentionValue;
		set_tv: fn(a: self ref Atom, tv: ref TruthValue);
		set_av: fn(a: self ref Atom, av: ref AttentionValue);
		get_incoming: fn(a: self ref Atom): list of ref Atom;
		get_outgoing: fn(a: self ref Atom): list of ref Atom;
		add_incoming: fn(a: self ref Atom, link: ref Atom);
		add_outgoing: fn(a: self ref Atom, target: ref Atom);
		to_string: fn(a: self ref Atom): string;
	};
	
	# Truth value representation (PLN-style)
	TruthValue: adt {
		strength: real;      # Probability/strength [0, 1]
		confidence: real;    # Confidence/count [0, 1]
		
		# Methods
		get_strength: fn(tv: self ref TruthValue): real;
		get_confidence: fn(tv: self ref TruthValue): real;
		set_strength: fn(tv: self ref TruthValue, s: real);
		set_confidence: fn(tv: self ref TruthValue, c: real);
		merge: fn(tv: self ref TruthValue, other: ref TruthValue): ref TruthValue;
		to_string: fn(tv: self ref TruthValue): string;
	};
	
	# Attention value (ECAN-style)
	AttentionValue: adt {
		sti: int;            # Short-term importance [-32768, 32767]
		lti: int;            # Long-term importance [-32768, 32767]
		vlti: int;           # Very long-term importance [0, 65535]
		
		# Methods
		get_sti: fn(av: self ref AttentionValue): int;
		get_lti: fn(av: self ref AttentionValue): int;
		get_vlti: fn(av: self ref AttentionValue): int;
		set_sti: fn(av: self ref AttentionValue, sti: int);
		set_lti: fn(av: self ref AttentionValue, lti: int);
		set_vlti: fn(av: self ref AttentionValue, vlti: int);
		decay_sti: fn(av: self ref AttentionValue, rate: real);
		to_string: fn(av: self ref AttentionValue): string;
	};
	
	# AtomSpace hypergraph database
	AtomSpace: adt {
		atoms: array of ref Atom;  # All atoms indexed by ID
		nodes: list of ref Atom;   # All node atoms
		links: list of ref Atom;   # All link atoms
		size: int;                 # Total number of atoms
		next_id: int;              # Next atom ID to assign
		
		# Initialization
		init: fn(as: self ref AtomSpace);
		
		# Atom creation
		add_node: fn(as: self ref AtomSpace, atype: string, name: string, tv: ref TruthValue): ref Atom;
		add_link: fn(as: self ref AtomSpace, ltype: string, outgoing: list of ref Atom, tv: ref TruthValue): ref Atom;
		
		# Atom retrieval
		get_atom: fn(as: self ref AtomSpace, id: int): ref Atom;
		get_node: fn(as: self ref AtomSpace, atype: string, name: string): ref Atom;
		get_atoms_by_type: fn(as: self ref AtomSpace, atype: string): list of ref Atom;
		get_incoming: fn(as: self ref AtomSpace, atom: ref Atom): list of ref Atom;
		get_outgoing: fn(as: self ref AtomSpace, atom: ref Atom): list of ref Atom;
		
		# Atom modification
		set_tv: fn(as: self ref AtomSpace, atom: ref Atom, tv: ref TruthValue);
		set_av: fn(as: self ref AtomSpace, atom: ref Atom, av: ref AttentionValue);
		
		# Atom deletion
		remove_atom: fn(as: self ref AtomSpace, atom: ref Atom): int;
		
		# Query operations
		get_all_atoms: fn(as: self ref AtomSpace): list of ref Atom;
		get_size: fn(as: self ref AtomSpace): int;
		clear: fn(as: self ref AtomSpace);
		
		# Persistence
		save: fn(as: self ref AtomSpace, filename: string): int;
		load: fn(as: self ref AtomSpace, filename: string): int;
		
		# Statistics
		get_node_count: fn(as: self ref AtomSpace): int;
		get_link_count: fn(as: self ref AtomSpace): int;
		print_stats: fn(as: self ref AtomSpace);
	};
	
	# Module initialization
	init: fn(ctxt: ref Draw->Context, argv: list of string);
	
	# Global AtomSpace instance
	global_atomspace: ref AtomSpace;
	
	# Utility functions
	create_truth_value: fn(strength: real, confidence: real): ref TruthValue;
	create_attention_value: fn(sti: int, lti: int, vlti: int): ref AttentionValue;
	default_tv: fn(): ref TruthValue;
	default_av: fn(): ref AttentionValue;
};

# Implementation
init(ctxt: ref Draw->Context, argv: list of string)
{
	sys = load Sys Sys->PATH;
	str = load String String->PATH;
	bufio = load Bufio Bufio->PATH;
	
	# Initialize global atomspace
	global_atomspace = ref AtomSpace;
	global_atomspace.init();
	
	print("AtomSpace kernel module initialized\n");
	print("AtomSpace size: %d atoms\n", global_atomspace.get_size());
}

# Atom methods
Atom.get_type(a: self ref Atom): string
{
	case a.atype {
		NODE => return "Node";
		LINK => return "Link";
		* => return "Unknown";
	}
}

Atom.get_name(a: self ref Atom): string
{
	return a.name;
}

Atom.get_tv(a: self ref Atom): ref TruthValue
{
	return a.tv;
}

Atom.get_av(a: self ref Atom): ref AttentionValue
{
	return a.av;
}

Atom.set_tv(a: self ref Atom, tv: ref TruthValue)
{
	a.tv = tv;
}

Atom.set_av(a: self ref Atom, av: ref AttentionValue)
{
	a.av = av;
}

Atom.get_incoming(a: self ref Atom): list of ref Atom
{
	return a.incoming;
}

Atom.get_outgoing(a: self ref Atom): list of ref Atom
{
	return a.outgoing;
}

Atom.add_incoming(a: self ref Atom, link: ref Atom)
{
	a.incoming = link :: a.incoming;
}

Atom.add_outgoing(a: self ref Atom, target: ref Atom)
{
	a.outgoing = target :: a.outgoing;
}

Atom.to_string(a: self ref Atom): string
{
	s := sprint("Atom[%d]: %s '%s' TV:%s AV:%s", 
		a.id, a.get_type(), a.name, 
		a.tv.to_string(), a.av.to_string());
	return s;
}

# TruthValue methods
TruthValue.get_strength(tv: self ref TruthValue): real
{
	return tv.strength;
}

TruthValue.get_confidence(tv: self ref TruthValue): real
{
	return tv.confidence;
}

TruthValue.set_strength(tv: self ref TruthValue, s: real)
{
	if (s < 0.0)
		s = 0.0;
	if (s > 1.0)
		s = 1.0;
	tv.strength = s;
}

TruthValue.set_confidence(tv: self ref TruthValue, c: real)
{
	if (c < 0.0)
		c = 0.0;
	if (c > 1.0)
		c = 1.0;
	tv.confidence = c;
}

TruthValue.merge(tv: self ref TruthValue, other: ref TruthValue): ref TruthValue
{
	# Simple confidence-weighted merge
	total_conf := tv.confidence + other.confidence;
	if (total_conf == 0.0)
		return tv;
	
	new_strength := (tv.strength * tv.confidence + other.strength * other.confidence) / total_conf;
	new_conf := total_conf / 2.0;  # Average confidence
	
	return create_truth_value(new_strength, new_conf);
}

TruthValue.to_string(tv: self ref TruthValue): string
{
	return sprint("(%.3f, %.3f)", tv.strength, tv.confidence);
}

# AttentionValue methods
AttentionValue.get_sti(av: self ref AttentionValue): int
{
	return av.sti;
}

AttentionValue.get_lti(av: self ref AttentionValue): int
{
	return av.lti;
}

AttentionValue.get_vlti(av: self ref AttentionValue): int
{
	return av.vlti;
}

AttentionValue.set_sti(av: self ref AttentionValue, sti: int)
{
	av.sti = sti;
}

AttentionValue.set_lti(av: self ref AttentionValue, lti: int)
{
	av.lti = lti;
}

AttentionValue.set_vlti(av: self ref AttentionValue, vlti: int)
{
	av.vlti = vlti;
}

AttentionValue.decay_sti(av: self ref AttentionValue, rate: real)
{
	av.sti = int(real(av.sti) * (1.0 - rate));
}

AttentionValue.to_string(av: self ref AttentionValue): string
{
	return sprint("[STI:%d, LTI:%d, VLTI:%d]", av.sti, av.lti, av.vlti);
}

# AtomSpace methods
AtomSpace.init(as: self ref AtomSpace)
{
	as.atoms = array[10000] of ref Atom;  # Initial capacity
	as.nodes = nil;
	as.links = nil;
	as.size = 0;
	as.next_id = 1;
}

AtomSpace.add_node(as: self ref AtomSpace, atype: string, name: string, tv: ref TruthValue): ref Atom
{
	# Check if node already exists
	existing := as.get_node(atype, name);
	if (existing != nil) {
		# Merge truth values
		existing.tv = existing.tv.merge(tv);
		return existing;
	}
	
	# Create new node
	atom := ref Atom;
	atom.id = as.next_id++;
	atom.atype = NODE;
	atom.name = name;
	atom.tv = tv;
	atom.av = default_av();
	atom.incoming = nil;
	atom.outgoing = nil;
	
	# Add to atomspace
	as.atoms[atom.id] = atom;
	as.nodes = atom :: as.nodes;
	as.size++;
	
	return atom;
}

AtomSpace.add_link(as: self ref AtomSpace, ltype: string, outgoing: list of ref Atom, tv: ref TruthValue): ref Atom
{
	# Create new link
	atom := ref Atom;
	atom.id = as.next_id++;
	atom.atype = LINK;
	atom.name = ltype;
	atom.tv = tv;
	atom.av = default_av();
	atom.incoming = nil;
	atom.outgoing = outgoing;
	
	# Update incoming sets of target atoms
	for (targets := outgoing; targets != nil; targets = tl targets) {
		target := hd targets;
		target.add_incoming(atom);
	}
	
	# Add to atomspace
	as.atoms[atom.id] = atom;
	as.links = atom :: as.links;
	as.size++;
	
	return atom;
}

AtomSpace.get_atom(as: self ref AtomSpace, id: int): ref Atom
{
	if (id < 0 || id >= len as.atoms)
		return nil;
	return as.atoms[id];
}

AtomSpace.get_node(as: self ref AtomSpace, atype: string, name: string): ref Atom
{
	for (nodes := as.nodes; nodes != nil; nodes = tl nodes) {
		node := hd nodes;
		if (node.name == name)
			return node;
	}
	return nil;
}

AtomSpace.get_size(as: self ref AtomSpace): int
{
	return as.size;
}

AtomSpace.get_node_count(as: self ref AtomSpace): int
{
	count := 0;
	for (nodes := as.nodes; nodes != nil; nodes = tl nodes)
		count++;
	return count;
}

AtomSpace.get_link_count(as: self ref AtomSpace): int
{
	count := 0;
	for (links := as.links; links != nil; links = tl links)
		count++;
	return count;
}

AtomSpace.print_stats(as: self ref AtomSpace)
{
	print("AtomSpace Statistics:\n");
	print("  Total atoms: %d\n", as.size);
	print("  Nodes: %d\n", as.get_node_count());
	print("  Links: %d\n", as.get_link_count());
}

# Utility functions
create_truth_value(strength: real, confidence: real): ref TruthValue
{
	tv := ref TruthValue;
	tv.strength = strength;
	tv.confidence = confidence;
	return tv;
}

create_attention_value(sti: int, lti: int, vlti: int): ref AttentionValue
{
	av := ref AttentionValue;
	av.sti = sti;
	av.lti = lti;
	av.vlti = vlti;
	return av;
}

default_tv(): ref TruthValue
{
	return create_truth_value(1.0, 0.0);  # Full strength, zero confidence
}

default_av(): ref AttentionValue
{
	return create_attention_value(0, 0, 0);  # Zero attention
}
