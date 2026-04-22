#pragma once

/**
 * ATenSpace - ATen Tensor-based adaptation of OpenCog AtomSpace
 * 
 * This library provides a hypergraph knowledge representation system
 * using ATen tensors for efficient storage and computation.
 * 
 * Core concepts:
 * - Atom: Base class for all knowledge units (nodes and links)
 * - Node: Represents entities/concepts, can have tensor embeddings
 * - Link: Represents relationships between atoms (hypergraph edges)
 * - AtomSpace: Container managing the hypergraph database
 * - TimeServer: Manages temporal information for atoms
 * - AttentionBank: Manages attention values and cognitive focus
 * - ECAN: Economic Attention Networks for attention allocation
 * - PatternMatcher: Pattern matching and unification engine
 * - TruthValue: PLN truth value formulas
 * - ForwardChainer: Forward chaining inference engine
 * - BackwardChainer: Backward chaining (goal-directed) inference
 * 
 * Features:
 * - Tensor-based truth values and embeddings
 * - Similarity-based queries using tensor operations
 * - Thread-safe atom management
 * - Immutable atoms with unique identity
 * - Temporal reasoning and time-stamped atoms
 * - Attention allocation mechanisms (ECAN)
 * - Persistent storage via serialization
 * - PLN (Probabilistic Logic Networks) reasoning
 * - Pattern matching with variable binding
 */

#include "Atom.h"
#include "AtomSpace.h"
#include "TimeServer.h"
#include "AttentionBank.h"
#include "Serializer.h"
#include "PatternMatcher.h"
#include "TruthValue.h"
#include "ForwardChainer.h"
#include "BackwardChainer.h"
#include "ECAN.h"

