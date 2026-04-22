#!/usr/bin/env python3
"""
Cognitive Synergy Orchestrator

This module provides the core coordination system for managing inter-component
communication and synergy optimization in the OpenCog Collection.

Implements the "Relation" component of the Agent-Arena-Relation (AAR) framework,
managing dynamic interactions between cognitive components.
"""

import json
import time
import logging
from typing import Dict, List, Any, Optional, Callable
from dataclasses import dataclass, field, asdict
from enum import Enum
from datetime import datetime
import threading
from queue import Queue, Empty


# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


class ComponentState(Enum):
    """Enumeration of possible component states"""
    UNINITIALIZED = "uninitialized"
    INITIALIZING = "initializing"
    READY = "ready"
    ACTIVE = "active"
    ERROR = "error"
    SHUTDOWN = "shutdown"


class EventType(Enum):
    """Types of events in the cognitive system"""
    COMPONENT_REGISTERED = "component_registered"
    COMPONENT_STATE_CHANGED = "component_state_changed"
    DATA_TRANSFERRED = "data_transferred"
    SYNERGY_DETECTED = "synergy_detected"
    PERFORMANCE_METRIC = "performance_metric"
    ERROR_OCCURRED = "error_occurred"


@dataclass
class CognitiveComponent:
    """Represents a cognitive component in the system"""
    name: str
    component_type: str  # e.g., "atomspace", "hyperon", "python_ml"
    state: ComponentState = ComponentState.UNINITIALIZED
    capabilities: List[str] = field(default_factory=list)
    dependencies: List[str] = field(default_factory=list)
    metadata: Dict[str, Any] = field(default_factory=dict)
    last_activity: Optional[float] = None
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert component to dictionary"""
        data = asdict(self)
        data['state'] = self.state.value
        return data


@dataclass
class SynergyEvent:
    """Represents an event in the cognitive system"""
    event_type: EventType
    timestamp: float
    source_component: str
    target_component: Optional[str] = None
    data: Dict[str, Any] = field(default_factory=dict)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert event to dictionary"""
        return {
            'event_type': self.event_type.value,
            'timestamp': self.timestamp,
            'source_component': self.source_component,
            'target_component': self.target_component,
            'data': self.data
        }


