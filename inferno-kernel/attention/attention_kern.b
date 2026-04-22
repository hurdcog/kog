implement AttentionKern;

# Inferno Kernel Module: Economic Attention Network (ECAN)
# Implements attention allocation as a kernel-level scheduling service
# Atoms compete for limited attention resources using economic principles

include "sys.m";
	sys: Sys;
	print, fprint, sprint: import sys;

include "draw.m";

include "math.m";
	math: Math;
	sqrt, exp, log, pow: import math;

include "../atomspace/atomspace_kern.m";
	atomspace: AtomSpaceKern;
	Atom, AtomSpace, AttentionValue: import atomspace;

AttentionKern: module {
	PATH: con "/dis/inferno-kernel/attention_kern.dis";
	
	# Attention parameters
	AttentionBank: adt {
		total_sti: int;           # Total STI in the system
		sti_funds: int;           # Available STI funds
		target_sti: int;          # Target total STI
		max_af_size: int;         # Maximum attentional focus size
		min_af_sti: int;          # Minimum STI for attentional focus
		
		# Decay parameters
		sti_decay_rate: real;     # STI decay rate per cycle
		lti_decay_rate: real;     # LTI decay rate per cycle
		
		# Rent parameters
		af_rent: int;             # Rent charged for being in AF
		
		# Wage parameters
		sti_wage: int;            # STI wage for important atoms
		lti_wage: int;            # LTI wage for persistent atoms
		
		# Methods
		init: fn(ab: self ref AttentionBank);
		update_funds: fn(ab: self ref AttentionBank, atomspace: ref AtomSpace);
		collect_rent: fn(ab: self ref AttentionBank, atomspace: ref AtomSpace);
		pay_wages: fn(ab: self ref AttentionBank, atomspace: ref AtomSpace);
		decay_attention: fn(ab: self ref AttentionBank, atomspace: ref AtomSpace);
		get_stats: fn(ab: self ref AttentionBank): string;
	};
	
	# Attentional Focus (AF)
	AttentionalFocus: adt {
		atoms: list of ref Atom;  # Atoms currently in focus
		size: int;                # Current AF size
		max_size: int;            # Maximum AF size
		min_sti: int;             # Minimum STI threshold
		
		# Methods
		init: fn(af: self ref AttentionalFocus, max_size: int, min_sti: int);
		update: fn(af: self ref AttentionalFocus, atomspace: ref AtomSpace);
		contains: fn(af: self ref AttentionalFocus, atom: ref Atom): int;
		get_atoms: fn(af: self ref AttentionalFocus): list of ref Atom;
		get_size: fn(af: self ref AttentionalFocus): int;
		print_focus: fn(af: self ref AttentionalFocus);
	};
	
	# Stimulus source for attention spreading
	Stimulus: adt {
		atom: ref Atom;           # Source atom
		amount: int;              # Stimulus amount
		
		# Methods
		spread: fn(s: self ref Stimulus, atomspace: ref AtomSpace, diffusion: real);
	};
	
	# Attention spreading mechanism
	AttentionSpreading: adt {
		sources: list of ref Stimulus;  # Stimulus sources
		diffusion_rate: real;           # Diffusion rate
		
		# Methods
		init: fn(as: self ref AttentionSpreading);
		add_stimulus: fn(as: self ref AttentionSpreading, atom: ref Atom, amount: int);
		spread: fn(as: self ref AttentionSpreading, atomspace: ref AtomSpace);
		clear: fn(as: self ref AttentionSpreading);
	};
	
	# Importance updating
	ImportanceUpdater: adt {
		# Methods
		update_sti: fn(atom: ref Atom, delta: int);
		update_lti: fn(atom: ref Atom, delta: int);
		update_vlti: fn(atom: ref Atom, delta: int);
		normalize_sti: fn(atomspace: ref AtomSpace, target: int);
	};
	
	# Hebbian learning for attention
	HebbianUpdater: adt {
		learning_rate: real;      # Hebbian learning rate
		
		# Methods
		init: fn(hu: self ref HebbianUpdater, rate: real);
		update: fn(hu: self ref HebbianUpdater, source: ref Atom, target: ref Atom);
		update_link: fn(hu: self ref HebbianUpdater, link: ref Atom);
	};
	
	# Module initialization
	init: fn(ctxt: ref Draw->Context, argv: list of string);
	
	# Global attention structures
	global_attention_bank: ref AttentionBank;
	global_attentional_focus: ref AttentionalFocus;
	global_attention_spreading: ref AttentionSpreading;
	global_hebbian_updater: ref HebbianUpdater;
	
	# Main attention allocation cycle
	attention_cycle: fn(atomspace: ref AtomSpace);
	
	# Utility functions
	stimulate_atom: fn(atom: ref Atom, amount: int);
	get_attentional_focus: fn(): list of ref Atom;
	get_af_size: fn(): int;
	set_af_size: fn(size: int);
};

