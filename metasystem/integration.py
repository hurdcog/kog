"""
Metasystem Integration - Bridges to OCC Components

Integrates the metasystem with existing OCC components:
- Synergy orchestrator
- CogSelf identity
- AAR (Agent-Arena-Relation) framework
- Membrane system
- AtomSpace (when available)

This module provides the glue between the theoretical metasystem
framework and the practical cognitive architecture components.
"""

from dataclasses import dataclass, field
from typing import List, Dict, Any, Optional, TYPE_CHECKING
from pathlib import Path
import sys

# Add parent path for imports
sys.path.insert(0, str(Path(__file__).parent.parent))

from .metasystem_core import Metasystem, MetasystemConfig, MetasystemState
from .planes import ThreePlanes, PlaneOfEnergy, PlaneOfInformation, PlaneOfTotality
from .cycles import SixCycles
from .spiral_triad import SpiralTriad, SpiralStep
from .evolutionary_helix import EvolutionaryHelix, HelixStage
from .enterprise_autogenesis import (
    EnterpriseAutogenesis,
    EnterpriseStage,
    ThinSlice,
    LearningLoop,
    AutonomousPod,
)


@dataclass
class AARMetasystemBridge:
    """
    Bridge between AAR (Agent-Arena-Relation) and Metasystem

    Maps AAR concepts to metasystem concepts:
    - Agent (urge-to-act) -> Plane of Energy (objects, structures)
    - Arena (need-to-be) -> Plane of Information (relations, constraints)
    - Relation (self) -> Plane of Totality (wholeness, identity)

    AAR provides the self-awareness mechanism while metasystem
    provides the organizational framework.
    """
    metasystem: Metasystem = field(default_factory=Metasystem)

    def aar_to_metasystem(self, aar_state: Dict[str, Any]) -> Dict[str, Any]:
        """
        Transform AAR state to metasystem input

        Maps:
        - agent_state -> energy plane input
        - arena_state -> information plane constraints
        - coherence/emergence -> totality plane updates
        """
        # Extract AAR components
        agent_norm = aar_state.get('agent_norm', 0.5)
        arena_capacity = aar_state.get('arena_capacity', 0.5)
        coherence = aar_state.get('coherence', 0.0)
        emergence = aar_state.get('emergence', 0.0)

        # Map to metasystem planes
        self.metasystem.planes.energy.flux = agent_norm
        self.metasystem.planes.information.coherence = coherence
        self.metasystem.planes.totality.wholeness = emergence

        # Update totality being with AAR self-state
        self.metasystem.planes.totality.update_being({
            'aar_coherence': coherence,
            'aar_emergence': emergence,
            'agent_norm': agent_norm,
            'arena_capacity': arena_capacity,
        })

        return {
            'mapped': True,
            'energy_flux': agent_norm,
            'info_coherence': coherence,
            'totality_wholeness': emergence,
        }

    def metasystem_to_aar(self) -> Dict[str, Any]:
        """
        Transform metasystem state back to AAR-compatible format

        Provides:
        - Guidance for agent state updates
        - Arena constraint suggestions
        - Self-representation updates
        """
        ms_state = self.metasystem.get_state()
        spiral_state = ms_state['spiral']
        helix_state = ms_state['helix']

        # Map metasystem metrics to AAR guidance
        return {
            'agent_guidance': {
                'energy_level': self.metasystem.planes.energy.flux,
                'stability': self.metasystem.planes.energy.stability,
                'spiral_step': spiral_state['current_step'],
            },
            'arena_guidance': {
                'coherence_target': ms_state['state']['coherence'],
                'constraint_update': self.metasystem.planes.information.homeostatic_setpoint,
            },
            'self_guidance': {
                'emergence_level': ms_state['state']['emergence'],
                'autonomy_level': ms_state['state']['autonomy'],
                'helix_stage': helix_state['current_stage'],
            }
        }

    def cognitive_cycle(self, aar_state: Dict[str, Any]) -> Dict[str, Any]:
        """
        Execute combined AAR-Metasystem cognitive cycle

        1. Import AAR state to metasystem
        2. Run metasystem step
        3. Export guidance back to AAR
        """
        # 1. Import
        import_result = self.aar_to_metasystem(aar_state)

        # 2. Step metasystem with AAR coherence as input
        input_value = aar_state.get('coherence', 0.5)
        step_result = self.metasystem.step(input_value)

        # 3. Export
        export_result = self.metasystem_to_aar()

        return {
            'import': import_result,
            'metasystem': step_result,
            'export': export_result,
        }


