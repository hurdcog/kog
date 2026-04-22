# ATenSpace - Tensor-based AtomSpace

ATenSpace is an ATen tensor-based adaptation of [OpenCog's AtomSpace](https://github.com/opencog/atomspace), providing a hypergraph knowledge representation system with efficient tensor operations.

## Overview

ATenSpace brings together symbolic AI (knowledge graphs) and neural AI (tensor embeddings) by representing knowledge as a hypergraph where:

- **Nodes** represent entities or concepts and can have tensor embeddings
- **Links** represent relationships between atoms (hypergraph edges)
- **Truth Values** are stored as tensors for flexible probabilistic reasoning
- **Similarity Queries** leverage tensor operations for efficient semantic search

## Core Concepts

### Atom
The base class for all knowledge units. Atoms are:
- **Immutable**: Once created, their core identity doesn't change
- **Unique**: AtomSpace ensures no duplicate atoms exist
- **Typed**: Each atom has a specific type (e.g., ConceptNode, InheritanceLink)

### Node
Represents entities or concepts. Features:
- Named entities (e.g., "cat", "mammal")
- Optional tensor embeddings for semantic representation
- Support for different node types (ConceptNode, PredicateNode, VariableNode)

### Link
Represents relationships between atoms. Features:
- Connects any number of atoms (not just pairs)
- Can connect nodes and other links (forming a hypergraph)
- Maintains ordered outgoing sets
- Support for various link types (InheritanceLink, EvaluationLink, logical links, etc.)

### AtomSpace
The container managing the hypergraph. Features:
- Thread-safe atom management
- Efficient indexing for fast lookup
- Similarity-based queries using tensor embeddings
- Incoming set tracking (what links reference each atom)

### TimeServer
Manages temporal information for atoms. Features:
- Tracks creation, access, and modification times
- Records custom events with timestamps
- Supports time-range queries
- Essential for episodic memory and temporal reasoning

### AttentionBank
Manages attention values and cognitive focus. Features:
- Short-Term Importance (STI) for current salience
- Long-Term Importance (LTI) for historical significance
- Very Long-Term Importance (VLTI) for archival value
- Attentional focus mechanism (top-K atoms)
- Attention dynamics (stimulation, decay, transfer)

### Serializer
Provides persistence support. Features:
- Save/load AtomSpace to/from files
- Text-based format for readability
- Preserves truth values and attention values
- Export to string representation

### PatternMatcher (NEW in Phase 2)
Pattern matching and unification engine. Features:
- Variable binding with VariableNode
- Pattern queries with wildcards
- Unification of structures
- Substitution of variables with bindings
- Essential for inference rules

### TruthValue (NEW in Phase 2)
PLN (Probabilistic Logic Networks) truth value formulas. Features:
- Strength-confidence representation [s, c]
- Deduction: (A→B, B→C) ⊢ A→C
- Induction: generalize from observations
- Abduction: reason to best explanation
- Revision: combine evidence
- Logical operations: conjunction, disjunction, negation
- Implication and similarity measures

### ForwardChainer (NEW in Phase 2)
Forward chaining inference engine. Features:
- Apply inference rules to derive new knowledge
- Deduction, induction, abduction rules
- Attention-guided inference
- Confidence threshold filtering
- Iterative rule application

### BackwardChainer (NEW in Phase 2)
Goal-directed backward chaining. Features:
- Prove goals by working backwards
- Construct proof trees
- Recursive subgoal solving
- Query provable facts
- Best proof selection by confidence

## Usage Example

```cpp
#include <ATen/atomspace/ATenSpace.h>

using namespace at::atomspace;

// Create an AtomSpace
AtomSpace space;

// Create concept nodes
auto cat = createConceptNode(space, "cat");
auto mammal = createConceptNode(space, "mammal");
auto animal = createConceptNode(space, "animal");

// Create inheritance hierarchy: cat -> mammal -> animal
auto inh1 = createInheritanceLink(space, cat, mammal);
auto inh2 = createInheritanceLink(space, mammal, animal);

// Create nodes with embeddings for similarity search
auto dog = createConceptNode(space, "dog", torch::randn({128}));
auto fish = createConceptNode(space, "fish", torch::randn({128}));

// Query for similar concepts
Tensor query = torch::randn({128});
auto results = space.querySimilar(query, /*k=*/5);

// Create relations using evaluation links
auto hasProperty = createPredicateNode(space, "has-property");
auto furry = createConceptNode(space, "furry");
auto eval = createEvaluationLink(space, hasProperty, {cat, furry});

// Set truth values (strength and confidence)
cat->setTruthValue(torch::tensor({0.9f, 0.8f}));

// Query atoms by type
auto conceptNodes = space.getAtomsByType(Atom::Type::CONCEPT_NODE);
```

## PLN (Probabilistic Logic Networks) Examples

### Pattern Matching with Variables

```cpp
#include <ATen/atomspace/ATenSpace.h>

using namespace at::atomspace;

AtomSpace space;

// Create knowledge base
auto cat = createConceptNode(space, "cat");
auto dog = createConceptNode(space, "dog");
auto mammal = createConceptNode(space, "mammal");

createInheritanceLink(space, cat, mammal);
createInheritanceLink(space, dog, mammal);

// Create pattern with variable
auto varX = createVariableNode(space, "$X");
auto pattern = createInheritanceLink(space, varX, mammal);

// Find all matches
auto matches = PatternMatcher::findMatches(space, pattern);
// Returns: cat→mammal and dog→mammal with bindings for $X
```

### Truth Value Reasoning

```cpp
// Deduction: A→B, B→C ⊢ A→C
auto tvAB = TruthValue::create(0.9f, 0.8f);  // [strength, confidence]
auto tvBC = TruthValue::create(0.85f, 0.9f);
auto tvAC = TruthValue::deduction(tvAB, tvBC);
// Result: A→C with computed probability

// Induction from observations
auto tvInduced = TruthValue::induction(8, 10);  // 8 positive out of 10
// Result: strength ≈ 0.8, confidence based on sample size

// Logical operations
auto tvA = TruthValue::create(0.8f, 0.9f);
auto tvB = TruthValue::create(0.7f, 0.85f);
auto tvAnd = TruthValue::conjunction(tvA, tvB);
auto tvOr = TruthValue::disjunction(tvA, tvB);
auto tvNot = TruthValue::negation(tvA);
```

### Forward Chaining Inference

```cpp
AtomSpace space;

// Build knowledge base
auto cat = createConceptNode(space, "cat");
auto mammal = createConceptNode(space, "mammal");
auto animal = createConceptNode(space, "animal");

auto catMammal = createInheritanceLink(space, cat, mammal);
catMammal->setTruthValue(TruthValue::create(0.95f, 0.9f));

auto mammalAnimal = createInheritanceLink(space, mammal, animal);
mammalAnimal->setTruthValue(TruthValue::create(0.98f, 0.95f));

// Run forward chaining
ForwardChainer chainer(space);
chainer.setConfidenceThreshold(0.1f);
int newInferences = chainer.run();

// Check inferred knowledge
auto catAnimal = space.getLink(Atom::Type::INHERITANCE_LINK, {cat, animal});
// catAnimal now exists with computed truth value
```

### Backward Chaining (Goal-Directed Reasoning)

```cpp
AtomSpace space;

// Facts
auto socrates = createConceptNode(space, "Socrates");
auto human = createConceptNode(space, "human");
auto mortal = createConceptNode(space, "mortal");

auto socratesHuman = createInheritanceLink(space, socrates, human);
socratesHuman->setTruthValue(TruthValue::create(1.0f, 0.95f));

auto humanMortal = createInheritanceLink(space, human, mortal);
humanMortal->setTruthValue(TruthValue::create(0.99f, 0.99f));

// Goal: Prove Socrates is mortal
auto goal = createInheritanceLink(space, socrates, mortal);

BackwardChainer chainer(space);
chainer.addRule(std::make_shared<DeductionRule>());

auto proofs = chainer.prove(goal);
// Returns proof tree showing: socrates→human, human→mortal ⊢ socrates→mortal

// Query truth value of goal
auto tv = chainer.query(goal);
// Returns high confidence truth value
```

### Attention-Guided Inference

```cpp
AtomSpace space;
AttentionBank attentionBank;

auto bird = createConceptNode(space, "bird");
auto canFly = createConceptNode(space, "can-fly");

auto birdFly = createInheritanceLink(space, bird, canFly);
birdFly->setTruthValue(TruthValue::create(0.9f, 0.8f));
attentionBank.setSTI(birdFly, 100.0f);  // High attention

ForwardChainer chainer(space);
int newInferences = chainer.run(&attentionBank);
// Inference prioritizes high-attention atoms
```

## API Reference

### Creating Atoms

#### Nodes
```cpp
// Basic node creation
Handle addNode(Atom::Type type, const std::string& name);

// Node with embedding
Handle addNode(Atom::Type type, const std::string& name, const Tensor& embedding);

// Convenience functions
Handle createConceptNode(AtomSpace& space, const std::string& name);
Handle createPredicateNode(AtomSpace& space, const std::string& name);
```

#### Links
```cpp
// Generic link creation
Handle addLink(Atom::Type type, const std::vector<Handle>& outgoing);

// Convenience functions
Handle createInheritanceLink(AtomSpace& space, Handle from, Handle to);
Handle createEvaluationLink(AtomSpace& space, Handle predicate, 
                            const std::vector<Handle>& args);
Handle createListLink(AtomSpace& space, const std::vector<Handle>& atoms);
```

### Querying Atoms

```cpp
// Get all atoms
AtomSet getAtoms() const;

// Get atoms by type
std::vector<Handle> getAtomsByType(Atom::Type type) const;

// Get specific node
Handle getNode(Atom::Type type, const std::string& name) const;

// Similarity search (for atoms with embeddings)
std::vector<std::pair<Handle, float>> querySimilar(
    const Tensor& query, 
    size_t k = 10, 
    float threshold = 0.0) const;

// Get number of atoms
size_t size() const;
```

### Atom Operations

```cpp
// Truth values
void setTruthValue(const Tensor& tv);
Tensor getTruthValue() const;

// Attention values
void setAttention(float attention);
float getAttention() const;

// Node-specific
std::string getName() const;  // For nodes only
void setEmbedding(const Tensor& embedding);
Tensor getEmbedding() const;

// Link-specific
const OutgoingSet& getOutgoingSet() const;  // For links only
size_t getArity() const;
Handle getOutgoingAtom(size_t index) const;

// Incoming set (what references this atom)
const std::vector<WeakHandle>& getIncomingSet() const;
```

### TimeServer Operations

```cpp
// Record temporal events
void recordCreation(Handle atom);
void recordAccess(Handle atom);
void recordModification(Handle atom);
void recordEvent(Handle atom, const std::string& eventDescription);

// Query temporal information
TemporalInfo getTemporalInfo(Handle atom) const;
TimePoint getCreationTime(Handle atom) const;
TimePoint getLastAccessTime(Handle atom) const;
TimePoint getLastModifiedTime(Handle atom) const;
std::vector<std::pair<TimePoint, std::string>> getEventHistory(Handle atom) const;

// Time-range queries
std::vector<Handle> getAtomsCreatedBetween(TimePoint start, TimePoint end) const;
std::vector<Handle> getAtomsAccessedBetween(TimePoint start, TimePoint end) const;
std::vector<Handle> getAtomsModifiedBetween(TimePoint start, TimePoint end) const;
```

### AttentionBank Operations

```cpp
// Set and get attention values
void setAttentionValue(Handle atom, const AttentionValue& av);
AttentionValue getAttentionValue(Handle atom) const;

// Update specific importance values
void updateSTI(Handle atom, float sti);
void updateLTI(Handle atom, float lti);
void updateVLTI(Handle atom, float vlti);

// Attention dynamics
void stimulate(Handle atom, float amount);
void decaySTI(float factor = 0.9f);
void transferSTI(Handle from, Handle to, float amount);

// Query by importance
std::vector<Handle> getAttentionalFocus() const;
std::vector<Handle> getAtomsAboveSTI(float threshold) const;
std::vector<std::pair<Handle, float>> getTopSTI(size_t n) const;
std::vector<std::pair<Handle, float>> getTopLTI(size_t n) const;

// Configuration
void setMaxAFSize(size_t size);
void setMinSTIThreshold(float threshold);
```

### Serialization Operations

```cpp
// Save and load AtomSpace
static bool save(const AtomSpace& space, const std::string& filename);
static bool load(AtomSpace& space, const std::string& filename);

// Export to string
static std::string toString(const AtomSpace& space);
```

### PLN Pattern Matching Operations (NEW)

```cpp
// Pattern matching with variables
bool match(const Handle& pattern, const Handle& target, VariableBinding& bindings);
std::vector<std::pair<Handle, VariableBinding>> findMatches(AtomSpace& space, 
                                                            const Handle& pattern);

// Substitution
Handle substitute(const Handle& pattern, const VariableBinding& bindings, 
                 AtomSpace& space);

// Unification
bool unify(const Handle& pattern1, const Handle& pattern2, 
          VariableBinding& bindings);

// Pattern queries
void query(AtomSpace& space, const Handle& pattern,
          std::function<void(const Handle&, const VariableBinding&)> callback);

// Pattern utilities
bool isVariable(const Handle& atom);
bool hasVariables(const Handle& pattern);
std::vector<Handle> getVariables(const Handle& pattern);
```

### PLN Truth Value Operations (NEW)

```cpp
// Create truth values [strength, confidence]
Tensor create(float strength, float confidence);
float getStrength(const Tensor& tv);
float getConfidence(const Tensor& tv);

// Inference formulas
Tensor deduction(const Tensor& tv1, const Tensor& tv2);      // A→B, B→C ⊢ A→C
Tensor induction(int positiveCount, int totalCount);         // Generalize from observations
Tensor abduction(const Tensor& tvB, const Tensor& tvAB);    // B, A→B ⊢ A
Tensor revision(const Tensor& tv1, const Tensor& tv2);       // Combine evidence

// Logical operations
Tensor conjunction(const Tensor& tvA, const Tensor& tvB);    // A ∧ B
Tensor disjunction(const Tensor& tvA, const Tensor& tvB);    // A ∨ B
Tensor negation(const Tensor& tvA);                          // ¬A
Tensor implication(const Tensor& tvA, const Tensor& tvB);    // A → B

// Utilities
float similarity(const Tensor& tv1, const Tensor& tv2);
Tensor indefinite(int positiveCount, int negativeCount);
Tensor defaultTV();
Tensor trueTV();
Tensor falseTV();
```

### Forward Chaining Operations (NEW)

```cpp
// Create forward chainer
ForwardChainer(AtomSpace& space);

// Configuration
void setMaxIterations(int maxIter);
void setConfidenceThreshold(float threshold);
void addRule(std::shared_ptr<InferenceRule> rule);

// Run inference
int run(AttentionBank* attentionBank = nullptr);  // Returns number of new atoms
int step(Handle target = nullptr, AttentionBank* attentionBank = nullptr);

// Apply rules manually
std::vector<Handle> applyRules(const std::vector<Handle>& premises);
```

### Backward Chaining Operations (NEW)

```cpp
// Create backward chainer
BackwardChainer(AtomSpace& space);

// Configuration
void setMaxDepth(int depth);
void setMaxProofs(int maxProofs);
void addRule(std::shared_ptr<InferenceRule> rule);

// Prove goals
std::vector<std::shared_ptr<Proof>> prove(const Handle& goal);
std::shared_ptr<Proof> getBestProof(const Handle& goal);
bool canProve(const Handle& goal);
Tensor query(const Handle& goal);  // Returns truth value if provable
```

### Inference Rules (NEW)

Built-in rules:
- `DeductionRule`: A→B, B→C ⊢ A→C
- `InductionRule`: Generalize from observations
- `AbductionRule`: B, A→B ⊢ A (reason to best explanation)

Custom rules can be created by inheriting from `InferenceRule`:
```cpp
class InferenceRule {
    virtual std::string getName() const = 0;
    virtual bool canApply(const std::vector<Handle>& premises) const = 0;
    virtual std::vector<Handle> apply(const std::vector<Handle>& premises, 
                                     AtomSpace& space) = 0;
};
```

## Atom Types

### Node Types
- `NODE` - Generic node
- `CONCEPT_NODE` - Represents a concept or entity
- `PREDICATE_NODE` - Represents a predicate/relation name
- `VARIABLE_NODE` - Represents a variable (for pattern matching)

### Link Types

#### Basic Links
- `LINK` - Generic link
- `INHERITANCE_LINK` - Represents inheritance (is-a) relationships
- `IMPLICATION_LINK` - Represents logical implication (A→B) **[NEW]**
- `EVALUATION_LINK` - Represents predicate evaluation
- `LIST_LINK` - Ordered list of atoms
- `ORDERED_LINK` - Generic ordered link
- `UNORDERED_LINK` - Generic unordered link

#### Logical Links
- `AND_LINK` - Logical conjunction
- `OR_LINK` - Logical disjunction
- `NOT_LINK` - Logical negation

#### Set Links
- `MEMBER_LINK` - Element membership in a set
- `SUBSET_LINK` - Subset relationship

#### Contextual Links
- `CONTEXT_LINK` - Contextual relationships

#### Temporal Links
- `SEQUENTIAL_LINK` - Temporal sequence
- `SIMULTANEOUS_LINK` - Simultaneous occurrence

#### Similarity Links
- `SIMILARITY_LINK` - Similarity relationship

#### Execution Links
- `EXECUTION_LINK` - Procedure execution

## Design Principles

1. **Tensor Integration**: All value representations use ATen tensors for efficiency and GPU compatibility
2. **Immutability**: Atoms are immutable after creation, ensuring cache consistency
3. **Uniqueness**: AtomSpace maintains a single instance of each unique atom
4. **Thread Safety**: All AtomSpace operations are thread-safe
5. **Hypergraph Structure**: Links can connect any number of atoms, not just pairs

## Comparison with OpenCog AtomSpace

| Feature | OpenCog AtomSpace | ATenSpace |
|---------|------------------|-----------|
| Data Structure | Hypergraph | Hypergraph |
| Storage | Custom C++ | ATen Tensors |
| Embeddings | Via Values | Native Tensor Support |
| GPU Support | Limited | Full via ATen |
| Similarity Search | Pattern Matching | Tensor Operations |
| Language | C++ (with Scheme) | C++ (with ATen) |

## Building and Running

```bash
# Navigate to the ATen directory
cd aten

# Build ATen with atomspace support
mkdir -p build && cd build
cmake ..
make

# Run the basic example
./atomspace_example

# Run the advanced features example
./atomspace_example_advanced

# Run the basic tests
./atomspace_test

# Run the advanced features tests
./atomspace_test_advanced
```

## Future Enhancements

Potential areas for expansion:
- Pattern matching and unification ✅ VariableNode added
- Backward chaining inference
- Distributed atomspace support
- ~~Persistent storage (serialization)~~ ✅ Implemented
- Python bindings
- GPU-accelerated operations
- Advanced query languages

## References

- [OpenCog AtomSpace](https://github.com/opencog/atomspace)
- [OpenCog Wiki](https://wiki.opencog.org/w/AtomSpace)
- [ATen (PyTorch Tensor Library)](https://pytorch.org/cppdocs/)

## License

This project follows the same license as the parent ATen/PyTorch project.