# Implementation
init(ctxt: ref Draw->Context, argv: list of string)
{
	sys = load Sys Sys->PATH;
	math = load Math Math->PATH;
	atomspace = load AtomSpaceKern AtomSpaceKern->PATH;
	
	# Initialize global structures
	global_attention_bank = ref AttentionBank;
	global_attention_bank.init();
	
	global_attentional_focus = ref AttentionalFocus;
	global_attentional_focus.init(100, 10);  # Max 100 atoms, min STI 10
	
	global_attention_spreading = ref AttentionSpreading;
	global_attention_spreading.init();
	
	global_hebbian_updater = ref HebbianUpdater;
	global_hebbian_updater.init(0.1);  # 10% learning rate
	
	print("Attention kernel module initialized\n");
	print("Attentional focus: max_size=%d, min_sti=%d\n", 
		global_attentional_focus.max_size, global_attentional_focus.min_sti);
}

# AttentionBank methods
AttentionBank.init(ab: self ref AttentionBank)
{
	ab.total_sti = 0;
	ab.sti_funds = 100000;      # Initial STI funds
	ab.target_sti = 100000;     # Target total STI
	ab.max_af_size = 100;       # Max 100 atoms in AF
	ab.min_af_sti = 10;         # Min STI 10 for AF
	ab.sti_decay_rate = 0.01;   # 1% decay per cycle
	ab.lti_decay_rate = 0.001;  # 0.1% decay per cycle
	ab.af_rent = 1;             # 1 STI rent per cycle
	ab.sti_wage = 10;           # 10 STI wage for important atoms
	ab.lti_wage = 5;            # 5 LTI wage for persistent atoms
}

AttentionBank.update_funds(ab: self ref AttentionBank, atomspace: ref AtomSpace)
{
	# Calculate total STI in system
	total := 0;
	for (i := 0; i < atomspace.size; i++) {
		atom := atomspace.atoms[i];
		if (atom != nil) {
			av := atom.get_av();
			total += av.get_sti();
		}
	}
	ab.total_sti = total;
	
	# Update available funds
	ab.sti_funds = ab.target_sti - ab.total_sti;
}

AttentionBank.collect_rent(ab: self ref AttentionBank, atomspace: ref AtomSpace)
{
	# Collect rent from atoms in attentional focus
	for (atoms := global_attentional_focus.atoms; atoms != nil; atoms = tl atoms) {
		atom := hd atoms;
		av := atom.get_av();
		sti := av.get_sti();
		
		# Charge rent
		new_sti := sti - ab.af_rent;
		av.set_sti(new_sti);
		ab.sti_funds += ab.af_rent;
	}
}

AttentionBank.pay_wages(ab: self ref AttentionBank, atomspace: ref AtomSpace)
{
	# Pay wages to high-importance atoms
	if (ab.sti_funds <= 0)
		return;
	
	# Find top atoms by STI
	for (atoms := global_attentional_focus.atoms; atoms != nil; atoms = tl atoms) {
		atom := hd atoms;
		av := atom.get_av();
		
		# Pay STI wage
		if (ab.sti_funds >= ab.sti_wage) {
			sti := av.get_sti();
			av.set_sti(sti + ab.sti_wage);
			ab.sti_funds -= ab.sti_wage;
		}
		
		# Pay LTI wage
		lti := av.get_lti();
		av.set_lti(lti + ab.lti_wage);
	}
}

