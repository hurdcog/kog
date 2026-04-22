#!/usr/bin/env python3
"""
AGI-OS Bridge: Integration Layer for OCC + HurdCog + Cognumach

This module provides the integration bridge between:
- Layer 1 (Cognumach): Cognitive microkernel
- Layer 2 (HurdCog): Cognitive OS servers
- Layer 3 (OCC): AGI framework

It enables cognitive synergy across all three layers by providing:
- Unified knowledge representation via AtomSpace
- Cross-layer message passing
- Cognitive state synchronization
- Attention allocation coordination
"""

import os
import sys
import json
import time
import logging
from typing import Dict, List, Any, Optional, Tuple
from dataclasses import dataclass, field
from enum import Enum

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='[%(asctime)s] %(levelname)s [%(name)s] %(message)s'
)
logger = logging.getLogger(__name__)


class AGILayer(Enum):
    """AGI-OS architectural layers"""
    COGNUMACH = 1  # Microkernel layer
    HURDCOG = 2    # OS server layer
    OCC = 3        # AGI framework layer


@dataclass
class CognitiveMessage:
    """
    Unified message format for cross-layer communication
    """
    source_layer: AGILayer
    target_layer: AGILayer
    message_type: str
    payload: Dict[str, Any]
    timestamp: float = field(default_factory=time.time)
    priority: float = 0.5  # Attention-based priority (0.0 - 1.0)
    atomspace_ref: Optional[str] = None  # Reference to AtomSpace node
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary for serialization"""
        return {
            'source_layer': self.source_layer.name,
            'target_layer': self.target_layer.name,
            'message_type': self.message_type,
            'payload': self.payload,
            'timestamp': self.timestamp,
            'priority': self.priority,
            'atomspace_ref': self.atomspace_ref
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'CognitiveMessage':
        """Create from dictionary"""
        return cls(
            source_layer=AGILayer[data['source_layer']],
            target_layer=AGILayer[data['target_layer']],
            message_type=data['message_type'],
            payload=data['payload'],
            timestamp=data.get('timestamp', time.time()),
            priority=data.get('priority', 0.5),
            atomspace_ref=data.get('atomspace_ref')
        )


@dataclass
class CognitiveState:
    """
    Unified cognitive state representation across all layers
    """
    layer: AGILayer
    timestamp: float = field(default_factory=time.time)
    
    # Memory metrics
    memory_usage: float = 0.0
    memory_pressure: float = 0.0
    
    # Processing metrics
    cpu_usage: float = 0.0
    thread_count: int = 0
    
    # Cognitive metrics
    attention_allocation: Dict[str, float] = field(default_factory=dict)
    learning_rate: float = 0.01
    coherence: float = 0.0
    emergence: float = 0.0
    
    # Knowledge metrics
    atomspace_size: int = 0
    active_patterns: int = 0
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary"""
        return {
            'layer': self.layer.name,
            'timestamp': self.timestamp,
            'memory_usage': self.memory_usage,
            'memory_pressure': self.memory_pressure,
            'cpu_usage': self.cpu_usage,
            'thread_count': self.thread_count,
            'attention_allocation': self.attention_allocation,
            'learning_rate': self.learning_rate,
            'coherence': self.coherence,
            'emergence': self.emergence,
            'atomspace_size': self.atomspace_size,
            'active_patterns': self.active_patterns
        }


