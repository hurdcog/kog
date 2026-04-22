"""
Six Cycles Defining Viable Natural Systems

Based on Eric Schwarz's organizational systems theory:

Horizontal Cycles (within planes - stability):
- Vortices (2): Cyclic exchanges of energy and matter, metabolism
- Retroaction Loops (3): Homeostasis and cybernetic controls
- Self-Referential Loop (5): Self-knowledge from being-image dialogue

Vertical Cycles (between planes - change):
- Morphogenesis (1): Emergence, regeneration, evolution
- Autopoiesis (4): Mutual production of network and structures
- Autogenesis (6): Self-creation leading to autonomy

The horizontal cycles maintain stability within each plane,
while vertical cycles drive change and evolution between planes.
"""

from dataclasses import dataclass, field
from typing import List, Dict, Any, Optional, Callable
from enum import Enum
from abc import ABC, abstractmethod
import numpy as np


class CycleType(Enum):
    """Types of cycles"""
    HORIZONTAL = "horizontal"  # Within planes - stability
    VERTICAL = "vertical"      # Between planes - change


class CycleState(Enum):
    """Possible cycle states"""
    # Vortex states
    CYCLING = "cycling"
    ACCUMULATING = "accumulating"
    DISSIPATING = "dissipating"

    # Retroaction states
    REGULATING = "regulating"
    OSCILLATING = "oscillating"
    DIVERGING = "diverging"

    # Self-referential states
    OBSERVING = "observing"
    MODELING = "modeling"
    INTEGRATING = "integrating"

    # Morphogenesis states
    EMERGING = "emerging"
    DIFFERENTIATING = "differentiating"

    # Autopoiesis states
    PRODUCING = "producing"
    MAINTAINING = "maintaining"
    REGENERATING = "regenerating"

    # Autogenesis states
    CREATING = "creating"
    TRANSCENDING = "transcending"
    AUTONOMOUS = "autonomous"


@dataclass
class Cycle(ABC):
    """Abstract base class for all cycles"""
    name: str
    cycle_type: CycleType
    plane: str  # Which plane(s) this cycle operates on
    state: CycleState
    rate: float = 0.5
    feedback_history: List[float] = field(default_factory=list)

    @abstractmethod
    def step(self, input_value: float) -> Dict[str, Any]:
        """Execute one cycle step"""
        pass

    def record_feedback(self, value: float) -> None:
        """Record feedback value"""
        self.feedback_history.append(value)
        # Keep history bounded
        if len(self.feedback_history) > 100:
            self.feedback_history = self.feedback_history[-100:]


@dataclass
class VortexCycle(Cycle):
    """
    Cycle 2: Vortices - Cyclic exchanges of energy and matter

    Horizontal cycle within the Plane of Energy.

    Characteristics:
    - Matter recycling
    - Pulsations and oscillations
    - Metabolism
    - Necessary for material perennity
    """

    def __init__(self, rate: float = 0.5):
        super().__init__(
            name="vortex",
            cycle_type=CycleType.HORIZONTAL,
            plane="energy",
            state=CycleState.CYCLING,
            rate=rate
        )
        self.phase: float = 0.0  # Current phase in cycle

    def step(self, input_value: float) -> Dict[str, Any]:
        """Execute vortex cycle step"""
        # Advance phase
        self.phase += self.rate * 0.1

        # Cycle through states
        if self.state == CycleState.CYCLING:
            self.state = CycleState.ACCUMULATING
        elif self.state == CycleState.ACCUMULATING:
            self.state = CycleState.DISSIPATING
        else:
            self.state = CycleState.CYCLING

        # Output is oscillating
        output = input_value * (0.5 + 0.5 * np.sin(self.phase))
        self.record_feedback(output)

        return {
            'output': output,
            'state': self.state.value,
            'phase': self.phase,
            'cycling': True
        }


@dataclass
class RetroactionCycle(Cycle):
    """
    Cycle 3: Retroaction Loops - Homeostasis and cybernetic controls

    Horizontal cycle within the Plane of Information.

    Characteristics:
    - Negative feedback for stability
    - Setpoint maintenance
    - Error correction
    - Cybernetic regulation
    """

    def __init__(self, rate: float = 0.5, setpoint: float = 0.5):
        super().__init__(
            name="retroaction",
            cycle_type=CycleType.HORIZONTAL,
            plane="information",
            state=CycleState.REGULATING,
            rate=rate
        )
        self.setpoint: float = setpoint
        self.integral_error: float = 0.0

    def step(self, input_value: float) -> Dict[str, Any]:
        """Execute retroaction cycle step"""
        # Calculate error
        error = input_value - self.setpoint
        self.integral_error += error * 0.1

        # PID-like correction
        correction = -(self.rate * error + 0.1 * self.integral_error)
        output = input_value + correction

        # Update state based on error magnitude
        if abs(error) < 0.1:
            self.state = CycleState.REGULATING
        elif abs(error) < 0.3:
            self.state = CycleState.OSCILLATING
        else:
            self.state = CycleState.DIVERGING

        self.record_feedback(error)

        return {
            'output': output,
            'error': error,
            'correction': correction,
            'state': self.state.value,
            'stable': abs(error) < 0.1
        }


