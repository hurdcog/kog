# Cognitive Synergy Database Integration

This directory contains the database infrastructure for persistent tracking of cognitive synergy metrics, hypergraph dynamics, and component interactions in the OpenCog Collection.

## Overview

The database integration enables **long-term tracking and analysis** of cognitive synergy by persisting:

- **Component states and metrics** over time
- **Inter-component interactions** and communication patterns
- **Hypergraph evolution** (atoms and hyperedges)
- **Synergy scores** and performance metrics
- **Identity refinement** from conversational data
- **Resource utilization** and system health

## Architecture

```
synergy/database/
├── schema.sql              # PostgreSQL schema for Supabase/Neon
├── db_connector.py         # Unified database connector
├── README.md              # This file
└── migrations/            # Database migrations (future)
```

## Database Schema

The schema includes the following core tables:

### Component Management
- **`components`**: Registry of all cognitive components
- **`component_states`**: Time-series component state snapshots

### Hypergraph Storage
- **`atoms`**: Core hypergraph nodes (concepts, data)
- **`hyperedges`**: Relationships between atoms
- **`hyperedge_members`**: Multi-atom hyperedge connections

### Interaction Tracking
- **`interactions`**: Communication events between components

### Synergy Metrics
- **`synergy_scores`**: Overall system synergy over time
- **`cognitive_tasks`**: Individual cognitive operations
- **`resource_snapshots`**: System resource utilization

### Identity Refinement
- **`identity_tuples`**: Identity refinement from conversations
- **`identity_snapshots`**: System identity evolution

### Views and Functions
- **`active_components`**: Currently active components
- **`recent_interactions`**: Recent communication events
- **`synergy_trend`**: Synergy score trends
- **`component_performance`**: Component performance metrics

## Setup

### Prerequisites

Install required Python packages:

```bash
pip install psycopg2-binary supabase
```

### Supabase Setup

