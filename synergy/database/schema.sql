-- Cognitive Synergy Database Schema
-- For tracking hypergraph dynamics, component interactions, and synergy evolution
-- Compatible with PostgreSQL (Supabase/Neon)

-- ============================================================================
-- CORE TABLES: Components and State
-- ============================================================================

-- Component registry: Track all cognitive components in the system
CREATE TABLE IF NOT EXISTS components (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    name VARCHAR(255) UNIQUE NOT NULL,
    component_type VARCHAR(100) NOT NULL,
    description TEXT,
    metadata JSONB DEFAULT '{}',
    status VARCHAR(50) DEFAULT 'inactive',
    created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW(),
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);

-- Component state snapshots: Track component state over time
CREATE TABLE IF NOT EXISTS component_states (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    component_id UUID REFERENCES components(id) ON DELETE CASCADE,
    status VARCHAR(50) NOT NULL,
    load_factor FLOAT CHECK (load_factor >= 0 AND load_factor <= 1),
    metrics JSONB DEFAULT '{}',
    timestamp TIMESTAMP WITH TIME ZONE DEFAULT NOW(),
    CONSTRAINT fk_component FOREIGN KEY (component_id) REFERENCES components(id)
);

-- Index for time-series queries
CREATE INDEX IF NOT EXISTS idx_component_states_timestamp ON component_states(timestamp DESC);
CREATE INDEX IF NOT EXISTS idx_component_states_component ON component_states(component_id, timestamp DESC);

-- ============================================================================
-- HYPERGRAPH TABLES: Atoms and Relationships
-- ============================================================================

-- Atoms: Core hypergraph nodes
CREATE TABLE IF NOT EXISTS atoms (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    atom_type VARCHAR(100) NOT NULL,
    name VARCHAR(255),
    content TEXT,
    truth_value JSONB DEFAULT '{"strength": 1.0, "confidence": 1.0}',
    metadata JSONB DEFAULT '{}',
    source_component UUID REFERENCES components(id),
    created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW(),
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);

-- Hyperedges: Relationships between atoms (can connect multiple atoms)
CREATE TABLE IF NOT EXISTS hyperedges (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    edge_type VARCHAR(100) NOT NULL,
    source_atom_id UUID REFERENCES atoms(id) ON DELETE CASCADE,
    target_atom_id UUID REFERENCES atoms(id) ON DELETE CASCADE,
    truth_value JSONB DEFAULT '{"strength": 1.0, "confidence": 1.0}',
    metadata JSONB DEFAULT '{}',
    created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);

-- Hyperedge members: For hyperedges connecting more than 2 atoms
CREATE TABLE IF NOT EXISTS hyperedge_members (
    hyperedge_id UUID REFERENCES hyperedges(id) ON DELETE CASCADE,
    atom_id UUID REFERENCES atoms(id) ON DELETE CASCADE,
    position INTEGER NOT NULL,
    PRIMARY KEY (hyperedge_id, atom_id, position)
);

-- Indexes for hypergraph queries
CREATE INDEX IF NOT EXISTS idx_atoms_type ON atoms(atom_type);
CREATE INDEX IF NOT EXISTS idx_atoms_name ON atoms(name);
CREATE INDEX IF NOT EXISTS idx_atoms_source ON atoms(source_component);
CREATE INDEX IF NOT EXISTS idx_hyperedges_type ON hyperedges(edge_type);
CREATE INDEX IF NOT EXISTS idx_hyperedges_source ON hyperedges(source_atom_id);
CREATE INDEX IF NOT EXISTS idx_hyperedges_target ON hyperedges(target_atom_id);

-- ============================================================================
-- INTERACTION TABLES: Component Communication
-- ============================================================================

-- Component interactions: Track communication between components
CREATE TABLE IF NOT EXISTS interactions (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    source_component_id UUID REFERENCES components(id) ON DELETE CASCADE,
    target_component_id UUID REFERENCES components(id) ON DELETE CASCADE,
    interaction_type VARCHAR(100) NOT NULL,
    data_transferred JSONB DEFAULT '{}',
    latency_ms FLOAT,
    success BOOLEAN DEFAULT true,
    timestamp TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);

-- Index for interaction analysis
CREATE INDEX IF NOT EXISTS idx_interactions_timestamp ON interactions(timestamp DESC);
CREATE INDEX IF NOT EXISTS idx_interactions_source ON interactions(source_component_id, timestamp DESC);
CREATE INDEX IF NOT EXISTS idx_interactions_target ON interactions(target_component_id, timestamp DESC);

-- ============================================================================
-- SYNERGY METRICS TABLES: Performance and Health
-- ============================================================================

