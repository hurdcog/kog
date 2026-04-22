"""
Three Planes of Existence

Based on Eric Schwarz's organizational systems theory:
- Plane of Energy (Objectal): Physical structures, objects in space-time
- Plane of Information (Relational): Relations, causal networks
- Plane of Totality (Existential): The whole, being, identity, self

These three planes form the ontological foundation for viable natural systems.
"""

from dataclasses import dataclass, field
from typing import List, Dict, Any, Optional
from enum import Enum
import numpy as np


class PlaneType(Enum):
    """Types of existence planes"""
    ENERGY = "energy"
    INFORMATION = "information"
    TOTALITY = "totality"


@dataclass
class PlaneOfEnergy:
    """
    Plane of Energy - The Objectal/Physical Plane

    Represents:
    - Physical structures produced by the network
    - Objects realized in space and time
    - Energy-matter fluxes and exchanges
    - Vortices and metabolic cycles

    Characteristics:
    - Structures, "Energy"
    - Space-time forms
    - Matter recycling, oscillations, pulsations
    """
    objects: List[Any] = field(default_factory=list)
    structures: List[Any] = field(default_factory=list)
    flux: float = 0.5  # Rate of energy exchange
    stability: float = 0.5
    entropy: float = 0.5  # Entropic drift

    def add_object(self, obj: Any) -> None:
        """Add an object to the plane"""
        self.objects.append(obj)

    def add_structure(self, structure: Any) -> None:
        """Add a structure to the plane"""
        self.structures.append(structure)

    def compute_entropic_drift(self) -> float:
        """
        Compute entropic drift - tendency toward uniformisation

        Internal/external energy dissipation, decrease of differences
        """
        # Entropy increases naturally, structures resist it
        structure_resistance = len(self.structures) * 0.05
        self.entropy = max(0, min(1, self.entropy + 0.01 - structure_resistance))
        return self.entropy

    def morphogenesis(self, input_energy: float) -> Dict[str, Any]:
        """
        Execute morphogenesis - creation of space-time forms

        Self-organization through integration and differentiation
        """
        if input_energy > 0.5:
            new_structure = {
                'type': 'emergent',
                'energy': input_energy,
                'timestamp': len(self.structures)
            }
            self.structures.append(new_structure)
            self.stability += 0.1
            return {
                'created': True,
                'structure': new_structure,
                'stability': self.stability
            }
        return {'created': False, 'stability': self.stability}

    def vortex_cycle(self) -> Dict[str, Any]:
        """
        Execute vortex cycle - cyclic exchanges of energy and matter

        Matter recycling, oscillations, pulsations
        """
        # Cycle flux between states
        old_flux = self.flux
        self.flux = np.sin(len(self.objects) * 0.1) * 0.5 + 0.5

        return {
            'flux_change': self.flux - old_flux,
            'current_flux': self.flux,
            'cycling': True
        }

    def state(self) -> Dict[str, Any]:
        """Get current plane state"""
        return {
            'plane': PlaneType.ENERGY.value,
            'object_count': len(self.objects),
            'structure_count': len(self.structures),
            'flux': self.flux,
            'stability': self.stability,
            'entropy': self.entropy
        }


@dataclass
class PlaneOfInformation:
    """
    Plane of Information - The Relational/Logical Plane

    Represents:
    - Network of virtual relations (causal relations)
    - Logical world of abstract relations
    - Potential causation network
    - Information processing and diffusion

    Characteristics:
    - Networks, "Information"
    - Relations between components
    - Homeostasis and cybernetic controls
    """
    relations: List[Any] = field(default_factory=list)
    causation_network: Dict[str, List[str]] = field(default_factory=dict)
    coherence: float = 0.5
    information_entropy: float = 0.5
    homeostatic_setpoint: float = 0.5

    def add_relation(self, relation: Any) -> None:
        """Add a relation to the plane"""
        self.relations.append(relation)

    def add_causation(self, cause: str, effect: str) -> None:
        """Add a causal relationship"""
        if cause not in self.causation_network:
            self.causation_network[cause] = []
        self.causation_network[cause].append(effect)

    def compute_information_drift(self) -> float:
        """
        Compute information drift - diffusion of information

        Increase of differences, complexification
        """
        # Information diffuses and creates complexity
        complexity = len(self.causation_network) * 0.02
        self.information_entropy = min(1, self.information_entropy + 0.005 + complexity)
        return self.information_entropy

    def retroaction_cycle(self, current_value: float) -> Dict[str, Any]:
        """
        Execute retroaction cycle - homeostasis and regulation

        Maintains stability through negative feedback
        """
        error = current_value - self.homeostatic_setpoint
        correction = -0.3 * error  # Negative feedback

        # Determine regulation state
        if abs(error) < 0.1:
            state = 'regulating'
        elif abs(error) < 0.3:
            state = 'oscillating'
        else:
            state = 'diverging'

        self.coherence = max(0, min(1, 1 - abs(error)))

        return {
            'error': error,
            'correction': correction,
            'corrected_value': current_value + correction,
            'state': state,
            'coherence': self.coherence
        }

    def self_regulation(self) -> Dict[str, Any]:
        """
        Execute self-regulation - emergence of functional stability

        Integration leading to coherence, containing the third
        """
        # Count functional relations
        functional_relations = len([r for r in self.relations if r])
        network_density = len(self.causation_network) / max(1, len(self.relations))

        self.coherence = min(1, 0.5 * functional_relations * 0.1 + 0.5 * network_density)

        return {
            'functional_relations': functional_relations,
            'network_density': network_density,
            'coherence': self.coherence,
            'self_regulating': self.coherence > 0.5
        }

    def state(self) -> Dict[str, Any]:
        """Get current plane state"""
        return {
            'plane': PlaneType.INFORMATION.value,
            'relation_count': len(self.relations),
            'causation_nodes': len(self.causation_network),
            'coherence': self.coherence,
            'information_entropy': self.information_entropy,
            'homeostatic_setpoint': self.homeostatic_setpoint
        }


