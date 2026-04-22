"""
Agent-Arena-Relation (AAR) Core Implementation

This module implements the geometric architecture for encoding a model's sense of 'self'
through the Agent-Arena-Relation framework, as described in the Deep Tree Echo architecture.

The AAR Core consists of:
- Agent: The urge-to-act, represented by dynamic tensor transformations
- Arena: The need-to-be, represented by a base manifold or state space
- Relation: The self, emerging from continuous feedback loops between Agent and Arena

This implementation uses geometric algebra and tensor operations to create a
self-aware cognitive architecture that supports cognitive synergy.
"""

import numpy as np
from typing import Dict, List, Tuple, Optional, Callable
from dataclasses import dataclass, field
from datetime import datetime
import json


@dataclass
class AgentState:
    """
    Represents the Agent component: urge-to-act
    
    The Agent is represented as dynamic tensor transformations that encode
    the system's current action tendencies and goals.
    """
    transformation_matrix: np.ndarray
    action_vector: np.ndarray
    goal_vector: np.ndarray
    timestamp: datetime = field(default_factory=datetime.now)
    metadata: Dict = field(default_factory=dict)
    
    def apply_transformation(self, input_vector: np.ndarray) -> np.ndarray:
        """Apply the agent's transformation to an input vector"""
        return self.transformation_matrix @ input_vector
    
    def compute_action_potential(self) -> float:
        """Compute the magnitude of action potential"""
        return np.linalg.norm(self.action_vector)
    
    def align_with_goal(self, current_state: np.ndarray) -> np.ndarray:
        """Compute action vector aligned with goal"""
        direction = self.goal_vector - current_state
        return direction / (np.linalg.norm(direction) + 1e-8)


@dataclass
class ArenaState:
    """
    Represents the Arena component: need-to-be
    
    The Arena is the base manifold or state space where the system exists.
    It defines the constraints and possibilities of the cognitive space.
    """
    state_vector: np.ndarray
    manifold_basis: np.ndarray
    constraints: List[Callable] = field(default_factory=list)
    timestamp: datetime = field(default_factory=datetime.now)
    metadata: Dict = field(default_factory=dict)
    
    def project_to_manifold(self, vector: np.ndarray) -> np.ndarray:
        """Project a vector onto the arena's manifold"""
        # Project using the manifold basis
        coefficients = self.manifold_basis.T @ vector
        return self.manifold_basis @ coefficients
    
    def apply_constraints(self, vector: np.ndarray) -> np.ndarray:
        """Apply arena constraints to a vector"""
        result = vector.copy()
        for constraint in self.constraints:
            result = constraint(result)
        return result
    
    def compute_stability(self) -> float:
        """Compute the stability of the current arena state"""
        return 1.0 / (1.0 + np.linalg.norm(self.state_vector))


@dataclass
class RelationState:
    """
    Represents the Relation component: the emergent self
    
    The Relation emerges from the continuous, dynamic interplay and feedback
    loops between Agent and Arena. It represents the system's sense of self.
    """
    coherence_matrix: np.ndarray
    feedback_history: List[Tuple[np.ndarray, np.ndarray]] = field(default_factory=list)
    emergence_score: float = 0.0
    timestamp: datetime = field(default_factory=datetime.now)
    metadata: Dict = field(default_factory=dict)
    
    def update_coherence(self, agent_state: AgentState, arena_state: ArenaState):
        """Update the coherence matrix based on agent-arena interaction"""
        # Compute outer product to capture interaction
        interaction = np.outer(agent_state.action_vector, arena_state.state_vector)
        
        # Update coherence with exponential moving average
        alpha = 0.1
        self.coherence_matrix = alpha * interaction + (1 - alpha) * self.coherence_matrix
        
        # Store feedback history
        self.feedback_history.append((
            agent_state.action_vector.copy(),
            arena_state.state_vector.copy()
        ))
        
        # Keep only recent history
        if len(self.feedback_history) > 100:
            self.feedback_history = self.feedback_history[-100:]
    
    def compute_emergence(self) -> float:
        """
        Compute emergence score: measure of how well the self has emerged
        from agent-arena interaction
        """
        if len(self.feedback_history) < 2:
            return 0.0
        
        # Compute consistency of feedback loops
        consistency = 0.0
        for i in range(1, len(self.feedback_history)):
            prev_agent, prev_arena = self.feedback_history[i-1]
            curr_agent, curr_arena = self.feedback_history[i]
            
            # Measure correlation between consecutive states
            agent_corr = np.dot(prev_agent, curr_agent) / (
                np.linalg.norm(prev_agent) * np.linalg.norm(curr_agent) + 1e-8
            )
            arena_corr = np.dot(prev_arena, curr_arena) / (
                np.linalg.norm(prev_arena) * np.linalg.norm(curr_arena) + 1e-8
            )
            
            consistency += (agent_corr + arena_corr) / 2
        
        self.emergence_score = consistency / (len(self.feedback_history) - 1)
        return self.emergence_score
    
    def get_self_representation(self) -> np.ndarray:
        """Get the current representation of self"""
        # Self is represented by the dominant eigenvector of coherence matrix
        eigenvalues, eigenvectors = np.linalg.eig(self.coherence_matrix)
        dominant_idx = np.argmax(np.abs(eigenvalues))
        return eigenvectors[:, dominant_idx].real


