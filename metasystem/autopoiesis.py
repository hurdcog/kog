"""
Autopoiesis - Self-Production Mechanisms

Based on Maturana and Varela's theory of autopoiesis, implemented
within Eric Schwarz's organizational systems framework.

Autopoiesis (auto = self, poiesis = creation/production) is the
process by which a system:
1. Produces its own components
2. Maintains its own boundary
3. Regenerates damaged components
4. Creates operational closure

The autopoietic organization distinguishes living from non-living
systems and is key to understanding cognitive systems.
"""

from dataclasses import dataclass, field
from typing import List, Dict, Any, Optional, Set
from enum import Enum
import numpy as np


class AutopoieticState(Enum):
    """States of autopoietic operation"""
    DORMANT = "dormant"
    PRODUCING = "producing"
    MAINTAINING = "maintaining"
    REGENERATING = "regenerating"
    ADAPTING = "adapting"


class ComponentType(Enum):
    """Types of autopoietic components"""
    STRUCTURAL = "structural"
    RELATIONAL = "relational"
    BOUNDARY = "boundary"
    PROCESS = "process"


@dataclass
class AutopoieticComponent:
    """A component within an autopoietic system"""
    id: str
    component_type: ComponentType
    integrity: float = 1.0
    age: int = 0
    produced_by: Optional[str] = None  # ID of producing component
    produces: List[str] = field(default_factory=list)

    def decay(self, rate: float = 0.01) -> None:
        """Natural decay of component"""
        self.age += 1
        self.integrity = max(0, self.integrity - rate)

    def repair(self, amount: float = 0.1) -> None:
        """Repair component integrity"""
        self.integrity = min(1.0, self.integrity + amount)

    def is_functional(self, threshold: float = 0.3) -> bool:
        """Check if component is functional"""
        return self.integrity >= threshold

    def state(self) -> Dict[str, Any]:
        """Get component state"""
        return {
            'id': self.id,
            'type': self.component_type.value,
            'integrity': self.integrity,
            'age': self.age,
            'functional': self.is_functional(),
        }


@dataclass
class AutopoieticBoundary:
    """
    The boundary of an autopoietic system

    The boundary:
    - Separates inside from outside
    - Controls what enters and exits
    - Is itself produced by the system
    - Maintains system identity
    """
    permeability: float = 0.5  # How open the boundary is
    integrity: float = 1.0
    components: List[AutopoieticComponent] = field(default_factory=list)

    def add_boundary_component(self, component: AutopoieticComponent) -> None:
        """Add a component to the boundary"""
        if component.component_type == ComponentType.BOUNDARY:
            self.components.append(component)

    def compute_integrity(self) -> float:
        """Compute overall boundary integrity"""
        if not self.components:
            return self.integrity

        total_integrity = sum(c.integrity for c in self.components)
        self.integrity = total_integrity / len(self.components)
        return self.integrity

    def filter_input(self, input_value: float) -> float:
        """Filter input through boundary based on permeability"""
        return input_value * self.permeability * self.integrity

    def filter_output(self, output_value: float) -> float:
        """Filter output through boundary"""
        return output_value * (1 - self.permeability * 0.5) * self.integrity

    def adapt_permeability(self, internal_state: float, external_state: float) -> None:
        """Adapt permeability based on internal/external conditions"""
        difference = abs(internal_state - external_state)
        # Reduce permeability if difference is high (protect internal)
        self.permeability = max(0.1, min(0.9, 1 - difference))

    def decay(self, rate: float = 0.005) -> None:
        """Decay boundary components"""
        for component in self.components:
            component.decay(rate)
        self.compute_integrity()

    def state(self) -> Dict[str, Any]:
        """Get boundary state"""
        return {
            'permeability': self.permeability,
            'integrity': self.integrity,
            'component_count': len(self.components),
            'components': [c.state() for c in self.components],
        }


