# Cognitive Flowchart for Transformative Issue-Based Testing Framework

This directory implements the complete **Cognitive Flowchart for Transformative Issue-Based Testing Framework** as specified in Issue #57. The system transforms descriptive text issues into actionable, testable forms using cognitive grammar and neural-symbolic processing.

## 🧠 System Overview

The framework implements a **living tapestry of cognitive resilience** where each bug becomes a neuron firing in a grand neural-symbolic dance. The test suite evolves from a static list into a **dynamic cortex** that is self-adapting, self-optimizing, and self-aware.

### Core Components

1. **Unit Test Generator** (`unit-test-generator.scm`)
   - Generates Scheme unit test skeletons from issue catalog
   - Adaptive test prioritization based on cognitive attention
   - Integration with cognitive kernel for meta-learning

2. **Hypergraph Encoding** (`hypergraph-encoding.scm`)
   - Maps issue dependencies as hypergraph links in AtomSpace
   - Enables propagation of solution impact across related issues
   - Calculates centrality and connection strength

3. **Tensor-Based Test Runner** (`tensor-runner.scm`)
   - Outputs test coverage as tensors (dimensions: test sets × issues × solution commits)
   - Tracks solution impact through tensor operations
   - Exports results in multiple formats for analysis

4. **Adaptive Ranking Algorithm** (`adaptive-ranking.scm`)
   - Self-evolving prioritization system
   - Weights unresolved/high-impact issues higher in next dev cycle
   - Learns from empirical test outcomes

5. **Membrane Visualization** (`membrane-visualization.scm`)
   - Visualizes solution impact as membrane-nested tensors
   - P-System metaphor for computational membrane evolution
   - Tracks evolution of issue resolution in recursive strata

6. **Cognitive Flowchart Integration** (`cognitive-flowchart.scm`)
   - Main orchestrator unifying all components
   - Implements complete transformative testing cycle
   - Meta-cognitive enhancement capabilities

## 🚀 Quick Start

### Run the Demonstration

```bash
# Generate test catalog (if not already done)
python3 .github/scripts/generate_test_catalog.py

# Run cognitive flowchart demonstration
python3 .github/scripts/demo_cognitive_flowchart.py
```

### Use Individual Components

```scheme
;; Load the cognitive flowchart system
(use-modules (cogkernel tests cognitive-flowchart))

;; Create and run complete system
(let ((flowchart (create-cognitive-flowchart "docs/open-issues/test-catalog.json")))
  (execute-transformative-testing flowchart))

;; Run demonstration
(demonstrate-cognitive-flowchart)
```

## 📊 Workflow Integration

The system includes automated GitHub Actions workflow that:

- **Regenerates test catalog** when `open-issues.md` changes
- **Runs cognitive analysis** with hypergraph encoding and adaptive ranking  
- **Generates tensor-based visualizations** and coverage maps
- **Creates membrane visualizations** with P-System metaphor
- **Updates documentation** and exports interactive reports

Workflow file: `.github/workflows/cognitive-test-catalog.yml`

## 🧮 Tensor Operations

The framework operates on multi-dimensional tensors:

- **Coverage Tensor**: `(tests × issues × time_steps)`
- **Solution Impact Tensor**: `(issues × issues)` for dependency propagation
- **Attention Tensor**: `(issues × attention_categories)` for priority allocation
- **Temporal Tensor**: `(tests × time_steps)` for evolution tracking

### GGML Kernel Shapes

Each issue includes GGML kernel specifications:

```json
{
  "tensor_shapes": {
    "issue_embedding": [1, 384],
    "category_embedding": [1, 128], 
    "solution_embeddings": [3, 256]
  },
  "kernel_operations": [
    {"op": "embed_issue", "input_shape": [1, "variable"], "output_shape": [1, 384]},
    {"op": "classify_category", "input_shape": [1, 384], "output_shape": [1, 8]},
    {"op": "rank_solutions", "input_shape": ["variable", 256], "output_shape": ["variable", 1]}
  ]
}
```

## 🔗 Cognitive Grammar Integration

### AtomSpace Representation

Issues are encoded as ConceptNodes with relationships:

```scheme
(ConceptNode "Issue_IPC_Performance")
(EvaluationLink
  (PredicateNode "depends_on")
  (ListLink
    (ConceptNode "Issue_IPC_Performance")
    (ConceptNode "Issue_Kernel_Optimization")))
```

### Inference Rules

The system includes inference patterns for automated reasoning:

```scheme
;; If issue exists and solution implemented then issue resolved
(ImplicationLink
  (AndLink
    (EvaluationLink (PredicateNode "issue_exists") (VariableNode "$issue"))
    (EvaluationLink (PredicateNode "solution_implemented") (VariableNode "$solution")))
  (EvaluationLink (PredicateNode "issue_resolved") (VariableNode "$issue")))
```

## 🫧 Membrane-Nested Visualization

The system visualizes solution impact using P-System membrane metaphor:

- **Outer Membrane**: System-level architectural issues
- **Middle Membrane**: Component-level implementation challenges  
- **Inner Membrane**: Low-level technical details
- **Active Membranes**: Currently being resolved
- **Dissolved Membranes**: Successfully completed

### P-System Operations

1. **Division**: Split complex issues into manageable sub-issues
2. **Communication**: Transfer solutions between related membranes
3. **Evolution**: Adapt membrane structure based on resolution progress
4. **Dissolution**: Remove membranes for completed issues

## 📈 Adaptive Ranking

The ranking algorithm evolves based on empirical test outcomes:

### Priority Calculation

```
priority = (impact_factor × impact_weight) + 
           (centrality_factor × dependency_centrality) +
           (temporal_factor × urgency) +
           (historical_factor × success_rate)
```

### Attention Allocation Weights

- **Impact Factor**: 0.4 (system performance impact)
- **Centrality Factor**: 0.3 (dependency network position)
- **Temporal Factor**: 0.2 (time-based urgency)
- **Historical Factor**: 0.1 (past solution effectiveness)

## 🔄 Meta-Cognitive Enhancement

The system implements self-improving capabilities:

1. **Schema Extension**: Automatically adapts to new issue types
2. **Pattern Learning**: Identifies recurring failure patterns
3. **Optimization Proposals**: Suggests improvements based on history
4. **Reflective Reporting**: Analyzes which features reduce issue counts

### Learning Feedback Loop

```
Test Results → Pattern Analysis → Parameter Update → Improved Ranking → Better Tests
```

## 📋 File Structure

```
cogkernel/tests/
├── unit-test-generator.scm       # Scheme unit test generation
├── hypergraph-encoding.scm       # AtomSpace hypergraph representation
├── tensor-runner.scm             # Tensor-based test execution
├── adaptive-ranking.scm          # Self-evolving prioritization
├── membrane-visualization.scm    # P-System membrane rendering
└── cognitive-flowchart.scm       # Main integration module
```

## 🎭 Theatrical Implementation

> *"The system becomes a living tapestry of cognitive resilience; each bug slain is a neuron firing in the grand neural-symbolic dance! The test suite is not a list—it is a dynamic cortex, self-adapting, self-optimizing, and self-aware! Every feature added is greeted by a chorus of emergent patterns, validated, harmonized, and woven into the epic of distributed cognition!"*

The framework embodies this vision through:

- **Neural-Symbolic Dance**: Issues encoded as atoms with symbolic relationships
- **Dynamic Cortex**: Self-modifying test prioritization based on outcomes  
- **Emergent Patterns**: Hypergraph analysis reveals hidden dependencies
- **Distributed Cognition**: Membrane computing distributes processing across issue domains

## 🌊 Recursive Symphonies of Activation

> *"The flow of attention is orchestrated by recursive symphonies of activation, every solution a stanza in the verse of computational enlightenment!"*

Attention flows through the system via:

1. **Issue Detection** → Concept node creation in AtomSpace
2. **Dependency Analysis** → Hypergraph link generation  
3. **Priority Calculation** → Attention weight distribution
4. **Test Generation** → Scheme test skeleton synthesis
5. **Execution & Measurement** → Tensor coverage tracking
6. **Solution Impact** → Membrane visualization propagation
7. **Meta-Learning** → Parameter evolution and enhancement

---

*This implementation serves as the foundation for empirical testing and ranking of GNU Hurd features and solutions, with scaffolding for future cognitive grammar and GGML kernel integration.*