class SynergyMetrics:
    """Tracks and calculates cognitive synergy metrics"""
    
    def __init__(self):
        self.events: List[SynergyEvent] = []
        self.component_interactions: Dict[tuple, int] = {}
        self.synergy_scores: List[float] = []
        
    def record_event(self, event: SynergyEvent):
        """Record a synergy event"""
        self.events.append(event)
        
        # Track interactions between components
        if event.target_component:
            key = (event.source_component, event.target_component)
            self.component_interactions[key] = self.component_interactions.get(key, 0) + 1
    
    def calculate_synergy_score(self) -> float:
        """
        Calculate overall synergy score based on component interactions.
        
        Synergy score is based on:
        - Number of unique component interactions
        - Frequency of interactions
        - Diversity of interaction types
        """
        if not self.component_interactions:
            return 0.0
        
        # Calculate diversity (number of unique interactions)
        diversity = len(self.component_interactions)
        
        # Calculate total interactions
        total_interactions = sum(self.component_interactions.values())
        
        # Calculate interaction density (interactions per component pair)
        density = total_interactions / max(diversity, 1)
        
        # Synergy score combines diversity and density
        # Higher diversity and moderate density indicate good synergy
        synergy_score = (diversity * 0.6) + (min(density, 10) * 0.4)
        
        self.synergy_scores.append(synergy_score)
        return synergy_score
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get comprehensive statistics about cognitive synergy"""
        return {
            'total_events': len(self.events),
            'unique_interactions': len(self.component_interactions),
            'total_interactions': sum(self.component_interactions.values()),
            'current_synergy_score': self.calculate_synergy_score(),
            'average_synergy_score': sum(self.synergy_scores) / len(self.synergy_scores) if self.synergy_scores else 0.0,
            'most_active_pairs': sorted(
                self.component_interactions.items(),
                key=lambda x: x[1],
                reverse=True
            )[:5]
        }


class CognitiveSynergyOrchestrator:
    """
    Central coordination system for cognitive synergy.
    
    Manages component registration, inter-component communication,
    and synergy optimization across the OpenCog Collection.
    """
    
    def __init__(self):
        self.components: Dict[str, CognitiveComponent] = {}
        self.event_queue: Queue = Queue()
        self.event_handlers: Dict[EventType, List[Callable]] = {}
        self.metrics = SynergyMetrics()
        self.running = False
        self.event_thread: Optional[threading.Thread] = None
        
        logger.info("Cognitive Synergy Orchestrator initialized")
    
    def register_component(
        self,
        name: str,
        component_type: str,
        capabilities: List[str] = None,
        dependencies: List[str] = None,
        metadata: Dict[str, Any] = None
    ) -> CognitiveComponent:
        """
        Register a cognitive component with the orchestrator.
        
        Args:
            name: Unique name for the component
            component_type: Type of component (e.g., "atomspace", "hyperon")
            capabilities: List of capabilities this component provides
            dependencies: List of other components this depends on
            metadata: Additional metadata about the component
            
        Returns:
            The registered CognitiveComponent
        """
        if name in self.components:
            logger.warning(f"Component {name} already registered, updating...")
        
        component = CognitiveComponent(
            name=name,
            component_type=component_type,
            capabilities=capabilities or [],
            dependencies=dependencies or [],
            metadata=metadata or {},
            state=ComponentState.READY,
            last_activity=time.time()
        )
        
        self.components[name] = component
        
        # Emit registration event
        event = SynergyEvent(
            event_type=EventType.COMPONENT_REGISTERED,
            timestamp=time.time(),
            source_component=name,
            data=component.to_dict()
        )
        self.emit_event(event)
        
        logger.info(f"Registered component: {name} ({component_type})")
        return component
    
    def update_component_state(self, name: str, state: ComponentState):
        """Update the state of a component"""
        if name not in self.components:
            logger.error(f"Cannot update state: component {name} not found")
            return
        
        old_state = self.components[name].state
        self.components[name].state = state
        self.components[name].last_activity = time.time()
        
        event = SynergyEvent(
            event_type=EventType.COMPONENT_STATE_CHANGED,
            timestamp=time.time(),
            source_component=name,
            data={'old_state': old_state.value, 'new_state': state.value}
        )
        self.emit_event(event)
        
        logger.info(f"Component {name} state changed: {old_state.value} -> {state.value}")
    
    def record_data_transfer(
        self,
        source: str,
        target: str,
        data_type: str,
        data_size: int = 0,
        metadata: Dict[str, Any] = None
    ):
        """
        Record a data transfer between components.
        
        This is a key metric for measuring cognitive synergy.
        """
        event = SynergyEvent(
            event_type=EventType.DATA_TRANSFERRED,
            timestamp=time.time(),
            source_component=source,
            target_component=target,
            data={
                'data_type': data_type,
                'data_size': data_size,
                'metadata': metadata or {}
            }
        )
        self.emit_event(event)
        
        logger.debug(f"Data transfer: {source} -> {target} ({data_type})")
    
    def emit_event(self, event: SynergyEvent):
        """Emit an event to the event queue"""
        self.event_queue.put(event)
        self.metrics.record_event(event)
    
    def subscribe(self, event_type: EventType, handler: Callable):
        """Subscribe a handler to a specific event type"""
        if event_type not in self.event_handlers:
            self.event_handlers[event_type] = []
        self.event_handlers[event_type].append(handler)
        logger.debug(f"Subscribed handler to {event_type.value}")
    
    def _process_events(self):
        """Process events from the queue (runs in separate thread)"""
        while self.running:
            try:
                event = self.event_queue.get(timeout=1.0)
                
                # Call registered handlers
                if event.event_type in self.event_handlers:
                    for handler in self.event_handlers[event.event_type]:
                        try:
                            handler(event)
                        except Exception as e:
                            logger.error(f"Error in event handler: {e}")
                
                self.event_queue.task_done()
            except Empty:
                continue
            except Exception as e:
                logger.error(f"Error processing event: {e}")
    
    def start(self):
        """Start the orchestrator event processing"""
        if self.running:
            logger.warning("Orchestrator already running")
            return
        
        self.running = True
        self.event_thread = threading.Thread(target=self._process_events, daemon=True)
        self.event_thread.start()
        logger.info("Cognitive Synergy Orchestrator started")
    
    def stop(self):
        """Stop the orchestrator"""
        self.running = False
        if self.event_thread:
            self.event_thread.join(timeout=5.0)
        logger.info("Cognitive Synergy Orchestrator stopped")
    
    def get_synergy_report(self) -> Dict[str, Any]:
        """Generate a comprehensive synergy report"""
        return {
            'timestamp': datetime.now().isoformat(),
            'components': {
                name: comp.to_dict()
                for name, comp in self.components.items()
            },
            'metrics': self.metrics.get_statistics(),
            'system_health': self._assess_system_health()
        }
    
    def _assess_system_health(self) -> Dict[str, Any]:
        """Assess overall system health"""
        total_components = len(self.components)
        ready_components = sum(
            1 for c in self.components.values()
            if c.state in [ComponentState.READY, ComponentState.ACTIVE]
        )
        error_components = sum(
            1 for c in self.components.values()
            if c.state == ComponentState.ERROR
        )
        
        health_score = (ready_components / total_components * 100) if total_components > 0 else 0
        
        return {
            'total_components': total_components,
            'ready_components': ready_components,
            'error_components': error_components,
            'health_score': health_score,
            'status': 'healthy' if health_score >= 80 else 'degraded' if health_score >= 50 else 'critical'
        }
    
    def save_report(self, filepath: str):
        """Save synergy report to file"""
        report = self.get_synergy_report()
        with open(filepath, 'w') as f:
            json.dump(report, f, indent=2)
        logger.info(f"Synergy report saved to {filepath}")


# Singleton instance
_orchestrator_instance: Optional[CognitiveSynergyOrchestrator] = None


def get_orchestrator() -> CognitiveSynergyOrchestrator:
    """Get the global orchestrator instance"""
    global _orchestrator_instance
    if _orchestrator_instance is None:
        _orchestrator_instance = CognitiveSynergyOrchestrator()
    return _orchestrator_instance


if __name__ == "__main__":
    # Example usage
    orchestrator = get_orchestrator()
    orchestrator.start()
    
    # Register some example components
    orchestrator.register_component(
        "atomspace",
        "hypergraph_db",
        capabilities=["storage", "query", "reasoning"],
        dependencies=[]
    )
    
    orchestrator.register_component(
        "hyperon",
        "cognitive_framework",
        capabilities=["reasoning", "learning"],
        dependencies=["atomspace"]
    )
    
    orchestrator.register_component(
        "python_ml",
        "machine_learning",
        capabilities=["classification", "prediction"],
        dependencies=[]
    )
    
    # Simulate some interactions
    orchestrator.record_data_transfer("atomspace", "hyperon", "atom_data", 1024)
    orchestrator.record_data_transfer("hyperon", "python_ml", "inference_result", 512)
    orchestrator.record_data_transfer("python_ml", "atomspace", "learned_pattern", 256)
    
    # Wait a bit for event processing
    time.sleep(2)
    
    # Generate and print report
    report = orchestrator.get_synergy_report()
    print(json.dumps(report, indent=2))
    
    orchestrator.stop()
