"""
Spiral and Triad - Seven Steps of Self-Production

Based on Eric Schwarz's "The Spiral and the Triad: The Seven Steps
of the Self-Production of the Triad."

The seven steps describe the evolution from:
- Unity (0) - Entity in eigenbehavior
- Through Duality - Distinction and distanciation
- To Identity (6) - Autonomous whole through autogenesis

This is the fundamental developmental pattern that generates
viable self-organizing systems through progressive differentiation
and integration.
"""

from dataclasses import dataclass, field
from typing import List, Dict, Any, Optional, Tuple
from enum import Enum
import numpy as np


class SpiralStep(Enum):
    """
    The seven steps of self-production

    0 - UNITY: Entity in eigenbehavior, being without relations
    1 - DISTINCTION: Separation of ONE into TWO, morphogenesis
    2 - DISTANCIATION: Appearance of relations, vortices
    3 - EMERGENCE: Emergence of wholeness, self-regulation
    4 - AUTOPOIESIS: Self-productive dialogue
    5 - SELF_REFERENCE: Self-referential dialogue
    6 - AUTOGENESIS: Autonomous whole
    """
    UNITY = 0
    DISTINCTION = 1
    DISTANCIATION = 2
    EMERGENCE = 3
    AUTOPOIESIS = 4
    SELF_REFERENCE = 5
    AUTOGENESIS = 6


# Detailed descriptions for each step
STEP_DESCRIPTIONS = {
    SpiralStep.UNITY: {
        'name': 'Unity',
        'description': 'Spontaneous state of being, entity in eigenbehavior',
        'characteristics': ['vacuity', 'chaos', 'undifferentiated', 'indefinite'],
        'outside_time': 'Fluctuating vacuum, undifferentiated being/non-being',
        'within_time': 'Simple physical systems, vacuum fluctuations',
        'process': 'Eigen drift: being without relations',
        'produces': [],
    },
    SpiralStep.DISTINCTION: {
        'name': 'Distinction',
        'description': 'Separation of ONE into TWO, morphogenesis',
        'characteristics': ['duality', 'difference', 'alterity', 'reproduction'],
        'process': 'Division, replication, breeding of structures',
        'produces': ['differentiation', 'polarization', 'duality'],
        'tropic_drift': 'Entropic drift toward probable states',
    },
    SpiralStep.DISTANCIATION: {
        'name': 'Distanciation',
        'description': 'Establishment of relations between parts (enfolding)',
        'characteristics': ['complementarity', 'enantiodromia', 'yin-yang', 'vortex'],
        'process': 'Differentiation and polarization of opposites',
        'produces': ['circular_relation', 'material_perennity', 'oscillations'],
        'creates': 'Space-time circular relation (vortex)',
    },
    SpiralStep.EMERGENCE: {
        'name': 'Emergence of Wholeness',
        'description': 'Integration leading to coherence, containing the third',
        'characteristics': ['wholeness', 'self_regulation', 'correlation', 'paradox'],
        'process': 'Manifestation of network of relations on structure',
        'produces': ['coherence', 'stability', 'functional_unity'],
        'creates': 'Correlation between physical structure and logical network',
    },
    SpiralStep.AUTOPOIESIS: {
        'name': 'Autopoiesis',
        'description': 'Mutual production of physical structure and logical network',
        'characteristics': ['self_production', 'emergence_of_self_reference', 'dialogue'],
        'process': 'Self-productive dialogue between parts and relations producing whole',
        'produces': ['self_maintaining_structure', 'operational_closure'],
        'creates': 'Self-producing structures-organizations with emergence of self-reference',
    },
    SpiralStep.SELF_REFERENCE: {
        'name': 'Self-Reference',
        'description': 'Dialogue between compatible structures leading to identity',
        'characteristics': ['self_knowledge', 'object_image_dialogue', 'autonomy_emergence'],
        'process': 'Self-referential dialogue between self-productive structures and network',
        'produces': ['identity', 'self_awareness', 'mutual_recognition'],
        'creates': 'Identity between parts, relations, network and whole',
    },
    SpiralStep.AUTOGENESIS: {
        'name': 'Autogenesis',
        'description': 'Self-creation by metacoupling, leading to full autonomy',
        'characteristics': ['identity', 'unity', 'self_creation', 'autonomy'],
        'process': 'Self-creation through metacoupling between autopoietic dialogue and identity',
        'produces': ['autonomous_whole', 'self_creating_entity'],
        'creates': 'Entity producing its own rules of production',
    },
}