@dataclass
class AutopoieticProduction:
    """
    The production network of an autopoietic system

    Manages:
    - Component production
    - Production rules
    - Resource allocation
    - Production cycles
    """
    production_rules: Dict[str, List[str]] = field(default_factory=dict)
    production_capacity: float = 0.5
    production_history: List[Dict[str, Any]] = field(default_factory=list)

    def add_rule(self, producer: str, produces: List[str]) -> None:
        """Add a production rule"""
        self.production_rules[producer] = produces

    def can_produce(self, producer: str) -> bool:
        """Check if producer can produce"""
        return producer in self.production_rules and self.production_capacity > 0.1

    def produce(self, producer: str, energy: float) -> List[str]:
        """Execute production from a producer"""
        if not self.can_produce(producer):
            return []

        products = self.production_rules.get(producer, [])
        actual_production = []

        for product in products:
            if energy * self.production_capacity > 0.3:
                actual_production.append(product)
                self.production_capacity = max(0, self.production_capacity - 0.1)

        self.production_history.append({
            'producer': producer,
            'products': actual_production,
            'energy': energy,
        })

        return actual_production

    def replenish_capacity(self, amount: float = 0.1) -> None:
        """Replenish production capacity"""
        self.production_capacity = min(1.0, self.production_capacity + amount)

    def state(self) -> Dict[str, Any]:
        """Get production state"""
        return {
            'production_capacity': self.production_capacity,
            'rule_count': len(self.production_rules),
            'history_length': len(self.production_history),
            'rules': self.production_rules,
        }


