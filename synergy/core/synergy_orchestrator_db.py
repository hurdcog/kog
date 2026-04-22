#!/usr/bin/env python3
"""
Database-Integrated Cognitive Synergy Orchestrator

Enhanced version that persists all synergy metrics and interactions to database
for long-term tracking and analysis of cognitive evolution.
"""

import os
import sys
import time
import logging
from pathlib import Path
from typing import Dict, List, Any, Optional

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent))

from core.synergy_orchestrator import SynergyOrchestrator, SynergyEvent
from database.db_connector import CognitiveSynergyDB

logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


class DatabaseIntegratedOrchestrator(SynergyOrchestrator):
    """
    Enhanced Synergy Orchestrator with database persistence.
    
    Extends the base orchestrator to automatically persist all metrics,
    interactions, and state changes to Supabase/Neon for long-term tracking.
    """
    
    def __init__(self, config_path: Optional[Path] = None, db_type: str = "auto"):
        super().__init__(config_path)
        
        # Initialize database connection
        self.db = CognitiveSynergyDB(connection_type=db_type)
        
        if not self.db.is_connected():
            logger.warning("No database connection available - running in memory-only mode")
        else:
            logger.info("Database integration enabled")
            
        # Subscribe to events for database persistence
        self.event_bus.subscribe('metric', self._persist_metric_event)
        self.event_bus.subscribe('data', self._persist_interaction_event)
        
        # Periodic synergy score recording
        self._last_score_time = time.time()
        self._score_interval = 60  # Record synergy score every 60 seconds
        
    def register_component(self, name: str, metadata: Optional[Dict[str, Any]] = None):
        """Register component in both memory and database."""
        super().register_component(name, metadata)
        
        if self.db.is_connected():
            component_type = metadata.get('type', 'unknown') if metadata else 'unknown'
            description = metadata.get('description', '') if metadata else ''
            self.db.register_component(name, component_type, description, metadata)
            
    def update_component_state(self, name: str, status: str, load: float, 
                              metrics: Optional[Dict[str, Any]] = None):
        """Update component state in both memory and database."""
        super().update_component_state(name, status, load, metrics)
        
        if self.db.is_connected():
            self.db.update_component_state(name, status, load, metrics)
            
    def record_interaction(self, source: str, target: str, data: Dict[str, Any]):
        """Record interaction in both memory and database."""
        super().record_interaction(source, target, data)
        
        if self.db.is_connected():
            interaction_type = data.get('type', 'data_transfer')
            latency_ms = data.get('latency_ms')
            success = data.get('success', True)
            self.db.record_interaction(source, target, interaction_type, data, latency_ms, success)
            
    def _persist_metric_event(self, event: SynergyEvent):
        """Persist metric events to database."""
        if not self.db.is_connected():
            return
            
        # Check if it's time to record synergy score
        current_time = time.time()
        if current_time - self._last_score_time >= self._score_interval:
            self._record_synergy_snapshot()
            self._last_score_time = current_time
            
    def _persist_interaction_event(self, event: SynergyEvent):
        """Persist interaction events to database."""
        # Already handled in record_interaction method
        pass
        
    def _record_synergy_snapshot(self):
        """Record a complete synergy score snapshot to database."""
        if not self.db.is_connected():
            return
            
        # Calculate all metrics
        synergy_score = self.calculate_synergy_score()
        
        # Calculate component metrics
        total_possible = len(self.components) * (len(self.components) - 1)
        actual_interactions = sum(len(targets) for targets in self.interaction_graph.values())
        interaction_density = actual_interactions / max(total_possible, 1)
        
        loads = [c.load for c in self.components.values()]
        avg_load = sum(loads) / len(loads) if loads else 0
        load_variance = sum((l - avg_load) ** 2 for l in loads) / len(loads) if loads else 0
        load_balance = 1.0 - min(load_variance, 1.0)
        
        current_time = time.time()
        active_components = sum(
            1 for c in self.components.values()
            if current_time - c.last_activity < 60
        )
        activity_score = active_components / len(self.components) if self.components else 0
        
        # Record to database
        self.db.record_synergy_score(
            score=synergy_score,
            interaction_density=interaction_density,
            load_balance=load_balance,
            activity_score=activity_score,
            active_components=active_components,
            total_interactions=actual_interactions,
            metadata={
                'uptime': current_time - self.start_time,
                'component_count': len(self.components)
            }
        )
        
        logger.info(f"Recorded synergy snapshot: score={synergy_score:.3f}")
        
    def get_historical_synergy_trend(self, hours: int = 24) -> List[Dict[str, Any]]:
        """Get historical synergy trend from database."""
        if not self.db.is_connected():
            return []
            
        return self.db.get_synergy_trend(hours)
        
    def get_component_performance(self, component_name: str, hours: int = 24) -> Dict[str, Any]:
        """Get component performance metrics from database."""
        if not self.db.is_connected():
            return {}
            
        return self.db.get_component_performance(component_name, hours)
        
    def shutdown(self):
        """Shutdown orchestrator and record final state."""
        # Record final synergy snapshot
        self._record_synergy_snapshot()
        
        # Close database connection
        if self.db.postgres_conn:
            self.db.postgres_conn.close()
            
        super().shutdown()


def main():
    """Example usage with database integration."""
    orchestrator = DatabaseIntegratedOrchestrator()
    
    # Register components
    orchestrator.register_component('atomspace', {
        'type': 'knowledge_base',
        'description': 'Hypergraph knowledge representation'
    })
    orchestrator.register_component('hyperon', {
        'type': 'reasoning_engine',
        'description': 'Rust-based cognitive computing'
    })
    orchestrator.register_component('cogserver', {
        'type': 'communication_layer',
        'description': 'Network server for cognitive operations'
    })
    
    # Simulate activity over time
    for i in range(10):
        # Update component states
        orchestrator.update_component_state('atomspace', 'active', 0.5 + i*0.05, 
                                           {'atoms': 1000 + i*100})
        orchestrator.update_component_state('hyperon', 'processing', 0.6 + i*0.03,
                                           {'inferences': 50 + i*10})
        orchestrator.update_component_state('cogserver', 'active', 0.3 + i*0.02,
                                           {'connections': 5 + i})
        
        # Record interactions
        orchestrator.record_interaction('atomspace', 'hyperon', {
            'type': 'data_transfer',
            'atoms_transferred': 100 + i*10,
            'latency_ms': 5.0 + i*0.5,
            'success': True
        })
        orchestrator.record_interaction('hyperon', 'cogserver', {
            'type': 'result_delivery',
            'results': 10 + i,
            'latency_ms': 3.0 + i*0.3,
            'success': True
        })
        
        time.sleep(1)
    
    # Get current state
    state = orchestrator.get_system_state()
    print(f"\nFinal Synergy Score: {state['synergy_score']:.3f}")
    print(f"Active Components: {len(state['components'])}")
    print(f"Total Interactions: {sum(len(targets) for targets in state['interaction_graph'].values())}")
    
    # Shutdown
    orchestrator.shutdown()


if __name__ == '__main__':
    main()