1. **Create a Supabase project** at [supabase.com](https://supabase.com)

2. **Run the schema** in the Supabase SQL editor:
   ```bash
   cat schema.sql | pbcopy  # Copy to clipboard
   # Paste into Supabase SQL editor and run
   ```

3. **Set environment variables**:
   ```bash
   export SUPABASE_URL="https://your-project.supabase.co"
   export SUPABASE_KEY="your-anon-key"
   ```

### Neon Setup

1. **Create a Neon project** at [neon.tech](https://neon.tech)

2. **Get your connection string** from the Neon dashboard

3. **Run the schema**:
   ```bash
   psql "your-connection-string" < schema.sql
   ```

4. **Set environment variable**:
   ```bash
   export DATABASE_URL="your-connection-string"
   # or
   export NEON_DATABASE_URL="your-connection-string"
   ```

## Usage

### Basic Connection

```python
from synergy.database.db_connector import CognitiveSynergyDB

# Auto-detect connection (tries Supabase, then PostgreSQL)
db = CognitiveSynergyDB()

# Or specify connection type
db = CognitiveSynergyDB(connection_type="supabase")
db = CognitiveSynergyDB(connection_type="postgres")
```

### Component Management

```python
# Register a component
component_id = db.register_component(
    name="atomspace",
    component_type="knowledge_base",
    description="Hypergraph database",
    metadata={"version": "1.0"}
)

# Update component state
db.update_component_state(
    component_name="atomspace",
    status="active",
    load_factor=0.6,
    metrics={"atoms": 1000, "queries_per_sec": 50}
)
```

### Interaction Tracking

```python
# Record an interaction
db.record_interaction(
    source_component="atomspace",
    target_component="hyperon",
    interaction_type="data_transfer",
    data_transferred={"atoms": 100},
    latency_ms=15.5,
    success=True
)
```

### Synergy Metrics

```python
# Record synergy score
db.record_synergy_score(
    score=0.85,
    interaction_density=0.7,
    load_balance=0.9,
    activity_score=0.95,
    active_components=5,
    total_interactions=100,
    metadata={"calculation_method": "weighted_average"}
)

# Get synergy trend
trend = db.get_synergy_trend(hours=24)
for record in trend:
    print(f"{record['timestamp']}: {record['score']:.3f}")
```

### Hypergraph Operations

```python
# Create an atom
atom_id = db.create_atom(
    atom_type="ConceptNode",
    name="cat",
    content="A feline animal",
    truth_value={"strength": 0.9, "confidence": 0.8},
    source_component="atomspace",
    metadata={"category": "animal"}
)
```

### Enhanced Orchestrator

The enhanced orchestrator automatically persists all operations to the database:

```python
from synergy.core.synergy_orchestrator_enhanced import EnhancedSynergyOrchestrator

orchestrator = EnhancedSynergyOrchestrator()

# All operations are automatically persisted
orchestrator.register_component('atomspace', {'type': 'knowledge_base'})
orchestrator.update_component_state('atomspace', 'active', 0.6)
orchestrator.record_interaction('atomspace', 'hyperon')
score = orchestrator.calculate_synergy_score()

# Retrieve historical data
trend = orchestrator.get_synergy_trend(hours=24)
```

## Queries

### Get Active Components

```sql
SELECT * FROM active_components;
```

### Get Recent Interactions

```sql
SELECT * FROM recent_interactions
WHERE timestamp > NOW() - INTERVAL '1 hour';
```

### Get Synergy Trend

```sql
SELECT * FROM synergy_trend;
```

### Get Component Performance

```sql
SELECT * FROM component_performance
ORDER BY avg_quality_score DESC;
```

### Calculate Interaction Density

```sql
SELECT calculate_interaction_density(
    'component1_uuid'::UUID,
    'component2_uuid'::UUID,
    '1 hour'::INTERVAL
);
```

## Integration with Existing Systems

### AtomSpace Integration

The database can store a persistent copy of the AtomSpace hypergraph:

```python
# Export AtomSpace to database
for atom in atomspace.get_all_atoms():
    db.create_atom(
        atom_type=atom.type_name,
        name=atom.name,
        truth_value={"strength": atom.tv.mean, "confidence": atom.tv.confidence},
        source_component="atomspace"
    )
```

### Hyperon Integration

Track Hyperon reasoning operations:

```python
# Record Hyperon inference
db.record_interaction(
    source_component="hyperon",
    target_component="atomspace",
    interaction_type="inference",
    data_transferred={"rules_applied": 5, "conclusions": 3},
    latency_ms=120.5
)
```

### CogServer Integration

Monitor CogServer network activity:

```python
# Track CogServer connections
db.update_component_state(
    component_name="cogserver",
    status="active",
    load_factor=0.3,
    metrics={
        "active_connections": 5,
        "requests_per_sec": 20,
        "avg_response_time_ms": 45.2
    }
)
```

## Visualization

The database enables rich visualizations of cognitive synergy:

### Synergy Score Over Time

```python
import matplotlib.pyplot as plt

trend = db.get_synergy_trend(hours=24)
timestamps = [r['timestamp'] for r in trend]
scores = [r['score'] for r in trend]

plt.plot(timestamps, scores)
plt.xlabel('Time')
plt.ylabel('Synergy Score')
plt.title('Cognitive Synergy Over Time')
plt.show()
```

### Component Interaction Graph

```python
import networkx as nx

# Query interactions
interactions = db.get_recent_interactions(hours=1)

# Build graph
G = nx.DiGraph()
for interaction in interactions:
    G.add_edge(
        interaction['source_component'],
        interaction['target_component'],
        weight=interaction.get('latency_ms', 1)
    )

# Visualize
nx.draw(G, with_labels=True)
plt.show()
```

## Performance Considerations

### Indexing

The schema includes indexes on:
- Timestamp columns for time-series queries
- Component IDs for join operations
- Atom types and names for hypergraph queries

### Partitioning

For large-scale deployments, consider partitioning time-series tables:

```sql
-- Example: Partition synergy_scores by month
CREATE TABLE synergy_scores_2024_11 PARTITION OF synergy_scores
FOR VALUES FROM ('2024-11-01') TO ('2024-12-01');
```

### Caching

Use materialized views for frequently accessed aggregations:

```sql
CREATE MATERIALIZED VIEW synergy_summary AS
SELECT 
    DATE_TRUNC('hour', timestamp) AS hour,
    AVG(score) AS avg_score,
    MAX(score) AS max_score,
    MIN(score) AS min_score
FROM synergy_scores
GROUP BY DATE_TRUNC('hour', timestamp);

-- Refresh periodically
REFRESH MATERIALIZED VIEW synergy_summary;
```

## Future Enhancements

Planned improvements to the database integration:

1. **Real-time Subscriptions**: Use Supabase real-time features for live synergy monitoring
2. **Advanced Analytics**: Add graph algorithms for hypergraph analysis
3. **Machine Learning Integration**: Store model predictions and training data
4. **Multi-tenancy**: Support multiple cognitive systems in one database
5. **Backup and Recovery**: Automated backup strategies for critical data
6. **Data Retention Policies**: Automatic archival of old metrics

## Troubleshooting

### Connection Issues

If database connection fails:

1. Check environment variables are set correctly
2. Verify network connectivity to database
3. Check database credentials and permissions
4. Review firewall rules (especially for Neon)

### Performance Issues

If queries are slow:

1. Check if indexes are being used (`EXPLAIN ANALYZE`)
2. Consider adding more specific indexes
3. Use connection pooling for high-concurrency scenarios
4. Monitor database resource utilization

### Schema Updates

To update the schema:

1. Create a migration file in `migrations/`
2. Test on a development database first
3. Apply to production with proper backups
4. Update the schema version in the database

## Contributing

When adding new tables or features:

1. Update `schema.sql` with new definitions
2. Add corresponding methods to `db_connector.py`
3. Update this README with usage examples
4. Add tests for new functionality
5. Document any performance implications

## References

- [Supabase Documentation](https://supabase.com/docs)
- [Neon Documentation](https://neon.tech/docs)
- [PostgreSQL Documentation](https://www.postgresql.org/docs/)
- [Cognitive Synergy Principles](../README.md)
- [Deep Tree Echo Architecture](../../COGNITIVE_SYNERGY_ANALYSIS.md)

## License

This database integration is part of the OpenCog Collection and is licensed under GPL-3.0+.