namespace at {
namespace atomspace {

/**
 * Convenience functions for creating atoms
 */

// Create a concept node
inline Atom::Handle createConceptNode(
    AtomSpace& space, 
    const std::string& name) {
    return space.addNode(Atom::Type::CONCEPT_NODE, name);
}

// Create a concept node with embedding
inline Atom::Handle createConceptNode(
    AtomSpace& space, 
    const std::string& name,
    const Tensor& embedding) {
    return space.addNode(Atom::Type::CONCEPT_NODE, name, embedding);
}

// Create a predicate node
inline Atom::Handle createPredicateNode(
    AtomSpace& space, 
    const std::string& name) {
    return space.addNode(Atom::Type::PREDICATE_NODE, name);
}

// Create a variable node (for pattern matching)
inline Atom::Handle createVariableNode(
    AtomSpace& space, 
    const std::string& name) {
    return space.addNode(Atom::Type::VARIABLE_NODE, name);
}

// Create an inheritance link: A inherits from B
inline Atom::Handle createInheritanceLink(
    AtomSpace& space,
    Atom::Handle from,
    Atom::Handle to) {
    return space.addLink(Atom::Type::INHERITANCE_LINK, {from, to});
}

// Create an implication link: A implies B (logical implication)
inline Atom::Handle createImplicationLink(
    AtomSpace& space,
    Atom::Handle antecedent,
    Atom::Handle consequent) {
    return space.addLink(Atom::Type::IMPLICATION_LINK, {antecedent, consequent});
}

// Create an evaluation link: predicate(args...)
inline Atom::Handle createEvaluationLink(
    AtomSpace& space,
    Atom::Handle predicate,
    const std::vector<Atom::Handle>& args) {
    // Create list link for arguments
    auto listLink = space.addLink(Atom::Type::LIST_LINK, args);
    // Create evaluation link
    return space.addLink(Atom::Type::EVALUATION_LINK, {predicate, listLink});
}

// Create a list link
inline Atom::Handle createListLink(
    AtomSpace& space,
    const std::vector<Atom::Handle>& atoms) {
    return space.addLink(Atom::Type::LIST_LINK, atoms);
}

// Create an AND link (logical conjunction)
inline Atom::Handle createAndLink(
    AtomSpace& space,
    const std::vector<Atom::Handle>& atoms) {
    return space.addLink(Atom::Type::AND_LINK, atoms);
}

// Create an OR link (logical disjunction)
inline Atom::Handle createOrLink(
    AtomSpace& space,
    const std::vector<Atom::Handle>& atoms) {
    return space.addLink(Atom::Type::OR_LINK, atoms);
}

// Create a NOT link (logical negation)
inline Atom::Handle createNotLink(
    AtomSpace& space,
    Atom::Handle atom) {
    return space.addLink(Atom::Type::NOT_LINK, {atom});
}

// Create a member link: element is member of set
inline Atom::Handle createMemberLink(
    AtomSpace& space,
    Atom::Handle element,
    Atom::Handle set) {
    return space.addLink(Atom::Type::MEMBER_LINK, {element, set});
}

// Create a subset link: subset is subset of superset
inline Atom::Handle createSubsetLink(
    AtomSpace& space,
    Atom::Handle subset,
    Atom::Handle superset) {
    return space.addLink(Atom::Type::SUBSET_LINK, {subset, superset});
}

// Create a context link: atom in context
inline Atom::Handle createContextLink(
    AtomSpace& space,
    Atom::Handle context,
    Atom::Handle atom) {
    return space.addLink(Atom::Type::CONTEXT_LINK, {context, atom});
}

// Create a sequential link: atoms in temporal sequence
inline Atom::Handle createSequentialLink(
    AtomSpace& space,
    const std::vector<Atom::Handle>& atoms) {
    return space.addLink(Atom::Type::SEQUENTIAL_LINK, atoms);
}

// Create a simultaneous link: atoms occurring simultaneously
inline Atom::Handle createSimultaneousLink(
    AtomSpace& space,
    const std::vector<Atom::Handle>& atoms) {
    return space.addLink(Atom::Type::SIMULTANEOUS_LINK, atoms);
}

// Create a similarity link: atom1 is similar to atom2
inline Atom::Handle createSimilarityLink(
    AtomSpace& space,
    Atom::Handle atom1,
    Atom::Handle atom2) {
    return space.addLink(Atom::Type::SIMILARITY_LINK, {atom1, atom2});
}

// Create an execution link: execute procedure with arguments
inline Atom::Handle createExecutionLink(
    AtomSpace& space,
    Atom::Handle procedure,
    const std::vector<Atom::Handle>& args) {
    auto listLink = space.addLink(Atom::Type::LIST_LINK, args);
    return space.addLink(Atom::Type::EXECUTION_LINK, {procedure, listLink});
}

// ECAN Hebbian link convenience functions

// Create a symmetric Hebbian link (mutual reinforcement)
inline Atom::Handle createSymmetricHebbianLink(
    AtomSpace& space,
    Atom::Handle atom1,
    Atom::Handle atom2) {
    return space.addLink(Atom::Type::SYMMETRIC_HEBBIAN_LINK, {atom1, atom2});
}

// Create an asymmetric Hebbian link (directional activation)
inline Atom::Handle createAsymmetricHebbianLink(
    AtomSpace& space,
    Atom::Handle source,
    Atom::Handle target) {
    return space.addLink(Atom::Type::ASYMMETRIC_HEBBIAN_LINK, {source, target});
}

// Create an inverse Hebbian link (inhibition)
inline Atom::Handle createInverseHebbianLink(
    AtomSpace& space,
    Atom::Handle atom1,
    Atom::Handle atom2) {
    return space.addLink(Atom::Type::INVERSE_HEBBIAN_LINK, {atom1, atom2});
}

// Create a generic Hebbian link
inline Atom::Handle createHebbianLink(
    AtomSpace& space,
    Atom::Handle atom1,
    Atom::Handle atom2) {
    return space.addLink(Atom::Type::HEBBIAN_LINK, {atom1, atom2});
}

} // namespace atomspace
} // namespace at