-- Synergy scores: Track overall system synergy over time
CREATE TABLE IF NOT EXISTS synergy_scores (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    score FLOAT CHECK (score >= 0 AND score <= 1) NOT NULL,
    interaction_density FLOAT CHECK (interaction_density >= 0 AND interaction_density <= 1),
    load_balance FLOAT CHECK (load_balance >= 0 AND load_balance <= 1),
    activity_score FLOAT CHECK (activity_score >= 0 AND activity_score <= 1),
    active_components INTEGER,
    total_interactions INTEGER,
    metadata JSONB DEFAULT '{}',
    timestamp TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_synergy_scores_timestamp ON synergy_scores(timestamp DESC);

-- Cognitive tasks: Track individual cognitive operations
CREATE TABLE IF NOT EXISTS cognitive_tasks (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    component_id UUID REFERENCES components(id) ON DELETE CASCADE,
    task_type VARCHAR(100) NOT NULL,
    duration_ms FLOAT NOT NULL,
    success BOOLEAN NOT NULL,
    quality_score FLOAT CHECK (quality_score >= 0 AND quality_score <= 1),
    input_data JSONB DEFAULT '{}',
    output_data JSONB DEFAULT '{}',
    error_message TEXT,
    timestamp TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_cognitive_tasks_component ON cognitive_tasks(component_id, timestamp DESC);
CREATE INDEX IF NOT EXISTS idx_cognitive_tasks_type ON cognitive_tasks(task_type, timestamp DESC);

-- Resource utilization: System-wide resource tracking
CREATE TABLE IF NOT EXISTS resource_snapshots (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    cpu_percent FLOAT,
    memory_percent FLOAT,
    memory_used_mb FLOAT,
    memory_available_mb FLOAT,
    disk_percent FLOAT,
    network_sent_mb FLOAT,
    network_recv_mb FLOAT,
    timestamp TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_resource_snapshots_timestamp ON resource_snapshots(timestamp DESC);

-- ============================================================================
-- IDENTITY REFINEMENT TABLES: Self-Awareness and Evolution
-- ============================================================================

-- Identity tuples: Track identity refinement from conversations
CREATE TABLE IF NOT EXISTS identity_tuples (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    subject VARCHAR(255) NOT NULL,
    predicate VARCHAR(255) NOT NULL,
    object TEXT NOT NULL,
    confidence FLOAT CHECK (confidence >= 0 AND confidence <= 1) DEFAULT 1.0,
    source VARCHAR(100),
    context JSONB DEFAULT '{}',
    created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);

-- Identity evolution: Track how identity changes over time
CREATE TABLE IF NOT EXISTS identity_snapshots (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    identity_vector JSONB NOT NULL,
    core_attributes JSONB DEFAULT '{}',
    capabilities JSONB DEFAULT '{}',
    goals JSONB DEFAULT '{}',
    timestamp TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_identity_tuples_subject ON identity_tuples(subject);
CREATE INDEX IF NOT EXISTS idx_identity_snapshots_timestamp ON identity_snapshots(timestamp DESC);

-- ============================================================================
-- VIEWS: Convenient queries for common operations
-- ============================================================================

-- Active components view
CREATE OR REPLACE VIEW active_components AS
SELECT 
    c.id,
    c.name,
    c.component_type,
    cs.status,
    cs.load_factor,
    cs.timestamp
FROM components c
LEFT JOIN LATERAL (
    SELECT status, load_factor, timestamp
    FROM component_states
    WHERE component_id = c.id
    ORDER BY timestamp DESC
    LIMIT 1
) cs ON true
WHERE cs.status = 'active' OR cs.status IS NULL;

-- Recent interactions view
CREATE OR REPLACE VIEW recent_interactions AS
SELECT 
    i.id,
    sc.name AS source_component,
    tc.name AS target_component,
    i.interaction_type,
    i.latency_ms,
    i.success,
    i.timestamp
FROM interactions i
JOIN components sc ON i.source_component_id = sc.id
JOIN components tc ON i.target_component_id = tc.id
WHERE i.timestamp > NOW() - INTERVAL '1 hour'
ORDER BY i.timestamp DESC;

-- Synergy trend view (last 24 hours)
CREATE OR REPLACE VIEW synergy_trend AS
SELECT 
    DATE_TRUNC('hour', timestamp) AS hour,
    AVG(score) AS avg_score,
    AVG(interaction_density) AS avg_interaction_density,
    AVG(load_balance) AS avg_load_balance,
    AVG(activity_score) AS avg_activity_score,
    COUNT(*) AS sample_count
FROM synergy_scores
WHERE timestamp > NOW() - INTERVAL '24 hours'
GROUP BY DATE_TRUNC('hour', timestamp)
ORDER BY hour DESC;

-- Component performance view
CREATE OR REPLACE VIEW component_performance AS
SELECT 
    c.name,
    c.component_type,
    COUNT(ct.id) AS total_tasks,
    SUM(CASE WHEN ct.success THEN 1 ELSE 0 END) AS successful_tasks,
    AVG(ct.duration_ms) AS avg_duration_ms,
    AVG(ct.quality_score) AS avg_quality_score,
    MAX(ct.timestamp) AS last_task_time
FROM components c
LEFT JOIN cognitive_tasks ct ON c.id = ct.component_id
WHERE ct.timestamp > NOW() - INTERVAL '24 hours' OR ct.timestamp IS NULL
GROUP BY c.id, c.name, c.component_type;

-- ============================================================================
-- FUNCTIONS: Utility functions for synergy analysis
-- ============================================================================

-- Calculate interaction density between two components
CREATE OR REPLACE FUNCTION calculate_interaction_density(
    component1_id UUID,
    component2_id UUID,
    time_window INTERVAL DEFAULT '1 hour'
)
RETURNS FLOAT AS $$
DECLARE
    interaction_count INTEGER;
    max_possible INTEGER;
BEGIN
    SELECT COUNT(*) INTO interaction_count
    FROM interactions
    WHERE (source_component_id = component1_id AND target_component_id = component2_id)
       OR (source_component_id = component2_id AND target_component_id = component1_id)
       AND timestamp > NOW() - time_window;
    
    -- Assume max possible is 1 interaction per second
    max_possible := EXTRACT(EPOCH FROM time_window)::INTEGER;
    
    RETURN LEAST(interaction_count::FLOAT / max_possible, 1.0);
END;
$$ LANGUAGE plpgsql;

-- Get latest synergy score
CREATE OR REPLACE FUNCTION get_latest_synergy_score()
RETURNS FLOAT AS $$
DECLARE
    latest_score FLOAT;
BEGIN
    SELECT score INTO latest_score
    FROM synergy_scores
    ORDER BY timestamp DESC
    LIMIT 1;
    
    RETURN COALESCE(latest_score, 0.0);
END;
$$ LANGUAGE plpgsql;

-- ============================================================================
-- TRIGGERS: Automatic timestamp updates
-- ============================================================================

CREATE OR REPLACE FUNCTION update_updated_at_column()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_at = NOW();
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER update_components_updated_at
    BEFORE UPDATE ON components
    FOR EACH ROW
    EXECUTE FUNCTION update_updated_at_column();

CREATE TRIGGER update_atoms_updated_at
    BEFORE UPDATE ON atoms
    FOR EACH ROW
    EXECUTE FUNCTION update_updated_at_column();

-- ============================================================================
-- INITIAL DATA: Seed the database with core components
-- ============================================================================

INSERT INTO components (name, component_type, description, status) VALUES
    ('atomspace', 'knowledge_base', 'Hypergraph knowledge representation system', 'active'),
    ('hyperon', 'reasoning_engine', 'Rust-based cognitive computing framework', 'active'),
    ('cogserver', 'communication_layer', 'Network server for cognitive operations', 'active'),
    ('matrix', 'data_processing', 'Sparse vector and graph processing', 'active'),
    ('learn', 'learning_system', 'Symbolic learning algorithms', 'active'),
    ('agents', 'agent_system', 'Interactive cognitive agents', 'active'),
    ('sensory', 'dataflow_system', 'External world interaction system', 'active')
ON CONFLICT (name) DO NOTHING;

-- ============================================================================
-- COMMENTS: Documentation for tables and columns
-- ============================================================================

COMMENT ON TABLE components IS 'Registry of all cognitive components in the system';
COMMENT ON TABLE component_states IS 'Time-series data of component states and metrics';
COMMENT ON TABLE atoms IS 'Core hypergraph nodes representing concepts and data';
COMMENT ON TABLE hyperedges IS 'Relationships between atoms in the hypergraph';
COMMENT ON TABLE interactions IS 'Communication events between components';
COMMENT ON TABLE synergy_scores IS 'Overall system synergy metrics over time';
COMMENT ON TABLE cognitive_tasks IS 'Individual cognitive operations performed by components';
COMMENT ON TABLE resource_snapshots IS 'System resource utilization over time';
COMMENT ON TABLE identity_tuples IS 'Identity refinement data from conversations';
COMMENT ON TABLE identity_snapshots IS 'Snapshots of system identity evolution';
