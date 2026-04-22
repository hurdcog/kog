"""
Enterprise Autogenesis Model

Based on the Enterprise Product Development Framework depicting:
- Six stages of product/value creation
- Multiple feedback loops for continuous learning
- End-to-end thin slice delivery pattern
- Control through learning loops (vs linear command-and-control)

This module implements an organizational autogenesis framework that
enables self-organizing, adaptive enterprise systems capable of
generating their own operational rules and evolving autonomously.

The model maps to Schwarz's metasystem theory:
- Six stages align with the six cycles
- Feedback loops implement horizontal/vertical cycle dynamics
- Thin slices enable incremental autopoietic production
- Learning loops drive autogenesis (self-creation of rules)
"""

from dataclasses import dataclass, field
from typing import List, Dict, Any, Optional, Callable, Set
from enum import Enum
from abc import ABC, abstractmethod
import numpy as np


class EnterpriseStage(Enum):
    """Six stages of enterprise autogenesis"""
    EXECUTIVE_VISION = 1      # Goal-based vision setting
    PRODUCT_STRATEGY = 2      # Own the problem / Shape the approach
    PORTFOLIO_MANAGEMENT = 3  # Value-based investment
    PRODUCT_ARCHITECTURE = 4  # Actionable thin slice
    LEAN_DELIVERY = 5         # Autonomous pods, iterative creation
    MEASURE_VALUE = 6         # Outcomes not output


class LoopType(Enum):
    """Types of feedback loops in the enterprise system"""
    # Strategic loops (outer)
    ENTERPRISE_VISION_MARKET = "enterprise_vision_market_opportunity"
    CONTINUOUS_VISIONING = "continuous_visioning"
    CROSS_ENTERPRISE = "cross_enterprise"

    # Tactical loops (middle)
    PRODUCT_OPPORTUNITY = "product_opportunity_performance"
    PRODUCT_STRATEGY_ARCHITECTURE = "product_strategy_architecture"
    PRODUCT_ARCHITECTURE_MARKET = "product_architecture_market_performance"

    # Operational loops (inner)
    DESIGN_RESEARCH = "design_research"
    AGILE_DEVELOPMENT = "agile_development"
    DESIGN_DELIVERY = "design_delivery"


class ControlMode(Enum):
    """Control paradigms"""
    LINEAR_COMMAND = "linear_command_control"  # Specification -> Task -> Output -> Conformance
    LEARNING_LOOPS = "learning_loops"          # Idea -> Test -> Learn -> Outcome


@dataclass
class VisionGoalBet:
    """Executive vision structure: Vision -> Goals -> Bets"""
    vision: str
    goals: List[str] = field(default_factory=list)
    bets: List[Dict[str, Any]] = field(default_factory=list)

    def add_goal(self, goal: str) -> None:
        """Add a goal aligned with vision"""
        self.goals.append(goal)

    def add_bet(self, goal_index: int, bet: str, confidence: float = 0.5) -> None:
        """Add a bet (hypothesis) under a goal"""
        self.bets.append({
            'goal_index': goal_index,
            'bet': bet,
            'confidence': confidence,
            'validated': False
        })

    def validate_bet(self, bet_index: int, outcome: bool) -> None:
        """Validate a bet based on outcome"""
        if 0 <= bet_index < len(self.bets):
            self.bets[bet_index]['validated'] = True
            self.bets[bet_index]['outcome'] = outcome


@dataclass
class ThinSlice:
    """
    End-to-End Thin Slice

    A vertical slice through all six stages that delivers
    incremental value rather than changing everything at once.
    """
    id: str
    description: str
    stages_completed: Set[EnterpriseStage] = field(default_factory=set)
    value_delivered: float = 0.0
    outcomes_measured: List[Dict[str, Any]] = field(default_factory=list)

    def complete_stage(self, stage: EnterpriseStage) -> None:
        """Mark a stage as completed for this slice"""
        self.stages_completed.add(stage)

    def is_end_to_end(self) -> bool:
        """Check if slice passes through all stages"""
        return len(self.stages_completed) == 6

    def measure_outcome(self, metric: str, value: float) -> None:
        """Record an outcome measurement"""
        self.outcomes_measured.append({
            'metric': metric,
            'value': value,
            'stages': len(self.stages_completed)
        })
        self.value_delivered = sum(o['value'] for o in self.outcomes_measured) / len(self.outcomes_measured)


