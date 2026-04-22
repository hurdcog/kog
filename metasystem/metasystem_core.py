"""
Metasystem Core - Viable System Implementation

The central orchestrator that integrates all metasystem components:
- Three planes of existence
- Six cycles defining viable systems
- Seven steps of self-production (Spiral and Triad)
- 11-stage evolutionary helix
- Autopoietic operations

This module provides the unified interface for creating and
evolving complete metasystems with cognitive synergy capabilities.
"""

from dataclasses import dataclass, field
from typing import List, Dict, Any, Optional, Callable
from enum import Enum
import numpy as np

from .planes import ThreePlanes, PlaneOfEnergy, PlaneOfInformation, PlaneOfTotality
from .cycles import SixCycles
from .spiral_triad import SpiralTriad, SpiralStep
from .evolutionary_helix import EvolutionaryHelix, HelixStage, HelixPlane
from .autopoiesis import AutopoieticSystem


class MetasystemMode(Enum):
    """Operating modes of the metasystem"""
    DORMANT = "dormant"
    INITIALIZING = "initializing"
    OPERATING = "operating"
    EVOLVING = "evolving"
    REFLECTING = "reflecting"
    AUTONOMOUS = "autonomous"


@dataclass
class MetasystemConfig:
    """Configuration for the metasystem"""
    initial_energy: float = 0.5
    initial_momentum: float = 0.5
    decay_rate: float = 0.01
    production_rate: float = 0.1
    reflection_interval: int = 10  # Reflect every N cycles
    enable_autopoiesis: bool = True
    enable_evolution: bool = True


