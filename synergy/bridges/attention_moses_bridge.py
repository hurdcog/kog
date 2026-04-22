"""
Attention-Guided Evolutionary Learning Bridge

This module implements the integration between the Attention Allocation (ECAN)
system and MOSES (Meta-Optimizing Semantic Evolutionary Search) to enable
focused, importance-driven learning.

Emergent Capability: focused_learning
- System prioritizes learning based on what it deems important
- Attention signals guide evolutionary search
- Learning outcomes feed back to attention allocation

Architecture:
1. Extract high-STI (Short-Term Importance) atoms from attention system
2. Convert attention signals to MOSES fitness bonuses
3. Prioritize learning tasks based on importance
4. Create feedback loop from learning outcomes to attention

Based on: Autogenesis Roadmap Feature #2
Implementation Status: Initial (Phase 1)
"""

import logging
import numpy as np
from typing import Dict, List, Tuple, Optional, Set
from dataclasses import dataclass
from enum import Enum
from collections import defaultdict

logger = logging.getLogger(__name__)


class AttentionSignalType(Enum):
    """Types of attention signals that can guide learning"""
    STI_THRESHOLD = "sti_threshold"      # Atoms above STI threshold
    LTI_THRESHOLD = "lti_threshold"      # Atoms above LTI threshold
    ATTENTIONAL_FOCUS = "attentional_focus"  # Atoms in attentional focus
    DYNAMIC_IMPORTANCE = "dynamic_importance"  # Recently changed importance


@dataclass
class AttentionSignal:
    """Represents an attention signal for a specific atom or concept"""
    atom_id: str
    atom_type: str
    sti: float  # Short-Term Importance
    lti: float  # Long-Term Importance
    signal_type: AttentionSignalType
    timestamp: float
    context: Optional[Dict] = None


@dataclass
class LearningTask:
    """Represents a learning task guided by attention"""
    task_id: str
    description: str
    target_atoms: List[str]
    base_fitness: float
    attention_bonus: float
    priority: float
    status: str = "pending"
    results: Optional[Dict] = None