AttentionBank.decay_attention(ab: self ref AttentionBank, atomspace: ref AtomSpace)
{
	# Decay STI and LTI for all atoms
	for (i := 0; i < atomspace.size; i++) {
		atom := atomspace.atoms[i];
		if (atom != nil) {
			av := atom.get_av();
			
			# Decay STI
			av.decay_sti(ab.sti_decay_rate);
			
			# Decay LTI
			lti := av.get_lti();
			new_lti := int(real(lti) * (1.0 - ab.lti_decay_rate));
			av.set_lti(new_lti);
		}
	}
}

AttentionBank.get_stats(ab: self ref AttentionBank): string
{
	return sprint("AttentionBank: total_sti=%d, funds=%d, target=%d", 
		ab.total_sti, ab.sti_funds, ab.target_sti);
}

# AttentionalFocus methods
AttentionalFocus.init(af: self ref AttentionalFocus, max_size: int, min_sti: int)
{
	af.atoms = nil;
	af.size = 0;
	af.max_size = max_size;
	af.min_sti = min_sti;
}

AttentionalFocus.update(af: self ref AttentionalFocus, atomspace: ref AtomSpace)
{
	# Clear current focus
	af.atoms = nil;
	af.size = 0;
	
	# Build list of atoms with STI >= min_sti
	candidates: list of ref Atom = nil;
	for (i := 0; i < atomspace.size; i++) {
		atom := atomspace.atoms[i];
		if (atom != nil) {
			av := atom.get_av();
			if (av.get_sti() >= af.min_sti) {
				candidates = atom :: candidates;
			}
		}
	}
	
	# Sort by STI (simple insertion sort)
	sorted: list of ref Atom = nil;
	while (candidates != nil) {
		# Find max STI
		max_atom := hd candidates;
		max_sti := max_atom.get_av().get_sti();
		rest := tl candidates;
		
		for (atoms := rest; atoms != nil; atoms = tl atoms) {
			atom := hd atoms;
			sti := atom.get_av().get_sti();
			if (sti > max_sti) {
				max_atom = atom;
				max_sti = sti;
			}
		}
		
		# Add to sorted list
		sorted = max_atom :: sorted;
		
		# Remove from candidates
		new_candidates: list of ref Atom = nil;
		for (atoms = candidates; atoms != nil; atoms = tl atoms) {
			atom := hd atoms;
			if (atom != max_atom)
				new_candidates = atom :: new_candidates;
		}
		candidates = new_candidates;
	}
	
	# Take top max_size atoms
	count := 0;
	for (atoms := sorted; atoms != nil && count < af.max_size; atoms = tl atoms) {
		af.atoms = hd atoms :: af.atoms;
		count++;
	}
	af.size = count;
}

AttentionalFocus.contains(af: self ref AttentionalFocus, atom: ref Atom): int
{
	for (atoms := af.atoms; atoms != nil; atoms = tl atoms) {
		if (hd atoms == atom)
			return 1;
	}
	return 0;
}

AttentionalFocus.get_atoms(af: self ref AttentionalFocus): list of ref Atom
{
	return af.atoms;
}

AttentionalFocus.get_size(af: self ref AttentionalFocus): int
{
	return af.size;
}

AttentionalFocus.print_focus(af: self ref AttentionalFocus)
{
	print("Attentional Focus (%d atoms):\n", af.size);
	count := 0;
	for (atoms := af.atoms; atoms != nil && count < 10; atoms = tl atoms) {
		atom := hd atoms;
		av := atom.get_av();
		print("  %s STI=%d\n", atom.get_name(), av.get_sti());
		count++;
	}
	if (af.size > 10)
		print("  ... and %d more\n", af.size - 10);
}

