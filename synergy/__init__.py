"""
OpenCog Collection - Cognitive Synergy Framework

This package provides the core infrastructure for cognitive synergy
in the OpenCog Collection, including:

- Synergy Orchestrator: Coordinates inter-component communication
- Hypergraph Bridge: Seamless data flow between AtomSpace and Hyperon
- Cognitive Monitor: System-wide monitoring and introspection
- Integration Tests: Validates synergistic behaviors

For more information, see the documentation in synergy/docs/
"""

__version__ = "0.1.0"
__author__ = "OpenCog Collection Contributors"

# Import main components for easy access
try:
    from synergy.core.orchestrator import get_orchestrator, CognitiveSynergyOrchestrator
    from synergy.core.hypergraph_bridge import get_bridge, HypergraphBridge, Atom, AtomType
    from synergy.monitoring.cognitive_monitor import get_monitor, CognitiveMonitor, PerformanceTimer
    
    __all__ = [
        'get_orchestrator',
        'CognitiveSynergyOrchestrator',
        'get_bridge',
        'HypergraphBridge',
        'Atom',
        'AtomType',
        'get_monitor',
        'CognitiveMonitor',
        'PerformanceTimer'
    ]
except ImportError:
    # Modules may not be available during initial setup
    __all__ = []