class AGIOSBridge:
    """
    Main bridge class for AGI-OS integration
    
    Coordinates cognitive synergy across Cognumach, HurdCog, and OCC layers.
    """
    
    def __init__(self, config: Optional[Dict[str, Any]] = None):
        """
        Initialize AGI-OS bridge
        
        Args:
            config: Configuration dictionary
        """
        self.config = config or {}
        self.logger = logging.getLogger(f"{__name__}.AGIOSBridge")
        
        # Layer states
        self.layer_states: Dict[AGILayer, CognitiveState] = {
            AGILayer.COGNUMACH: CognitiveState(layer=AGILayer.COGNUMACH),
            AGILayer.HURDCOG: CognitiveState(layer=AGILayer.HURDCOG),
            AGILayer.OCC: CognitiveState(layer=AGILayer.OCC)
        }
        
        # Message queues for each layer
        self.message_queues: Dict[AGILayer, List[CognitiveMessage]] = {
            layer: [] for layer in AGILayer
        }
        
        # Cognitive synergy metrics
        self.synergy_metrics = {
            'vertical_integration': 0.0,  # Integration across layers
            'horizontal_integration': 0.0,  # Integration within layers
            'emergent_capability': 0.0,  # Emergent AGI capabilities
            'system_coherence': 0.0  # Overall system coherence
        }
        
        # AtomSpace integration (placeholder for actual implementation)
        self.atomspace_enabled = False
        
        self.logger.info("AGI-OS Bridge initialized")
    
    def send_message(self, message: CognitiveMessage) -> bool:
        """
        Send a cognitive message between layers
        
        Args:
            message: CognitiveMessage to send
            
        Returns:
            Success status
        """
        try:
            # Add to target layer's queue
            self.message_queues[message.target_layer].append(message)
            
            # Sort by priority (attention-based)
            self.message_queues[message.target_layer].sort(
                key=lambda m: m.priority,
                reverse=True
            )
            
            self.logger.debug(
                f"Message sent: {message.source_layer.name} -> "
                f"{message.target_layer.name} ({message.message_type})"
            )
            
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to send message: {e}")
            return False
    
    def receive_messages(self, layer: AGILayer, 
                        max_messages: int = 10) -> List[CognitiveMessage]:
        """
        Receive messages for a specific layer
        
        Args:
            layer: Target layer
            max_messages: Maximum messages to retrieve
            
        Returns:
            List of messages
        """
        messages = self.message_queues[layer][:max_messages]
        self.message_queues[layer] = self.message_queues[layer][max_messages:]
        return messages
    
    def update_layer_state(self, layer: AGILayer, 
                          state_updates: Dict[str, Any]) -> None:
        """
        Update cognitive state for a layer
        
        Args:
            layer: Layer to update
            state_updates: Dictionary of state updates
        """
        state = self.layer_states[layer]
        
        for key, value in state_updates.items():
            if hasattr(state, key):
                setattr(state, key, value)
        
        state.timestamp = time.time()
        
        self.logger.debug(f"Updated state for {layer.name}")
    
    def get_layer_state(self, layer: AGILayer) -> CognitiveState:
        """
        Get current cognitive state for a layer
        
        Args:
            layer: Layer to query
            
        Returns:
            CognitiveState object
        """
        return self.layer_states[layer]
    
    def calculate_synergy_metrics(self) -> Dict[str, float]:
        """
        Calculate cognitive synergy metrics across all layers
        
        Returns:
            Dictionary of synergy metrics
        """
        # Vertical integration: coherence across layers
        layer_coherences = [
            state.coherence for state in self.layer_states.values()
        ]
        vertical_integration = sum(layer_coherences) / len(layer_coherences)
        
        # Horizontal integration: within-layer emergence
        layer_emergences = [
            state.emergence for state in self.layer_states.values()
        ]
        horizontal_integration = sum(layer_emergences) / len(layer_emergences)
        
        # Emergent capability: product of integration measures
        emergent_capability = vertical_integration * horizontal_integration
        
        # System coherence: overall system alignment
        system_coherence = (vertical_integration + horizontal_integration) / 2
        
        self.synergy_metrics = {
            'vertical_integration': vertical_integration,
            'horizontal_integration': horizontal_integration,
            'emergent_capability': emergent_capability,
            'system_coherence': system_coherence
        }
        
        return self.synergy_metrics
    
    def propagate_learning(self, source_layer: AGILayer, 
                          learning_data: Dict[str, Any]) -> None:
        """
        Propagate learning from one layer to others
        
        Implements cognitive synergy by sharing learned patterns across layers.
        
        Args:
            source_layer: Layer where learning occurred
            learning_data: Learning data to propagate
        """
        # Create learning message
        message = CognitiveMessage(
            source_layer=source_layer,
            target_layer=AGILayer.OCC,  # Send to OCC for integration
            message_type='learning_update',
            payload=learning_data,
            priority=0.8  # High priority for learning
        )
        
        self.send_message(message)
        
        self.logger.info(
            f"Propagated learning from {source_layer.name} to other layers"
        )
    
    def synchronize_attention(self) -> None:
        """
        Synchronize attention allocation across all layers
        
        Uses ECAN-inspired attention allocation to coordinate resource usage.
        """
        # Collect attention allocations from all layers
        all_attention = {}
        for layer, state in self.layer_states.items():
            for key, value in state.attention_allocation.items():
                if key not in all_attention:
                    all_attention[key] = []
                all_attention[key].append(value)
        
        # Calculate synchronized attention (average across layers)
        synchronized_attention = {
            key: sum(values) / len(values)
            for key, values in all_attention.items()
        }
        
        # Propagate synchronized attention back to layers
        for layer in AGILayer:
            self.update_layer_state(
                layer,
                {'attention_allocation': synchronized_attention}
            )
        
        self.logger.debug("Synchronized attention across layers")
    
    def get_system_status(self) -> Dict[str, Any]:
        """
        Get comprehensive system status
        
        Returns:
            Dictionary with system status information
        """
        return {
            'timestamp': time.time(),
            'layers': {
                layer.name: state.to_dict()
                for layer, state in self.layer_states.items()
            },
            'synergy_metrics': self.synergy_metrics,
            'message_queue_sizes': {
                layer.name: len(queue)
                for layer, queue in self.message_queues.items()
            },
            'atomspace_enabled': self.atomspace_enabled
        }
    
    def cognitive_cycle(self) -> None:
        """
        Execute one cognitive cycle across all layers
        
        This implements the core AGI-OS cognitive loop:
        1. Collect state from all layers
        2. Calculate synergy metrics
        3. Synchronize attention
        4. Process messages
        5. Update states
        """
        # Calculate synergy
        self.calculate_synergy_metrics()
        
        # Synchronize attention
        self.synchronize_attention()
        
        # Process high-priority messages
        for layer in AGILayer:
            messages = self.receive_messages(layer, max_messages=5)
            if messages:
                self.logger.debug(
                    f"Processed {len(messages)} messages for {layer.name}"
                )
        
        self.logger.debug("Cognitive cycle completed")