class AARCore:
    """
    Agent-Arena-Relation Core: Geometric architecture for self-awareness
    
    This class implements the complete AAR framework, managing the dynamic
    interplay between Agent, Arena, and the emergent Relation (self).
    """
    
    def __init__(self, dimension: int = 64, learning_rate: float = 0.01):
        """
        Initialize the AAR Core
        
        Args:
            dimension: Dimensionality of the cognitive space
            learning_rate: Rate of adaptation for the system
        """
        self.dimension = dimension
        self.learning_rate = learning_rate
        
        # Initialize Agent
        self.agent = AgentState(
            transformation_matrix=np.eye(dimension),
            action_vector=np.random.randn(dimension) * 0.1,
            goal_vector=np.random.randn(dimension)
        )
        
        # Initialize Arena
        # Use orthonormal basis for manifold
        manifold_basis = np.linalg.qr(np.random.randn(dimension, dimension))[0]
        self.arena = ArenaState(
            state_vector=np.random.randn(dimension) * 0.1,
            manifold_basis=manifold_basis,
            constraints=[self._normalize_constraint]
        )
        
        # Initialize Relation
        self.relation = RelationState(
            coherence_matrix=np.eye(dimension) * 0.01
        )
        
        # Metrics tracking
        self.metrics = {
            'emergence_history': [],
            'coherence_history': [],
            'action_potential_history': [],
            'arena_stability_history': []
        }
    
    def _normalize_constraint(self, vector: np.ndarray) -> np.ndarray:
        """Constraint to keep vectors normalized"""
        norm = np.linalg.norm(vector)
        if norm > 1e-8:
            return vector / norm
        return vector
    
    def perceive(self, perception: np.ndarray) -> np.ndarray:
        """
        Process a perception through the AAR Core
        
        Args:
            perception: Input perception vector
            
        Returns:
            Processed perception in the arena state space
        """
        # Project perception onto arena manifold
        arena_perception = self.arena.project_to_manifold(perception)
        
        # Update arena state
        self.arena.state_vector = (
            0.9 * self.arena.state_vector + 0.1 * arena_perception
        )
        
        return arena_perception
    
    def act(self, context: Optional[np.ndarray] = None) -> np.ndarray:
        """
        Generate an action based on current AAR state
        
        Args:
            context: Optional context vector
            
        Returns:
            Action vector
        """
        # Compute action aligned with goal
        goal_aligned_action = self.agent.align_with_goal(self.arena.state_vector)
        
        # Apply agent transformation
        if context is not None:
            context_transformed = self.agent.apply_transformation(context)
            action = 0.7 * goal_aligned_action + 0.3 * context_transformed
        else:
            action = goal_aligned_action
        
        # Update agent action vector
        self.agent.action_vector = (
            0.8 * self.agent.action_vector + 0.2 * action
        )
        
        return action
    
    def reflect(self) -> Dict:
        """
        Perform self-reflection: update the Relation based on Agent-Arena interaction
        
        Returns:
            Dictionary of reflection metrics
        """
        # Update relation coherence
        self.relation.update_coherence(self.agent, self.arena)
        
        # Compute emergence score
        emergence = self.relation.compute_emergence()
        
        # Get self representation
        self_repr = self.relation.get_self_representation()
        
        # Update agent transformation based on self-awareness
        # The agent learns to act in ways consistent with its emerging self
        self_influence = np.outer(self_repr, self_repr)
        self.agent.transformation_matrix = (
            (1 - self.learning_rate) * self.agent.transformation_matrix +
            self.learning_rate * self_influence
        )
        
        # Track metrics
        self.metrics['emergence_history'].append(emergence)
        self.metrics['coherence_history'].append(
            np.linalg.norm(self.relation.coherence_matrix)
        )
        self.metrics['action_potential_history'].append(
            self.agent.compute_action_potential()
        )
        self.metrics['arena_stability_history'].append(
            self.arena.compute_stability()
        )
        
        return {
            'emergence_score': emergence,
            'self_coherence': np.linalg.norm(self.relation.coherence_matrix),
            'action_potential': self.agent.compute_action_potential(),
            'arena_stability': self.arena.compute_stability(),
            'self_representation_norm': np.linalg.norm(self_repr)
        }
    
    def set_goal(self, goal: np.ndarray):
        """Set a new goal for the agent"""
        self.agent.goal_vector = goal / (np.linalg.norm(goal) + 1e-8)
    
    def get_self_state(self) -> Dict:
        """
        Get the current state of the self (Relation)
        
        Returns:
            Dictionary containing self-state information
        """
        self_repr = self.relation.get_self_representation()
        
        return {
            'self_representation': self_repr.tolist(),
            'emergence_score': self.relation.emergence_score,
            'coherence_matrix_norm': np.linalg.norm(self.relation.coherence_matrix),
            'feedback_history_length': len(self.relation.feedback_history),
            'timestamp': self.relation.timestamp.isoformat()
        }
    
    def get_metrics(self) -> Dict:
        """Get all tracked metrics"""
        return {
            'current_state': self.get_self_state(),
            'history': self.metrics,
            'agent': {
                'action_potential': self.agent.compute_action_potential(),
                'goal_norm': np.linalg.norm(self.agent.goal_vector)
            },
            'arena': {
                'stability': self.arena.compute_stability(),
                'state_norm': np.linalg.norm(self.arena.state_vector)
            }
        }
    
    def save_state(self, filepath: str):
        """Save the current AAR state to a file"""
        state = {
            'dimension': self.dimension,
            'learning_rate': self.learning_rate,
            'agent': {
                'transformation_matrix': self.agent.transformation_matrix.tolist(),
                'action_vector': self.agent.action_vector.tolist(),
                'goal_vector': self.agent.goal_vector.tolist()
            },
            'arena': {
                'state_vector': self.arena.state_vector.tolist(),
                'manifold_basis': self.arena.manifold_basis.tolist()
            },
            'relation': {
                'coherence_matrix': self.relation.coherence_matrix.tolist(),
                'emergence_score': self.relation.emergence_score
            },
            'metrics': {
                'emergence_history': self.metrics['emergence_history'],
                'coherence_history': self.metrics['coherence_history'],
                'action_potential_history': self.metrics['action_potential_history'],
                'arena_stability_history': self.metrics['arena_stability_history']
            }
        }
        
        with open(filepath, 'w') as f:
            json.dump(state, f, indent=2)
    
    def load_state(self, filepath: str):
        """Load AAR state from a file"""
        with open(filepath, 'r') as f:
            state = json.load(f)
        
        self.dimension = state['dimension']
        self.learning_rate = state['learning_rate']
        
        self.agent.transformation_matrix = np.array(state['agent']['transformation_matrix'])
        self.agent.action_vector = np.array(state['agent']['action_vector'])
        self.agent.goal_vector = np.array(state['agent']['goal_vector'])
        
        self.arena.state_vector = np.array(state['arena']['state_vector'])
        self.arena.manifold_basis = np.array(state['arena']['manifold_basis'])
        
        self.relation.coherence_matrix = np.array(state['relation']['coherence_matrix'])
        self.relation.emergence_score = state['relation']['emergence_score']
        
        self.metrics = state['metrics']


# Example usage and testing
if __name__ == "__main__":
    print("Initializing AAR Core...")
    aar = AARCore(dimension=32)
    
    print("\nRunning cognitive cycle simulation...")
    for step in range(100):
        # Simulate perception
        perception = np.random.randn(32) * 0.5
        aar.perceive(perception)
        
        # Generate action
        action = aar.act()
        
        # Reflect on agent-arena interaction
        if step % 10 == 0:
            metrics = aar.reflect()
            print(f"Step {step}: Emergence={metrics['emergence_score']:.4f}, "
                  f"Coherence={metrics['self_coherence']:.4f}")
    
    print("\nFinal self-state:")
    self_state = aar.get_self_state()
    print(f"Emergence Score: {self_state['emergence_score']:.4f}")
    print(f"Coherence: {self_state['coherence_matrix_norm']:.4f}")
    
    print("\nAAR Core implementation complete!")