@dataclass
class SynergyMetasystemBridge:
    """
    Bridge between Synergy Orchestrator and Metasystem

    Maps synergy concepts to metasystem concepts:
    - Components -> Objects in Plane of Energy
    - Interactions -> Relations in Plane of Information
    - Synergy score -> Emergence metric

    The metasystem provides the theoretical framework for
    understanding and optimizing cognitive synergy.
    """
    metasystem: Metasystem = field(default_factory=Metasystem)

    def synergy_to_metasystem(self, synergy_state: Dict[str, Any]) -> Dict[str, Any]:
        """
        Transform synergy orchestrator state to metasystem input

        Maps:
        - component states -> energy plane objects
        - interaction metrics -> information plane relations
        - synergy score -> totality emergence
        """
        # Extract synergy components
        components = synergy_state.get('components', [])
        synergy_score = synergy_state.get('synergy_score', 0.0)
        interaction_density = synergy_state.get('interaction_density', 0.0)

        # Add components as objects
        for comp in components:
            self.metasystem.planes.energy.add_object(comp)

        # Update information plane with interaction density
        self.metasystem.planes.information.coherence = interaction_density

        # Update totality with synergy score
        self.metasystem.planes.totality.wholeness = synergy_score

        return {
            'mapped': True,
            'objects_added': len(components),
            'interaction_density': interaction_density,
            'synergy_score': synergy_score,
        }

    def metasystem_to_synergy(self) -> Dict[str, Any]:
        """
        Transform metasystem insights to synergy guidance

        Provides recommendations for:
        - Component coordination
        - Interaction optimization
        - Synergy enhancement
        """
        ms_state = self.metasystem.get_state()

        # Determine synergy recommendations based on metasystem state
        recommendations = []

        if ms_state['state']['coherence'] < 0.3:
            recommendations.append({
                'type': 'increase_coordination',
                'priority': 'high',
                'target': 'retroaction_cycles',
            })

        if ms_state['state']['emergence'] < 0.3:
            recommendations.append({
                'type': 'activate_emergence',
                'priority': 'medium',
                'target': 'morphogenesis_cycles',
            })

        spiral_step = ms_state['spiral']['current_step']
        if spiral_step in ['UNITY', 'DISTINCTION']:
            recommendations.append({
                'type': 'develop_relations',
                'priority': 'high',
                'target': 'component_interactions',
            })

        return {
            'synergy_guidance': {
                'target_coherence': ms_state['state']['coherence'],
                'target_emergence': ms_state['state']['emergence'],
                'spiral_phase': spiral_step,
                'recommendations': recommendations,
            }
        }