@dataclass
class SelfReferentialCycle(Cycle):
    """
    Cycle 5: Self-Referential Loop - Self-knowledge through being-image dialogue

    Horizontal cycle within the Plane of Totality.

    Characteristics:
    - Self-knowledge from dialogue between itself and its image
    - The closer the image from the object, the better the autonomy
    - Leads to autonomy
    """

    def __init__(self, rate: float = 0.5):
        super().__init__(
            name="self_referential",
            cycle_type=CycleType.HORIZONTAL,
            plane="totality",
            state=CycleState.OBSERVING,
            rate=rate
        )
        self.being_state: Dict[str, Any] = {}
        self.image_state: Dict[str, Any] = {}
        self.self_awareness: float = 0.0

    def update_being(self, state: Dict[str, Any]) -> None:
        """Update the being state"""
        self.being_state.update(state)

    def update_image(self, image: Dict[str, Any]) -> None:
        """Update the self-image"""
        self.image_state.update(image)

    def step(self, input_value: float) -> Dict[str, Any]:
        """Execute self-referential cycle step"""
        # Cycle through states
        if self.state == CycleState.OBSERVING:
            self.state = CycleState.MODELING
        elif self.state == CycleState.MODELING:
            self.state = CycleState.INTEGRATING
        else:
            self.state = CycleState.OBSERVING

        # Calculate being-image correspondence
        if self.being_state and self.image_state:
            being_keys = set(self.being_state.keys())
            image_keys = set(self.image_state.keys())
            common = being_keys & image_keys
            total = being_keys | image_keys
            match_ratio = len(common) / len(total) if total else 0
        else:
            match_ratio = input_value  # Use input as proxy

        self.self_awareness = 0.9 * self.self_awareness + 0.1 * match_ratio
        self.record_feedback(self.self_awareness)

        return {
            'output': input_value,
            'state': self.state.value,
            'self_awareness': self.self_awareness,
            'being_image_match': match_ratio
        }


@dataclass
class MorphogenesisCycle(Cycle):
    """
    Cycle 1: Morphogenesis - Emergence, regeneration, evolution

    Vertical cycle from Plane of Information to Plane of Energy.

    Characteristics:
    - Emergence and metamorphosis
    - Replication and division
    - Breeding of structures specified by the logical network
    - Creates new forms in space-time
    """

    def __init__(self, rate: float = 0.5):
        super().__init__(
            name="morphogenesis",
            cycle_type=CycleType.VERTICAL,
            plane="information->energy",
            state=CycleState.EMERGING,
            rate=rate
        )
        self.emergence_threshold: float = 0.6
        self.structures_created: int = 0

    def step(self, input_value: float) -> Dict[str, Any]:
        """Execute morphogenesis cycle step"""
        # Cycle through states
        if self.state == CycleState.EMERGING:
            self.state = CycleState.DIFFERENTIATING
        elif self.state == CycleState.DIFFERENTIATING:
            self.state = CycleState.INTEGRATING
        else:
            self.state = CycleState.EMERGING

        # Check for emergence
        new_structure = input_value > self.emergence_threshold
        if new_structure:
            self.structures_created += 1

        # Output amplified by rate
        output = input_value * (1 + self.rate)
        self.record_feedback(output)

        return {
            'output': output,
            'state': self.state.value,
            'new_structure': new_structure,
            'structures_created': self.structures_created
        }


@dataclass
class AutopoiesisCycle(Cycle):
    """
    Cycle 4: Autopoiesis - Mutual production of network and structures

    Vertical cycle between Plane of Information and Plane of Energy.

    Characteristics:
    - Mutual production of virtual network and actual structures
    - Self-production and self-maintenance
    - Operational closure
    - Creates self-maintaining systems
    """

    def __init__(self, rate: float = 0.5):
        super().__init__(
            name="autopoiesis",
            cycle_type=CycleType.VERTICAL,
            plane="information<->energy",
            state=CycleState.PRODUCING,
            rate=rate
        )
        self.production_capacity: float = 0.5
        self.maintenance_level: float = 0.5

    def step(self, input_value: float) -> Dict[str, Any]:
        """Execute autopoiesis cycle step"""
        # Cycle through states
        if self.state == CycleState.PRODUCING:
            self.state = CycleState.MAINTAINING
            self.production_capacity = min(1, self.production_capacity + 0.05 * input_value)
        elif self.state == CycleState.MAINTAINING:
            self.state = CycleState.REGENERATING
            self.maintenance_level = min(1, 0.9 * self.maintenance_level + 0.1 * input_value)
        else:
            self.state = CycleState.PRODUCING

        # Production based on input and capacity
        production = self.rate * input_value * self.production_capacity
        self.record_feedback(production)

        return {
            'output': input_value,
            'state': self.state.value,
            'production': production,
            'production_capacity': self.production_capacity,
            'maintenance_level': self.maintenance_level,
            'self_producing': production > 0.3
        }


