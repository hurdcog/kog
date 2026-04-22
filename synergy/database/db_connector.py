#!/usr/bin/env python3
"""
Database Connector for Cognitive Synergy Tracking

Provides unified interface for connecting to Supabase and Neon PostgreSQL databases
to track hypergraph dynamics, component interactions, and synergy evolution.
"""

import os
import json
from datetime import datetime, timedelta
from typing import Dict, List, Optional, Any, Tuple
from uuid import UUID, uuid4

try:
    import psycopg2
    from psycopg2.extras import RealDictCursor, Json
    PSYCOPG2_AVAILABLE = True
except ImportError:
    PSYCOPG2_AVAILABLE = False
    print("Warning: psycopg2 not available. Install with: pip install psycopg2-binary")

try:
    from supabase import create_client, Client
    SUPABASE_AVAILABLE = True
except ImportError:
    SUPABASE_AVAILABLE = False
    print("Warning: supabase not available. Install with: pip install supabase")


class CognitiveSynergyDB:
    """
    Unified database connector for cognitive synergy tracking.
    
    Supports both Supabase (via REST API) and direct PostgreSQL connections (Neon).
    """
    
    def __init__(self, connection_type: str = "auto"):
        """
        Initialize database connection.
        
        Args:
            connection_type: "supabase", "postgres", or "auto" (tries both)
        """
        self.connection_type = connection_type
        self.supabase_client: Optional[Client] = None
        self.postgres_conn = None
        
        if connection_type in ["supabase", "auto"]:
            self._init_supabase()
        
        if connection_type in ["postgres", "auto"]:
            self._init_postgres()
    
    def _init_supabase(self):
        """Initialize Supabase connection."""
        if not SUPABASE_AVAILABLE:
            return
        
        url = os.environ.get("SUPABASE_URL")
        key = os.environ.get("SUPABASE_KEY")
        
        if url and key:
            try:
                self.supabase_client = create_client(url, key)
                print(f"✓ Connected to Supabase: {url}")
            except Exception as e:
                print(f"✗ Failed to connect to Supabase: {e}")
    
    def _init_postgres(self):
        """Initialize direct PostgreSQL connection (Neon)."""
        if not PSYCOPG2_AVAILABLE:
            return
        
        # Try to get connection string from environment
        conn_string = os.environ.get("DATABASE_URL") or os.environ.get("NEON_DATABASE_URL")
        
        if conn_string:
            try:
                self.postgres_conn = psycopg2.connect(conn_string)
                print(f"✓ Connected to PostgreSQL database")
            except Exception as e:
                print(f"✗ Failed to connect to PostgreSQL: {e}")
    
    def is_connected(self) -> bool:
        """Check if any database connection is active."""
        return self.supabase_client is not None or self.postgres_conn is not None
    
    # ========================================================================
    # COMPONENT MANAGEMENT
    # ========================================================================
    
    def register_component(self, name: str, component_type: str, 
                          description: str = "", metadata: Dict = None) -> Optional[str]:
        """
        Register a new cognitive component.
        
        Returns:
            Component ID (UUID as string) or None if failed
        """
        data = {
            "name": name,
            "component_type": component_type,
            "description": description,
            "metadata": metadata or {},
            "status": "active"
        }
        
        if self.supabase_client:
            try:
                result = self.supabase_client.table("components").insert(data).execute()
                return result.data[0]["id"] if result.data else None
            except Exception as e:
                print(f"Error registering component: {e}")
                return None
        
        if self.postgres_conn:
            try:
                with self.postgres_conn.cursor() as cur:
                    cur.execute("""
                        INSERT INTO components (name, component_type, description, metadata, status)
                        VALUES (%s, %s, %s, %s, %s)
                        ON CONFLICT (name) DO UPDATE SET
                            component_type = EXCLUDED.component_type,
                            description = EXCLUDED.description,
                            metadata = EXCLUDED.metadata,
                            status = EXCLUDED.status,
                            updated_at = NOW()
                        RETURNING id
                    """, (name, component_type, description, Json(metadata or {}), "active"))
                    self.postgres_conn.commit()
                    return str(cur.fetchone()[0])
            except Exception as e:
                print(f"Error registering component: {e}")
                self.postgres_conn.rollback()
                return None
        
        return None
    
    def update_component_state(self, component_name: str, status: str, 
                              load_factor: float, metrics: Dict = None) -> bool:
        """Update the state of a component."""
        # First get component ID
        component_id = self._get_component_id(component_name)
        if not component_id:
            return False
        
        data = {
            "component_id": component_id,
            "status": status,
            "load_factor": load_factor,
            "metrics": metrics or {}
        }
        
        if self.supabase_client:
            try:
                self.supabase_client.table("component_states").insert(data).execute()
                return True
            except Exception as e:
                print(f"Error updating component state: {e}")
                return False
        
        if self.postgres_conn:
            try:
                with self.postgres_conn.cursor() as cur:
                    cur.execute("""
                        INSERT INTO component_states (component_id, status, load_factor, metrics)
                        VALUES (%s, %s, %s, %s)
                    """, (component_id, status, load_factor, Json(metrics or {})))
                    self.postgres_conn.commit()
                    return True
            except Exception as e:
                print(f"Error updating component state: {e}")
                self.postgres_conn.rollback()
                return False
        
        return False
    
    def _get_component_id(self, component_name: str) -> Optional[str]:
        """Get component ID by name."""
        if self.supabase_client:
            try:
                result = self.supabase_client.table("components")\
                    .select("id")\
                    .eq("name", component_name)\
                    .execute()
                return result.data[0]["id"] if result.data else None
            except Exception as e:
                print(f"Error getting component ID: {e}")
                return None
        
        if self.postgres_conn:
            try:
                with self.postgres_conn.cursor() as cur:
                    cur.execute("SELECT id FROM components WHERE name = %s", (component_name,))
                    result = cur.fetchone()
                    return str(result[0]) if result else None
            except Exception as e:
                print(f"Error getting component ID: {e}")
                return None
        
        return None
    
    # ========================================================================
    # INTERACTION TRACKING
    # ========================================================================
    
    def record_interaction(self, source_component: str, target_component: str,
                          interaction_type: str, data_transferred: Dict = None,
                          latency_ms: float = None, success: bool = True) -> bool:
        """Record an interaction between two components."""
        source_id = self._get_component_id(source_component)
        target_id = self._get_component_id(target_component)
        
        if not source_id or not target_id:
            return False
        
        data = {
            "source_component_id": source_id,
            "target_component_id": target_id,
            "interaction_type": interaction_type,
            "data_transferred": data_transferred or {},
            "latency_ms": latency_ms,
            "success": success
        }
        
        if self.supabase_client:
            try:
                self.supabase_client.table("interactions").insert(data).execute()
                return True
            except Exception as e:
                print(f"Error recording interaction: {e}")
                return False
        
        if self.postgres_conn:
            try:
                with self.postgres_conn.cursor() as cur:
                    cur.execute("""
                        INSERT INTO interactions 
                        (source_component_id, target_component_id, interaction_type, 
                         data_transferred, latency_ms, success)
                        VALUES (%s, %s, %s, %s, %s, %s)
                    """, (source_id, target_id, interaction_type, 
                         Json(data_transferred or {}), latency_ms, success))
                    self.postgres_conn.commit()
                    return True
            except Exception as e:
                print(f"Error recording interaction: {e}")
                self.postgres_conn.rollback()
                return False
        
        return False
    
    # ========================================================================
    # SYNERGY METRICS
    # ========================================================================
    
    def record_synergy_score(self, score: float, interaction_density: float,
                            load_balance: float, activity_score: float,
                            active_components: int, total_interactions: int,
                            metadata: Dict = None) -> bool:
        """Record a synergy score snapshot."""
        data = {
            "score": score,
            "interaction_density": interaction_density,
            "load_balance": load_balance,
            "activity_score": activity_score,
            "active_components": active_components,
            "total_interactions": total_interactions,
            "metadata": metadata or {}
        }
        
        if self.supabase_client:
            try:
                self.supabase_client.table("synergy_scores").insert(data).execute()
                return True
            except Exception as e:
                print(f"Error recording synergy score: {e}")
                return False
        
        if self.postgres_conn:
            try:
                with self.postgres_conn.cursor() as cur:
                    cur.execute("""
                        INSERT INTO synergy_scores 
                        (score, interaction_density, load_balance, activity_score,
                         active_components, total_interactions, metadata)
                        VALUES (%s, %s, %s, %s, %s, %s, %s)
                    """, (score, interaction_density, load_balance, activity_score,
                         active_components, total_interactions, Json(metadata or {})))
                    self.postgres_conn.commit()
                    return True
            except Exception as e:
                print(f"Error recording synergy score: {e}")
                self.postgres_conn.rollback()
                return False
        
        return False
    
    def get_synergy_trend(self, hours: int = 24) -> List[Dict]:
        """Get synergy score trend over time."""
        if self.supabase_client:
            try:
                cutoff = (datetime.now() - timedelta(hours=hours)).isoformat()
                result = self.supabase_client.table("synergy_scores")\
                    .select("*")\
                    .gte("timestamp", cutoff)\
                    .order("timestamp", desc=True)\
                    .execute()
                return result.data
            except Exception as e:
                print(f"Error getting synergy trend: {e}")
                return []
        
        if self.postgres_conn:
            try:
                with self.postgres_conn.cursor(cursor_factory=RealDictCursor) as cur:
                    cur.execute("""
                        SELECT * FROM synergy_scores
                        WHERE timestamp > NOW() - INTERVAL '%s hours'
                        ORDER BY timestamp DESC
                    """, (hours,))
                    return [dict(row) for row in cur.fetchall()]
            except Exception as e:
                print(f"Error getting synergy trend: {e}")
                return []
        
        return []
    
    # ========================================================================
    # HYPERGRAPH OPERATIONS
    # ========================================================================
    
    def create_atom(self, atom_type: str, name: str = None, content: str = None,
                   truth_value: Dict = None, source_component: str = None,
                   metadata: Dict = None) -> Optional[str]:
        """Create a new atom in the hypergraph."""
        source_id = self._get_component_id(source_component) if source_component else None
        
        data = {
            "atom_type": atom_type,
            "name": name,
            "content": content,
            "truth_value": truth_value or {"strength": 1.0, "confidence": 1.0},
            "source_component": source_id,
            "metadata": metadata or {}
        }
        
        if self.supabase_client:
            try:
                result = self.supabase_client.table("atoms").insert(data).execute()
                return result.data[0]["id"] if result.data else None
            except Exception as e:
                print(f"Error creating atom: {e}")
                return None
        
        if self.postgres_conn:
            try:
                with self.postgres_conn.cursor() as cur:
                    cur.execute("""
                        INSERT INTO atoms (atom_type, name, content, truth_value, source_component, metadata)
                        VALUES (%s, %s, %s, %s, %s, %s)
                        RETURNING id
                    """, (atom_type, name, content, Json(truth_value or {"strength": 1.0, "confidence": 1.0}),
                         source_id, Json(metadata or {})))
                    self.postgres_conn.commit()
                    return str(cur.fetchone()[0])
            except Exception as e:
                print(f"Error creating atom: {e}")
                self.postgres_conn.rollback()
                return None
        
        return None
    
    def close(self):
        """Close database connections."""
        if self.postgres_conn:
            self.postgres_conn.close()
            print("✓ PostgreSQL connection closed")