@dataclass
class SpiralTriad:
    """
    The Spiral and Triad - Seven Steps of Self-Production

    Manages the developmental progression from Unity through Autogenesis,
    tracking energy, integration, and the emergent qualities at each step.
    """
    current_step: SpiralStep = SpiralStep.UNITY
    energy: float = 1.0
    integration: float = 0.0
    history: List[Dict[str, Any]] = field(default_factory=list)
    spiral_level: int = 0  # Number of complete spirals

    # Thresholds for advancing
    advancement_thresholds: Dict[SpiralStep, Tuple[float, float]] = field(
        default_factory=lambda: {
            SpiralStep.UNITY: (0.5, 0.0),        # (energy, integration)
            SpiralStep.DISTINCTION: (0.4, 0.0),
            SpiralStep.DISTANCIATION: (0.35, 0.1),
            SpiralStep.EMERGENCE: (0.3, 0.2),
            SpiralStep.AUTOPOIESIS: (0.25, 0.4),
            SpiralStep.SELF_REFERENCE: (0.2, 0.6),
            SpiralStep.AUTOGENESIS: (0.15, 0.8),
        }
    )

    def step_info(self) -> Dict[str, Any]:
        """Get detailed information about current step"""
        return {
            'step': self.current_step.value,
            'name': self.current_step.name,
            **STEP_DESCRIPTIONS.get(self.current_step, {}),
            'energy': self.energy,
            'integration': self.integration,
            'spiral_level': self.spiral_level,
        }

    def _execute_step_unity(self, input_energy: float) -> Dict[str, Any]:
        """
        Step 0: Unity - Entity in eigenbehavior

        Spontaneous state of being without relations.
        Vacuity, chaos, undifferentiated being/non-being.
        """
        # In unity, energy accumulates
        self.energy = min(1.0, self.energy + 0.1 * input_energy)

        return {
            'step': SpiralStep.UNITY,
            'action': 'accumulating_potential',
            'state': 'undifferentiated',
            'energy': self.energy,
        }

    def _execute_step_distinction(self, input_energy: float) -> Dict[str, Any]:
        """
        Step 1: Distinction - Separation of ONE into TWO

        Morphogenesis: division, replication, differentiation.
        Production of the same-different.
        """
        # Energy consumed in differentiation
        self.energy *= 0.9

        return {
            'step': SpiralStep.DISTINCTION,
            'action': 'differentiating',
            'state': 'duality_emerging',
            'energy': self.energy,
            'produces': ['differentiation', 'polarization'],
        }

    def _execute_step_distanciation(self, input_energy: float) -> Dict[str, Any]:
        """
        Step 2: Distanciation - Appearance of relations

        Establishment of actual interactions and potential relations
        between parts (enfolding). Vortex creation.
        """
        self.energy *= 0.85
        self.integration += 0.1

        return {
            'step': SpiralStep.DISTANCIATION,
            'action': 'creating_relations',
            'state': 'vortex_forming',
            'energy': self.energy,
            'integration': self.integration,
            'produces': ['circular_relation', 'oscillations'],
        }

    def _execute_step_emergence(self, input_energy: float) -> Dict[str, Any]:
        """
        Step 3: Emergence of Wholeness - Self-regulation

        Integration leading to coherence or paradox.
        Containing third (instead of excluded third).
        """
        self.energy *= 0.8
        self.integration += 0.2

        return {
            'step': SpiralStep.EMERGENCE,
            'action': 'integrating',
            'state': 'wholeness_emerging',
            'energy': self.energy,
            'integration': self.integration,
            'produces': ['coherence', 'stability', 'self_regulation'],
        }

    def _execute_step_autopoiesis(self, input_energy: float) -> Dict[str, Any]:
        """
        Step 4: Autopoiesis - Self-productive dialogue

        Mutual production of the physical structure and
        the logical network of the organism.
        """
        self.energy *= 0.75
        self.integration += 0.25

        return {
            'step': SpiralStep.AUTOPOIESIS,
            'action': 'self_producing',
            'state': 'autopoietic_closure',
            'energy': self.energy,
            'integration': self.integration,
            'produces': ['self_maintaining_structure', 'operational_closure'],
        }

    def _execute_step_self_reference(self, input_energy: float) -> Dict[str, Any]:
        """
        Step 5: Self-Referential Dialogue - Self-reference

        Dialogue between ever more compatible and self-productive
        structures and network, leading to identity.
        """
        self.energy *= 0.7
        self.integration += 0.25

        return {
            'step': SpiralStep.SELF_REFERENCE,
            'action': 'self_reflecting',
            'state': 'identity_forming',
            'energy': self.energy,
            'integration': self.integration,
            'produces': ['identity', 'self_awareness', 'mutual_recognition'],
        }

    def _execute_step_autogenesis(self, input_energy: float) -> Dict[str, Any]:
        """
        Step 6: Autogenesis - Autonomous Whole

        Self-creation by metacoupling between autopoietic
        dialogue and identity emerging from the dialogue.
        """
        self.energy *= 0.65
        self.integration = min(1.0, self.integration + 0.2)

        return {
            'step': SpiralStep.AUTOGENESIS,
            'action': 'self_creating',
            'state': 'autonomous',
            'energy': self.energy,
            'integration': self.integration,
            'produces': ['autonomous_whole', 'self_creating_entity'],
            'complete': True,
        }

    def execute_current_step(self, input_energy: float = 0.5) -> Dict[str, Any]:
        """Execute the current step with given input energy"""
        step_methods = {
            SpiralStep.UNITY: self._execute_step_unity,
            SpiralStep.DISTINCTION: self._execute_step_distinction,
            SpiralStep.DISTANCIATION: self._execute_step_distanciation,
            SpiralStep.EMERGENCE: self._execute_step_emergence,
            SpiralStep.AUTOPOIESIS: self._execute_step_autopoiesis,
            SpiralStep.SELF_REFERENCE: self._execute_step_self_reference,
            SpiralStep.AUTOGENESIS: self._execute_step_autogenesis,
        }

        method = step_methods.get(self.current_step)
        if method:
            result = method(input_energy)
            self.history.append(result)
            return result
        return {'error': 'Unknown step'}

    def can_advance(self) -> bool:
        """Check if conditions are met to advance to next step"""
        threshold = self.advancement_thresholds.get(self.current_step)
        if threshold:
            energy_threshold, integration_threshold = threshold
            return (self.energy >= energy_threshold and
                    self.integration >= integration_threshold)
        return False

    def advance(self, input_energy: float = 0.5) -> Dict[str, Any]:
        """
        Execute current step and advance if conditions are met

        Returns result of step execution and advancement status.
        """
        # Execute current step
        result = self.execute_current_step(input_energy)

        # Check for advancement
        if self.can_advance():
            current_value = self.current_step.value

            if current_value >= 6:  # Completed autogenesis
                # Complete spiral, restart with energy boost
                self.spiral_level += 1
                self.current_step = SpiralStep.UNITY
                self.energy *= 1.2  # Energy gain from completion
                self.integration = 0.0  # Reset integration for new spiral

                return {
                    **result,
                    'advanced': True,
                    'from': SpiralStep.AUTOGENESIS,
                    'to': SpiralStep.UNITY,
                    'spiral_complete': True,
                    'spiral_level': self.spiral_level,
                }
            else:
                # Advance to next step
                self.current_step = SpiralStep(current_value + 1)
                return {
                    **result,
                    'advanced': True,
                    'from': SpiralStep(current_value),
                    'to': self.current_step,
                }

        return {
            **result,
            'advanced': False,
            'at': self.current_step,
        }

    def run_spiral(self, steps: int, input_generator=None) -> List[Dict[str, Any]]:
        """
        Run the spiral for a number of steps

        Args:
            steps: Number of steps to run
            input_generator: Callable that generates input energy (default: oscillating)

        Returns:
            List of step results
        """
        if input_generator is None:
            def input_generator(i):
                return 0.5 + 0.3 * np.sin(i * 0.2)

        results = []
        for i in range(steps):
            input_energy = input_generator(i)
            result = self.advance(input_energy)
            results.append(result)

        return results

    def state(self) -> Dict[str, Any]:
        """Get complete state of the spiral"""
        return {
            'current_step': self.current_step.name,
            'step_value': self.current_step.value,
            'energy': self.energy,
            'integration': self.integration,
            'spiral_level': self.spiral_level,
            'can_advance': self.can_advance(),
            'step_info': self.step_info(),
            'history_length': len(self.history),
        }

    def trajectory(self) -> List[Dict[str, Any]]:
        """Get the historical trajectory"""
        return self.history.copy()


