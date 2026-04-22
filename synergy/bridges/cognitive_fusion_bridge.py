"""
Cross-Modal Cognitive Fusion Bridge

This module implements the integration between reasoning (PLN), learning (MOSES),
and memory (AtomSpace) into unified cognitive loops, enabling emergent cognitive
processing capabilities.

Emergent Capability: unified_cognitive_processing
- Reasoning outcomes guide learning
- Learning results enrich knowledge representation
- Memory patterns inform reasoning strategies
- Continuous feedback loops enable meta-learning

Architecture:
1. Shared representation layer in AtomSpace
2. PLN -> MOSES feedback loop (reasoning guides learning)
3. MOSES -> PLN feedback loop (learning enriches reasoning)
4. Meta-learning from reasoning outcomes
5. Evolution of reasoning strategies

Based on: Autogenesis Roadmap Feature #1
Implementation Status: Phase 1
"""

import logging
import numpy as np
from typing import Dict, List, Tuple, Optional, Set, Any
from dataclasses import dataclass, field
from enum import Enum
from collections import defaultdict
import time

logger = logging.getLogger(__name__)


class CognitiveMode(Enum):
    """Different modes of cognitive processing"""
    REASONING = "reasoning"          # PLN inference
    LEARNING = "learning"            # MOSES evolution
    CONSOLIDATION = "consolidation"  # Memory integration
    META_LEARNING = "meta_learning"  # Learning about learning


class FeedbackType(Enum):
    """Types of feedback between cognitive modes"""
    REASONING_TO_LEARNING = "reasoning_to_learning"
    LEARNING_TO_REASONING = "learning_to_reasoning"
    MEMORY_TO_REASONING = "memory_to_reasoning"
    MEMORY_TO_LEARNING = "memory_to_learning"
    META_FEEDBACK = "meta_feedback"


@dataclass
class CognitiveState:
    """Represents the current state of cognitive processing"""
    mode: CognitiveMode
    active_atoms: Set[str] = field(default_factory=set)
    reasoning_quality: float = 0.0
    learning_progress: float = 0.0
    memory_coherence: float = 0.0
    timestamp: float = field(default_factory=time.time)
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class ReasoningOutcome:
    """Result from PLN reasoning process"""
    conclusion: str
    confidence: float
    inference_steps: int
    supporting_atoms: List[str]
    reasoning_patterns: List[str]
    timestamp: float = field(default_factory=time.time)


@dataclass
class LearningOutcome:
    """Result from MOSES learning process"""
    learned_program: str
    fitness_score: float
    generations: int
    discovered_patterns: List[str]
    timestamp: float = field(default_factory=time.time)


@dataclass
class FeedbackSignal:
    """Feedback signal between cognitive modes"""
    source_mode: CognitiveMode
    target_mode: CognitiveMode
    feedback_type: FeedbackType
    strength: float
    content: Dict[str, Any]
    timestamp: float = field(default_factory=time.time)


class SharedRepresentationLayer:
    """
    Shared representation layer in AtomSpace for cross-modal integration
    
    This layer provides a common knowledge representation that all cognitive
    modes can read from and write to, enabling seamless integration.
    """
    
    def __init__(self):
        self.atoms: Dict[str, Dict] = {}
        self.connections: Dict[str, List[str]] = defaultdict(list)
        self.annotations: Dict[str, Dict] = defaultdict(dict)
        logger.info("SharedRepresentationLayer initialized")
    
    def add_atom(self, atom_id: str, atom_type: str, content: Any, 
                 source_mode: CognitiveMode) -> None:
        """Add or update an atom in the shared representation"""
        self.atoms[atom_id] = {
            'type': atom_type,
            'content': content,
            'source': source_mode.value,
            'created': time.time(),
            'access_count': 0
        }
        logger.debug(f"Added atom {atom_id} from {source_mode.value}")
    
    def get_atom(self, atom_id: str) -> Optional[Dict]:
        """Retrieve an atom and increment its access count"""
        if atom_id in self.atoms:
            self.atoms[atom_id]['access_count'] += 1
            return self.atoms[atom_id]
        return None
    
    def add_connection(self, source_id: str, target_id: str, 
                       relation_type: str) -> None:
        """Create a connection between atoms"""
        self.connections[source_id].append(target_id)
        self.annotate_atom(source_id, f"relation_{relation_type}", target_id)
        logger.debug(f"Connected {source_id} -> {target_id} ({relation_type})")
    
    def annotate_atom(self, atom_id: str, key: str, value: Any) -> None:
        """Add metadata annotation to an atom"""
        self.annotations[atom_id][key] = value
    
    def get_connected_atoms(self, atom_id: str) -> List[str]:
        """Get all atoms connected to a given atom"""
        return self.connections.get(atom_id, [])
    
    def get_atoms_by_source(self, source_mode: CognitiveMode) -> List[str]:
        """Get all atoms created by a specific cognitive mode"""
        return [
            atom_id for atom_id, atom in self.atoms.items()
            if atom['source'] == source_mode.value
        ]
    
    def get_high_access_atoms(self, threshold: int = 5) -> List[str]:
        """Get atoms that have been accessed frequently"""
        return [
            atom_id for atom_id, atom in self.atoms.items()
            if atom['access_count'] >= threshold
        ]


