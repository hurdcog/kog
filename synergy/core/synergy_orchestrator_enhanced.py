#!/usr/bin/env python3
"""
Enhanced Synergy Orchestrator with Database Integration

Extends the base synergy orchestrator with persistent storage of synergy metrics,
component interactions, and hypergraph dynamics in Supabase/Neon databases.
"""

import sys
import os
from pathlib import Path

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent))

from synergy.core.synergy_orchestrator import SynergyOrchestrator

try:
    from synergy.database.db_connector import CognitiveSynergyDB
    DB_AVAILABLE = True
except ImportError:
    DB_AVAILABLE = False
    print("Warning: Database connector not available")


class EnhancedSynergyOrchestrator(SynergyOrchestrator):
    """
    Enhanced synergy orchestrator with database persistence.
    
    Extends the base orchestrator to automatically persist all synergy metrics,
    component states, and interactions to a database for long-term tracking and analysis.
    """
    
    def __init__(self, enable_db: bool = True):
        """
        Initialize enhanced orchestrator.
        
        Args:
            enable_db: Whether to enable database persistence
        """
        super().__init__()
        
        self.db_enabled = enable_db and DB_AVAILABLE
        self.db = None
        
        if self.db_enabled:
            try:
                self.db = CognitiveSynergyDB()
                if self.db.is_connected():
                    print("✓ Enhanced orchestrator connected to database")
                else:
                    print("⚠ Database connection failed, running without persistence")
                    self.db_enabled = False
            except Exception as e:
                print(f"⚠ Failed to initialize database: {e}")
                self.db_enabled = False
    
    def register_component(self, component_name: str, metadata: dict = None) -> bool:
        """
        Register a component with database persistence.
        
        Args:
            component_name: Name of the component
            metadata: Component metadata (must include 'type')
        
        Returns:
            True if registration successful
        """
        # Call parent method
        success = super().register_component(component_name, metadata)
        
        # Persist to database
        if success and self.db_enabled and self.db:
            component_type = metadata.get('type', 'unknown') if metadata else 'unknown'
            description = metadata.get('description', '') if metadata else ''
            
            db_id = self.db.register_component(
                name=component_name,
                component_type=component_type,
                description=description,
                metadata=metadata or {}
            )
            
            if db_id:
                print(f"✓ Component '{component_name}' persisted to database (ID: {db_id})")
            else:
                print(f"⚠ Failed to persist component '{component_name}' to database")
        
        return success
    
    def update_component_state(self, component_name: str, status: str, 
                              load_factor: float, metrics: dict = None) -> bool:
        """
        Update component state with database persistence.
        
        Args:
            component_name: Name of the component
            status: Component status ('active', 'idle', 'error')
            load_factor: Load factor (0.0 to 1.0)
            metrics: Additional metrics dictionary
        
        Returns:
            True if update successful
        """
        # Call parent method
        success = super().update_component_state(component_name, status, load_factor, metrics)
        
        # Persist to database
        if success and self.db_enabled and self.db:
            db_success = self.db.update_component_state(
                component_name=component_name,
                status=status,
                load_factor=load_factor,
                metrics=metrics or {}
            )
            
            if not db_success:
                print(f"⚠ Failed to persist state update for '{component_name}' to database")
        
        return success
    
    def record_interaction(self, source: str, target: str, metadata: dict = None) -> bool:
        """
        Record interaction with database persistence.
        
        Args:
            source: Source component name
            target: Target component name
            metadata: Interaction metadata
        
        Returns:
            True if recording successful
        """
        # Call parent method
        success = super().record_interaction(source, target, metadata)
        
        # Persist to database
        if success and self.db_enabled and self.db:
            interaction_type = metadata.get('type', 'data_transfer') if metadata else 'data_transfer'
            data_transferred = metadata.get('data', {}) if metadata else {}
            latency_ms = metadata.get('latency_ms') if metadata else None
            
            db_success = self.db.record_interaction(
                source_component=source,
                target_component=target,
                interaction_type=interaction_type,
                data_transferred=data_transferred,
                latency_ms=latency_ms,
                success=True
            )
            
            if not db_success:
                print(f"⚠ Failed to persist interaction {source} -> {target} to database")
        
        return success
    
    def calculate_synergy_score(self) -> float:
        """
        Calculate synergy score with database persistence.
        
        Returns:
            Synergy score (0.0 to 1.0)
        """
        # Call parent method
        score = super().calculate_synergy_score()
        
        # Persist to database
        if self.db_enabled and self.db:
            # Get component metrics for detailed storage
            interaction_density = self._calculate_interaction_density()
            load_balance = self._calculate_load_balance()
            activity_score = self._calculate_activity_score()
            
            active_components = len([
                c for c, state in self.component_states.items()
                if state.get('status') == 'active'
            ])
            
            total_interactions = len(self.interactions)
            
            db_success = self.db.record_synergy_score(
                score=score,
                interaction_density=interaction_density,
                load_balance=load_balance,
                activity_score=activity_score,
                active_components=active_components,
                total_interactions=total_interactions,
                metadata={
                    'components': list(self.components.keys()),
                    'calculation_method': 'weighted_average'
                }
            )
            
            if not db_success:
                print(f"⚠ Failed to persist synergy score to database")
        
        return score
    
    def _calculate_interaction_density(self) -> float:
        """Calculate interaction density metric."""
        if len(self.components) < 2:
            return 0.0
        
        max_possible_interactions = len(self.components) * (len(self.components) - 1)
        if max_possible_interactions == 0:
            return 0.0
        
        # Count unique component pairs that have interacted
        interacting_pairs = set()
        for interaction in self.interactions:
            source = interaction.get('source')
            target = interaction.get('target')
            if source and target:
                pair = tuple(sorted([source, target]))
                interacting_pairs.add(pair)
        
        return len(interacting_pairs) / (max_possible_interactions / 2)
    
    def _calculate_load_balance(self) -> float:
        """Calculate load balance metric."""
        if not self.component_states:
            return 1.0
        
        load_factors = [
            state.get('load_factor', 0.0)
            for state in self.component_states.values()
        ]
        
        if not load_factors:
            return 1.0
        
        avg_load = sum(load_factors) / len(load_factors)
        if avg_load == 0:
            return 1.0
        
        variance = sum((lf - avg_load) ** 2 for lf in load_factors) / len(load_factors)
        std_dev = variance ** 0.5
        
        # Convert to balance score (lower variance = higher balance)
        balance = 1.0 - min(std_dev / avg_load, 1.0)
        return balance
    
    def _calculate_activity_score(self) -> float:
        """Calculate activity score metric."""
        if not self.components:
            return 0.0
        
        active_count = len([
            c for c, state in self.component_states.items()
            if state.get('status') == 'active'
        ])
        
        return active_count / len(self.components)
    
    def get_synergy_trend(self, hours: int = 24) -> list:
        """
        Get synergy score trend from database.
        
        Args:
            hours: Number of hours to look back
        
        Returns:
            List of synergy score records
        """
        if not self.db_enabled or not self.db:
            return []
        
        return self.db.get_synergy_trend(hours=hours)
    
    def close(self):
        """Close database connections."""
        if self.db:
            self.db.close()


