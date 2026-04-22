"""
Supabase Integration for Cognitive Synergy Persistence

This module provides database persistence for cognitive synergy components:
- AAR Core states (agent-arena-relation)
- Hypergraph identity nodes and edges
- Identity refinement tuples
- Cognitive synergy metrics over time

Enables continuous identity refinement across sessions and temporal analysis
of cognitive evolution.
"""

import os
from typing import Dict, List, Optional, Tuple, Any
from datetime import datetime
import numpy as np
from supabase import create_client, Client

class SupabaseSynergyStore:
    """
    Persistent storage for cognitive synergy components using Supabase.
    
    Provides methods to store and retrieve:
    - AAR Core states
    - Hypergraph identity structures
    - Identity refinement tuples
    - Synergy metrics
    """
    
    def __init__(self, supabase_url: Optional[str] = None, supabase_key: Optional[str] = None):
        """Initialize Supabase client for cognitive synergy storage."""
        self.url = supabase_url or os.getenv('SUPABASE_URL')
        self.key = supabase_key or os.getenv('SUPABASE_KEY')
        
        if not self.url or not self.key:
            raise ValueError("Supabase URL and KEY must be provided or set in environment variables")
        
        self.client: Client = create_client(self.url, self.key)
    
    def store_aar_state(
        self,
        agent_state: np.ndarray,
        arena_state: np.ndarray,
        coherence_matrix: np.ndarray,
        emergence_score: float,
        self_representation: np.ndarray,
        metadata: Optional[Dict] = None
    ) -> str:
        """Store an AAR Core state snapshot."""
        data = {
            'agent_state': agent_state.tolist(),
            'arena_state': arena_state.tolist(),
            'coherence_matrix': coherence_matrix.tolist(),
            'emergence_score': float(emergence_score),
            'self_representation': self_representation.tolist(),
            'metadata': metadata or {},
            'timestamp': datetime.now().isoformat()
        }
        
        result = self.client.table('aar_states').insert(data).execute()
        return result.data[0]['id']
    
    def store_identity_node(
        self,
        node_id: str,
        node_type: str,
        content: str,
        embedding: Optional[np.ndarray] = None,
        centrality: Optional[float] = None,
        metadata: Optional[Dict] = None
    ) -> str:
        """Store a hypergraph identity node."""
        data = {
            'node_id': node_id,
            'node_type': node_type,
            'content': content,
            'embedding': embedding.tolist() if embedding is not None else None,
            'centrality': float(centrality) if centrality is not None else None,
            'metadata': metadata or {}
        }
        
        result = self.client.table('identity_nodes')\
            .upsert(data, on_conflict='node_id')\
            .execute()
        
        return result.data[0]['id']
    
    def store_synergy_metrics(
        self,
        emergence_score: float,
        identity_coherence: float,
        membrane_efficiency: float,
        cross_component_interactions: int,
        metadata: Optional[Dict] = None
    ) -> str:
        """Store cognitive synergy metrics snapshot."""
        data = {
            'emergence_score': float(emergence_score),
            'identity_coherence': float(identity_coherence),
            'membrane_efficiency': float(membrane_efficiency),
            'cross_component_interactions': int(cross_component_interactions),
            'metadata': metadata or {},
            'timestamp': datetime.now().isoformat()
        }
        
        result = self.client.table('synergy_metrics').insert(data).execute()
        return result.data[0]['id']
    
    def get_synergy_summary(self) -> Dict[str, Any]:
        """Get summary statistics of cognitive synergy."""
        latest = self.client.table('synergy_metrics')\
            .select('*')\
            .order('timestamp', desc=True)\
            .limit(1)\
            .execute()
        
        if not latest.data:
            return {}
        
        return {'latest': latest.data[0]}


if __name__ == '__main__':
    # Example usage
    store = SupabaseSynergyStore()
    print("Supabase Synergy Store initialized successfully")