class PLNToMOSESFeedback:
    """
    Implements feedback loop from PLN reasoning to MOSES learning
    
    Reasoning outcomes guide the evolutionary search by:
    - Identifying promising search directions
    - Providing fitness bonuses for patterns that match reasoning
    - Suggesting new learning objectives based on reasoning gaps
    """
    
    def __init__(self, shared_layer: SharedRepresentationLayer):
        self.shared_layer = shared_layer
        self.feedback_history: List[FeedbackSignal] = []
        logger.info("PLNToMOSESFeedback initialized")
    
    def process_reasoning_outcome(self, outcome: ReasoningOutcome) -> FeedbackSignal:
        """Convert reasoning outcome into learning guidance"""
        # Extract patterns from reasoning
        guidance = {
            'search_focus': outcome.reasoning_patterns,
            'confidence_threshold': outcome.confidence,
            'supporting_context': outcome.supporting_atoms,
            'inference_depth': outcome.inference_steps
        }
        
        # Store reasoning patterns in shared layer
        for pattern in outcome.reasoning_patterns:
            pattern_id = f"reasoning_pattern_{hash(pattern)}"
            self.shared_layer.add_atom(
                pattern_id, 
                'reasoning_pattern',
                pattern,
                CognitiveMode.REASONING
            )
        
        signal = FeedbackSignal(
            source_mode=CognitiveMode.REASONING,
            target_mode=CognitiveMode.LEARNING,
            feedback_type=FeedbackType.REASONING_TO_LEARNING,
            strength=outcome.confidence,
            content=guidance
        )
        
        self.feedback_history.append(signal)
        logger.info(f"Generated reasoning->learning feedback (strength={outcome.confidence:.3f})")
        return signal
    
    def calculate_fitness_bonus(self, program: str, 
                                reasoning_context: List[str]) -> float:
        """Calculate fitness bonus for a program based on reasoning context"""
        bonus = 0.0
        
        # Check if program uses patterns identified during reasoning
        for pattern in reasoning_context:
            if pattern in program:
                bonus += 0.1
        
        # Check if program connects to high-value atoms
        high_value_atoms = self.shared_layer.get_high_access_atoms()
        for atom_id in high_value_atoms:
            if atom_id in program:
                bonus += 0.05
        
        return min(bonus, 1.0)  # Cap at 1.0


class MOSESToPLNFeedback:
    """
    Implements feedback loop from MOSES learning to PLN reasoning
    
    Learning outcomes enrich reasoning by:
    - Adding discovered patterns to the knowledge base
    - Suggesting new inference rules based on learned programs
    - Identifying frequently used reasoning strategies
    """
    
    def __init__(self, shared_layer: SharedRepresentationLayer):
        self.shared_layer = shared_layer
        self.feedback_history: List[FeedbackSignal] = []
        logger.info("MOSESToPLNFeedback initialized")
    
    def process_learning_outcome(self, outcome: LearningOutcome) -> FeedbackSignal:
        """Convert learning outcome into reasoning enrichment"""
        # Extract new patterns discovered by learning
        enrichment = {
            'new_patterns': outcome.discovered_patterns,
            'program_structure': outcome.learned_program,
            'fitness_indicator': outcome.fitness_score,
            'evolution_depth': outcome.generations
        }
        
        # Store learned patterns in shared layer
        for pattern in outcome.discovered_patterns:
            pattern_id = f"learned_pattern_{hash(pattern)}"
            self.shared_layer.add_atom(
                pattern_id,
                'learned_pattern',
                pattern,
                CognitiveMode.LEARNING
            )
            
            # Connect to reasoning patterns if they exist
            reasoning_patterns = self.shared_layer.get_atoms_by_source(
                CognitiveMode.REASONING
            )
            for rp_id in reasoning_patterns:
                rp = self.shared_layer.get_atom(rp_id)
                if rp and self._patterns_compatible(pattern, rp['content']):
                    self.shared_layer.add_connection(
                        pattern_id, rp_id, 'compatible_with'
                    )
        
        signal = FeedbackSignal(
            source_mode=CognitiveMode.LEARNING,
            target_mode=CognitiveMode.REASONING,
            feedback_type=FeedbackType.LEARNING_TO_REASONING,
            strength=outcome.fitness_score,
            content=enrichment
        )
        
        self.feedback_history.append(signal)
        logger.info(f"Generated learning->reasoning feedback (strength={outcome.fitness_score:.3f})")
        return signal
    
    def _patterns_compatible(self, pattern1: str, pattern2: str) -> bool:
        """Check if two patterns are compatible/related"""
        # Simple heuristic: check for common substrings
        if len(pattern1) < 3 or len(pattern2) < 3:
            return False
        return any(
            substr in pattern2 
            for substr in [pattern1[i:i+3] for i in range(len(pattern1)-2)]
        )
    
    def extract_inference_rules(self, learned_program: str) -> List[str]:
        """Extract potential inference rules from learned programs"""
        # Simplified extraction - in real implementation, would use AST parsing
        rules = []
        
        # Look for if-then patterns
        if 'if' in learned_program and 'then' in learned_program:
            rules.append(f"conditional_rule_{hash(learned_program)}")
        
        # Look for function compositions
        if '->' in learned_program or '.' in learned_program:
            rules.append(f"composition_rule_{hash(learned_program)}")
        
        return rules