def main():
    """Test enhanced orchestrator."""
    print("=== Enhanced Synergy Orchestrator Test ===\n")
    
    orchestrator = EnhancedSynergyOrchestrator()
    
    print("\n--- Registering Components ---")
    orchestrator.register_component('atomspace', {'type': 'knowledge_base', 'description': 'Hypergraph database'})
    orchestrator.register_component('hyperon', {'type': 'reasoning_engine', 'description': 'Rust cognitive framework'})
    orchestrator.register_component('cogserver', {'type': 'communication_layer', 'description': 'Network server'})
    
    print("\n--- Updating Component States ---")
    orchestrator.update_component_state('atomspace', 'active', 0.6, {'atoms': 1000})
    orchestrator.update_component_state('hyperon', 'active', 0.4, {'rules': 50})
    orchestrator.update_component_state('cogserver', 'active', 0.3, {'connections': 5})
    
    print("\n--- Recording Interactions ---")
    orchestrator.record_interaction('atomspace', 'hyperon', {'type': 'data_transfer', 'atoms_transferred': 100})
    orchestrator.record_interaction('hyperon', 'cogserver', {'type': 'query', 'query_type': 'inference'})
    orchestrator.record_interaction('cogserver', 'atomspace', {'type': 'update', 'atoms_updated': 10})
    
    print("\n--- Calculating Synergy Score ---")
    score = orchestrator.calculate_synergy_score()
    print(f"Synergy Score: {score:.3f}")
    
    print("\n--- Retrieving Synergy Trend ---")
    trend = orchestrator.get_synergy_trend(hours=24)
    print(f"Retrieved {len(trend)} historical synergy records")
    
    if trend:
        print("\nLatest synergy scores:")
        for record in trend[:5]:
            timestamp = record.get('timestamp', 'unknown')
            score = record.get('score', 0.0)
            print(f"  {timestamp}: {score:.3f}")
    
    print("\n--- Generating Report ---")
    report = orchestrator.get_state_report()
    print(f"Active Components: {len(report['components'])}")
    print(f"Total Interactions: {report['total_interactions']}")
    print(f"Synergy Score: {report['synergy_score']:.3f}")
    
    orchestrator.close()
    print("\n=== Test Complete ===")


if __name__ == "__main__":
    main()