@dataclass
class PlaneOfTotality:
    """
    Plane of Totality - The Existential/Holistic Plane

    Represents:
    - The system as existing whole
    - Emerging holistic features not present in parts
    - Identity and self
    - Being and becoming

    Characteristics:
    - Whole, "Being"
    - Emerging wholeness
    - Self-reference and self-creation
    """
    being: Dict[str, Any] = field(default_factory=dict)
    image: Dict[str, Any] = field(default_factory=dict)  # Self-image
    identity: Dict[str, Any] = field(default_factory=dict)
    wholeness: float = 0.0
    autonomy: float = 0.0
    self_awareness: float = 0.0

    def update_being(self, state: Dict[str, Any]) -> None:
        """Update the being state"""
        self.being.update(state)

    def update_image(self, image: Dict[str, Any]) -> None:
        """Update self-image"""
        self.image.update(image)

    def self_referential_loop(self) -> Dict[str, Any]:
        """
        Execute self-referential loop

        Self-knowledge from dialogue between itself and its own image.
        The closer the image from the object, the better the autonomy.
        """
        # Compare being with image
        being_keys = set(self.being.keys())
        image_keys = set(self.image.keys())

        if not being_keys or not image_keys:
            match_ratio = 0.0
        else:
            common = being_keys & image_keys
            total = being_keys | image_keys
            match_ratio = len(common) / len(total) if total else 0

        self.self_awareness = match_ratio

        return {
            'being_aspects': len(being_keys),
            'image_aspects': len(image_keys),
            'match_ratio': match_ratio,
            'self_awareness': self.self_awareness,
            'state': 'observing' if match_ratio < 0.3 else (
                'modeling' if match_ratio < 0.7 else 'integrating'
            )
        }

    def autogenesis(self, integration_level: float) -> Dict[str, Any]:
        """
        Execute autogenesis - self-creation

        Self-creation by metacoupling between autopoietic dialogue
        and identity emerging from the dialogue. Leads to autonomy.
        """
        if integration_level > 0.6:
            self.autonomy = min(1, self.autonomy + 0.1)
            self.identity['created'] = True
            self.identity['integration'] = integration_level

            return {
                'autonomous': self.autonomy > 0.8,
                'autonomy_level': self.autonomy,
                'identity_created': True,
                'state': 'autonomous' if self.autonomy > 0.8 else 'creating'
            }

        return {
            'autonomous': False,
            'autonomy_level': self.autonomy,
            'identity_created': False,
            'state': 'preparing'
        }

    def referential_drift(self) -> float:
        """
        Compute referential drift

        Intensification of self-reference leading to identisation
        and emergence of consciousness
        """
        # Self-reference intensifies with self-awareness
        drift = self.self_awareness * 0.1
        self.wholeness = min(1, self.wholeness + drift)
        return self.wholeness

    def state(self) -> Dict[str, Any]:
        """Get current plane state"""
        return {
            'plane': PlaneType.TOTALITY.value,
            'being_state': self.being,
            'image_state': self.image,
            'identity': self.identity,
            'wholeness': self.wholeness,
            'autonomy': self.autonomy,
            'self_awareness': self.self_awareness
        }


@dataclass
class ThreePlanes:
    """
    Orchestrator for the three planes of existence

    Manages interactions between:
    - Plane of Energy (Objects)
    - Plane of Information (Relations)
    - Plane of Totality (Whole)
    """
    energy: PlaneOfEnergy = field(default_factory=PlaneOfEnergy)
    information: PlaneOfInformation = field(default_factory=PlaneOfInformation)
    totality: PlaneOfTotality = field(default_factory=PlaneOfTotality)

    def step(self, input_value: float = 0.5) -> Dict[str, Any]:
        """Execute one step across all planes"""
        # Energy plane operations
        energy_vortex = self.energy.vortex_cycle()
        energy_morpho = self.energy.morphogenesis(input_value)
        energy_drift = self.energy.compute_entropic_drift()

        # Information plane operations
        info_retro = self.information.retroaction_cycle(input_value)
        info_regulation = self.information.self_regulation()
        info_drift = self.information.compute_information_drift()

        # Totality plane operations
        self_ref = self.totality.self_referential_loop()
        ref_drift = self.totality.referential_drift()

        # Update totality being based on other planes
        self.totality.update_being({
            'energy_stability': self.energy.stability,
            'information_coherence': self.information.coherence,
        })

        return {
            'energy': {
                'vortex': energy_vortex,
                'morphogenesis': energy_morpho,
                'entropic_drift': energy_drift,
            },
            'information': {
                'retroaction': info_retro,
                'self_regulation': info_regulation,
                'information_drift': info_drift,
            },
            'totality': {
                'self_reference': self_ref,
                'referential_drift': ref_drift,
            }
        }

    def state(self) -> Dict[str, Any]:
        """Get complete state of all planes"""
        return {
            'energy': self.energy.state(),
            'information': self.information.state(),
            'totality': self.totality.state(),
        }

    def coherence(self) -> float:
        """Calculate overall coherence across planes"""
        return (
            self.energy.stability * 0.3 +
            self.information.coherence * 0.3 +
            self.totality.wholeness * 0.4
        )
