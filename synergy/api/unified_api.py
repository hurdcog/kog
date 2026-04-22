"""
Unified Synergy API Implementation

Single entry point for all cognitive synergy operations, coordinating
AAR Core, Hypergraph Identity, and Membrane Manager components.
"""

from dataclasses import dataclass, field
from typing import Any, Dict, List, Optional
import numpy as np

try:
    from ..core.aar_core import AARCore
    from ..identity.hypergraph_identity import HypergraphIdentitySystem
    from ..membranes.membrane_manager import MembraneManager, MessageType
    COMPONENTS_AVAILABLE = True
except ImportError:
    COMPONENTS_AVAILABLE = False
    print("Warning: Synergy components not fully available")


@dataclass
class SynergyConfig:
    """Configuration for cognitive synergy system."""
    
    # AAR Core configuration
    aar_dimension: int = 64
    aar_learning_rate: float = 0.01
    
    # Hypergraph configuration
    enable_hypergraph: bool = True
    hypergraph_auto_refine: bool = True
    
    # Membrane configuration
    enable_membranes: bool = True
    membrane_message_queue_size: int = 1000
    
    # Integration configuration
    auto_sync: bool = True
    sync_interval: int = 10  # Sync every N cycles
    
    # Metrics configuration
    collect_metrics: bool = True
    metrics_history_size: int = 100