@dataclass
class LearningLoop:
    """
    Learning Loop: Idea -> Test -> Learn -> Outcome

    Replaces linear command-and-control with adaptive learning.
    """
    name: str
    loop_type: LoopType
    phase: str = "idea"  # idea, test, learn, outcome
    iterations: int = 0
    learnings: List[Dict[str, Any]] = field(default_factory=list)

    def step(self, input_data: Dict[str, Any]) -> Dict[str, Any]:
        """Execute one step of the learning loop"""
        self.iterations += 1

        if self.phase == "idea":
            # Generate hypothesis from input
            result = {
                'phase': 'idea',
                'hypothesis': input_data.get('hypothesis', 'default_hypothesis'),
                'ready_for_test': True
            }
            self.phase = "test"

        elif self.phase == "test":
            # Execute test
            result = {
                'phase': 'test',
                'test_result': input_data.get('test_result', 0.5),
                'ready_for_learn': True
            }
            self.phase = "learn"

        elif self.phase == "learn":
            # Extract learning
            learning = {
                'iteration': self.iterations,
                'insight': input_data.get('insight', 'no_insight'),
                'confidence': input_data.get('confidence', 0.5)
            }
            self.learnings.append(learning)
            result = {
                'phase': 'learn',
                'learning': learning,
                'ready_for_outcome': True
            }
            self.phase = "outcome"

        else:  # outcome
            # Measure and integrate outcome
            result = {
                'phase': 'outcome',
                'outcome_value': input_data.get('outcome_value', 0.5),
                'cycle_complete': True,
                'total_learnings': len(self.learnings)
            }
            self.phase = "idea"  # Reset for next cycle

        return result


@dataclass
class AutonomousPod:
    """
    Autonomous Pod for Lean Delivery

    Self-organizing team unit that operates within
    the enterprise autogenesis framework.
    """
    id: str
    name: str
    capabilities: List[str] = field(default_factory=list)
    active_slices: List[ThinSlice] = field(default_factory=list)
    velocity: float = 0.5
    autonomy_level: float = 0.5

    def assign_slice(self, slice: ThinSlice) -> None:
        """Assign a thin slice to this pod"""
        self.active_slices.append(slice)

    def deliver_increment(self) -> Dict[str, Any]:
        """Deliver an increment of value"""
        if not self.active_slices:
            return {'delivered': False, 'reason': 'no_active_slices'}

        # Work on first active slice
        current = self.active_slices[0]

        # Complete next stage based on velocity
        remaining = set(EnterpriseStage) - current.stages_completed
        if remaining:
            next_stage = min(remaining, key=lambda s: s.value)
            current.complete_stage(next_stage)

            if current.is_end_to_end():
                self.active_slices.pop(0)
                return {
                    'delivered': True,
                    'slice_complete': True,
                    'slice_id': current.id,
                    'value': current.value_delivered
                }

            return {
                'delivered': True,
                'stage_complete': next_stage.name,
                'slice_id': current.id,
                'progress': len(current.stages_completed) / 6
            }

        return {'delivered': False, 'reason': 'slice_already_complete'}


@dataclass
class EnterpriseStageState:
    """State of an enterprise stage"""
    stage: EnterpriseStage
    active: bool = False
    throughput: float = 0.0
    backlog: List[Dict[str, Any]] = field(default_factory=list)
    completed: List[Dict[str, Any]] = field(default_factory=list)

    def add_to_backlog(self, item: Dict[str, Any]) -> None:
        """Add item to stage backlog"""
        self.backlog.append(item)

    def process(self, capacity: float = 1.0) -> Dict[str, Any]:
        """Process items through the stage"""
        if not self.backlog:
            return {'processed': 0}

        # Process based on capacity
        to_process = int(capacity * len(self.backlog))
        if to_process < 1 and self.backlog:
            to_process = 1

        processed = self.backlog[:to_process]
        self.backlog = self.backlog[to_process:]
        self.completed.extend(processed)

        self.throughput = len(processed) / max(1, len(processed) + len(self.backlog))

        return {
            'processed': len(processed),
            'remaining': len(self.backlog),
            'throughput': self.throughput
        }


