"""
Evolutionary Helix - 11 Stages of Long-Term Evolution

Based on Eric Schwarz's "Generic scheme for the long term evolution
of self-organizing systems through complexification to autonomy."

The helical pattern shows the successive emergence of the six cycles
of viable systems across three planes of existence:
- Objectal (Physical) Plane: Stages 0-2
- Relational (Logical) Plane: Stages 3-5
- Existential (Holistic) Plane: Stages 6-10

Each complete cycle represents one spiral level, with higher spirals
operating at increased complexity and integration.
"""

from dataclasses import dataclass, field
from typing import List, Dict, Any, Optional, Tuple
from enum import Enum
import numpy as np


class HelixPlane(Enum):
    """The three planes traversed by the helix"""
    OBJECTAL = "objectal"      # Physical plane - structures, energy
    RELATIONAL = "relational"  # Logical plane - networks, information
    EXISTENTIAL = "existential"  # Holistic plane - whole, being


class HelixStage(Enum):
    """
    The 11 stages of the evolutionary helix

    Objectal Plane (Structures, Energy):
    0 - ENTROPIC_DRIFT: Internal/external energy dissipation
    1 - SELF_ORGANIZATION: Morphogenesis - creation of space-time forms
    2 - VORTICES: Matter recycling, oscillations, pulsations

    Relational Plane (Networks, Information):
    3 - SELF_REGULATION: Homeostasis, functional stability
    4 - STARTING_SELF_PRODUCTION: Appearance of self-producing cycles
    5 - INFORMATION_DRIFT: Diffusion of information, complexification

    Existential Plane (Whole, Being):
    6 - SELF_PRODUCTION: Autopoiesis - self-producing structures
    7 - SELF_REFERENCE: Mutuality between things and laws
    8 - REFERENTIAL_DRIFT: Intensification of self-reference, identisation
    9 - SELF_CREATION: Autogenesis - entity producing its own rules
    10 - TOWARD_AUTONOMY: Full autonomy and self-determination
    """
    ENTROPIC_DRIFT = 0
    SELF_ORGANIZATION = 1
    VORTICES = 2
    SELF_REGULATION = 3
    STARTING_SELF_PRODUCTION = 4
    INFORMATION_DRIFT = 5
    SELF_PRODUCTION = 6
    SELF_REFERENCE = 7
    REFERENTIAL_DRIFT = 8
    SELF_CREATION = 9
    TOWARD_AUTONOMY = 10