@dataclass
class MembraneMetasystemBridge:
    """
    Bridge between Membrane System and Metasystem

    Maps membrane concepts to metasystem concepts:
    - Membrane boundaries -> Autopoietic boundary
    - Membrane hierarchy -> Three planes structure
    - Membrane processes -> Six cycles

    The metasystem provides the autopoietic foundation
    for understanding membrane dynamics.
    """
    metasystem: Metasystem = field(default_factory=Metasystem)

    def membrane_to_metasystem(self, membrane_state: Dict[str, Any]) -> Dict[str, Any]:
        """
        Transform membrane state to metasystem input

        Maps:
        - membrane permeability -> autopoietic boundary
        - membrane contents -> plane objects/relations
        - membrane hierarchy -> organizational levels
        """
        permeability = membrane_state.get('permeability', 0.5)
        integrity = membrane_state.get('integrity', 1.0)
        processes = membrane_state.get('processes', [])

        # Update autopoietic boundary
        if self.metasystem.config.enable_autopoiesis:
            self.metasystem.autopoiesis.boundary.permeability = permeability
            self.metasystem.autopoiesis.boundary.integrity = integrity

        # Map processes to plane objects
        for process in processes:
            if process.get('type') == 'cognitive':
                self.metasystem.planes.information.add_relation(process)
            else:
                self.metasystem.planes.energy.add_object(process)

        return {
            'mapped': True,
            'boundary_permeability': permeability,
            'boundary_integrity': integrity,
            'processes_mapped': len(processes),
        }

    def metasystem_to_membrane(self) -> Dict[str, Any]:
        """
        Transform metasystem state to membrane guidance

        Provides:
        - Boundary permeability recommendations
        - Process allocation suggestions
        - Hierarchical organization guidance
        """
        ms_state = self.metasystem.get_state()

        # Get autopoietic metrics if available
        autopoiesis_state = ms_state.get('autopoiesis', {})
        boundary_state = autopoiesis_state.get('boundary', {}) if autopoiesis_state else {}

        return {
            'membrane_guidance': {
                'recommended_permeability': boundary_state.get('permeability', 0.5),
                'boundary_health': boundary_state.get('integrity', 1.0),
                'operational_closure': autopoiesis_state.get('operational_closure', 0) if autopoiesis_state else 0,
                'plane_mapping': {
                    'root': 'totality',
                    'cognitive': 'information',
                    'extension': 'energy',
                }
            }
        }


@dataclass
class IdentityMetasystemBridge:
    """
    Bridge between CogPrime Identity and Metasystem

    Maps identity concepts to metasystem concepts:
    - Cognitive characteristics -> Plane metrics
    - Episodic memory -> History tracking
    - Goals -> Spiral/Helix targets
    - Self-coherence -> Totality wholeness

    The metasystem provides the self-referential framework
    for identity formation and maintenance.
    """
    metasystem: Metasystem = field(default_factory=Metasystem)

    def identity_to_metasystem(self, identity_state: Dict[str, Any]) -> Dict[str, Any]:
        """
        Transform CogPrime identity state to metasystem input

        Maps:
        - cognitive_synergy -> cycle coordination
        - self_awareness -> self-referential loop
        - identity_stability -> coherence metric
        """
        synergy = identity_state.get('cognitive_synergy', 0.0)
        self_awareness = identity_state.get('self_awareness', 0.0)
        stability = identity_state.get('identity_stability', 0.0)
        goals = identity_state.get('goals', [])

        # Update totality plane with identity metrics
        self.metasystem.planes.totality.update_being({
            'cognitive_synergy': synergy,
            'self_awareness': self_awareness,
            'identity_stability': stability,
        })

        # Update self-image
        self.metasystem.planes.totality.update_image({
            'synergy_target': 0.8,
            'awareness_target': 0.7,
            'stability_target': 0.9,
        })

        # Map goals to spiral targets
        goal_energy = len(goals) * 0.1 if goals else 0.5

        return {
            'mapped': True,
            'synergy': synergy,
            'self_awareness': self_awareness,
            'stability': stability,
            'goal_energy': goal_energy,
        }

    def metasystem_to_identity(self) -> Dict[str, Any]:
        """
        Transform metasystem state to identity guidance

        Provides:
        - Self-model updates
        - Identity development recommendations
        - Goal alignment suggestions
        """
        ms_state = self.metasystem.get_state()
        reflection = self.metasystem.reflect()

        return {
            'identity_guidance': {
                'coherence_measure': ms_state['state']['coherence'],
                'emergence_measure': ms_state['state']['emergence'],
                'autonomy_measure': ms_state['state']['autonomy'],
                'self_awareness': reflection['self_awareness'],
                'development_stage': {
                    'spiral': ms_state['spiral']['current_step'],
                    'helix': ms_state['helix']['current_stage'],
                },
                'recommendations': reflection['recommendations'],
            }
        }