class AttentionMOSESBridge:
    """
    Bridge between Attention Allocation and MOSES for focused learning.
    
    This class implements the core integration logic:
    - Extracts attention signals from the attention system
    - Converts signals to fitness bonuses for MOSES
    - Prioritizes learning tasks based on importance
    - Feeds learning outcomes back to attention
    """
    
    def __init__(self,
                 sti_threshold: float = 100.0,
                 lti_threshold: float = 50.0,
                 fitness_scaling: float = 1.0,
                 max_tasks: int = 10,
                 max_importance: float = 1000.0,
                 feedback_lti_delta: float = 10.0,
                 feedback_sti_boost: float = 50.0):
        """
        Initialize the Attention-MOSES bridge.
        
        Args:
            sti_threshold: Minimum STI for atom to be considered important
            lti_threshold: Minimum LTI for atom to be considered important
            fitness_scaling: Scaling factor for attention to fitness conversion
            max_tasks: Maximum number of concurrent learning tasks
            max_importance: Maximum expected importance value for normalization
            feedback_lti_delta: LTI increase for successfully learned atoms
            feedback_sti_boost: STI boost for newly discovered patterns
        """
        self.sti_threshold = sti_threshold
        self.lti_threshold = lti_threshold
        self.fitness_scaling = fitness_scaling
        self.max_tasks = max_tasks
        self.max_importance = max_importance
        self.feedback_lti_delta = feedback_lti_delta
        self.feedback_sti_boost = feedback_sti_boost
        
        # State tracking
        self.attention_signals: List[AttentionSignal] = []
        self.learning_tasks: Dict[str, LearningTask] = {}
        self.task_priority_queue: List[str] = []
        self.feedback_history: List[Dict] = []
        
        # Statistics
        self.stats = {
            'signals_processed': 0,
            'tasks_created': 0,
            'tasks_completed': 0,
            'total_attention_bonus': 0.0,
            'feedback_cycles': 0
        }
        
        logger.info(f"AttentionMOSESBridge initialized with STI threshold: {sti_threshold}")
    
    def extract_attention_signals(self,
                                   attention_atoms: List[Dict],
                                   signal_types: Optional[Set[AttentionSignalType]] = None) -> List[AttentionSignal]:
        """
        Extract attention signals from atoms with attention values.
        
        Args:
            attention_atoms: List of atoms with attention values
                Each atom should have: id, type, sti, lti, vlti
            signal_types: Which types of signals to extract (default: all)
        
        Returns:
            List of AttentionSignal objects
        """
        if signal_types is None:
            signal_types = set(AttentionSignalType)
        
        signals = []
        import time
        timestamp = time.time()
        
        for atom in attention_atoms:
            atom_id = atom.get('id', 'unknown')
            atom_type = atom.get('type', 'unknown')
            sti = atom.get('sti', 0.0)
            lti = atom.get('lti', 0.0)
            
            # Check STI threshold
            if AttentionSignalType.STI_THRESHOLD in signal_types and sti >= self.sti_threshold:
                signals.append(AttentionSignal(
                    atom_id=atom_id,
                    atom_type=atom_type,
                    sti=sti,
                    lti=lti,
                    signal_type=AttentionSignalType.STI_THRESHOLD,
                    timestamp=timestamp,
                    context={'threshold': self.sti_threshold}
                ))
            
            # Check LTI threshold
            if AttentionSignalType.LTI_THRESHOLD in signal_types and lti >= self.lti_threshold:
                signals.append(AttentionSignal(
                    atom_id=atom_id,
                    atom_type=atom_type,
                    sti=sti,
                    lti=lti,
                    signal_type=AttentionSignalType.LTI_THRESHOLD,
                    timestamp=timestamp,
                    context={'threshold': self.lti_threshold}
                ))
        
        self.attention_signals.extend(signals)
        self.stats['signals_processed'] += len(signals)
        
        logger.debug(f"Extracted {len(signals)} attention signals")
        return signals
    
    def convert_attention_to_fitness_bonus(self,
                                           signal: AttentionSignal,
                                           base_fitness: float = 0.0) -> float:
        """
        Convert attention signal to fitness bonus for MOSES.
        
        The fitness bonus encourages MOSES to focus on learning patterns
        related to important atoms.
        
        Args:
            signal: AttentionSignal to convert
            base_fitness: Base fitness value before bonus
        
        Returns:
            Fitness bonus value (scaled STI/LTI combination)
        """
        # Combine STI and LTI with different weights
        # STI gets higher weight for immediate importance
        # LTI contributes to longer-term focus
        sti_weight = 0.7
        lti_weight = 0.3
        
        # Normalize to 0-1 range using configurable max importance
        normalized_sti = min(signal.sti / self.max_importance, 1.0)
        normalized_lti = min(signal.lti / self.max_importance, 1.0)
        
        # Calculate weighted importance
        importance = (sti_weight * normalized_sti + lti_weight * normalized_lti)
        
        # Apply scaling and return bonus
        bonus = importance * self.fitness_scaling
        
        self.stats['total_attention_bonus'] += bonus
        
        return bonus
    
    def create_learning_task(self,
                            task_id: str,
                            description: str,
                            attention_signals: List[AttentionSignal],
                            base_fitness: float = 0.0) -> LearningTask:
        """
        Create a learning task guided by attention signals.
        
        Args:
            task_id: Unique identifier for task
            description: Human-readable description
            attention_signals: Relevant attention signals
            base_fitness: Base fitness for the task
        
        Returns:
            LearningTask object with computed priority
        """
        # Extract target atoms from signals
        target_atoms = [signal.atom_id for signal in attention_signals]
        
        # Calculate attention bonus (average of all signal bonuses)
        if attention_signals:
            attention_bonus = np.mean([
                self.convert_attention_to_fitness_bonus(signal, base_fitness)
                for signal in attention_signals
            ])
        else:
            attention_bonus = 0.0
        
        # Priority is base fitness + attention bonus
        priority = base_fitness + attention_bonus
        
        task = LearningTask(
            task_id=task_id,
            description=description,
            target_atoms=target_atoms,
            base_fitness=base_fitness,
            attention_bonus=attention_bonus,
            priority=priority,
            status="pending"
        )
        
        self.learning_tasks[task_id] = task
        self.task_priority_queue.append(task_id)
        # Sort by priority (descending)
        self.task_priority_queue.sort(
            key=lambda tid: self.learning_tasks[tid].priority,
            reverse=True
        )
        
        self.stats['tasks_created'] += 1
        
        logger.info(f"Created learning task '{task_id}' with priority {priority:.4f}")
        return task
    
    def get_prioritized_tasks(self, limit: Optional[int] = None) -> List[LearningTask]:
        """
        Get learning tasks in priority order.
        
        Args:
            limit: Maximum number of tasks to return (default: max_tasks)
        
        Returns:
            List of LearningTask objects in priority order
        """
        if limit is None:
            limit = self.max_tasks
        
        # Get top tasks from priority queue
        top_task_ids = self.task_priority_queue[:limit]
        return [self.learning_tasks[tid] for tid in top_task_ids]
    
    def update_task_status(self,
                          task_id: str,
                          status: str,
                          results: Optional[Dict] = None):
        """
        Update the status of a learning task.
        
        Args:
            task_id: Task identifier
            status: New status ('pending', 'running', 'completed', 'failed')
            results: Optional results from MOSES learning
        """
        if task_id not in self.learning_tasks:
            logger.warning(f"Task {task_id} not found")
            return
        
        task = self.learning_tasks[task_id]
        task.status = status
        
        if results is not None:
            task.results = results
        
        if status == 'completed':
            self.stats['tasks_completed'] += 1
            # Remove from priority queue
            if task_id in self.task_priority_queue:
                self.task_priority_queue.remove(task_id)
        
        logger.debug(f"Task {task_id} status updated to {status}")
    
    def feedback_to_attention(self,
                             task_id: str,
                             learning_outcomes: Dict) -> Dict:
        """
        Feed learning outcomes back to attention system.
        
        This creates a feedback loop where successful learning can
        influence future attention allocation.
        
        Args:
            task_id: Task identifier
            learning_outcomes: Results from MOSES learning
                Should include: success, patterns_learned, accuracy, etc.
        
        Returns:
            Attention update recommendations
        """
        if task_id not in self.learning_tasks:
            logger.warning(f"Task {task_id} not found for feedback")
            return {}
        
        task = self.learning_tasks[task_id]
        
        # Analyze learning outcomes
        success = learning_outcomes.get('success', False)
        patterns_learned = learning_outcomes.get('patterns_learned', [])
        accuracy = learning_outcomes.get('accuracy', 0.0)
        
        # Create attention recommendations
        recommendations = {
            'task_id': task_id,
            'target_atoms': task.target_atoms,
            'updates': []
        }
        
        # If learning was successful, recommend increasing LTI
        # (long-term importance) of related atoms
        if success and accuracy > 0.7:
            for atom_id in task.target_atoms:
                recommendations['updates'].append({
                    'atom_id': atom_id,
                    'lti_delta': self.feedback_lti_delta,
                    'reason': f'successful_learning_task_{task_id}'
                })
        
        # If learning discovered new patterns, those patterns
        # should get initial STI boost
        for pattern in patterns_learned:
            recommendations['updates'].append({
                'atom_id': pattern.get('id', 'unknown'),
                'sti_delta': self.feedback_sti_boost,
                'reason': f'new_pattern_from_{task_id}'
            })
        
        # Store feedback history
        self.feedback_history.append({
            'task_id': task_id,
            'outcomes': learning_outcomes,
            'recommendations': recommendations
        })
        
        self.stats['feedback_cycles'] += 1
        
        logger.info(f"Generated {len(recommendations['updates'])} attention updates from task {task_id}")
        return recommendations
    
    def get_stats(self) -> Dict:
        """Get bridge statistics"""
        return {
            **self.stats,
            'active_tasks': len([t for t in self.learning_tasks.values() if t.status != 'completed']),
            'pending_tasks': len(self.task_priority_queue),
            'total_tasks': len(self.learning_tasks),
            'total_signals': len(self.attention_signals)
        }
    
    def reset(self):
        """Reset the bridge state (for testing/debugging)"""
        self.attention_signals.clear()
        self.learning_tasks.clear()
        self.task_priority_queue.clear()
        self.feedback_history.clear()
        
        # Reset stats but keep cumulative counters
        logger.info("AttentionMOSESBridge reset")