# Detailed stage information
STAGE_DEFINITIONS = {
    HelixStage.ENTROPIC_DRIFT: {
        'name': 'Entropic Drift',
        'stage': 0,
        'plane': HelixPlane.OBJECTAL,
        'description': 'Internal/external energy dissipation, uniformisation',
        'process': 'Decrease of differences, aging, degradation, wear',
        'direction': 'tropic_drift',
        'polarity': '-',
    },
    HelixStage.SELF_ORGANIZATION: {
        'name': 'Self-Organization',
        'stage': 1,
        'plane': HelixPlane.OBJECTAL,
        'description': 'Morphogenesis - creation of space-time forms',
        'process': 'Integration and differentiation',
        'direction': 'metamorphosis',
        'polarity': '+',
        'emergence': 'Starting of self-organization, positive feedback loops',
    },
    HelixStage.VORTICES: {
        'name': 'Vortices',
        'stage': 2,
        'plane': HelixPlane.OBJECTAL,
        'description': 'Matter recycling, oscillations, pulsations',
        'process': 'Cycles of matter and energy',
        'direction': 'stability',
        'polarity': '-',
    },
    HelixStage.SELF_REGULATION: {
        'name': 'Self-Regulation',
        'stage': 3,
        'plane': HelixPlane.RELATIONAL,
        'description': 'Homeostasis, functional stability',
        'process': 'Negative feedback maintaining setpoints',
        'direction': 'stability',
        'polarity': '-',
    },
    HelixStage.STARTING_SELF_PRODUCTION: {
        'name': 'Starting Self-Production',
        'stage': 4,
        'plane': HelixPlane.RELATIONAL,
        'description': 'Appearance of positive self-producing ontological cycles',
        'process': 'Bootstrapping autopoiesis',
        'direction': 'metamorphosis',
        'polarity': '+',
    },
    HelixStage.INFORMATION_DRIFT: {
        'name': 'Information Drift',
        'stage': 5,
        'plane': HelixPlane.RELATIONAL,
        'description': 'Diffusion of information, complexification',
        'process': 'Aggregation of cycles into hypercycles and hierarchisation',
        'direction': 'tropic_drift',
        'polarity': '+',
    },
    HelixStage.SELF_PRODUCTION: {
        'name': 'Self-Production',
        'stage': 6,
        'plane': HelixPlane.EXISTENTIAL,
        'description': 'Autopoiesis - creation of self-producing structures',
        'process': 'Emergence of self-reference dialogue',
        'direction': 'metamorphosis',
        'polarity': '+',
    },
    HelixStage.SELF_REFERENCE: {
        'name': 'Self-Reference',
        'stage': 7,
        'plane': HelixPlane.EXISTENTIAL,
        'description': 'Mutuality between things and laws',
        'process': 'Identity forming through self-reference',
        'direction': 'stability',
        'polarity': '-',
    },
    HelixStage.REFERENTIAL_DRIFT: {
        'name': 'Referential Drift',
        'stage': 8,
        'plane': HelixPlane.EXISTENTIAL,
        'description': 'Intensification of self-reference, identisation',
        'process': 'Integration of differences, emergence of consciousness',
        'direction': 'tropic_drift',
        'polarity': '+',
        'emergence': 'Creation of identity = emergence of consciousness',
    },
    HelixStage.SELF_CREATION: {
        'name': 'Self-Creation',
        'stage': 9,
        'plane': HelixPlane.EXISTENTIAL,
        'description': 'Autogenesis - entity producing its own rules of production',
        'process': 'Creation of entity with self-determined organization',
        'direction': 'metamorphosis',
        'polarity': '+',
    },
    HelixStage.TOWARD_AUTONOMY: {
        'name': 'Toward Autonomy',
        'stage': 10,
        'plane': HelixPlane.EXISTENTIAL,
        'description': 'Full autonomy and self-determination',
        'process': 'Complete self-organization with stability',
        'direction': 'stability',
        'polarity': '-',
        'emergence': 'Cosmic return - ready for new spiral at higher level',
    },
}


