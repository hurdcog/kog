#!/usr/bin/env python3
"""
Cognitive Monitoring Dashboard Server

Real-time visualization and monitoring of AGI-OS cognitive state,
including AtomSpace visualization, PLN inference tracking, and
ECAN attention allocation monitoring.

Author: OCC Development Team
Date: 2025-11-30
Version: 1.0.0
"""

import json
import time
import asyncio
from datetime import datetime
from typing import Dict, List, Any
from pathlib import Path

from flask import Flask, render_template, jsonify, request
from flask_cors import CORS


app = Flask(__name__)
CORS(app)

# Global state
cognitive_state = {
    "atomspace": {
        "total_atoms": 0,
        "atom_types": {},
        "recent_atoms": []
    },
    "inference": {
        "active_inferences": 0,
        "completed_inferences": 0,
        "avg_inference_time_ms": 0,
        "recent_inferences": []
    },
    "attention": {
        "sti_distribution": [],
        "lti_distribution": [],
        "attentional_focus": []
    },
    "learning": {
        "active_learners": 0,
        "patterns_learned": 0,
        "learning_rate": 0.0
    },
    "system": {
        "uptime_seconds": 0,
        "cpu_usage": 0.0,
        "memory_usage_mb": 0,
        "kernel_status": "unknown"
    },
    "hurdcog": {
        "connected": False,
        "kernel_version": "unknown",
        "cognitive_ops_per_sec": 0,
        "last_sync": None
    }
}


@app.route('/')
def index():
    """Main dashboard page"""
    return render_template('dashboard.html')


@app.route('/api/status')
def get_status():
    """Get overall system status"""
    return jsonify({
        "status": "operational" if cognitive_state["hurdcog"]["connected"] else "degraded",
        "timestamp": datetime.now().isoformat(),
        "components": {
            "atomspace": "healthy",
            "hurdcog_bridge": "connected" if cognitive_state["hurdcog"]["connected"] else "disconnected",
            "inference_engine": "active",
            "attention_allocation": "active"
        }
    })


@app.route('/api/cognitive-state')
def get_cognitive_state():
    """Get complete cognitive state"""
    return jsonify(cognitive_state)


@app.route('/api/atomspace/stats')
def get_atomspace_stats():
    """Get AtomSpace statistics"""
    return jsonify(cognitive_state["atomspace"])


@app.route('/api/atomspace/atoms')
def get_atoms():
    """Get atoms from AtomSpace"""
    limit = request.args.get('limit', 100, type=int)
    atom_type = request.args.get('type', None)
    
    # TODO: Integrate with actual AtomSpace
    # For now, return simulated data
    atoms = generate_sample_atoms(limit, atom_type)
    
    return jsonify({
        "atoms": atoms,
        "total": len(atoms),
        "timestamp": datetime.now().isoformat()
    })


@app.route('/api/atomspace/graph')
def get_atomspace_graph():
    """Get AtomSpace as graph data for visualization"""
    # TODO: Integrate with actual AtomSpace
    # For now, return simulated graph
    graph = generate_sample_graph()
    
    return jsonify(graph)


@app.route('/api/inference/recent')
def get_recent_inferences():
    """Get recent PLN inferences"""
    limit = request.args.get('limit', 20, type=int)
    
    return jsonify({
        "inferences": cognitive_state["inference"]["recent_inferences"][:limit],
        "total": len(cognitive_state["inference"]["recent_inferences"])
    })


@app.route('/api/inference/run', methods=['POST'])
def run_inference():
    """Run a PLN inference query"""
    data = request.get_json()
    query = data.get('query', '')
    
    if not query:
        return jsonify({"error": "No query provided"}), 400
    
    # TODO: Integrate with actual PLN engine
    # For now, simulate inference
    result = simulate_inference(query)
    
    # Update state
    cognitive_state["inference"]["completed_inferences"] += 1
    cognitive_state["inference"]["recent_inferences"].insert(0, result)
    if len(cognitive_state["inference"]["recent_inferences"]) > 100:
        cognitive_state["inference"]["recent_inferences"].pop()
    
    return jsonify(result)


@app.route('/api/attention/distribution')
def get_attention_distribution():
    """Get ECAN attention value distribution"""
    return jsonify({
        "sti": cognitive_state["attention"]["sti_distribution"],
        "lti": cognitive_state["attention"]["lti_distribution"],
        "focus": cognitive_state["attention"]["attentional_focus"]
    })