# Utility functions for integration

def create_demo_attention_atoms(count: int = 10) -> List[Dict]:
    """
    Create demo attention atoms for testing.
    
    Args:
        count: Number of atoms to create
    
    Returns:
        List of atom dictionaries with attention values
    """
    atoms = []
    for i in range(count):
        atoms.append({
            'id': f'atom_{i}',
            'type': 'ConceptNode' if i % 2 == 0 else 'PredicateNode',
            'sti': np.random.uniform(0, 200),
            'lti': np.random.uniform(0, 100),
            'vlti': 0
        })
    return atoms


def demo_attention_guided_learning():
    """
    Demonstration of attention-guided evolutionary learning.
    
    This shows the complete cycle:
    1. Extract attention signals
    2. Create learning tasks
    3. Prioritize based on importance
    4. Simulate learning
    5. Feed back to attention
    """
    print("=" * 80)
    print("ATTENTION-GUIDED EVOLUTIONARY LEARNING DEMO")
    print("=" * 80)
    
    # Initialize bridge
    bridge = AttentionMOSESBridge(
        sti_threshold=100.0,
        fitness_scaling=2.0
    )
    
    # Create demo atoms with attention values
    atoms = create_demo_attention_atoms(20)
    print(f"\n1. Created {len(atoms)} atoms with attention values")
    
    # Extract attention signals
    signals = bridge.extract_attention_signals(atoms)
    print(f"2. Extracted {len(signals)} attention signals")
    
    # Create learning tasks based on signals
    for i, signal in enumerate(signals[:5]):  # Top 5 signals
        task = bridge.create_learning_task(
            task_id=f'task_{i}',
            description=f'Learn patterns related to {signal.atom_id}',
            attention_signals=[signal],
            base_fitness=0.5
        )
        print(f"   Created task: {task.task_id} (priority: {task.priority:.4f})")
    
    # Get prioritized tasks
    print(f"\n3. Prioritized learning tasks:")
    for task in bridge.get_prioritized_tasks(limit=3):
        print(f"   - {task.task_id}: {task.description}")
        print(f"     Priority: {task.priority:.4f} (base: {task.base_fitness:.4f}, "
              f"attention bonus: {task.attention_bonus:.4f})")
    
    # Simulate learning and feedback
    print(f"\n4. Simulating learning and feedback:")
    for task in bridge.get_prioritized_tasks(limit=2):
        bridge.update_task_status(task.task_id, 'running')
        
        # Simulate learning outcomes
        outcomes = {
            'success': True,
            'accuracy': 0.85,
            'patterns_learned': [
                {'id': f'pattern_{task.task_id}_1', 'confidence': 0.9},
                {'id': f'pattern_{task.task_id}_2', 'confidence': 0.7}
            ]
        }
        
        bridge.update_task_status(task.task_id, 'completed', outcomes)
        
        # Feed back to attention
        recommendations = bridge.feedback_to_attention(task.task_id, outcomes)
        print(f"   Task {task.task_id} completed:")
        print(f"   - Generated {len(recommendations['updates'])} attention updates")
    
    # Show statistics
    print(f"\n5. Bridge Statistics:")
    stats = bridge.get_stats()
    for key, value in stats.items():
        print(f"   - {key}: {value}")
    
    print("\n" + "=" * 80)
    print("Demo completed! Emergent capability: focused_learning ✓")
    print("=" * 80)


if __name__ == '__main__':
    # Configure logging
    logging.basicConfig(
        level=logging.INFO,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    )
    
    # Run demo
    demo_attention_guided_learning()