@dataclass
class EvolutionaryHelix:
    """
    The 11-Stage Evolutionary Helix

    Manages the long-term evolution of self-organizing systems
    through complexification to autonomy, tracking progress
    through three planes and multiple spiral levels.
    """
    current_stage: HelixStage = HelixStage.ENTROPIC_DRIFT
    spiral_level: int = 0
    momentum: float = 0.5
    accumulated_characteristics: List[str] = field(default_factory=list)
    history: List[Dict[str, Any]] = field(default_factory=list)

    # Stage-specific metrics
    stability: float = 0.5
    metamorphosis: float = 0.0
    tropic_drift: float = 0.0

    def stage_info(self) -> Dict[str, Any]:
        """Get detailed information about current stage"""
        return {
            'stage_value': self.current_stage.value,
            'name': self.current_stage.name,
            **STAGE_DEFINITIONS.get(self.current_stage, {}),
            'spiral_level': self.spiral_level,
            'momentum': self.momentum,
        }

    def current_plane(self) -> HelixPlane:
        """Get the current plane based on stage"""
        stage_value = self.current_stage.value
        if stage_value <= 2:
            return HelixPlane.OBJECTAL
        elif stage_value <= 5:
            return HelixPlane.RELATIONAL
        else:
            return HelixPlane.EXISTENTIAL

    def _compute_threshold(self) -> float:
        """Compute advancement threshold based on stage"""
        # Higher stages require more energy/momentum
        base = 0.3
        stage_factor = self.current_stage.value * 0.03
        spiral_factor = self.spiral_level * 0.05
        return base + stage_factor + spiral_factor

    def _update_metrics(self, input_energy: float) -> None:
        """Update internal metrics based on current stage"""
        stage_info = STAGE_DEFINITIONS.get(self.current_stage, {})
        direction = stage_info.get('direction', 'stability')
        polarity = stage_info.get('polarity', '-')

        # Update based on direction and polarity
        if direction == 'stability':
            self.stability += 0.05 * input_energy if polarity == '-' else -0.02
        elif direction == 'metamorphosis':
            self.metamorphosis += 0.05 * input_energy if polarity == '+' else -0.02
        elif direction == 'tropic_drift':
            self.tropic_drift += 0.03 * input_energy

        # Clamp values
        self.stability = max(0, min(1, self.stability))
        self.metamorphosis = max(0, min(1, self.metamorphosis))
        self.tropic_drift = max(0, min(1, self.tropic_drift))

    def advance(self, input_energy: float) -> Dict[str, Any]:
        """
        Attempt to advance the helix with given input energy

        Returns:
            Result dictionary with advancement status and stage info
        """
        old_stage = self.current_stage
        old_momentum = self.momentum

        # Update momentum with input
        self.momentum = 0.9 * self.momentum + 0.1 * input_energy
        self._update_metrics(input_energy)

        # Calculate combined energy for advancement
        threshold = self._compute_threshold()
        combined = 0.7 * input_energy + 0.3 * self.momentum

        # Record characteristic
        stage_info = STAGE_DEFINITIONS.get(self.current_stage, {})
        if stage_info.get('description'):
            if stage_info['description'] not in self.accumulated_characteristics:
                self.accumulated_characteristics.append(stage_info['description'])

        can_advance = combined > threshold

        result = {
            'stage': self.current_stage,
            'stage_info': stage_info,
            'plane': self.current_plane().value,
            'momentum': self.momentum,
            'threshold': threshold,
            'combined_energy': combined,
            'stability': self.stability,
            'metamorphosis': self.metamorphosis,
            'tropic_drift': self.tropic_drift,
        }

        if can_advance:
            if self.current_stage.value >= 10:
                # Completed full helix - cosmic return
                self.current_stage = HelixStage.ENTROPIC_DRIFT
                self.spiral_level += 1
                self.momentum *= 1.1  # Momentum boost for completion

                result.update({
                    'advanced': True,
                    'from': old_stage,
                    'to': HelixStage.ENTROPIC_DRIFT,
                    'cosmic_return': True,
                    'spiral_level': self.spiral_level,
                })
            else:
                # Normal advancement
                self.current_stage = HelixStage(self.current_stage.value + 1)
                result.update({
                    'advanced': True,
                    'from': old_stage,
                    'to': self.current_stage,
                })
        else:
            result.update({
                'advanced': False,
                'at': self.current_stage,
                'needed': threshold,
                'had': combined,
            })

        self.history.append(result)
        return result

    def run_helix(self, steps: int, input_generator=None) -> List[Dict[str, Any]]:
        """
        Run the helix for a number of steps

        Args:
            steps: Number of steps to run
            input_generator: Callable that generates input energy

        Returns:
            List of step results
        """
        if input_generator is None:
            def input_generator(i):
                # Oscillating with gradual increase
                base = 0.5 + 0.1 * (i / steps)
                oscillation = 0.2 * np.sin(i * 0.15)
                return min(1.0, base + oscillation)

        results = []
        for i in range(steps):
            input_energy = input_generator(i)
            result = self.advance(input_energy)
            results.append(result)

        return results

    def state(self) -> Dict[str, Any]:
        """Get complete state of the helix"""
        return {
            'current_stage': self.current_stage.name,
            'stage_value': self.current_stage.value,
            'plane': self.current_plane().value,
            'spiral_level': self.spiral_level,
            'momentum': self.momentum,
            'stability': self.stability,
            'metamorphosis': self.metamorphosis,
            'tropic_drift': self.tropic_drift,
            'characteristics_accumulated': len(self.accumulated_characteristics),
            'history_length': len(self.history),
            'stage_info': self.stage_info(),
        }

    def progress_summary(self) -> Dict[str, Any]:
        """Get summary of evolutionary progress"""
        # Calculate progress through current spiral
        current_progress = self.current_stage.value / 10.0

        # Calculate overall progress
        total_stages = self.spiral_level * 11 + self.current_stage.value
        overall_progress = total_stages / max(1, (self.spiral_level + 1) * 11)

        # Plane distribution
        plane_stages = {
            HelixPlane.OBJECTAL: [0, 1, 2],
            HelixPlane.RELATIONAL: [3, 4, 5],
            HelixPlane.EXISTENTIAL: [6, 7, 8, 9, 10],
        }

        return {
            'current_spiral_progress': current_progress,
            'overall_progress': overall_progress,
            'total_stages_traversed': total_stages,
            'spiral_level': self.spiral_level,
            'current_plane': self.current_plane().value,
            'stages_in_objectal': sum(1 for h in self.history
                                      if h.get('plane') == HelixPlane.OBJECTAL.value),
            'stages_in_relational': sum(1 for h in self.history
                                        if h.get('plane') == HelixPlane.RELATIONAL.value),
            'stages_in_existential': sum(1 for h in self.history
                                         if h.get('plane') == HelixPlane.EXISTENTIAL.value),
        }

    def trajectory(self) -> List[Dict[str, Any]]:
        """Get the historical trajectory"""
        return self.history.copy()