@dataclass
class EnterpriseAutogenesis:
    """
    Enterprise Autogenesis System

    Implements the complete enterprise autogenesis model with:
    - Six stages of value creation
    - Multiple feedback loops
    - Thin slice delivery
    - Learning-based control
    - Autonomous pods

    Maps to Schwarz metasystem:
    - Stages 1-2: Plane of Totality (vision, identity)
    - Stages 3-4: Plane of Information (portfolio, architecture)
    - Stages 5-6: Plane of Energy (delivery, measurement)

    Cycles mapping:
    - Stage 1 (Vision): Self-referential cycle
    - Stage 2 (Strategy): Autogenesis cycle
    - Stage 3 (Portfolio): Retroaction cycle
    - Stage 4 (Architecture): Morphogenesis cycle
    - Stage 5 (Delivery): Autopoiesis cycle
    - Stage 6 (Measure): Vortex cycle (feedback)
    """

    # Core state
    vision: VisionGoalBet = field(default_factory=lambda: VisionGoalBet(vision=""))
    stages: Dict[EnterpriseStage, EnterpriseStageState] = field(default_factory=dict)

    # Feedback loops
    loops: Dict[LoopType, LearningLoop] = field(default_factory=dict)

    # Delivery
    pods: List[AutonomousPod] = field(default_factory=list)
    thin_slices: List[ThinSlice] = field(default_factory=list)

    # Metrics
    control_mode: ControlMode = ControlMode.LEARNING_LOOPS
    coherence: float = 0.0
    emergence: float = 0.0
    autonomy: float = 0.0
    value_throughput: float = 0.0

    # History
    cycle_count: int = 0
    history: List[Dict[str, Any]] = field(default_factory=list)

    def __post_init__(self):
        """Initialize after dataclass creation"""
        # Initialize stages
        for stage in EnterpriseStage:
            self.stages[stage] = EnterpriseStageState(stage=stage)

        # Initialize loops
        for loop_type in LoopType:
            self.loops[loop_type] = LearningLoop(
                name=loop_type.value,
                loop_type=loop_type
            )

    def initialize(self, vision: str, goals: List[str] = None) -> Dict[str, Any]:
        """Initialize the enterprise autogenesis system"""
        self.vision = VisionGoalBet(vision=vision)

        if goals:
            for goal in goals:
                self.vision.add_goal(goal)

        # Activate first stage
        self.stages[EnterpriseStage.EXECUTIVE_VISION].active = True

        return {
            'initialized': True,
            'vision': vision,
            'goals': len(self.vision.goals),
            'stages_ready': len(self.stages)
        }

    def create_thin_slice(self, id: str, description: str) -> ThinSlice:
        """Create a new thin slice for incremental delivery"""
        slice = ThinSlice(id=id, description=description)
        self.thin_slices.append(slice)

        # Add to first stage backlog
        self.stages[EnterpriseStage.EXECUTIVE_VISION].add_to_backlog({
            'slice_id': id,
            'type': 'thin_slice'
        })

        return slice

    def create_pod(self, id: str, name: str, capabilities: List[str] = None) -> AutonomousPod:
        """Create an autonomous delivery pod"""
        pod = AutonomousPod(
            id=id,
            name=name,
            capabilities=capabilities or []
        )
        self.pods.append(pod)
        return pod

    def step(self, input_value: float = 0.5) -> Dict[str, Any]:
        """Execute one enterprise autogenesis step"""
        self.cycle_count += 1
        result = {'cycle': self.cycle_count}

        # 1. Process stages (pipeline flow)
        stage_results = {}
        for stage in EnterpriseStage:
            stage_state = self.stages[stage]
            stage_result = stage_state.process(input_value)
            stage_results[stage.name] = stage_result

            # Flow completed items to next stage
            if stage.value < 6:
                next_stage = EnterpriseStage(stage.value + 1)
                for item in stage_state.completed[-stage_result['processed']:]:
                    self.stages[next_stage].add_to_backlog(item)

        result['stages'] = stage_results

        # 2. Execute feedback loops
        loop_results = {}
        for loop_type, loop in self.loops.items():
            loop_input = {
                'hypothesis': f'cycle_{self.cycle_count}',
                'test_result': input_value,
                'insight': f'learning_{loop.iterations}',
                'confidence': input_value,
                'outcome_value': input_value
            }
            loop_result = loop.step(loop_input)
            loop_results[loop_type.value] = loop_result

        result['loops'] = loop_results

        # 3. Pod delivery
        pod_results = []
        for pod in self.pods:
            pod_result = pod.deliver_increment()
            pod_results.append({
                'pod_id': pod.id,
                'result': pod_result
            })

        result['pods'] = pod_results

        # 4. Update metrics
        self._update_metrics(result)
        result['metrics'] = {
            'coherence': self.coherence,
            'emergence': self.emergence,
            'autonomy': self.autonomy,
            'value_throughput': self.value_throughput
        }

        # Record history
        self.history.append(result)

        return result

    def _update_metrics(self, step_result: Dict[str, Any]) -> None:
        """Update enterprise metrics"""
        # Coherence: alignment between vision and delivery
        vision_clarity = 1.0 if self.vision.vision else 0.0
        goal_alignment = len(self.vision.goals) / max(1, len(self.vision.goals) + 3)
        stage_flow = sum(
            s.throughput for s in self.stages.values()
        ) / len(self.stages)

        self.coherence = (vision_clarity * 0.3 + goal_alignment * 0.3 + stage_flow * 0.4)

        # Emergence: new value and learnings
        total_learnings = sum(len(l.learnings) for l in self.loops.values())
        completed_slices = sum(1 for s in self.thin_slices if s.is_end_to_end())

        self.emergence = min(1.0, (total_learnings * 0.05 + completed_slices * 0.1))

        # Autonomy: pod self-organization
        if self.pods:
            avg_pod_autonomy = sum(p.autonomy_level for p in self.pods) / len(self.pods)
        else:
            avg_pod_autonomy = 0.0

        loop_maturity = sum(l.iterations for l in self.loops.values()) / (len(self.loops) * 100)

        self.autonomy = (avg_pod_autonomy * 0.6 + min(1.0, loop_maturity) * 0.4)

        # Value throughput
        if self.thin_slices:
            self.value_throughput = sum(s.value_delivered for s in self.thin_slices) / len(self.thin_slices)
        else:
            self.value_throughput = 0.0

    def evolve(self, steps: int, input_generator: Optional[Callable] = None) -> List[Dict[str, Any]]:
        """Evolve the enterprise system for multiple steps"""
        if input_generator is None:
            def input_generator(i):
                return 0.5 + 0.3 * np.sin(i * 0.1)

        results = []
        for i in range(steps):
            input_value = input_generator(i)
            result = self.step(input_value)
            results.append(result)

        return results

    def reflect(self) -> Dict[str, Any]:
        """Enterprise self-reflection and diagnosis"""
        diagnosis = []
        recommendations = []

        # Vision assessment
        if not self.vision.vision:
            diagnosis.append("No vision defined")
            recommendations.append("Establish clear executive vision")
        elif len(self.vision.goals) == 0:
            diagnosis.append("Vision lacks goals")
            recommendations.append("Define measurable goals aligned with vision")

        # Stage flow assessment
        bottlenecks = []
        for stage, state in self.stages.items():
            if len(state.backlog) > 10:
                bottlenecks.append(stage.name)

        if bottlenecks:
            diagnosis.append(f"Bottlenecks at: {', '.join(bottlenecks)}")
            recommendations.append("Increase capacity at bottleneck stages")

        # Delivery assessment
        if not self.pods:
            diagnosis.append("No autonomous pods")
            recommendations.append("Create autonomous delivery pods")
        elif all(len(p.active_slices) == 0 for p in self.pods):
            diagnosis.append("Pods have no active work")
            recommendations.append("Assign thin slices to pods")

        # Learning assessment
        low_learning_loops = [
            l.name for l in self.loops.values() if len(l.learnings) < 3
        ]
        if low_learning_loops:
            diagnosis.append(f"Low learning in loops: {', '.join(low_learning_loops[:3])}")
            recommendations.append("Increase experimentation and reflection")

        # Coherence/emergence/autonomy
        if self.coherence < 0.3:
            diagnosis.append("Low coherence - vision-delivery misalignment")
            recommendations.append("Strengthen vision-to-delivery connection")

        if self.emergence < 0.3:
            diagnosis.append("Low emergence - insufficient innovation")
            recommendations.append("Create more thin slices and run experiments")

        if self.autonomy < 0.3:
            diagnosis.append("Low autonomy - command-control patterns")
            recommendations.append("Empower pods with decision authority")

        if not diagnosis:
            diagnosis.append("Enterprise operating normally")

        return {
            'diagnosis': diagnosis,
            'recommendations': recommendations,
            'metrics': {
                'coherence': self.coherence,
                'emergence': self.emergence,
                'autonomy': self.autonomy,
                'value_throughput': self.value_throughput
            },
            'health': {
                'vision_defined': bool(self.vision.vision),
                'goals_count': len(self.vision.goals),
                'pods_count': len(self.pods),
                'active_slices': sum(len(p.active_slices) for p in self.pods),
                'total_learnings': sum(len(l.learnings) for l in self.loops.values())
            }
        }

    def get_state(self) -> Dict[str, Any]:
        """Get complete enterprise state"""
        return {
            'cycle_count': self.cycle_count,
            'control_mode': self.control_mode.value,
            'vision': {
                'vision': self.vision.vision,
                'goals': self.vision.goals,
                'bets': len(self.vision.bets)
            },
            'stages': {
                s.name: {
                    'active': self.stages[s].active,
                    'backlog': len(self.stages[s].backlog),
                    'completed': len(self.stages[s].completed),
                    'throughput': self.stages[s].throughput
                }
                for s in EnterpriseStage
            },
            'loops': {
                l.value: {
                    'phase': self.loops[l].phase,
                    'iterations': self.loops[l].iterations,
                    'learnings': len(self.loops[l].learnings)
                }
                for l in LoopType
            },
            'delivery': {
                'pods': len(self.pods),
                'thin_slices': len(self.thin_slices),
                'completed_slices': sum(1 for s in self.thin_slices if s.is_end_to_end())
            },
            'metrics': {
                'coherence': self.coherence,
                'emergence': self.emergence,
                'autonomy': self.autonomy,
                'value_throughput': self.value_throughput
            }
        }

    def to_metasystem_mapping(self) -> Dict[str, Any]:
        """
        Map enterprise autogenesis to Schwarz metasystem concepts

        Returns mapping showing how enterprise stages and loops
        correspond to metasystem planes, cycles, and operations.
        """
        return {
            'planes': {
                'totality': {
                    'stages': [EnterpriseStage.EXECUTIVE_VISION.name,
                              EnterpriseStage.PRODUCT_STRATEGY.name],
                    'function': 'Vision, identity, and strategic direction'
                },
                'information': {
                    'stages': [EnterpriseStage.PORTFOLIO_MANAGEMENT.name,
                              EnterpriseStage.PRODUCT_ARCHITECTURE.name],
                    'function': 'Portfolio optimization and architectural patterns'
                },
                'energy': {
                    'stages': [EnterpriseStage.LEAN_DELIVERY.name,
                              EnterpriseStage.MEASURE_VALUE.name],
                    'function': 'Value delivery and outcome measurement'
                }
            },
            'cycles': {
                'self_referential': {
                    'enterprise_stage': EnterpriseStage.EXECUTIVE_VISION.name,
                    'loops': [LoopType.CONTINUOUS_VISIONING.value],
                    'function': 'Vision-reality dialogue'
                },
                'autogenesis': {
                    'enterprise_stage': EnterpriseStage.PRODUCT_STRATEGY.name,
                    'loops': [LoopType.ENTERPRISE_VISION_MARKET.value],
                    'function': 'Self-creation of strategic rules'
                },
                'retroaction': {
                    'enterprise_stage': EnterpriseStage.PORTFOLIO_MANAGEMENT.name,
                    'loops': [LoopType.PRODUCT_OPPORTUNITY.value],
                    'function': 'Investment homeostasis'
                },
                'morphogenesis': {
                    'enterprise_stage': EnterpriseStage.PRODUCT_ARCHITECTURE.name,
                    'loops': [LoopType.PRODUCT_STRATEGY_ARCHITECTURE.value],
                    'function': 'Emergence of new structures'
                },
                'autopoiesis': {
                    'enterprise_stage': EnterpriseStage.LEAN_DELIVERY.name,
                    'loops': [LoopType.AGILE_DEVELOPMENT.value,
                             LoopType.DESIGN_RESEARCH.value],
                    'function': 'Self-production through pods'
                },
                'vortex': {
                    'enterprise_stage': EnterpriseStage.MEASURE_VALUE.name,
                    'loops': [LoopType.CROSS_ENTERPRISE.value],
                    'function': 'Value cycling and feedback'
                }
            },
            'thin_slice_as_autopoietic_unit': {
                'description': 'Each thin slice is an autopoietic unit that produces '
                              'value through all stages, maintaining organizational '
                              'closure while enabling incremental evolution',
                'mapping': 'Thin slice = minimal viable autopoietic cycle'
            },
            'learning_loop_as_autogenesis': {
                'description': 'Learning loops (Idea->Test->Learn->Outcome) implement '
                              'autogenesis by creating new operational rules from '
                              'experiential feedback rather than top-down specification',
                'mapping': 'Learning loop = autogenetic rule generation'
            }
        }