def main():
    """Test database connection and basic operations."""
    print("=== Cognitive Synergy Database Connector Test ===\n")
    
    db = CognitiveSynergyDB()
    
    if not db.is_connected():
        print("✗ No database connection available")
        print("\nTo connect, set one of:")
        print("  - SUPABASE_URL and SUPABASE_KEY for Supabase")
        print("  - DATABASE_URL or NEON_DATABASE_URL for PostgreSQL")
        return
    
    print("\n--- Testing Component Registration ---")
    component_id = db.register_component(
        name="test_component",
        component_type="test",
        description="Test component for database verification"
    )
    print(f"Registered component: {component_id}")
    
    print("\n--- Testing Component State Update ---")
    success = db.update_component_state(
        component_name="test_component",
        status="active",
        load_factor=0.5,
        metrics={"test_metric": 42}
    )
    print(f"State update: {'✓ Success' if success else '✗ Failed'}")
    
    print("\n--- Testing Synergy Score Recording ---")
    success = db.record_synergy_score(
        score=0.85,
        interaction_density=0.7,
        load_balance=0.9,
        activity_score=0.95,
        active_components=5,
        total_interactions=100
    )
    print(f"Synergy score recorded: {'✓ Success' if success else '✗ Failed'}")
    
    print("\n--- Testing Synergy Trend Retrieval ---")
    trend = db.get_synergy_trend(hours=24)
    print(f"Retrieved {len(trend)} synergy score records")
    
    db.close()
    print("\n=== Test Complete ===")


if __name__ == "__main__":
    main()
