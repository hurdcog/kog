implement AttentionModule;

# Inferno Kernel Module: Economic Attention Network (ECAN)
# Implements attention allocation as a kernel-level service
# Atoms compete for limited attention resources using economic principles

include "sys.m";
	sys: Sys;
	print, fprint, sprint: import sys;

include "draw.m";

include "math.m";
	math: Math;
	sqrt, exp, log: import math;

include "../atomspace/atomspace_kern.m";
	atomspace: AtomSpaceKern;
	Atom, AtomSpace, AttentionValue: import atomspace;

AttentionModule: module {
	PATH: con "/dis/inferno-kernel/attention.dis";

	# Attention Bank - manages the economy of attention
	AttentionBank: adt {
		total_sti: int;           # Total STI in the system
		sti_funds: int;           # Available STI funds
		lti_funds: int;           # Available LTI funds
		target_sti: int;          # Target total STI
		max_af_size: int;         # Maximum attentional focus size
		af_threshold: int;        # Minimum STI for attentional focus
		rent_rate: real;          # Rent collection rate (fraction)
		wage_rate: real;          # Wage distribution rate
	};

	# Attention statistics
	AttentionStats: adt {
		atoms_in_focus: int;
		total_stimulations: int;
		total_spreads: int;
		total_rent_collected: int;
		avg_sti: real;
		max_sti: int;
		min_sti: int;
	};

	# Core operations
	init: fn(as: ref AtomSpace): ref AttentionBank;
	shutdown: fn(bank: ref AttentionBank);

	# Stimulation
	stimulate: fn(bank: ref AttentionBank, atom_id: int, amount: int): int;
	spread_importance: fn(bank: ref AttentionBank, source: int);

	# Attentional focus
	get_focus: fn(bank: ref AttentionBank): list of int;
	set_af_threshold: fn(bank: ref AttentionBank, threshold: int);
	set_af_max_size: fn(bank: ref AttentionBank, size: int);

	# Economic operations
	collect_rent: fn(bank: ref AttentionBank): int;
	distribute_wages: fn(bank: ref AttentionBank);

	# Statistics
	get_stats: fn(bank: ref AttentionBank): AttentionStats;
};

# Initialize attention bank
init(as: ref AtomSpace): ref AttentionBank
{
	bank := ref AttentionBank;
	bank.total_sti = 0;
	bank.sti_funds = 10000;
	bank.lti_funds = 10000;
	bank.target_sti = 10000;
	bank.max_af_size = 100;
	bank.af_threshold = 50;
	bank.rent_rate = 0.1;
	bank.wage_rate = 0.05;

	print("ECAN Attention Bank initialized\n");
	print("  STI funds: " + string bank.sti_funds + "\n");
	print("  AF threshold: " + string bank.af_threshold + "\n");
	return bank;
}

# Shutdown attention bank
shutdown(bank: ref AttentionBank)
{
	print("ECAN Attention Bank shutdown\n");
	print("  Final total STI: " + string bank.total_sti + "\n");
}

# Stimulate an atom with attention
stimulate(bank: ref AttentionBank, atom_id: int, amount: int): int
{
	# Bound by available funds
	actual := amount;
	if (actual > bank.sti_funds)
		actual = bank.sti_funds;

	bank.sti_funds -= actual;
	bank.total_sti += actual;

	return actual;
}

# Spread importance from a source atom to its neighbors
spread_importance(bank: ref AttentionBank, source: int)
{
	# In a full implementation, this would:
	# 1. Get the source atom's outgoing links
	# 2. Distribute a fraction of its STI to neighbors
	# 3. Adjust based on link weights
}

# Get atoms in attentional focus
get_focus(bank: ref AttentionBank): list of int
{
	focus: list of int;
	focus = nil;
	# Would query AtomSpace for atoms with STI >= af_threshold
	return focus;
}

# Set attentional focus threshold
set_af_threshold(bank: ref AttentionBank, threshold: int)
{
	bank.af_threshold = threshold;
}

# Set maximum attentional focus size
set_af_max_size(bank: ref AttentionBank, size: int)
{
	bank.max_af_size = size;
}

# Collect rent from all atoms (economic pressure)
collect_rent(bank: ref AttentionBank): int
{
	collected := 0;
	# Would iterate over all atoms, collect rent proportional to STI
	# Atoms that can't pay rent lose importance
	return collected;
}

# Distribute wages to useful atoms
distribute_wages(bank: ref AttentionBank)
{
	# Would distribute STI to atoms that participated in
	# successful inferences or pattern matches
}

# Get attention statistics
get_stats(bank: ref AttentionBank): AttentionStats
{
	stats: AttentionStats;
	stats.atoms_in_focus = 0;
	stats.total_stimulations = 0;
	stats.total_spreads = 0;
	stats.total_rent_collected = 0;
	stats.avg_sti = 0.0;
	stats.max_sti = 0;
	stats.min_sti = 0;
	return stats;
}