# Stimulus methods
Stimulus.spread(s: self ref Stimulus, atomspace: ref AtomSpace, diffusion: real)
{
	# Spread stimulus to connected atoms
	source_av := s.atom.get_av();
	source_sti := source_av.get_sti();
	
	# Get incoming and outgoing atoms
	incoming := s.atom.get_incoming();
	outgoing := s.atom.get_outgoing();
	
	# Calculate spread amount
	spread_amount := int(real(s.amount) * diffusion);
	
	# Spread to incoming atoms
	for (atoms := incoming; atoms != nil; atoms = tl atoms) {
		target := hd atoms;
		target_av := target.get_av();
		target_sti := target_av.get_sti();
		target_av.set_sti(target_sti + spread_amount);
	}
	
	# Spread to outgoing atoms
	for (atoms = outgoing; atoms != nil; atoms = tl atoms) {
		target := hd atoms;
		target_av := target.get_av();
		target_sti := target_av.get_sti();
		target_av.set_sti(target_sti + spread_amount);
	}
}

# AttentionSpreading methods
AttentionSpreading.init(as: self ref AttentionSpreading)
{
	as.sources = nil;
	as.diffusion_rate = 0.5;  # 50% diffusion
}

AttentionSpreading.add_stimulus(as: self ref AttentionSpreading, atom: ref Atom, amount: int)
{
	stim := ref Stimulus;
	stim.atom = atom;
	stim.amount = amount;
	as.sources = stim :: as.sources;
}

AttentionSpreading.spread(as: self ref AttentionSpreading, atomspace: ref AtomSpace)
{
	# Spread stimulus from all sources
	for (sources := as.sources; sources != nil; sources = tl sources) {
		stim := hd sources;
		stim.spread(atomspace, as.diffusion_rate);
	}
}

AttentionSpreading.clear(as: self ref AttentionSpreading)
{
	as.sources = nil;
}

# HebbianUpdater methods
HebbianUpdater.init(hu: self ref HebbianUpdater, rate: real)
{
	hu.learning_rate = rate;
}

HebbianUpdater.update(hu: self ref HebbianUpdater, source: ref Atom, target: ref Atom)
{
	# Hebbian learning: "neurons that fire together wire together"
	source_av := source.get_av();
	target_av := target.get_av();
	
	source_sti := source_av.get_sti();
	target_sti := target_av.get_sti();
	
	# If both have high STI, strengthen connection
	if (source_sti > 0 && target_sti > 0) {
		# Increase LTI (long-term importance)
		source_lti := source_av.get_lti();
		target_lti := target_av.get_lti();
		
		delta := int(real(source_sti + target_sti) * hu.learning_rate);
		
		source_av.set_lti(source_lti + delta);
		target_av.set_lti(target_lti + delta);
	}
}

# Main attention cycle
attention_cycle(atomspace: ref AtomSpace)
{
	# 1. Update attention bank funds
	global_attention_bank.update_funds(atomspace);
	
	# 2. Decay attention values
	global_attention_bank.decay_attention(atomspace);
	
	# 3. Update attentional focus
	global_attentional_focus.update(atomspace);
	
	# 4. Collect rent from AF atoms
	global_attention_bank.collect_rent(atomspace);
	
	# 5. Pay wages to important atoms
	global_attention_bank.pay_wages(atomspace);
	
	# 6. Spread attention
	global_attention_spreading.spread(atomspace);
	global_attention_spreading.clear();
	
	# 7. Hebbian learning for AF atoms
	for (atoms := global_attentional_focus.atoms; atoms != nil; atoms = tl atoms) {
		atom := hd atoms;
		outgoing := atom.get_outgoing();
		for (targets := outgoing; targets != nil; targets = tl targets) {
			target := hd targets;
			global_hebbian_updater.update(atom, target);
		}
	}
}

# Utility functions
stimulate_atom(atom: ref Atom, amount: int)
{
	global_attention_spreading.add_stimulus(atom, amount);
}

get_attentional_focus(): list of ref Atom
{
	return global_attentional_focus.get_atoms();
}

get_af_size(): int
{
	return global_attentional_focus.get_size();
}

set_af_size(size: int)
{
	global_attentional_focus.max_size = size;
}