def demonstrate_spiral():
    """Demonstrate the Spiral and Triad"""
    print("=== Spiral and Triad Demonstration ===\n")

    spiral = SpiralTriad(energy=0.8)

    print("1. Initial State:")
    state = spiral.state()
    print(f"   Step: {state['current_step']}")
    print(f"   Energy: {state['energy']:.3f}")
    print(f"   Integration: {state['integration']:.3f}")
    print()

    print("2. Running spiral for 30 steps...")
    results = spiral.run_spiral(30)

    # Count advancements
    advancements = [r for r in results if r.get('advanced')]
    spiral_completions = [r for r in results if r.get('spiral_complete')]

    print(f"   Advancements: {len(advancements)}")
    print(f"   Spiral completions: {len(spiral_completions)}")
    print()

    print("3. Final State:")
    state = spiral.state()
    print(f"   Step: {state['current_step']}")
    print(f"   Energy: {state['energy']:.3f}")
    print(f"   Integration: {state['integration']:.3f}")
    print(f"   Spiral Level: {state['spiral_level']}")
    print()

    print("4. Step Info:")
    info = spiral.step_info()
    print(f"   Name: {info['name']}")
    print(f"   Description: {info['description']}")
    print(f"   Characteristics: {info.get('characteristics', [])}")
    print()

    print("=== Demonstration Complete ===")


if __name__ == "__main__":
    demonstrate_spiral()