@dataclass
class EnterpriseMetasystemBridge:
    """
    Bridge between Enterprise Autogenesis and Metasystem

    Maps enterprise concepts to metasystem concepts:
    - Executive Vision (Stage 1) -> Self-Referential Cycle / Plane of Totality
    - Product Strategy (Stage 2) -> Autogenesis Cycle
    - Portfolio Management (Stage 3) -> Retroaction Cycle / Plane of Information
    - Product Architecture (Stage 4) -> Morphogenesis Cycle
    - Lean Delivery (Stage 5) -> Autopoiesis Cycle / Plane of Energy
    - Measure Value (Stage 6) -> Vortex Cycle

    Learning loops map to autogenetic rule generation.
    Thin slices map to autopoietic production units.
    """
    metasystem: Metasystem = field(default_factory=Metasystem)
    enterprise: EnterpriseAutogenesis = field(default_factory=EnterpriseAutogenesis)

    def enterprise_to_metasystem(self, enterprise_state: Dict[str, Any] = None) -> Dict[str, Any]:
        """
        Transform enterprise autogenesis state to metasystem input

        Maps:
        - enterprise coherence -> totality wholeness
        - enterprise emergence -> morphogenesis activation
        - enterprise autonomy -> autogenesis progress
        - thin slice throughput -> autopoietic production
        """
        if enterprise_state is None:
            enterprise_state = self.enterprise.get_state()

        metrics = enterprise_state.get('metrics', {})
        coherence = metrics.get('coherence', 0.0)
        emergence = metrics.get('emergence', 0.0)
        autonomy = metrics.get('autonomy', 0.0)
        value_throughput = metrics.get('value_throughput', 0.0)

        # Map to metasystem planes
        self.metasystem.planes.totality.wholeness = coherence
        self.metasystem.planes.information.coherence = emergence
        self.metasystem.planes.energy.flux = value_throughput

        # Update totality being with enterprise metrics
        self.metasystem.planes.totality.update_being({
            'enterprise_coherence': coherence,
            'enterprise_emergence': emergence,
            'enterprise_autonomy': autonomy,
            'value_throughput': value_throughput,
        })

        return {
            'mapped': True,
            'coherence': coherence,
            'emergence': emergence,
            'autonomy': autonomy,
            'value_throughput': value_throughput,
        }

    def metasystem_to_enterprise(self) -> Dict[str, Any]:
        """
        Transform metasystem state to enterprise guidance

        Provides:
        - Vision clarity recommendations
        - Portfolio optimization suggestions
        - Delivery acceleration guidance
        - Learning loop activation
        """
        ms_state = self.metasystem.get_state()
        spiral_state = ms_state['spiral']
        helix_state = ms_state['helix']

        # Generate enterprise recommendations
        recommendations = []

        if ms_state['state']['coherence'] < 0.3:
            recommendations.append({
                'stage': 'EXECUTIVE_VISION',
                'action': 'strengthen_vision_goal_alignment',
                'priority': 'high',
            })

        if ms_state['state']['emergence'] < 0.3:
            recommendations.append({
                'stage': 'PRODUCT_ARCHITECTURE',
                'action': 'create_more_thin_slices',
                'priority': 'medium',
            })

        if ms_state['state']['autonomy'] < 0.3:
            recommendations.append({
                'stage': 'LEAN_DELIVERY',
                'action': 'empower_autonomous_pods',
                'priority': 'high',
            })

        # Map spiral step to enterprise focus
        spiral_step = spiral_state['current_step']
        enterprise_focus = {
            'UNITY': 'EXECUTIVE_VISION',
            'DISTINCTION': 'PRODUCT_STRATEGY',
            'DISTANCIATION': 'PORTFOLIO_MANAGEMENT',
            'EMERGENCE': 'PRODUCT_ARCHITECTURE',
            'AUTOPOIESIS': 'LEAN_DELIVERY',
            'SELF_REFERENCE': 'MEASURE_VALUE',
            'AUTOGENESIS': 'EXECUTIVE_VISION',  # Return to vision with new rules
        }.get(spiral_step, 'EXECUTIVE_VISION')

        return {
            'enterprise_guidance': {
                'current_focus': enterprise_focus,
                'spiral_alignment': spiral_step,
                'helix_stage': helix_state['current_stage'],
                'recommendations': recommendations,
                'learning_loop_priority': 'cross_enterprise' if ms_state['state']['autonomy'] > 0.5 else 'agile_development',
            }
        }

    def synchronized_step(self, input_value: float = 0.5) -> Dict[str, Any]:
        """
        Execute synchronized step of both enterprise and metasystem

        1. Step enterprise autogenesis
        2. Map enterprise state to metasystem
        3. Step metasystem
        4. Generate enterprise guidance from metasystem
        """
        # Step enterprise
        enterprise_result = self.enterprise.step(input_value)

        # Map to metasystem
        mapping_result = self.enterprise_to_metasystem()

        # Step metasystem
        metasystem_result = self.metasystem.step(input_value)

        # Generate guidance
        guidance = self.metasystem_to_enterprise()

        return {
            'enterprise': enterprise_result,
            'mapping': mapping_result,
            'metasystem': metasystem_result,
            'guidance': guidance,
        }

    def get_mapping(self) -> Dict[str, Any]:
        """Get the theoretical mapping between enterprise and metasystem"""
        return self.enterprise.to_metasystem_mapping()