@dataclass
class MetasystemState:
    """Current state of the metasystem"""
    mode: MetasystemMode = MetasystemMode.DORMANT
    coherence: float = 0.0
    emergence: float = 0.0
    autonomy: float = 0.0
    cycle_count: int = 0

    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary"""
        return {
            'mode': self.mode.value,
            'coherence': self.coherence,
            'emergence': self.emergence,
            'autonomy': self.autonomy,
            'cycle_count': self.cycle_count,
        }


@dataclass
class Metasystem:
    """
    Complete Metasystem Implementation

    Integrates all components of Eric Schwarz's organizational
    systems theory into a unified viable system:

    - Three Planes: Energy, Information, Totality
    - Six Cycles: Vortex, Retroaction, Self-Reference,
                  Morphogenesis, Autopoiesis, Autogenesis
    - Seven Steps: Unity through Autogenesis (Spiral)
    - 11 Stages: Evolutionary Helix
    - Autopoiesis: Self-production and maintenance

    The metasystem provides:
    1. Self-production (autopoiesis)
    2. Self-reference (identity)
    3. Self-evolution (development)
    4. Self-organization (emergence)
    5. Self-determination (autonomy)
    """
    config: MetasystemConfig = field(default_factory=MetasystemConfig)
    state: MetasystemState = field(default_factory=MetasystemState)

    # Core components
    planes: ThreePlanes = field(default_factory=ThreePlanes)
    cycles: SixCycles = field(default_factory=SixCycles)
    spiral: SpiralTriad = field(default_factory=SpiralTriad)
    helix: EvolutionaryHelix = field(default_factory=EvolutionaryHelix)
    autopoiesis: AutopoieticSystem = field(default_factory=AutopoieticSystem)

    # History and metrics
    history: List[Dict[str, Any]] = field(default_factory=list)

    def __post_init__(self):
        """Initialize after dataclass creation"""
        self.spiral = SpiralTriad(energy=self.config.initial_energy)
        self.helix = EvolutionaryHelix(momentum=self.config.initial_momentum)

    def initialize(self) -> Dict[str, Any]:
        """Initialize the metasystem"""
        self.state.mode = MetasystemMode.INITIALIZING

        # Initialize autopoietic system
        if self.config.enable_autopoiesis:
            self.autopoiesis.initialize()

        self.state.mode = MetasystemMode.OPERATING

        return {
            'initialized': True,
            'mode': self.state.mode.value,
            'config': {
                'initial_energy': self.config.initial_energy,
                'initial_momentum': self.config.initial_momentum,
                'enable_autopoiesis': self.config.enable_autopoiesis,
                'enable_evolution': self.config.enable_evolution,
            }
        }

    def step(self, input_value: float = 0.5) -> Dict[str, Any]:
        """
        Execute one metasystem step

        Orchestrates all components in proper sequence:
        1. Planes step (physical, relational, existential)
        2. Cycles step (stability and change)
        3. Spiral advance (self-production steps)
        4. Helix advance (evolution)
        5. Autopoietic cycle (self-maintenance)
        6. Integration and reflection
        """
        self.state.cycle_count += 1
        result = {'cycle': self.state.cycle_count}

        # 1. Planes step
        planes_result = self.planes.step(input_value)
        result['planes'] = planes_result

        # 2. Cycles step
        cycles_result = self.cycles.step(input_value)
        result['cycles'] = cycles_result

        # 3. Spiral advance
        spiral_result = self.spiral.advance(input_value)
        result['spiral'] = spiral_result

        # 4. Helix advance (if evolution enabled)
        if self.config.enable_evolution:
            helix_input = (
                cycles_result['stability_index'] * 0.5 +
                cycles_result['change_index'] * 0.5
            )
            helix_result = self.helix.advance(helix_input)
            result['helix'] = helix_result

        # 5. Autopoietic cycle (if enabled)
        if self.config.enable_autopoiesis:
            external_state = self.planes.energy.flux
            autopoiesis_result = self.autopoiesis.autopoietic_cycle(
                input_value, external_state
            )
            result['autopoiesis'] = autopoiesis_result

        # 6. Integration - compute overall metrics
        self._integrate_metrics(result)
        result['state'] = self.state.to_dict()

        # 7. Periodic reflection
        if self.state.cycle_count % self.config.reflection_interval == 0:
            reflection = self.reflect()
            result['reflection'] = reflection

        # Record history
        self.history.append(result)

        return result

    def _integrate_metrics(self, step_result: Dict[str, Any]) -> None:
        """Integrate metrics from all components"""
        # Coherence from multiple sources
        plane_coherence = self.planes.coherence()
        cycle_stability = step_result.get('cycles', {}).get('stability_index', 0)
        spiral_integration = self.spiral.integration

        self.state.coherence = (
            plane_coherence * 0.3 +
            cycle_stability * 0.3 +
            spiral_integration * 0.4
        )

        # Emergence from creation and change
        cycle_change = step_result.get('cycles', {}).get('change_index', 0)
        spiral_step = self.spiral.current_step.value / 6.0
        helix_stage = self.helix.current_stage.value / 10.0

        self.state.emergence = (
            cycle_change * 0.3 +
            spiral_step * 0.35 +
            helix_stage * 0.35
        )

        # Autonomy from autogenesis and autopoiesis
        is_autonomous_spiral = self.spiral.current_step == SpiralStep.AUTOGENESIS
        helix_toward_autonomy = self.helix.current_stage.value >= 9

        autopoietic_closure = 0.0
        if self.config.enable_autopoiesis:
            autopoietic_closure = self.autopoiesis.operational_closure

        self.state.autonomy = (
            (1.0 if is_autonomous_spiral else 0.0) * 0.25 +
            (1.0 if helix_toward_autonomy else 0.0) * 0.25 +
            autopoietic_closure * 0.5
        )

        # Update mode based on autonomy
        if self.state.autonomy > 0.8:
            self.state.mode = MetasystemMode.AUTONOMOUS
        elif self.state.mode != MetasystemMode.REFLECTING:
            self.state.mode = MetasystemMode.EVOLVING if self.config.enable_evolution else MetasystemMode.OPERATING

    def reflect(self) -> Dict[str, Any]:
        """
        Metasystem self-reflection

        The metasystem reflects on its own state and organization,
        generating insights and recommendations for development.
        """
        previous_mode = self.state.mode
        self.state.mode = MetasystemMode.REFLECTING

        # Update totality plane self-image
        self.planes.totality.update_being({
            'coherence': self.state.coherence,
            'emergence': self.state.emergence,
            'autonomy': self.state.autonomy,
            'spiral_step': self.spiral.current_step.name,
            'helix_stage': self.helix.current_stage.name,
        })

        self.planes.totality.update_image({
            'coherence': self.state.coherence,
            'emergence': self.state.emergence,
            'autonomy': self.state.autonomy,
        })

        # Execute self-referential loop
        self_ref_result = self.planes.totality.self_referential_loop()

        # Generate diagnosis
        diagnosis = []
        recommendations = []

        if self.state.coherence < 0.3:
            diagnosis.append("Low coherence detected")
            recommendations.append("Strengthen self-referential cycles")
            recommendations.append("Increase retroaction stability")

        if self.state.emergence < 0.3:
            diagnosis.append("Low emergence detected")
            recommendations.append("Activate morphogenesis cycles")
            recommendations.append("Advance spiral through distinction")

        if self.state.autonomy < 0.3:
            diagnosis.append("Low autonomy detected")
            recommendations.append("Develop autopoietic closure")
            recommendations.append("Progress toward autogenesis")

        if not diagnosis:
            diagnosis.append("System operating within normal parameters")

        # Autogenesis check
        if self.config.enable_autopoiesis:
            autogenesis_result = self.planes.totality.autogenesis(
                self.spiral.integration
            )
        else:
            autogenesis_result = {'autonomous': False}

        self.state.mode = previous_mode

        return {
            'self_reference': self_ref_result,
            'autogenesis': autogenesis_result,
            'diagnosis': diagnosis,
            'recommendations': recommendations,
            'self_awareness': self_ref_result.get('self_awareness', 0),
            'metrics': {
                'coherence': self.state.coherence,
                'emergence': self.state.emergence,
                'autonomy': self.state.autonomy,
            }
        }

    def evolve(self, steps: int, input_generator: Optional[Callable] = None) -> List[Dict[str, Any]]:
        """
        Evolve the metasystem for multiple steps

        Args:
            steps: Number of evolution steps
            input_generator: Optional function generating input values

        Returns:
            List of step results
        """
        if input_generator is None:
            def input_generator(i):
                # Oscillating input with gradual increase
                base = 0.5 + 0.1 * (i / steps)
                oscillation = 0.2 * np.sin(i * 0.15)
                return min(1.0, base + oscillation)

        results = []
        for i in range(steps):
            input_value = input_generator(i)
            result = self.step(input_value)
            results.append(result)

        return results

    def get_state(self) -> Dict[str, Any]:
        """Get complete metasystem state"""
        return {
            'state': self.state.to_dict(),
            'planes': self.planes.state(),
            'cycles': self.cycles.state(),
            'spiral': self.spiral.state(),
            'helix': self.helix.state(),
            'autopoiesis': self.autopoiesis.get_state() if self.config.enable_autopoiesis else None,
            'history_length': len(self.history),
        }

    def coherence(self) -> float:
        """Get overall coherence"""
        return self.state.coherence

    def emergence_score(self) -> float:
        """Get emergence score"""
        return self.state.emergence

    def autonomy_level(self) -> float:
        """Get autonomy level"""
        return self.state.autonomy

    def is_viable(self) -> bool:
        """
        Check if metasystem is viable

        A viable system has:
        1. Sufficient coherence (stability)
        2. Ongoing emergence (development)
        3. Some autonomy (self-determination)
        """
        return (
            self.state.coherence > 0.3 and
            self.state.emergence > 0.2 and
            self.state.autonomy > 0.1
        )

    def is_autonomous(self) -> bool:
        """Check if metasystem has achieved autonomy"""
        return self.state.autonomy > 0.8

    def progress_summary(self) -> Dict[str, Any]:
        """Get summary of metasystem progress"""
        spiral_progress = self.spiral.current_step.value / 6.0
        helix_progress = self.helix.current_stage.value / 10.0

        autopoietic_viable = False
        if self.config.enable_autopoiesis:
            autopoietic_viable = self.autopoiesis.is_autopoietic()

        return {
            'cycle_count': self.state.cycle_count,
            'mode': self.state.mode.value,
            'coherence': self.state.coherence,
            'emergence': self.state.emergence,
            'autonomy': self.state.autonomy,
            'spiral': {
                'step': self.spiral.current_step.name,
                'progress': spiral_progress,
                'level': self.spiral.spiral_level,
            },
            'helix': {
                'stage': self.helix.current_stage.name,
                'progress': helix_progress,
                'spiral_level': self.helix.spiral_level,
                'plane': self.helix.current_plane().value,
            },
            'autopoiesis': {
                'enabled': self.config.enable_autopoiesis,
                'viable': autopoietic_viable,
                'closure': self.autopoiesis.operational_closure if self.config.enable_autopoiesis else 0,
            },
            'viable': self.is_viable(),
            'autonomous': self.is_autonomous(),
        }


def demonstrate_metasystem():
    """Demonstrate the complete metasystem"""
    print("=== Metasystem Demonstration ===\n")

    # Create metasystem
    config = MetasystemConfig(
        initial_energy=0.7,
        initial_momentum=0.6,
        enable_autopoiesis=True,
        enable_evolution=True,
    )
    metasystem = Metasystem(config=config)

    # Initialize
    print("1. Initializing Metasystem...")
    init_result = metasystem.initialize()
    print(f"   Initialized: {init_result['initialized']}")
    print(f"   Mode: {init_result['mode']}")
    print()

    # Initial state
    print("2. Initial State:")
    state = metasystem.get_state()
    print(f"   Coherence: {state['state']['coherence']:.3f}")
    print(f"   Emergence: {state['state']['emergence']:.3f}")
    print(f"   Autonomy: {state['state']['autonomy']:.3f}")
    print(f"   Spiral step: {state['spiral']['current_step']}")
    print(f"   Helix stage: {state['helix']['current_stage']}")
    print()

    # Evolve
    print("3. Evolving for 30 steps...")
    results = metasystem.evolve(30)
    print(f"   Completed {len(results)} steps")
    print()

    # Progress summary
    print("4. Progress Summary:")
    summary = metasystem.progress_summary()
    print(f"   Cycle count: {summary['cycle_count']}")
    print(f"   Mode: {summary['mode']}")
    print(f"   Coherence: {summary['coherence']:.3f}")
    print(f"   Emergence: {summary['emergence']:.3f}")
    print(f"   Autonomy: {summary['autonomy']:.3f}")
    print(f"   Spiral: {summary['spiral']['step']} (progress: {summary['spiral']['progress']:.1%})")
    print(f"   Helix: {summary['helix']['stage']} (progress: {summary['helix']['progress']:.1%})")
    print(f"   Autopoiesis viable: {summary['autopoiesis']['viable']}")
    print(f"   System viable: {summary['viable']}")
    print(f"   System autonomous: {summary['autonomous']}")
    print()

    # Final reflection
    print("5. Final Reflection:")
    reflection = metasystem.reflect()
    print(f"   Self-awareness: {reflection['self_awareness']:.3f}")
    print(f"   Diagnosis: {reflection['diagnosis']}")
    print(f"   Recommendations: {reflection['recommendations']}")
    print()

    print("=== Demonstration Complete ===")


if __name__ == "__main__":
    demonstrate_metasystem()