class MetaLearningSystem:
    """
    Meta-learning system that learns from the interaction between reasoning
    and learning, evolving better strategies over time
    """
    
    def __init__(self, shared_layer: SharedRepresentationLayer):
        self.shared_layer = shared_layer
        self.strategy_performance: Dict[str, List[float]] = defaultdict(list)
        self.current_strategies: Set[str] = set()
        logger.info("MetaLearningSystem initialized")
    
    def observe_interaction(self, reasoning: ReasoningOutcome, 
                           learning: LearningOutcome) -> None:
        """Observe and learn from reasoning-learning interaction"""
        # Calculate synergy score
        synergy = self._calculate_synergy(reasoning, learning)
        
        # Identify strategy used
        strategy = self._identify_strategy(reasoning, learning)
        
        # Record performance
        self.strategy_performance[strategy].append(synergy)
        self.current_strategies.add(strategy)
        
        logger.info(f"Observed interaction with synergy={synergy:.3f} (strategy={strategy})")
    
    def _calculate_synergy(self, reasoning: ReasoningOutcome, 
                          learning: LearningOutcome) -> float:
        """Calculate synergy between reasoning and learning outcomes"""
        # High synergy when:
        # 1. Reasoning confidence is high
        # 2. Learning fitness is high
        # 3. They share common patterns
        
        base_synergy = (reasoning.confidence + learning.fitness_score) / 2
        
        # Bonus for shared patterns
        reasoning_set = set(reasoning.reasoning_patterns)
        learning_set = set(learning.discovered_patterns)
        common_patterns = len(reasoning_set & learning_set)
        pattern_bonus = min(common_patterns * 0.1, 0.3)
        
        return min(base_synergy + pattern_bonus, 1.0)
    
    def _identify_strategy(self, reasoning: ReasoningOutcome,
                          learning: LearningOutcome) -> str:
        """Identify the cognitive strategy being used"""
        # Classify based on characteristics
        if reasoning.inference_steps > 5 and learning.generations > 10:
            return "deep_exploration"
        elif reasoning.confidence > 0.8 and learning.fitness_score > 0.8:
            return "confident_convergence"
        elif reasoning.inference_steps < 3 and learning.generations < 5:
            return "quick_iteration"
        else:
            return "balanced_search"
    
    def get_best_strategy(self) -> Optional[str]:
        """Get the currently best performing strategy"""
        if not self.strategy_performance:
            return None
        
        avg_scores = {
            strategy: np.mean(scores)
            for strategy, scores in self.strategy_performance.items()
        }
        
        return max(avg_scores.items(), key=lambda x: x[1])[0]
    
    def evolve_strategy(self, current_strategy: str) -> str:
        """Evolve a strategy based on performance history"""
        best_strategy = self.get_best_strategy()
        
        if best_strategy and best_strategy != current_strategy:
            # Switch to best performing strategy
            logger.info(f"Evolving strategy: {current_strategy} -> {best_strategy}")
            return best_strategy
        
        return current_strategy


