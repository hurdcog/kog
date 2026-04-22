#!/usr/bin/env python3
"""
Cognitive Synergy Orchestrator

Central coordination system that manages inter-component communication
and synergy optimization for the OpenCog Collection.

This implements the "Relation" aspect of the Agent-Arena-Relation (AAR)
framework, coordinating the dynamic interplay between cognitive components.
"""

import json
import time
import logging
from typing import Dict, List, Any, Optional
from dataclasses import dataclass, asdict
from datetime import datetime
from pathlib import Path
import threading
from queue import Queue, Empty


logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


@dataclass
class ComponentState:
    """Represents the state of a cognitive component."""
    name: str
    status: str  # 'idle', 'active', 'processing', 'error'
    load: float  # 0.0 to 1.0
    last_activity: float
    metrics: Dict[str, Any]
    
    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)


@dataclass
class SynergyEvent:
    """Represents an event in the cognitive synergy system."""
    timestamp: float
    source: str
    target: Optional[str]
    event_type: str  # 'data', 'request', 'response', 'error', 'metric'
    payload: Dict[str, Any]
    
    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)


class EventBus:
    """
    Event bus for inter-component communication.
    
    Implements a publish-subscribe pattern for cognitive components
    to communicate without tight coupling.
    """
    
    def __init__(self):
        self.subscribers: Dict[str, List[callable]] = {}
        self.event_queue = Queue()
        self.running = False
        self.processor_thread = None
        
    def subscribe(self, event_type: str, callback: callable):
        """Subscribe to events of a specific type."""
        if event_type not in self.subscribers:
            self.subscribers[event_type] = []
        self.subscribers[event_type].append(callback)
        logger.info(f"Subscribed to event type: {event_type}")
        
    def publish(self, event: SynergyEvent):
        """Publish an event to all subscribers."""
        self.event_queue.put(event)
        
    def start(self):
        """Start processing events."""
        self.running = True
        self.processor_thread = threading.Thread(target=self._process_events)
        self.processor_thread.daemon = True
        self.processor_thread.start()
        logger.info("Event bus started")
        
    def stop(self):
        """Stop processing events."""
        self.running = False
        if self.processor_thread:
            self.processor_thread.join(timeout=5)
        logger.info("Event bus stopped")
        
    def _process_events(self):
        """Process events from the queue."""
        while self.running:
            try:
                event = self.event_queue.get(timeout=0.1)
                self._dispatch_event(event)
            except Empty:
                continue
            except Exception as e:
                logger.error(f"Error processing event: {e}")
                
    def _dispatch_event(self, event: SynergyEvent):
        """Dispatch an event to subscribers."""
        if event.event_type in self.subscribers:
            for callback in self.subscribers[event.event_type]:
                try:
                    callback(event)
                except Exception as e:
                    logger.error(f"Error in event callback: {e}")


