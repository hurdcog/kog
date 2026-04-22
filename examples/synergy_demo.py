#!/usr/bin/env python3
"""
Cognitive Synergy Demonstration

This example demonstrates the unified cognitive synergy API in action,
showing how AAR Core, Hypergraph Identity, and Membrane Manager work
together to create emergent intelligence.
"""

import sys
from pathlib import Path
import numpy as np
import time

# Add synergy module to path
sys.path.insert(0, str(Path(__file__).parent.parent))

try:
    from synergy.api import CognitiveSynergy, SynergyConfig
    SYNERGY_AVAILABLE = True
except ImportError as e:
    print(f"Error: Could not import synergy API: {e}")
    print("Please ensure synergy components are installed.")
    sys.exit(1)


def print_header(text):
    """Print a formatted header."""
    print("\n" + "=" * 60)
    print(text)
    print("=" * 60)


def print_metrics(metrics, indent=0):
    """Pretty print metrics dictionary."""
    prefix = "  " * indent
    for key, value in metrics.items():
        if isinstance(value, dict):
            print(f"{prefix}{key}:")
            print_metrics(value, indent + 1)
        elif isinstance(value, (list, np.ndarray)):
            if isinstance(value, np.ndarray):
                print(f"{prefix}{key}: array shape {value.shape}")
            else:
                print(f"{prefix}{key}: list length {len(value)}")
        elif isinstance(value, float):
            print(f"{prefix}{key}: {value:.4f}")
        else:
            print(f"{prefix}{key}: {value}")


def demo_basic_cycle():
    """Demonstrate a basic cognitive cycle."""
    print_header("Demo 1: Basic Cognitive Cycle")

    # Initialize with default configuration
    cs = CognitiveSynergy()
    print(f"Initialized: {cs}")

    # Create a random perception
    perception = np.random.randn(64)
    print(f"\nPerception shape: {perception.shape}")

    # Execute cognitive cycle
    print("\nExecuting cognitive cycle...")
    results = cs.cognitive_cycle(perception)

    print("\nCycle Results:")
    print(f"  Cycle number: {results['cycle']}")
    print(f"  Action shape: {len(results['action'])}")
    print(f"  Emergence score: {results['metrics']['aar']['emergence_score']:.4f}")
    print(f"  Coherence: {results['metrics']['aar']['coherence']:.4f}")

    print(f"\nFinal state: {cs}")


def demo_multiple_cycles():
    """Demonstrate learning over multiple cycles."""
    print_header("Demo 2: Learning Over Multiple Cycles")

    cs = CognitiveSynergy()

    print("Running 10 cognitive cycles...\n")

    emergence_scores = []
    coherence_scores = []

    for i in range(10):
        # Generate perception with some structure
        perception = np.random.randn(64) + np.sin(i * 0.5)

        results = cs.cognitive_cycle(perception)

        emergence = results['metrics']['aar']['emergence_score']
        coherence = results['metrics']['aar']['coherence']

        emergence_scores.append(emergence)
        coherence_scores.append(coherence)

        print(f"Cycle {i+1:2d}: Emergence={emergence:.4f}, Coherence={coherence:.4f}")

    print(f"\nAverage Emergence: {np.mean(emergence_scores):.4f}")
    print(f"Average Coherence: {np.mean(coherence_scores):.4f}")
    print(f"Synergy Score: {cs.get_synergy_score():.4f}")


def demo_hypergraph_identity():
    """Demonstrate hypergraph identity refinement."""
    print_header("Demo 3: Hypergraph Identity Refinement")

    config = SynergyConfig(
        enable_hypergraph=True,
        hypergraph_auto_refine=True,
        collect_metrics=True
    )
    cs = CognitiveSynergy(config=config)

    print("Running cycles with identity refinement...\n")

    for i in range(5):
        perception = np.random.randn(64)
        cs.cognitive_cycle(perception)

    metrics = cs.get_all_metrics()

    print("Hypergraph Identity State:")
    print(f"  Total nodes: {metrics['hypergraph']['total_nodes']}")
    print(f"  Total edges: {metrics['hypergraph']['total_edges']}")
    print(f"  Identity strength: {metrics['hypergraph']['identity_strength']:.4f}")
    print(f"  Core attributes: {len(metrics['hypergraph']['core_attributes'])}")