class CognitiveFusionBridge:
    """
    Main bridge coordinating cross-modal cognitive fusion
    
    Integrates reasoning, learning, and memory into unified cognitive loops.
    """
    
    def __init__(self):
        self.shared_layer = SharedRepresentationLayer()
        self.pln_to_moses = PLNToMOSESFeedback(self.shared_layer)
        self.moses_to_pln = MOSESToPLNFeedback(self.shared_layer)
        self.meta_learning = MetaLearningSystem(self.shared_layer)
        
        self.cognitive_state = CognitiveState(mode=CognitiveMode.REASONING)
        self.cycle_count = 0
        
        logger.info("CognitiveFusionBridge initialized")
    
    def process_reasoning_outcome(self, outcome: ReasoningOutcome) -> FeedbackSignal:
        """Process reasoning outcome and generate learning guidance"""
        self.cognitive_state.mode = CognitiveMode.REASONING
        self.cognitive_state.reasoning_quality = outcome.confidence
        
        return self.pln_to_moses.process_reasoning_outcome(outcome)
    
    def process_learning_outcome(self, outcome: LearningOutcome) -> FeedbackSignal:
        """Process learning outcome and enrich reasoning"""
        self.cognitive_state.mode = CognitiveMode.LEARNING
        self.cognitive_state.learning_progress = outcome.fitness_score
        
        return self.moses_to_pln.process_learning_outcome(outcome)
    
    def cognitive_cycle(self, reasoning: ReasoningOutcome, 
                       learning: LearningOutcome) -> Dict[str, Any]:
        """
        Execute a complete cognitive cycle with reasoning and learning
        
        Returns:
            Summary of the cognitive cycle including synergy metrics
        """
        # Process both outcomes
        r_to_l_signal = self.process_reasoning_outcome(reasoning)
        l_to_r_signal = self.process_learning_outcome(learning)
        
        # Meta-learning observation
        self.meta_learning.observe_interaction(reasoning, learning)
        
        # Update state
        self.cognitive_state.mode = CognitiveMode.CONSOLIDATION
        self.cycle_count += 1
        
        # Calculate metrics
        synergy_score = self.meta_learning._calculate_synergy(reasoning, learning)
        best_strategy = self.meta_learning.get_best_strategy()
        
        summary = {
            'cycle': self.cycle_count,
            'synergy_score': synergy_score,
            'reasoning_quality': reasoning.confidence,
            'learning_progress': learning.fitness_score,
            'best_strategy': best_strategy,
            'shared_atoms': len(self.shared_layer.atoms),
            'feedback_signals': 2
        }
        
        logger.info(f"Cognitive cycle {self.cycle_count} complete: synergy={synergy_score:.3f}")
        return summary
    
    def get_state(self) -> CognitiveState:
        """Get current cognitive state"""
        return self.cognitive_state
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get bridge statistics"""
        return {
            'total_cycles': self.cycle_count,
            'shared_atoms': len(self.shared_layer.atoms),
            'shared_connections': sum(len(conns) for conns in self.shared_layer.connections.values()),
            'pln_to_moses_signals': len(self.pln_to_moses.feedback_history),
            'moses_to_pln_signals': len(self.moses_to_pln.feedback_history),
            'known_strategies': list(self.meta_learning.current_strategies),
            'best_strategy': self.meta_learning.get_best_strategy()
        }


# Convenience functions for external use

def create_cognitive_fusion_bridge() -> CognitiveFusionBridge:
    """Create and initialize a cognitive fusion bridge"""
    return CognitiveFusionBridge()


def demonstrate_cognitive_fusion():
    """Demonstrate cognitive fusion capabilities"""
    print("=" * 80)
    print("Cross-Modal Cognitive Fusion Demonstration")
    print("=" * 80)
    
    bridge = create_cognitive_fusion_bridge()
    
    # Simulate reasoning outcomes
    reasoning1 = ReasoningOutcome(
        conclusion="If X then Y",
        confidence=0.85,
        inference_steps=7,
        supporting_atoms=["atom_1", "atom_2"],
        reasoning_patterns=["conditional", "implication"]
    )
    
    # Simulate learning outcomes
    learning1 = LearningOutcome(
        learned_program="(if (X) (Y))",
        fitness_score=0.78,
        generations=12,
        discovered_patterns=["conditional", "boolean_logic"]
    )
    
    # Execute cognitive cycle
    print("\nExecuting cognitive cycle...")
    summary = bridge.cognitive_cycle(reasoning1, learning1)
    
    print(f"\nCycle Summary:")
    for key, value in summary.items():
        print(f"  {key}: {value}")
    
    print(f"\nBridge Statistics:")
    stats = bridge.get_statistics()
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    print("\n" + "=" * 80)
    print("Demonstration complete!")
    print("=" * 80)


if __name__ == "__main__":
    logging.basicConfig(
        level=logging.INFO,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    )
    demonstrate_cognitive_fusion()