@dataclass
class AutopoieticSystem:
    """
    A complete autopoietic system

    Implements the core autopoietic operations:
    1. Self-production of components
    2. Boundary maintenance
    3. Component regeneration
    4. Operational closure

    The system maintains itself through continuous
    production and regeneration of its components.
    """
    state: AutopoieticState = AutopoieticState.DORMANT
    components: Dict[str, AutopoieticComponent] = field(default_factory=dict)
    boundary: AutopoieticBoundary = field(default_factory=AutopoieticBoundary)
    production: AutopoieticProduction = field(default_factory=AutopoieticProduction)

    # Internal metrics
    internal_energy: float = 0.5
    internal_coherence: float = 0.5
    operational_closure: float = 0.0

    # History
    history: List[Dict[str, Any]] = field(default_factory=list)
    cycle_count: int = 0

    def initialize(self) -> None:
        """Initialize the autopoietic system with basic components"""
        # Create initial boundary components
        for i in range(3):
            boundary_comp = AutopoieticComponent(
                id=f"boundary_{i}",
                component_type=ComponentType.BOUNDARY
            )
            self.components[boundary_comp.id] = boundary_comp
            self.boundary.add_boundary_component(boundary_comp)

        # Create initial structural components
        for i in range(3):
            struct_comp = AutopoieticComponent(
                id=f"structure_{i}",
                component_type=ComponentType.STRUCTURAL
            )
            self.components[struct_comp.id] = struct_comp

        # Create initial relational components
        for i in range(2):
            rel_comp = AutopoieticComponent(
                id=f"relation_{i}",
                component_type=ComponentType.RELATIONAL
            )
            self.components[rel_comp.id] = rel_comp

        # Set up production rules (circular - operational closure)
        self.production.add_rule("structure_0", ["relation_0"])
        self.production.add_rule("relation_0", ["boundary_0"])
        self.production.add_rule("boundary_0", ["structure_1"])
        self.production.add_rule("structure_1", ["relation_1"])
        self.production.add_rule("relation_1", ["boundary_1"])
        self.production.add_rule("boundary_1", ["structure_2"])
        self.production.add_rule("structure_2", ["boundary_2"])
        self.production.add_rule("boundary_2", ["structure_0"])  # Closes the loop

        self.state = AutopoieticState.PRODUCING

    def _assess_damaged_components(self) -> List[AutopoieticComponent]:
        """Find components needing repair"""
        return [c for c in self.components.values()
                if not c.is_functional()]

    def _compute_operational_closure(self) -> float:
        """
        Compute degree of operational closure

        Operational closure = all processes produce components
        that participate in the processes that produce them
        """
        if not self.production.production_rules:
            return 0.0

        # Count circular production chains
        visited = set()
        circular_chains = 0
        total_chains = 0

        for producer in self.production.production_rules:
            products = self.production.production_rules[producer]
            for product in products:
                total_chains += 1
                # Check if product eventually leads back to producer
                if self._traces_back_to(product, producer, visited):
                    circular_chains += 1

        return circular_chains / max(1, total_chains)

    def _traces_back_to(self, current: str, target: str,
                        visited: Set[str], depth: int = 0) -> bool:
        """Check if production chain traces back to target"""
        if depth > 10 or current in visited:
            return False
        if current == target:
            return True

        visited.add(current)

        # Get what current produces
        products = self.production.production_rules.get(current, [])
        for product in products:
            # Extract base name (e.g., "boundary_0" -> "boundary")
            base_name = product.rsplit('_', 1)[0] if '_' in product else product
            # Check any component of this type
            for comp_id in self.production.production_rules:
                if comp_id.startswith(base_name):
                    if self._traces_back_to(comp_id, target, visited, depth + 1):
                        return True

        return False

    def produce_self(self, input_energy: float) -> Dict[str, Any]:
        """
        Execute autopoietic self-production

        The network produces components that realize the network.
        """
        self.state = AutopoieticState.PRODUCING

        # Filter input through boundary
        filtered_energy = self.boundary.filter_input(input_energy)
        self.internal_energy = 0.8 * self.internal_energy + 0.2 * filtered_energy

        produced = []
        for comp_id, component in self.components.items():
            if component.is_functional():
                products = self.production.produce(comp_id, self.internal_energy)
                produced.extend(products)

        # Replenish production capacity based on component health
        healthy_ratio = sum(1 for c in self.components.values() if c.is_functional())
        healthy_ratio /= max(1, len(self.components))
        self.production.replenish_capacity(0.05 * healthy_ratio)

        return {
            'produced': produced,
            'internal_energy': self.internal_energy,
            'production_capacity': self.production.production_capacity,
        }

    def maintain_boundary(self, external_state: float = 0.5) -> Dict[str, Any]:
        """
        Maintain the autopoietic boundary

        The boundary is produced by the system and maintains
        the system's identity by separating inside from outside.
        """
        self.state = AutopoieticState.MAINTAINING

        # Adapt permeability
        self.boundary.adapt_permeability(self.internal_energy, external_state)

        # Repair boundary components
        for component in self.boundary.components:
            if not component.is_functional():
                component.repair(0.1 * self.internal_energy)

        # Compute integrity
        integrity = self.boundary.compute_integrity()

        return {
            'permeability': self.boundary.permeability,
            'integrity': integrity,
            'boundary_functional': integrity > 0.5,
        }

    def regenerate_components(self) -> Dict[str, Any]:
        """
        Regenerate damaged components

        The system repairs itself through production of
        new components to replace damaged ones.
        """
        self.state = AutopoieticState.REGENERATING

        damaged = self._assess_damaged_components()
        regenerated = []

        for component in damaged:
            if self.internal_energy > 0.2:
                component.repair(0.2)
                self.internal_energy -= 0.05
                if component.is_functional():
                    regenerated.append(component.id)

        return {
            'damaged_count': len(damaged),
            'regenerated': regenerated,
            'remaining_damaged': len(damaged) - len(regenerated),
        }

    def decay_components(self, rate: float = 0.01) -> None:
        """Apply natural decay to all components"""
        for component in self.components.values():
            component.decay(rate)
        self.boundary.decay(rate)

    def autopoietic_cycle(self, input_energy: float,
                          external_state: float = 0.5) -> Dict[str, Any]:
        """
        Execute one complete autopoietic cycle

        1. Decay components (entropy)
        2. Produce new components
        3. Maintain boundary
        4. Regenerate damaged components
        5. Assess operational closure
        """
        self.cycle_count += 1

        # 1. Decay
        self.decay_components()

        # 2. Production
        production_result = self.produce_self(input_energy)

        # 3. Boundary maintenance
        boundary_result = self.maintain_boundary(external_state)

        # 4. Regeneration
        regeneration_result = self.regenerate_components()

        # 5. Compute metrics
        self.operational_closure = self._compute_operational_closure()
        self.internal_coherence = (
            self.boundary.integrity * 0.3 +
            self.production.production_capacity * 0.3 +
            self.operational_closure * 0.4
        )

        result = {
            'cycle': self.cycle_count,
            'state': self.state.value,
            'production': production_result,
            'boundary': boundary_result,
            'regeneration': regeneration_result,
            'operational_closure': self.operational_closure,
            'internal_coherence': self.internal_coherence,
            'internal_energy': self.internal_energy,
        }

        self.history.append(result)
        return result

    def run_cycles(self, n: int, input_generator=None) -> List[Dict[str, Any]]:
        """Run n autopoietic cycles"""
        if input_generator is None:
            def input_generator(i):
                return 0.5 + 0.2 * np.sin(i * 0.1)

        results = []
        for i in range(n):
            input_energy = input_generator(i)
            result = self.autopoietic_cycle(input_energy)
            results.append(result)

        return results

    def get_state(self) -> Dict[str, Any]:
        """Get complete system state"""
        return {
            'state': self.state.value,
            'cycle_count': self.cycle_count,
            'component_count': len(self.components),
            'functional_components': sum(
                1 for c in self.components.values() if c.is_functional()
            ),
            'internal_energy': self.internal_energy,
            'internal_coherence': self.internal_coherence,
            'operational_closure': self.operational_closure,
            'boundary': self.boundary.state(),
            'production': self.production.state(),
            'components': {k: v.state() for k, v in self.components.items()},
        }

    def is_autopoietic(self) -> bool:
        """
        Check if system is truly autopoietic

        Requirements:
        1. Has operational closure (circular production)
        2. Produces its own boundary
        3. Has sufficient internal coherence
        """
        has_closure = self.operational_closure > 0.5
        has_boundary = self.boundary.integrity > 0.5
        has_coherence = self.internal_coherence > 0.5

        return has_closure and has_boundary and has_coherence