class CognitiveSynergy:
    """
    Unified interface for cognitive synergy operations.
    
    Coordinates AAR Core, Hypergraph Identity, and Membrane Manager
    to provide seamless cognitive processing with emergent synergy.
    
    Example:
        >>> cs = CognitiveSynergy()
        >>> cs.perceive(perception_data)
        >>> cs.process()
        >>> action = cs.act()
        >>> metrics = cs.reflect()
    """
    
    def __init__(self, config: Optional[SynergyConfig] = None):
        """
        Initialize cognitive synergy system.
        
        Args:
            config: Configuration object. Uses defaults if None.
        """
        self.config = config or SynergyConfig()
        self.cycle_count = 0
        self.metrics_history: List[Dict[str, Any]] = []
        
        # Initialize components
        self._init_components()
        
        # State tracking
        self.last_perception = None
        self.last_action = None
        self.current_state = "initialized"
    
    def _init_components(self):
        """Initialize all synergy components."""
        if not COMPONENTS_AVAILABLE:
            raise ImportError("Synergy components not available. Install required dependencies.")
        
        # Initialize AAR Core
        self.aar = AARCore(
            dimension=self.config.aar_dimension,
            learning_rate=self.config.aar_learning_rate
        )
        
        # Initialize Hypergraph Identity
        if self.config.enable_hypergraph:
            self.hypergraph = HypergraphIdentitySystem()
        else:
            self.hypergraph = None
        
        # Initialize Membrane Manager
        if self.config.enable_membranes:
            self.membrane_manager = MembraneManager()
        else:
            self.membrane_manager = None
    
    def perceive(self, perception: np.ndarray) -> Dict[str, Any]:
        """
        Process perception through all synergy components.
        
        Args:
            perception: Input perception vector
            
        Returns:
            Dictionary with perception processing results
        """
        self.last_perception = perception
        self.current_state = "perceiving"
        
        results = {
            "timestamp": self.cycle_count,
            "perception_shape": perception.shape,
            "components": {}
        }
        
        # AAR Core perception
        self.aar.perceive(perception)
        results["components"]["aar"] = {
            "processed": True,
            "dimension": self.config.aar_dimension
        }
        
        # Store in memory membrane if available
        if self.membrane_manager:
            self.membrane_manager.send_message(
                source_id="perception",
                target_id="memory",
                message_type=MessageType.DATA,
                payload={"perception": perception.tolist()}
            )
            results["components"]["membrane"] = {
                "stored": True,
                "target": "memory"
            }
        
        return results
    
    def process(self) -> Dict[str, Any]:
        """
        Process current state through reasoning and integration.
        
        Returns:
            Dictionary with processing results
        """
        self.current_state = "processing"
        
        results = {
            "timestamp": self.cycle_count,
            "components": {}
        }
        
        # Process membrane messages if available
        if self.membrane_manager:
            processed = self.membrane_manager.process_all_messages()
            results["components"]["membrane"] = {
                "messages_processed": processed
            }
        
        # Trigger reasoning in reasoning membrane
        if self.membrane_manager:
            self.membrane_manager.send_message(
                source_id="memory",
                target_id="reasoning",
                message_type=MessageType.COMMAND,
                payload={"action": "reason"}
            )
        
        return results
    
    def act(self) -> np.ndarray:
        """
        Generate action based on current cognitive state.
        
        Returns:
            Action vector
        """
        self.current_state = "acting"
        
        # Generate action from AAR Core
        action = self.aar.act()
        self.last_action = action
        
        # Store action in membrane system
        if self.membrane_manager:
            self.membrane_manager.send_message(
                source_id="reasoning",
                target_id="action",
                message_type=MessageType.DATA,
                payload={"action": action.tolist()}
            )
        
        return action
    
    def reflect(self) -> Dict[str, Any]:
        """
        Reflect on cognitive cycle and update self-awareness.
        
        Returns:
            Dictionary with reflection metrics
        """
        self.current_state = "reflecting"
        
        # AAR reflection
        aar_metrics = self.aar.reflect()
        
        # Get AAR self-state
        self_state = self.aar.get_self_state()
        
        # Update hypergraph identity if enabled
        if self.hypergraph and self.config.hypergraph_auto_refine:
            # Create refinement tuple from self-state
            refinement = {
                "subject": "self",
                "predicate": "has_emergence",
                "object": f"score_{self_state['emergence_score']:.4f}",
                "context": f"cycle_{self.cycle_count}",
                "confidence": self_state.get("coherence", 0.5)
            }
            self.hypergraph.add_refinement_tuple(
                refinement["subject"],
                refinement["predicate"],
                refinement["object"],
                refinement["context"],
                refinement["confidence"]
            )
        
        # Compile comprehensive metrics
        metrics = {
            "cycle": self.cycle_count,
            "aar": {
                "emergence_score": self_state["emergence_score"],
                "coherence": self_state.get("coherence", 0.0),
                "self_representation": self_state.get("self_representation", None)
            }
        }
        
        if self.hypergraph:
            identity_summary = self.hypergraph.get_identity_summary()
            metrics["hypergraph"] = {
                "total_nodes": identity_summary.get("total_nodes", 0),
                "total_edges": identity_summary.get("total_edges", 0),
                "identity_strength": identity_summary.get("identity_strength", 0.0)
            }
        
        if self.membrane_manager:
            membrane_metrics = self.membrane_manager.get_metrics()
            metrics["membranes"] = membrane_metrics
        
        # Store metrics if enabled
        if self.config.collect_metrics:
            self.metrics_history.append(metrics)
            if len(self.metrics_history) > self.config.metrics_history_size:
                self.metrics_history.pop(0)
        
        # Increment cycle counter
        self.cycle_count += 1
        self.current_state = "ready"
        
        return metrics
    
    def cognitive_cycle(self, perception: np.ndarray) -> Dict[str, Any]:
        """
        Execute a complete cognitive cycle: perceive -> process -> act -> reflect.
        
        Args:
            perception: Input perception vector
            
        Returns:
            Dictionary with cycle results including action and metrics
        """
        # Perceive
        perception_results = self.perceive(perception)
        
        # Process
        process_results = self.process()
        
        # Act
        action = self.act()
        
        # Reflect
        metrics = self.reflect()
        
        return {
            "cycle": self.cycle_count - 1,
            "perception": perception_results,
            "processing": process_results,
            "action": action.tolist(),
            "metrics": metrics
        }
    
    def get_all_metrics(self) -> Dict[str, Any]:
        """
        Get comprehensive metrics from all components.
        
        Returns:
            Dictionary with all available metrics
        """
        metrics = {
            "cycle_count": self.cycle_count,
            "current_state": self.current_state,
            "config": {
                "aar_dimension": self.config.aar_dimension,
                "hypergraph_enabled": self.config.enable_hypergraph,
                "membranes_enabled": self.config.enable_membranes
            }
        }
        
        # AAR metrics
        self_state = self.aar.get_self_state()
        metrics["aar"] = self_state
        
        # Hypergraph metrics
        if self.hypergraph:
            metrics["hypergraph"] = self.hypergraph.get_identity_summary()
        
        # Membrane metrics
        if self.membrane_manager:
            metrics["membranes"] = self.membrane_manager.get_metrics()
            metrics["membrane_hierarchy"] = self.membrane_manager.get_hierarchy_state()
        
        # Metrics history
        if self.config.collect_metrics:
            metrics["history"] = self.metrics_history[-10:]  # Last 10 cycles
        
        return metrics
    
    def get_synergy_score(self) -> float:
        """
        Calculate overall synergy score.
        
        Returns:
            Synergy score between 0.0 and 1.0
        """
        scores = []
        
        # AAR emergence contribution
        self_state = self.aar.get_self_state()
        scores.append(self_state["emergence_score"])
        
        # Hypergraph identity contribution
        if self.hypergraph:
            identity_summary = self.hypergraph.get_identity_summary()
            scores.append(identity_summary.get("identity_strength", 0.0))
        
        # Membrane efficiency contribution
        if self.membrane_manager:
            membrane_metrics = self.membrane_manager.get_metrics()
            efficiency = min(membrane_metrics.get("resource_utilization", 0.0) / 100, 1.0)
            scores.append(efficiency)
        
        # Average of all scores
        return sum(scores) / len(scores) if scores else 0.0
    
    def reset(self):
        """Reset all components to initial state."""
        self._init_components()
        self.cycle_count = 0
        self.metrics_history.clear()
        self.last_perception = None
        self.last_action = None
        self.current_state = "initialized"
    
    def __repr__(self) -> str:
        """String representation."""
        return (
            f"CognitiveSynergy(cycles={self.cycle_count}, "
            f"state={self.current_state}, "
            f"synergy_score={self.get_synergy_score():.4f})"
        )