def demonstrate_helix():
    """Demonstrate the Evolutionary Helix"""
    print("=== Evolutionary Helix Demonstration ===\n")

    helix = EvolutionaryHelix(momentum=0.6)

    print("1. Initial State:")
    state = helix.state()
    print(f"   Stage: {state['current_stage']} ({state['stage_value']})")
    print(f"   Plane: {state['plane']}")
    print(f"   Spiral Level: {state['spiral_level']}")
    print(f"   Momentum: {state['momentum']:.3f}")
    print()

    print("2. Running helix for 50 steps...")
    results = helix.run_helix(50)

    # Count events
    advancements = [r for r in results if r.get('advanced')]
    cosmic_returns = [r for r in results if r.get('cosmic_return')]

    print(f"   Advancements: {len(advancements)}")
    print(f"   Cosmic returns: {len(cosmic_returns)}")
    print()

    print("3. Final State:")
    state = helix.state()
    print(f"   Stage: {state['current_stage']} ({state['stage_value']})")
    print(f"   Plane: {state['plane']}")
    print(f"   Spiral Level: {state['spiral_level']}")
    print(f"   Momentum: {state['momentum']:.3f}")
    print()

    print("4. Progress Summary:")
    summary = helix.progress_summary()
    print(f"   Current spiral progress: {summary['current_spiral_progress']:.1%}")
    print(f"   Total stages traversed: {summary['total_stages_traversed']}")
    print(f"   Stages in objectal: {summary['stages_in_objectal']}")
    print(f"   Stages in relational: {summary['stages_in_relational']}")
    print(f"   Stages in existential: {summary['stages_in_existential']}")
    print()

    print("5. Stage Info:")
    info = helix.stage_info()
    print(f"   Name: {info['name']}")
    print(f"   Description: {info.get('description', 'N/A')}")
    print(f"   Process: {info.get('process', 'N/A')}")
    print()

    print("=== Demonstration Complete ===")


if __name__ == "__main__":
    demonstrate_helix()