@app.route('/api/learning/stats')
def get_learning_stats():
    """Get learning statistics"""
    return jsonify(cognitive_state["learning"])


@app.route('/api/hurdcog/status')
def get_hurdcog_status():
    """Get HurdCog kernel status"""
    return jsonify(cognitive_state["hurdcog"])


@app.route('/api/system/metrics')
def get_system_metrics():
    """Get system performance metrics"""
    return jsonify(cognitive_state["system"])


# Helper functions

def generate_sample_atoms(limit: int, atom_type: str = None) -> List[Dict[str, Any]]:
    """Generate sample atoms for demonstration"""
    atoms = []
    types = ['ConceptNode', 'PredicateNode', 'InheritanceLink', 'SimilarityLink']
    
    for i in range(min(limit, 50)):
        atype = atom_type if atom_type else types[i % len(types)]
        atoms.append({
            "id": f"atom_{i}",
            "type": atype,
            "name": f"Sample{atype}_{i}",
            "tv": {
                "strength": 0.5 + (i % 5) * 0.1,
                "confidence": 0.7 + (i % 3) * 0.1
            },
            "av": {
                "sti": 50 + (i % 10) * 10,
                "lti": 100 + (i % 5) * 20
            }
        })
    
    return atoms


def generate_sample_graph() -> Dict[str, Any]:
    """Generate sample graph for visualization"""
    nodes = []
    links = []
    
    # Create sample nodes
    for i in range(20):
        nodes.append({
            "id": f"node_{i}",
            "label": f"Concept_{i}",
            "type": "ConceptNode" if i % 2 == 0 else "PredicateNode",
            "sti": 50 + i * 5,
            "size": 10 + i
        })
    
    # Create sample links
    for i in range(15):
        source = i
        target = (i + 3) % 20
        links.append({
            "source": f"node_{source}",
            "target": f"node_{target}",
            "type": "InheritanceLink" if i % 2 == 0 else "SimilarityLink",
            "strength": 0.5 + (i % 5) * 0.1
        })
    
    return {
        "nodes": nodes,
        "links": links
    }


def simulate_inference(query: str) -> Dict[str, Any]:
    """Simulate PLN inference"""
    return {
        "id": f"inf_{int(time.time() * 1000)}",
        "query": query,
        "result": "(ConceptNode \"InferenceResult\" (stv 0.8 0.9))",
        "confidence": 0.85,
        "processing_time_ms": 150 + (hash(query) % 100),
        "timestamp": datetime.now().isoformat(),
        "rules_applied": ["DeductionRule", "InductionRule"],
        "steps": 5
    }


def update_cognitive_state():
    """Periodically update cognitive state (background task)"""
    # TODO: Integrate with actual AtomSpace and HurdCog
    # For now, simulate updates
    
    cognitive_state["atomspace"]["total_atoms"] = 1000 + int(time.time()) % 100
    cognitive_state["system"]["uptime_seconds"] = int(time.time())
    cognitive_state["system"]["cpu_usage"] = 25.0 + (int(time.time()) % 30)
    cognitive_state["system"]["memory_usage_mb"] = 512 + (int(time.time()) % 256)
    
    # Simulate HurdCog connection
    cognitive_state["hurdcog"]["connected"] = True
    cognitive_state["hurdcog"]["kernel_version"] = "HurdCog 0.1.0"
    cognitive_state["hurdcog"]["cognitive_ops_per_sec"] = 100 + (int(time.time()) % 50)
    cognitive_state["hurdcog"]["last_sync"] = datetime.now().isoformat()


if __name__ == '__main__':
    print("=" * 60)
    print("AGI-OS Cognitive Monitoring Dashboard")
    print("=" * 60)
    print(f"Starting server at http://localhost:5000")
    print("Press Ctrl+C to stop")
    print("=" * 60)
    
    # Update state periodically
    import threading
    def background_update():
        while True:
            update_cognitive_state()
            time.sleep(2)
    
    update_thread = threading.Thread(target=background_update, daemon=True)
    update_thread.start()
    
    # Run Flask app
    app.run(host='0.0.0.0', port=5000, debug=True, use_reloader=False)