def demo_custom_configuration():
    """Demonstrate custom configuration."""
    print_header("Demo 4: Custom Configuration")

    # Create custom configuration
    config = SynergyConfig(
        aar_dimension=32,  # Smaller dimension for faster processing
        aar_learning_rate=0.05,  # Higher learning rate
        enable_hypergraph=True,
        enable_membranes=True,
        collect_metrics=True,
        metrics_history_size=20
    )

    print("Configuration:")
    print(f"  AAR Dimension: {config.aar_dimension}")
    print(f"  Learning Rate: {config.aar_learning_rate}")
    print(f"  Hypergraph: {config.enable_hypergraph}")
    print(f"  Membranes: {config.enable_membranes}")
    print(f"  Metrics History: {config.metrics_history_size}")

    cs = CognitiveSynergy(config=config)

    print("\nRunning 3 cycles with custom config...")
    for _ in range(3):
        perception = np.random.randn(32)  # Match custom dimension
        cs.cognitive_cycle(perception)

    print(f"\nFinal synergy score: {cs.get_synergy_score():.4f}")


def demo_comprehensive_metrics():
    """Demonstrate comprehensive metrics collection."""
    print_header("Demo 5: Comprehensive Metrics")

    cs = CognitiveSynergy()

    # Run several cycles
    for _ in range(3):
        perception = np.random.randn(64)
        cs.cognitive_cycle(perception)

    # Get all metrics
    all_metrics = cs.get_all_metrics()

    print("Complete System Metrics:")
    print_metrics(all_metrics)


def demo_performance_benchmark():
    """Benchmark cognitive cycle performance."""
    print_header("Demo 6: Performance Benchmark")

    cs = CognitiveSynergy()

    print("Benchmarking 100 cognitive cycles...\n")

    start_time = time.time()

    for _ in range(100):
        perception = np.random.randn(64)
        cs.cognitive_cycle(perception)

    elapsed = time.time() - start_time

    print(f"Total time: {elapsed:.3f}s")
    print(f"Average cycle time: {elapsed/100*1000:.2f}ms")
    print(f"Cycles per second: {100/elapsed:.2f}")


def demo_synergy_evolution():
    """Demonstrate synergy score evolution over time."""
    print_header("Demo 7: Synergy Score Evolution")

    cs = CognitiveSynergy()

    print("Tracking synergy score over 20 cycles...\n")

    scores = []
    for i in range(20):
        perception = np.random.randn(64)
        cs.cognitive_cycle(perception)
        score = cs.get_synergy_score()
        scores.append(score)

        if (i + 1) % 5 == 0:
            print(f"Cycle {i+1:2d}: Synergy Score = {score:.4f}")

    print(f"\nInitial score: {scores[0]:.4f}")
    print(f"Final score: {scores[-1]:.4f}")
    print(f"Average score: {np.mean(scores):.4f}")
    print(f"Score trend: {'+' if scores[-1] > scores[0] else '-'}{abs(scores[-1] - scores[0]):.4f}")


def main():
    """Run all demonstrations."""
    print("\n" + "=" * 60)
    print("COGNITIVE SYNERGY DEMONSTRATION")
    print("OpenCog Collection - Unified Synergy API")
    print("=" * 60)

    demos = [
        demo_basic_cycle,
        demo_multiple_cycles,
        demo_hypergraph_identity,
        demo_custom_configuration,
        demo_comprehensive_metrics,
        demo_performance_benchmark,
        demo_synergy_evolution
    ]

    for demo in demos:
        try:
            demo()
            time.sleep(0.5)  # Brief pause between demos
        except Exception as e:
            print(f"\n❌ Error in {demo.__name__}: {e}")
            import traceback
            traceback.print_exc()

    print("\n" + "=" * 60)
    print("DEMONSTRATION COMPLETE")
    print("=" * 60)
    print("\nFor more information, see:")
    print("  - synergy/README_COGNITIVE_SYNERGY.md")
    print("  - COGNITIVE_SYNERGY_IMPROVEMENTS_PLAN.md")
    print("  - synergy/api/unified_api.py")


if __name__ == "__main__":
    main()