def demonstrate_autopoiesis():
    """Demonstrate autopoietic system"""
    print("=== Autopoiesis Demonstration ===\n")

    # Create and initialize system
    system = AutopoieticSystem()
    system.initialize()

    print("1. Initial State:")
    state = system.get_state()
    print(f"   Components: {state['component_count']}")
    print(f"   Functional: {state['functional_components']}")
    print(f"   Internal energy: {state['internal_energy']:.3f}")
    print(f"   Is autopoietic: {system.is_autopoietic()}")
    print()

    print("2. Running 20 autopoietic cycles...")
    results = system.run_cycles(20)
    print()

    print("3. Final State:")
    state = system.get_state()
    print(f"   Cycles completed: {state['cycle_count']}")
    print(f"   Functional components: {state['functional_components']}/{state['component_count']}")
    print(f"   Internal energy: {state['internal_energy']:.3f}")
    print(f"   Internal coherence: {state['internal_coherence']:.3f}")
    print(f"   Operational closure: {state['operational_closure']:.3f}")
    print(f"   Boundary integrity: {state['boundary']['integrity']:.3f}")
    print(f"   Is autopoietic: {system.is_autopoietic()}")
    print()

    print("4. Production State:")
    prod = state['production']
    print(f"   Production capacity: {prod['production_capacity']:.3f}")
    print(f"   Production rules: {prod['rule_count']}")
    print()

    print("=== Demonstration Complete ===")


if __name__ == "__main__":
    demonstrate_autopoiesis()