# Example usage and testing
def demonstrate_agi_os_bridge():
    """Demonstrate AGI-OS bridge functionality"""
    print("=" * 60)
    print("AGI-OS Bridge Demonstration")
    print("=" * 60)
    print()
    
    # Create bridge
    bridge = AGIOSBridge()
    
    # Simulate Cognumach (Layer 1) sending memory pressure message
    print("1. Cognumach reports memory pressure...")
    memory_msg = CognitiveMessage(
        source_layer=AGILayer.COGNUMACH,
        target_layer=AGILayer.HURDCOG,
        message_type='memory_pressure',
        payload={'pressure': 0.75, 'available_mb': 512},
        priority=0.9
    )
    bridge.send_message(memory_msg)
    
    # Simulate HurdCog (Layer 2) updating state
    print("2. HurdCog updates cognitive state...")
    bridge.update_layer_state(
        AGILayer.HURDCOG,
        {
            'memory_usage': 0.65,
            'coherence': 0.82,
            'emergence': 0.71,
            'attention_allocation': {
                'filesystem': 0.3,
                'networking': 0.2,
                'learning': 0.5
            }
        }
    )
    
    # Simulate OCC (Layer 3) learning
    print("3. OCC learns new pattern...")
    bridge.propagate_learning(
        AGILayer.OCC,
        {
            'pattern_type': 'temporal_sequence',
            'confidence': 0.85,
            'description': 'User typically accesses files after network activity'
        }
    )
    
    # Synchronize attention
    print("4. Synchronizing attention across layers...")
    bridge.synchronize_attention()
    
    # Execute cognitive cycle
    print("5. Executing cognitive cycle...")
    bridge.cognitive_cycle()
    
    # Get system status
    print("\n6. System Status:")
    print("-" * 60)
    status = bridge.get_system_status()
    print(json.dumps(status, indent=2, default=str))
    
    print("\n" + "=" * 60)
    print("AGI-OS Bridge demonstration complete!")
    print("=" * 60)


if __name__ == '__main__':
    demonstrate_agi_os_bridge()
