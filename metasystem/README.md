# Metasystem - Viable System Implementation for OCC

Implementation of Eric Schwarz's organizational systems theory for the OpenCog Collection (OCC). This module provides a comprehensive framework for self-organizing, autopoietic cognitive systems with full metasystem capabilities.

## Overview

The metasystem implements the theoretical framework depicting viable natural systems through:

- **Three Planes of Existence**: Energy (physical), Information (relational), Totality (holistic)
- **Six Cycles**: Defining viable systems through horizontal (stability) and vertical (change) dynamics
- **Seven Steps of Self-Production**: The Spiral and Triad from Unity to Autogenesis
- **11-Stage Evolutionary Helix**: Long-term evolution through complexification to autonomy
- **Autopoiesis**: Self-production, boundary maintenance, and operational closure

## Theoretical Background

Based on Eric Schwarz's work on organizational systems theory, this implementation captures:

### Three Planes of Existence

1. **Plane of Energy (Objectal)**: Physical structures, objects in space-time
2. **Plane of Information (Relational)**: Relations, causal networks, logical world
3. **Plane of Totality (Existential)**: The whole, being, identity, self

### Six Cycles Defining Viable Systems

**Horizontal Cycles (Stability):**
- **Vortices (2)**: Cyclic exchanges of energy and matter (metabolism)
- **Retroaction Loops (3)**: Homeostasis and cybernetic controls
- **Self-Referential Loop (5)**: Self-knowledge from being-image dialogue

**Vertical Cycles (Change):**
- **Morphogenesis (1)**: Emergence, regeneration, evolution
- **Autopoiesis (4)**: Mutual production of network and structures
- **Autogenesis (6)**: Self-creation leading to autonomy

### Seven Steps of Self-Production (Spiral and Triad)

0. **Unity**: Entity in eigenbehavior
1. **Distinction**: Separation of ONE into TWO (morphogenesis)
2. **Distanciation**: Appearance of relations (vortices)
3. **Emergence**: Emergence of wholeness (self-regulation)
4. **Autopoiesis**: Self-productive dialogue
5. **Self-Reference**: Self-referential dialogue
6. **Autogenesis**: Autonomous whole

### 11-Stage Evolutionary Helix

Stages progress through three planes:

**Objectal Plane (0-2):**
- Entropic Drift, Self-Organization, Vortices

**Relational Plane (3-5):**
- Self-Regulation, Starting Self-Production, Information Drift

**Existential Plane (6-10):**
- Self-Production, Self-Reference, Referential Drift, Self-Creation, Toward Autonomy

## Installation

The metasystem is part of the OCC monorepo. No separate installation is required.

```python
from metasystem import Metasystem, MetasystemConfig
```

## Quick Start

```python
from metasystem import Metasystem, MetasystemConfig

# Create configuration
config = MetasystemConfig(
    initial_energy=0.7,
    initial_momentum=0.6,
    enable_autopoiesis=True,
    enable_evolution=True,
)

# Create and initialize metasystem
ms = Metasystem(config=config)
ms.initialize()

# Evolve the system
results = ms.evolve(30)

# Check progress
progress = ms.progress_summary()
print(f"Coherence: {progress['coherence']:.3f}")
print(f"Emergence: {progress['emergence']:.3f}")
print(f"Autonomy: {progress['autonomy']:.3f}")
print(f"Spiral step: {progress['spiral']['step']}")
print(f"Helix stage: {progress['helix']['stage']}")
print(f"System viable: {progress['viable']}")

# Self-reflection
reflection = ms.reflect()
print(f"Diagnosis: {reflection['diagnosis']}")
print(f"Recommendations: {reflection['recommendations']}")
```

## Module Structure

```
metasystem/
├── __init__.py           # Package initialization
├── planes.py             # Three planes of existence
├── cycles.py             # Six cycles
├── spiral_triad.py       # Seven steps of self-production
├── evolutionary_helix.py # 11-stage evolution
├── autopoiesis.py        # Autopoietic operations
├── metasystem_core.py    # Main orchestrator
├── integration.py        # OCC component bridges
├── README.md             # This file
└── tests/
    └── test_metasystem.py
```

## Components

### Planes (`planes.py`)

```python
from metasystem.planes import PlaneOfEnergy, PlaneOfInformation, PlaneOfTotality, ThreePlanes

# Create three planes
planes = ThreePlanes()

# Execute a step
result = planes.step(input_value=0.5)

# Get state
state = planes.state()
```

### Cycles (`cycles.py`)

```python
from metasystem.cycles import SixCycles

# Create all six cycles
cycles = SixCycles()

# Execute a step
result = cycles.step(0.5)

# Access individual cycles
vortex_state = cycles.vortex.state
```

### Spiral and Triad (`spiral_triad.py`)

```python
from metasystem.spiral_triad import SpiralTriad, SpiralStep

# Create spiral
spiral = SpiralTriad(energy=0.8)

# Advance through steps
result = spiral.advance(0.7)

# Check current step
print(spiral.current_step)  # SpiralStep.UNITY

# Run multiple steps
results = spiral.run_spiral(20)
```

### Evolutionary Helix (`evolutionary_helix.py`)

```python
from metasystem.evolutionary_helix import EvolutionaryHelix, HelixStage

# Create helix
helix = EvolutionaryHelix(momentum=0.6)

# Advance through stages
result = helix.advance(0.7)

# Check current stage and plane
print(helix.current_stage)  # HelixStage.ENTROPIC_DRIFT
print(helix.current_plane())  # HelixPlane.OBJECTAL

# Run evolution
results = helix.run_helix(50)
```

### Autopoiesis (`autopoiesis.py`)

```python
from metasystem.autopoiesis import AutopoieticSystem

# Create and initialize system
system = AutopoieticSystem()
system.initialize()

# Run autopoietic cycles
for i in range(20):
    result = system.autopoietic_cycle(0.7)

# Check if system is autopoietic
print(system.is_autopoietic())
print(f"Operational closure: {system.operational_closure:.3f}")
```

### Integration (`integration.py`)

```python
from metasystem.integration import IntegratedMetasystem, MetasystemConfig

# Create integrated system with OCC bridges
config = MetasystemConfig(enable_autopoiesis=True)
integrated = IntegratedMetasystem(config=config)
integrated.initialize()

# Run with OCC component states
result = integrated.integrated_step(
    aar_state={'coherence': 0.6, 'emergence': 0.4},
    synergy_state={'synergy_score': 0.5},
    identity_state={'self_awareness': 0.4}
)

# Get guidance for OCC components
guidance = result['guidance']
```

## Integration with OCC

The metasystem integrates with OCC components through bridges:

- **AARMetasystemBridge**: Links Agent-Arena-Relation framework
- **SynergyMetasystemBridge**: Connects synergy orchestrator
- **MembraneMetasystemBridge**: Integrates membrane system
- **IdentityMetasystemBridge**: Bridges CogPrime identity

## Testing

Run tests with pytest:

```bash
cd metasystem
pytest tests/ -v
```

## Scheme Implementation

A Scheme implementation is also available in `metamodel/metasystem.scm` for integration with the AtomSpace and Guile-based components.

```scheme
(use-modules (metamodel metasystem))

;; Create metasystem
(define ms (make-metasystem #:initial-energy 0.7))

;; Run evolution
(define results (metasystem-evolve ms 20))

;; Get state
(metasystem-state ms)
```

## References

- Eric Schwarz's organizational systems theory
- Maturana & Varela's theory of autopoiesis
- G. Spencer-Brown's Laws of Form
- OpenCog cognitive architecture

## License

Part of the OpenCog Collection (OCC) - see project LICENSE for details.
