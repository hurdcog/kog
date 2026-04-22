implement PLN;

#
# Probabilistic Logic Networks Module for Limbo
#
# Provides high-level PLN inference via the URE kernel service.
# Implements PLN formula wrappers (deduction, inversion, revision)
# and exposes them as a clean Limbo API.
#
# Copyright (C) 2026 OpenCog Community
# Licensed under AGPL-3.0
#

include "sys.m";
    sys: Sys;

include "draw.m";

PLN: module {
    PATH: con "/dis/inferno-kernel/pln.dis";

    init: fn();

    # Truth value type
    TruthValue: adt {
        strength:   real;
        confidence: real;

        # PLN formulas
        deduction:  fn(tv: self ref TruthValue, other: ref TruthValue,
                        link_tv: ref TruthValue): ref TruthValue;
        inversion:  fn(tv: self ref TruthValue,
                        a_prior: real, b_prior: real): ref TruthValue;
        revision:   fn(tv: self ref TruthValue, other: ref TruthValue): ref TruthValue;
        to_string:  fn(tv: self ref TruthValue): string;
    };

    # Create a simple truth value
    mktv: fn(strength: real, confidence: real): ref TruthValue;

    # PLN deduction rule: given P(A|B) and P(B|C), compute P(A|C)
    deduction: fn(ab: ref TruthValue, bc: ref TruthValue,
                   b_prior: real, c_prior: real): ref TruthValue;

    # PLN inversion rule: given P(A|B), compute P(B|A)
    inversion: fn(ab: ref TruthValue,
                   a_prior: real, b_prior: real): ref TruthValue;

    # PLN revision: merge two independent truth values
    revision: fn(tv1: ref TruthValue, tv2: ref TruthValue): ref TruthValue;

    # Compute count from confidence (PLN count formula)
    confidence_to_count: fn(c: real): real;

    # Compute confidence from count (PLN count formula)
    count_to_confidence: fn(k: real): real;
};

PLN_PRIOR := 0.01;    # Default prior
PLN_K     := 800.0;   # PLN confidence formula constant

init()
{
    sys = load Sys Sys->PATH;
}

mktv(strength: real, confidence: real): ref TruthValue
{
    tv := ref TruthValue;
    tv.strength   = strength;
    tv.confidence = confidence;
    return tv;
}

TruthValue.to_string(tv: self ref TruthValue): string
{
    return sys->sprint("<%.4f, %.4f>", tv.strength, tv.confidence);
}

#
# PLN Deduction rule
# sAC = sAB * sBC + (1 - sAB) * (sC - sBC * sB) / (1 - sB)
# (simplified when sB is not 1)
#
TruthValue.deduction(tv: self ref TruthValue, bc: ref TruthValue,
                     link_tv: ref TruthValue): ref TruthValue
{
    # tv = P(A|B), bc = P(B|C), link_tv used as prior carrier here
    sAB := tv.strength;
    sBC := bc.strength;
    sB  := link_tv.strength;

    sAC := sAB * sBC;
    if(sB < 1.0)
        sAC += (1.0 - sAB) * (sB - sBC * sB) / (1.0 - sB);

    nAB := confidence_to_count(tv.confidence);
    nBC := confidence_to_count(bc.confidence);
    nAC := nAB * nBC / (nAB + nBC + 1.0);
    cAC := count_to_confidence(nAC);

    return mktv(sAC, cAC);
}

#
# PLN Inversion rule
# sBA = sAB * sA / sB
#
TruthValue.inversion(tv: self ref TruthValue,
                     a_prior: real, b_prior: real): ref TruthValue
{
    sAB := tv.strength;
    if(b_prior <= 0.0)
        return mktv(0.0, 0.0);

    sBA := sAB * a_prior / b_prior;
    if(sBA > 1.0) sBA = 1.0;

    # Confidence is preserved (simplified)
    return mktv(sBA, tv.confidence);
}

#
# PLN Revision rule
# Weighted average of two independent estimates
#
TruthValue.revision(tv: self ref TruthValue, other: ref TruthValue): ref TruthValue
{
    return revision(tv, other);
}

deduction(ab: ref TruthValue, bc: ref TruthValue,
           b_prior: real, c_prior: real): ref TruthValue
{
    prior_tv := mktv(b_prior, c_prior);
    return ab.deduction(bc, prior_tv);
}

inversion(ab: ref TruthValue, a_prior: real, b_prior: real): ref TruthValue
{
    return ab.inversion(a_prior, b_prior);
}

revision(tv1: ref TruthValue, tv2: ref TruthValue): ref TruthValue
{
    n1 := confidence_to_count(tv1.confidence);
    n2 := confidence_to_count(tv2.confidence);
    total := n1 + n2;

    if(total <= 0.0)
        return mktv(0.5, 0.0);

    s_merged := (tv1.strength * n1 + tv2.strength * n2) / total;
    c_merged := count_to_confidence(total);
    if(c_merged > 1.0) c_merged = 1.0;

    return mktv(s_merged, c_merged);
}

confidence_to_count(c: real): real
{
    if(c >= 1.0) return PLN_K * 10000.0;
    if(c <= 0.0) return 0.0;
    return PLN_K * c / (1.0 - c);
}

count_to_confidence(k: real): real
{
    return k / (k + PLN_K);
}