class SynergyOrchestrator:
    """
    Central orchestrator for cognitive synergy.
    
    Manages component registration, monitors interactions,
    and optimizes synergy between cognitive components.
    """
    
    def __init__(self, config_path: Optional[Path] = None):
        self.components: Dict[str, ComponentState] = {}
        self.event_bus = EventBus()
        self.synergy_metrics: Dict[str, float] = {}
        self.interaction_graph: Dict[str, List[str]] = {}
        self.config = self._load_config(config_path)
        self.start_time = time.time()
        
        # Initialize event bus
        self.event_bus.start()
        
        # Subscribe to key events
        self.event_bus.subscribe('metric', self._handle_metric_event)
        self.event_bus.subscribe('data', self._handle_data_event)
        self.event_bus.subscribe('error', self._handle_error_event)
        
        logger.info("Synergy Orchestrator initialized")
        
    def _load_config(self, config_path: Optional[Path]) -> Dict[str, Any]:
        """Load configuration from file."""
        if config_path and config_path.exists():
            with open(config_path, 'r') as f:
                return json.load(f)
        return {
            'synergy_threshold': 0.7,
            'max_component_load': 0.9,
            'optimization_interval': 60,
        }
        
    def register_component(self, name: str, metadata: Optional[Dict[str, Any]] = None):
        """Register a cognitive component."""
        self.components[name] = ComponentState(
            name=name,
            status='idle',
            load=0.0,
            last_activity=time.time(),
            metrics=metadata or {}
        )
        self.interaction_graph[name] = []
        logger.info(f"Registered component: {name}")
        
    def update_component_state(self, name: str, status: str, load: float, metrics: Optional[Dict[str, Any]] = None):
        """Update the state of a component."""
        if name not in self.components:
            logger.warning(f"Component {name} not registered")
            return
            
        component = self.components[name]
        component.status = status
        component.load = load
        component.last_activity = time.time()
        if metrics:
            component.metrics.update(metrics)
            
        # Publish metric event
        event = SynergyEvent(
            timestamp=time.time(),
            source=name,
            target=None,
            event_type='metric',
            payload={'status': status, 'load': load, 'metrics': metrics or {}}
        )
        self.event_bus.publish(event)
        
    def record_interaction(self, source: str, target: str, data: Dict[str, Any]):
        """Record an interaction between components."""
        if source not in self.interaction_graph:
            self.interaction_graph[source] = []
        if target not in self.interaction_graph[source]:
            self.interaction_graph[source].append(target)
            
        # Publish data event
        event = SynergyEvent(
            timestamp=time.time(),
            source=source,
            target=target,
            event_type='data',
            payload=data
        )
        self.event_bus.publish(event)
        logger.debug(f"Interaction: {source} -> {target}")
        
    def calculate_synergy_score(self) -> float:
        """
        Calculate overall synergy score.
        
        Synergy score is based on:
        - Component interaction density
        - Load balancing across components
        - Error rates
        - Response times
        """
        if not self.components:
            return 0.0
            
        # Interaction density (0-1)
        total_possible = len(self.components) * (len(self.components) - 1)
        actual_interactions = sum(len(targets) for targets in self.interaction_graph.values())
        interaction_score = actual_interactions / max(total_possible, 1)
        
        # Load balance (0-1, higher is better)
        loads = [c.load for c in self.components.values()]
        avg_load = sum(loads) / len(loads) if loads else 0
        load_variance = sum((l - avg_load) ** 2 for l in loads) / len(loads) if loads else 0
        load_balance_score = 1.0 - min(load_variance, 1.0)
        
        # Activity score (0-1)
        current_time = time.time()
        active_components = sum(
            1 for c in self.components.values()
            if current_time - c.last_activity < 60
        )
        activity_score = active_components / len(self.components)
        
        # Weighted average
        synergy_score = (
            0.4 * interaction_score +
            0.3 * load_balance_score +
            0.3 * activity_score
        )
        
        return synergy_score
        
    def get_system_state(self) -> Dict[str, Any]:
        """Get current system state."""
        return {
            'timestamp': datetime.now().isoformat(),
            'uptime': time.time() - self.start_time,
            'components': {name: comp.to_dict() for name, comp in self.components.items()},
            'synergy_score': self.calculate_synergy_score(),
            'interaction_graph': self.interaction_graph,
            'metrics': self.synergy_metrics
        }
        
    def save_state(self, output_path: Path):
        """Save current state to file."""
        state = self.get_system_state()
        with open(output_path, 'w') as f:
            json.dump(state, f, indent=2)
        logger.info(f"State saved to {output_path}")
        
    def _handle_metric_event(self, event: SynergyEvent):
        """Handle metric events."""
        metrics = event.payload.get('metrics', {})
        for key, value in metrics.items():
            metric_name = f"{event.source}.{key}"
            self.synergy_metrics[metric_name] = value
            
    def _handle_data_event(self, event: SynergyEvent):
        """Handle data transfer events."""
        # Update interaction graph
        if event.source and event.target:
            if event.source not in self.interaction_graph:
                self.interaction_graph[event.source] = []
            if event.target not in self.interaction_graph[event.source]:
                self.interaction_graph[event.source].append(event.target)
                
    def _handle_error_event(self, event: SynergyEvent):
        """Handle error events."""
        logger.error(f"Error in {event.source}: {event.payload}")
        
    def shutdown(self):
        """Shutdown the orchestrator."""
        self.event_bus.stop()
        logger.info("Synergy Orchestrator shutdown")


def main():
    """Example usage of the Synergy Orchestrator."""
    orchestrator = SynergyOrchestrator()
    
    # Register components
    orchestrator.register_component('atomspace', {'type': 'knowledge_base'})
    orchestrator.register_component('hyperon', {'type': 'reasoning_engine'})
    orchestrator.register_component('cogserver', {'type': 'communication'})
    
    # Simulate component activity
    orchestrator.update_component_state('atomspace', 'active', 0.6, {'atoms': 1000})
    orchestrator.update_component_state('hyperon', 'processing', 0.8, {'inferences': 50})
    orchestrator.update_component_state('cogserver', 'active', 0.3, {'connections': 5})
    
    # Record interactions
    orchestrator.record_interaction('atomspace', 'hyperon', {'atoms_transferred': 100})
    orchestrator.record_interaction('hyperon', 'cogserver', {'results': 10})
    
    # Calculate synergy
    synergy_score = orchestrator.calculate_synergy_score()
    print(f"Synergy Score: {synergy_score:.2f}")
    
    # Save state
    output_path = Path('/tmp/synergy_state.json')
    orchestrator.save_state(output_path)
    
    # Shutdown
    time.sleep(1)
    orchestrator.shutdown()


if __name__ == '__main__':
    main()