@dataclass
class AutogenesisCycle(Cycle):
    """
    Cycle 6: Autogenesis - Self-creation leading to autonomy

    Vertical cycle from Plane of Totality through all planes.

    Characteristics:
    - Self-creation by metacoupling
    - Dialogue between autopoietic dialogue and emerging identity
    - Leads to autonomy
    - Creation of entity producing its own rules
    """

    def __init__(self, rate: float = 0.5):
        super().__init__(
            name="autogenesis",
            cycle_type=CycleType.VERTICAL,
            plane="totality->all",
            state=CycleState.CREATING,
            rate=rate
        )
        self.autonomy_level: float = 0.0
        self.identity_coherence: float = 0.0

    def step(self, input_value: float) -> Dict[str, Any]:
        """Execute autogenesis cycle step"""
        # Cycle through states
        if self.state == CycleState.CREATING:
            self.state = CycleState.TRANSCENDING
            self.identity_coherence = min(1, self.identity_coherence + 0.05 * input_value)
        elif self.state == CycleState.TRANSCENDING:
            if self.autonomy_level > 0.8:
                self.state = CycleState.AUTONOMOUS
            else:
                self.state = CycleState.CREATING
            self.autonomy_level = min(1, self.autonomy_level + 0.03 * input_value)
        else:  # AUTONOMOUS
            self.state = CycleState.CREATING

        # Output amplified toward autonomy
        output = input_value * (1 + 0.5 * self.rate)
        self.record_feedback(output)

        return {
            'output': output,
            'state': self.state.value,
            'autonomy': self.state == CycleState.AUTONOMOUS,
            'autonomy_level': self.autonomy_level,
            'identity_coherence': self.identity_coherence
        }


@dataclass
class SixCycles:
    """
    Orchestrator for the six cycles defining viable natural systems

    Manages:
    - Three horizontal cycles (stability)
    - Three vertical cycles (change)
    """
    vortex: VortexCycle = field(default_factory=VortexCycle)
    retroaction: RetroactionCycle = field(default_factory=RetroactionCycle)
    self_referential: SelfReferentialCycle = field(default_factory=SelfReferentialCycle)
    morphogenesis: MorphogenesisCycle = field(default_factory=MorphogenesisCycle)
    autopoiesis: AutopoiesisCycle = field(default_factory=AutopoiesisCycle)
    autogenesis: AutogenesisCycle = field(default_factory=AutogenesisCycle)

    def step(self, input_value: float = 0.5) -> Dict[str, Any]:
        """Execute one step of all cycles"""
        # Horizontal cycles (stability)
        vortex_result = self.vortex.step(input_value)
        retroaction_result = self.retroaction.step(input_value)
        self_ref_result = self.self_referential.step(input_value)

        # Vertical cycles (change) - use outputs from horizontal as inputs
        morpho_input = (vortex_result['output'] + input_value) / 2
        morpho_result = self.morphogenesis.step(morpho_input)

        autopoiesis_input = (retroaction_result['output'] + morpho_result['output']) / 2
        autopoiesis_result = self.autopoiesis.step(autopoiesis_input)

        autogenesis_input = self_ref_result['self_awareness']
        autogenesis_result = self.autogenesis.step(autogenesis_input)

        return {
            'horizontal': {
                'vortex': vortex_result,
                'retroaction': retroaction_result,
                'self_referential': self_ref_result,
            },
            'vertical': {
                'morphogenesis': morpho_result,
                'autopoiesis': autopoiesis_result,
                'autogenesis': autogenesis_result,
            },
            'stability_index': (
                (1 if retroaction_result['stable'] else 0) +
                self_ref_result['self_awareness']
            ) / 2,
            'change_index': (
                morpho_result['structures_created'] * 0.1 +
                autopoiesis_result['production'] +
                autogenesis_result['autonomy_level']
            ) / 3
        }

    def state(self) -> Dict[str, Any]:
        """Get state of all cycles"""
        return {
            'vortex': {
                'state': self.vortex.state.value,
                'phase': self.vortex.phase
            },
            'retroaction': {
                'state': self.retroaction.state.value,
                'setpoint': self.retroaction.setpoint
            },
            'self_referential': {
                'state': self.self_referential.state.value,
                'self_awareness': self.self_referential.self_awareness
            },
            'morphogenesis': {
                'state': self.morphogenesis.state.value,
                'structures_created': self.morphogenesis.structures_created
            },
            'autopoiesis': {
                'state': self.autopoiesis.state.value,
                'production_capacity': self.autopoiesis.production_capacity
            },
            'autogenesis': {
                'state': self.autogenesis.state.value,
                'autonomy_level': self.autogenesis.autonomy_level
            }
        }
