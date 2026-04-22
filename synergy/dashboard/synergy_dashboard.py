#!/usr/bin/env python3
"""
Real-Time Cognitive Synergy Dashboard

Web-based dashboard for monitoring cognitive synergy metrics in real-time.
Uses Flask for the backend and provides a REST API for metrics.
"""

import os
import sys
import json
import time
from pathlib import Path
from datetime import datetime, timedelta
from typing import Dict, List, Any

# Add parent directory to path
sys.path.insert(0, str(Path(__file__).parent.parent))

try:
    from flask import Flask, render_template_string, jsonify, request
    FLASK_AVAILABLE = True
except ImportError:
    FLASK_AVAILABLE = False
    print("Warning: Flask not available. Install with: pip install flask")

from database.db_connector import CognitiveSynergyDB

app = Flask(__name__)
db = CognitiveSynergyDB(connection_type="auto")

# HTML template for the dashboard
DASHBOARD_HTML = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Cognitive Synergy Dashboard</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: #fff;
            padding: 20px;
        }
        .container { max-width: 1400px; margin: 0 auto; }
        h1 {
            text-align: center;
            margin-bottom: 30px;
            font-size: 2.5em;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
        }
        .metrics-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }
        .metric-card {
            background: rgba(255, 255, 255, 0.1);
            backdrop-filter: blur(10px);
            border-radius: 15px;
            padding: 25px;
            box-shadow: 0 8px 32px 0 rgba(31, 38, 135, 0.37);
            border: 1px solid rgba(255, 255, 255, 0.18);
            transition: transform 0.3s ease;
        }
        .metric-card:hover { transform: translateY(-5px); }
        .metric-title {
            font-size: 0.9em;
            opacity: 0.8;
            margin-bottom: 10px;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        .metric-value {
            font-size: 2.5em;
            font-weight: bold;
            margin-bottom: 5px;
        }
        .metric-subtitle {
            font-size: 0.85em;
            opacity: 0.7;
        }
        .components-section {
            background: rgba(255, 255, 255, 0.1);
            backdrop-filter: blur(10px);
            border-radius: 15px;
            padding: 25px;
            margin-bottom: 30px;
        }
        .component-item {
            background: rgba(255, 255, 255, 0.05);
            padding: 15px;
            margin: 10px 0;
            border-radius: 10px;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
        .component-name {
            font-weight: bold;
            font-size: 1.1em;
        }
        .component-status {
            padding: 5px 15px;
            border-radius: 20px;
            font-size: 0.85em;
            font-weight: bold;
        }
        .status-active { background: #10b981; }
        .status-idle { background: #6b7280; }
        .status-processing { background: #f59e0b; }
        .status-error { background: #ef4444; }
        .load-bar {
            width: 200px;
            height: 8px;
            background: rgba(255, 255, 255, 0.2);
            border-radius: 4px;
            overflow: hidden;
            margin-left: 15px;
        }
        .load-fill {
            height: 100%;
            background: linear-gradient(90deg, #10b981, #3b82f6);
            transition: width 0.3s ease;
        }
        .refresh-info {
            text-align: center;
            opacity: 0.7;
            margin-top: 20px;
            font-size: 0.9em;
        }
        .chart-container {
            background: rgba(255, 255, 255, 0.1);
            backdrop-filter: blur(10px);
            border-radius: 15px;
            padding: 25px;
            margin-bottom: 30px;
            min-height: 300px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🧠 Cognitive Synergy Dashboard</h1>
        
        <div class="metrics-grid">
            <div class="metric-card">
                <div class="metric-title">Synergy Score</div>
                <div class="metric-value" id="synergy-score">--</div>
                <div class="metric-subtitle">Overall system coherence</div>
            </div>
            
            <div class="metric-card">
                <div class="metric-title">Active Components</div>
                <div class="metric-value" id="active-components">--</div>
                <div class="metric-subtitle">Currently processing</div>
            </div>
            
            <div class="metric-card">
                <div class="metric-title">Interaction Density</div>
                <div class="metric-value" id="interaction-density">--</div>
                <div class="metric-subtitle">Cross-component communication</div>
            </div>
            
            <div class="metric-card">
                <div class="metric-title">Load Balance</div>
                <div class="metric-value" id="load-balance">--</div>
                <div class="metric-subtitle">Resource distribution</div>
            </div>
        </div>
        
        <div class="components-section">
            <h2 style="margin-bottom: 20px;">Component Status</h2>
            <div id="components-list">
                <p style="opacity: 0.7;">Loading components...</p>
            </div>
        </div>
        
        <div class="chart-container">
            <h2 style="margin-bottom: 20px;">Synergy Trend (Last 24 Hours)</h2>
            <div id="synergy-chart">
                <p style="opacity: 0.7;">Chart visualization coming soon...</p>
            </div>
        </div>
        
        <div class="refresh-info">
            Auto-refreshing every 5 seconds | Last update: <span id="last-update">--</span>
        </div>
    </div>
    
    <script>
        async function fetchMetrics() {
            try {
                const response = await fetch('/api/metrics');
                const data = await response.json();
                
                // Update metrics
                document.getElementById('synergy-score').textContent = 
                    data.synergy_score ? data.synergy_score.toFixed(3) : '--';
                document.getElementById('active-components').textContent = 
                    data.active_components || '--';
                document.getElementById('interaction-density').textContent = 
                    data.interaction_density ? data.interaction_density.toFixed(3) : '--';
                document.getElementById('load-balance').textContent = 
                    data.load_balance ? data.load_balance.toFixed(3) : '--';
                
                // Update components list
                if (data.components && data.components.length > 0) {
                    const componentsHtml = data.components.map(comp => `
                        <div class="component-item">
                            <div>
                                <div class="component-name">${comp.name}</div>
                                <div style="font-size: 0.85em; opacity: 0.7; margin-top: 5px;">
                                    ${comp.component_type || 'Unknown type'}
                                </div>
                            </div>
                            <div style="display: flex; align-items: center;">
                                <span class="component-status status-${comp.status}">
                                    ${comp.status.toUpperCase()}
                                </span>
                                <div class="load-bar">
                                    <div class="load-fill" style="width: ${(comp.load_factor || 0) * 100}%"></div>
                                </div>
                            </div>
                        </div>
                    `).join('');
                    document.getElementById('components-list').innerHTML = componentsHtml;
                } else {
                    document.getElementById('components-list').innerHTML = 
                        '<p style="opacity: 0.7;">No components registered yet</p>';
                }
                
                // Update timestamp
                document.getElementById('last-update').textContent = 
                    new Date().toLocaleTimeString();
                    
            } catch (error) {
                console.error('Error fetching metrics:', error);
            }
        }
        
        // Initial fetch
        fetchMetrics();
        
        // Auto-refresh every 5 seconds
        setInterval(fetchMetrics, 5000);
    </script>
</body>
</html>
"""


@app.route('/')
def dashboard():
    """Render the main dashboard."""
    return render_template_string(DASHBOARD_HTML)


@app.route('/api/metrics')
def get_metrics():
    """Get current synergy metrics."""
    try:
        # Get latest synergy score
        latest_score = db.get_latest_synergy_score() if db.is_connected() else None
        
        # Get component states
        components = db.get_all_components() if db.is_connected() else []
        
        # Calculate metrics
        active_components = sum(1 for c in components if c.get('status') == 'active')
        
        return jsonify({
            'synergy_score': latest_score.get('score') if latest_score else 0.0,
            'interaction_density': latest_score.get('interaction_density') if latest_score else 0.0,
            'load_balance': latest_score.get('load_balance') if latest_score else 0.0,
            'activity_score': latest_score.get('activity_score') if latest_score else 0.0,
            'active_components': active_components,
            'total_components': len(components),
            'components': components,
            'timestamp': datetime.now().isoformat()
        })
    except Exception as e:
        return jsonify({
            'error': str(e),
            'synergy_score': 0.0,
            'active_components': 0,
            'components': []
        }), 500


@app.route('/api/trend')
def get_trend():
    """Get synergy trend data."""
    try:
        hours = int(request.args.get('hours', 24))
        trend_data = db.get_synergy_trend(hours) if db.is_connected() else []
        
        return jsonify({
            'trend': trend_data,
            'hours': hours
        })
    except Exception as e:
        return jsonify({'error': str(e), 'trend': []}), 500


@app.route('/api/component/<component_name>')
def get_component_performance(component_name):
    """Get performance metrics for a specific component."""
    try:
        hours = int(request.args.get('hours', 24))
        performance = db.get_component_performance(component_name, hours) if db.is_connected() else {}
        
        return jsonify({
            'component': component_name,
            'performance': performance,
            'hours': hours
        })
    except Exception as e:
        return jsonify({'error': str(e), 'performance': {}}), 500


def main():
    """Run the dashboard server."""
    if not FLASK_AVAILABLE:
        print("Error: Flask is required to run the dashboard")
        print("Install with: pip install flask")
        return
    
    print("=" * 60)
    print("🧠 Cognitive Synergy Dashboard")
    print("=" * 60)
    print(f"Database connected: {db.is_connected()}")
    print(f"Starting server on http://localhost:5000")
    print("Press Ctrl+C to stop")
    print("=" * 60)
    
    app.run(host='0.0.0.0', port=5000, debug=False)


if __name__ == '__main__':
    main()