def demonstrate_enterprise_autogenesis():
    """Demonstrate the enterprise autogenesis system"""
    print("=== Enterprise Autogenesis Demonstration ===\n")

    # Create system
    enterprise = EnterpriseAutogenesis()

    # Initialize with vision
    print("1. Initializing Enterprise...")
    init_result = enterprise.initialize(
        vision="Create cognitive synergy platform for AGI research",
        goals=[
            "Integrate OpenCog components",
            "Enable reproducible development",
            "Achieve cognitive autonomy"
        ]
    )
    print(f"   Initialized: {init_result['initialized']}")
    print(f"   Vision: {init_result['vision'][:50]}...")
    print(f"   Goals: {init_result['goals']}")
    print()

    # Create thin slices
    print("2. Creating Thin Slices...")
    slice1 = enterprise.create_thin_slice("TS-001", "AtomSpace integration")
    slice2 = enterprise.create_thin_slice("TS-002", "Metasystem implementation")
    print(f"   Created {len(enterprise.thin_slices)} thin slices")
    print()

    # Create autonomous pods
    print("3. Creating Autonomous Pods...")
    pod1 = enterprise.create_pod("POD-001", "Core Team", ["atomspace", "cogutil"])
    pod1.assign_slice(slice1)
    pod2 = enterprise.create_pod("POD-002", "Integration Team", ["synergy", "metasystem"])
    pod2.assign_slice(slice2)
    print(f"   Created {len(enterprise.pods)} pods")
    print()

    # Evolve
    print("4. Evolving for 20 cycles...")
    results = enterprise.evolve(20)
    print(f"   Completed {len(results)} cycles")
    print()

    # State
    print("5. Current State:")
    state = enterprise.get_state()
    print(f"   Coherence: {state['metrics']['coherence']:.3f}")
    print(f"   Emergence: {state['metrics']['emergence']:.3f}")
    print(f"   Autonomy: {state['metrics']['autonomy']:.3f}")
    print(f"   Value Throughput: {state['metrics']['value_throughput']:.3f}")
    print()

    # Reflection
    print("6. Self-Reflection:")
    reflection = enterprise.reflect()
    print(f"   Diagnosis: {reflection['diagnosis'][:2]}")
    print(f"   Recommendations: {reflection['recommendations'][:2]}")
    print()

    # Metasystem mapping
    print("7. Metasystem Mapping:")
    mapping = enterprise.to_metasystem_mapping()
    print(f"   Planes mapped: {list(mapping['planes'].keys())}")
    print(f"   Cycles mapped: {list(mapping['cycles'].keys())}")
    print()

    print("=== Demonstration Complete ===")


if __name__ == "__main__":
    demonstrate_enterprise_autogenesis()