@dataclass
class IntegratedMetasystem:
    """
    Fully integrated metasystem with all OCC bridges

    Provides unified interface for:
    - AAR integration (self-awareness)
    - Synergy integration (component coordination)
    - Membrane integration (boundary management)
    - Identity integration (self-model)
    - Enterprise integration (organizational autogenesis)
    """
    config: MetasystemConfig = field(default_factory=MetasystemConfig)
    metasystem: Metasystem = field(default_factory=lambda: None)

    # Bridges
    aar_bridge: AARMetasystemBridge = field(default_factory=lambda: None)
    synergy_bridge: SynergyMetasystemBridge = field(default_factory=lambda: None)
    membrane_bridge: MembraneMetasystemBridge = field(default_factory=lambda: None)
    identity_bridge: IdentityMetasystemBridge = field(default_factory=lambda: None)
    enterprise_bridge: EnterpriseMetasystemBridge = field(default_factory=lambda: None)

    def __post_init__(self):
        """Initialize all components"""
        self.metasystem = Metasystem(config=self.config)
        self.aar_bridge = AARMetasystemBridge(metasystem=self.metasystem)
        self.synergy_bridge = SynergyMetasystemBridge(metasystem=self.metasystem)
        self.membrane_bridge = MembraneMetasystemBridge(metasystem=self.metasystem)
        self.identity_bridge = IdentityMetasystemBridge(metasystem=self.metasystem)
        self.enterprise_bridge = EnterpriseMetasystemBridge(metasystem=self.metasystem)

    def initialize(self) -> Dict[str, Any]:
        """Initialize the integrated metasystem"""
        return self.metasystem.initialize()

    def integrated_step(self,
                        aar_state: Optional[Dict[str, Any]] = None,
                        synergy_state: Optional[Dict[str, Any]] = None,
                        membrane_state: Optional[Dict[str, Any]] = None,
                        identity_state: Optional[Dict[str, Any]] = None,
                        enterprise_state: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """
        Execute integrated step with all available inputs

        Combines inputs from all OCC components and runs
        unified metasystem cycle.
        """
        results = {}

        # Process AAR input
        if aar_state:
            results['aar'] = self.aar_bridge.aar_to_metasystem(aar_state)

        # Process synergy input
        if synergy_state:
            results['synergy'] = self.synergy_bridge.synergy_to_metasystem(synergy_state)

        # Process membrane input
        if membrane_state:
            results['membrane'] = self.membrane_bridge.membrane_to_metasystem(membrane_state)

        # Process identity input
        if identity_state:
            results['identity'] = self.identity_bridge.identity_to_metasystem(identity_state)

        # Process enterprise input
        if enterprise_state:
            results['enterprise'] = self.enterprise_bridge.enterprise_to_metasystem(enterprise_state)

        # Compute combined input value
        input_value = 0.5
        input_count = 0

        if aar_state:
            input_value += aar_state.get('coherence', 0)
            input_count += 1
        if synergy_state:
            input_value += synergy_state.get('synergy_score', 0)
            input_count += 1
        if identity_state:
            input_value += identity_state.get('self_awareness', 0)
            input_count += 1
        if enterprise_state:
            input_value += enterprise_state.get('metrics', {}).get('coherence', 0)
            input_count += 1

        if input_count > 0:
            input_value = input_value / (input_count + 1)

        # Run metasystem step
        step_result = self.metasystem.step(input_value)
        results['metasystem'] = step_result

        # Generate guidance for all components
        results['guidance'] = {
            'aar': self.aar_bridge.metasystem_to_aar(),
            'synergy': self.synergy_bridge.metasystem_to_synergy(),
            'membrane': self.membrane_bridge.metasystem_to_membrane(),
            'identity': self.identity_bridge.metasystem_to_identity(),
            'enterprise': self.enterprise_bridge.metasystem_to_enterprise(),
        }

        return results

    def get_state(self) -> Dict[str, Any]:
        """Get complete integrated state"""
        return {
            'metasystem': self.metasystem.get_state(),
            'progress': self.metasystem.progress_summary(),
        }


def demonstrate_integration():
    """Demonstrate integrated metasystem"""
    print("=== Integrated Metasystem Demonstration ===\n")

    # Create integrated metasystem
    config = MetasystemConfig(
        initial_energy=0.7,
        enable_autopoiesis=True,
        enable_evolution=True,
    )
    integrated = IntegratedMetasystem(config=config)
    integrated.initialize()

    print("1. Initial State:")
    state = integrated.get_state()
    progress = state['progress']
    print(f"   Coherence: {progress['coherence']:.3f}")
    print(f"   Emergence: {progress['emergence']:.3f}")
    print(f"   Autonomy: {progress['autonomy']:.3f}")
    print()

    print("2. Running integrated steps with simulated OCC inputs...")

    # Simulate OCC component states
    aar_state = {'coherence': 0.6, 'emergence': 0.4, 'agent_norm': 0.7, 'arena_capacity': 0.8}
    synergy_state = {'synergy_score': 0.5, 'interaction_density': 0.6, 'components': ['comp1', 'comp2']}
    membrane_state = {'permeability': 0.5, 'integrity': 0.9, 'processes': []}
    identity_state = {'cognitive_synergy': 0.5, 'self_awareness': 0.4, 'identity_stability': 0.7}

    for i in range(10):
        result = integrated.integrated_step(
            aar_state=aar_state,
            synergy_state=synergy_state,
            membrane_state=membrane_state,
            identity_state=identity_state,
        )

        # Update simulated states based on guidance
        aar_state['coherence'] = min(1, aar_state['coherence'] + 0.02)
        synergy_state['synergy_score'] = min(1, synergy_state['synergy_score'] + 0.03)

    print(f"   Completed 10 integrated steps")
    print()

    print("3. Final State:")
    state = integrated.get_state()
    progress = state['progress']
    print(f"   Coherence: {progress['coherence']:.3f}")
    print(f"   Emergence: {progress['emergence']:.3f}")
    print(f"   Autonomy: {progress['autonomy']:.3f}")
    print(f"   Spiral: {progress['spiral']['step']}")
    print(f"   Helix: {progress['helix']['stage']}")
    print(f"   Viable: {progress['viable']}")
    print()

    print("4. Generated Guidance:")
    guidance = result['guidance']
    print(f"   AAR target coherence: {guidance['aar']['self_guidance']['emergence_level']:.3f}")
    print(f"   Synergy recommendations: {len(guidance['synergy']['synergy_guidance']['recommendations'])}")
    print(f"   Identity development stage: {guidance['identity']['identity_guidance']['development_stage']['spiral']}")
    print()

    print("=== Demonstration Complete ===")


if __name__ == "__main__":
    demonstrate_integration()
