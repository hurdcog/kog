#!/usr/bin/env python3
"""
Cognitive Monitor

System-wide monitoring and introspection for the OpenCog Collection.

Implements the Introspection Membrane from Deep Tree Echo architecture,
enabling the system to observe and analyze its own cognitive state.
"""

import psutil
import time
import json
import logging
from typing import Dict, List, Any, Optional
from dataclasses import dataclass, asdict
from datetime import datetime
from pathlib import Path
from collections import deque


logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


@dataclass
class ResourceSnapshot:
    """Snapshot of system resource utilization."""
    timestamp: float
    cpu_percent: float
    memory_percent: float
    memory_used_mb: float
    memory_available_mb: float
    disk_io_read_mb: float
    disk_io_write_mb: float
    network_sent_mb: float
    network_recv_mb: float
    
    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)


@dataclass
class CognitiveMetrics:
    """Metrics for cognitive performance."""
    timestamp: float
    component: str
    task_type: str
    duration_ms: float
    success: bool
    throughput: Optional[float] = None
    error_rate: Optional[float] = None
    quality_score: Optional[float] = None
    
    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)


class CognitiveMonitor:
    """
    Monitor for cognitive system performance and health.
    
    Provides real-time monitoring, historical tracking, and
    introspection capabilities for the cognitive architecture.
    """
    
    def __init__(self, history_size: int = 1000):
        self.history_size = history_size
        self.resource_history: deque = deque(maxlen=history_size)
        self.cognitive_history: deque = deque(maxlen=history_size)
        self.component_metrics: Dict[str, List[CognitiveMetrics]] = {}
        self.alerts: List[Dict[str, Any]] = []
        self.start_time = time.time()
        
        # Baseline measurements
        self.baseline_cpu = psutil.cpu_percent(interval=1)
        self.baseline_memory = psutil.virtual_memory().percent
        
        # Disk I/O baseline
        self.disk_io_start = psutil.disk_io_counters()
        self.network_io_start = psutil.net_io_counters()
        
        logger.info("Cognitive Monitor initialized")
    
    def capture_resource_snapshot(self) -> ResourceSnapshot:
        """Capture current resource utilization."""
        memory = psutil.virtual_memory()
        disk_io = psutil.disk_io_counters()
        network_io = psutil.net_io_counters()
        
        # Calculate deltas from baseline
        disk_read_mb = (disk_io.read_bytes - self.disk_io_start.read_bytes) / (1024 * 1024)
        disk_write_mb = (disk_io.write_bytes - self.disk_io_start.write_bytes) / (1024 * 1024)
        network_sent_mb = (network_io.bytes_sent - self.network_io_start.bytes_sent) / (1024 * 1024)
        network_recv_mb = (network_io.bytes_recv - self.network_io_start.bytes_recv) / (1024 * 1024)
        
        snapshot = ResourceSnapshot(
            timestamp=time.time(),
            cpu_percent=psutil.cpu_percent(interval=0.1),
            memory_percent=memory.percent,
            memory_used_mb=memory.used / (1024 * 1024),
            memory_available_mb=memory.available / (1024 * 1024),
            disk_io_read_mb=disk_read_mb,
            disk_io_write_mb=disk_write_mb,
            network_sent_mb=network_sent_mb,
            network_recv_mb=network_recv_mb
        )
        
        self.resource_history.append(snapshot)
        self._check_resource_alerts(snapshot)
        
        return snapshot
    
    def record_cognitive_task(self, component: str, task_type: str, duration_ms: float,
                             success: bool, **kwargs) -> CognitiveMetrics:
        """Record a cognitive task execution."""
        metrics = CognitiveMetrics(
            timestamp=time.time(),
            component=component,
            task_type=task_type,
            duration_ms=duration_ms,
            success=success,
            throughput=kwargs.get('throughput'),
            error_rate=kwargs.get('error_rate'),
            quality_score=kwargs.get('quality_score')
        )
        
        self.cognitive_history.append(metrics)
        
        if component not in self.component_metrics:
            self.component_metrics[component] = []
        self.component_metrics[component].append(metrics)
        
        return metrics
    
    def get_resource_summary(self, window_seconds: int = 60) -> Dict[str, Any]:
        """Get summary of resource utilization over a time window."""
        cutoff_time = time.time() - window_seconds
        recent_snapshots = [s for s in self.resource_history if s.timestamp >= cutoff_time]
        
        if not recent_snapshots:
            return {}
        
        return {
            'window_seconds': window_seconds,
            'samples': len(recent_snapshots),
            'cpu': {
                'avg': sum(s.cpu_percent for s in recent_snapshots) / len(recent_snapshots),
                'max': max(s.cpu_percent for s in recent_snapshots),
                'min': min(s.cpu_percent for s in recent_snapshots)
            },
            'memory': {
                'avg_percent': sum(s.memory_percent for s in recent_snapshots) / len(recent_snapshots),
                'avg_used_mb': sum(s.memory_used_mb for s in recent_snapshots) / len(recent_snapshots),
                'max_used_mb': max(s.memory_used_mb for s in recent_snapshots)
            },
            'disk_io': {
                'total_read_mb': recent_snapshots[-1].disk_io_read_mb if recent_snapshots else 0,
                'total_write_mb': recent_snapshots[-1].disk_io_write_mb if recent_snapshots else 0
            },
            'network': {
                'total_sent_mb': recent_snapshots[-1].network_sent_mb if recent_snapshots else 0,
                'total_recv_mb': recent_snapshots[-1].network_recv_mb if recent_snapshots else 0
            }
        }
    
    def get_cognitive_summary(self, component: Optional[str] = None, window_seconds: int = 60) -> Dict[str, Any]:
        """Get summary of cognitive performance over a time window."""
        cutoff_time = time.time() - window_seconds
        
        if component:
            recent_metrics = [m for m in self.component_metrics.get(component, []) if m.timestamp >= cutoff_time]
        else:
            recent_metrics = [m for m in self.cognitive_history if m.timestamp >= cutoff_time]
        
        if not recent_metrics:
            return {}
        
        successful = [m for m in recent_metrics if m.success]
        failed = [m for m in recent_metrics if not m.success]
        
        summary = {
            'window_seconds': window_seconds,
            'total_tasks': len(recent_metrics),
            'successful_tasks': len(successful),
            'failed_tasks': len(failed),
            'success_rate': len(successful) / len(recent_metrics) if recent_metrics else 0,
            'avg_duration_ms': sum(m.duration_ms for m in recent_metrics) / len(recent_metrics),
            'max_duration_ms': max(m.duration_ms for m in recent_metrics),
            'min_duration_ms': min(m.duration_ms for m in recent_metrics)
        }
        
        # Add quality metrics if available
        quality_scores = [m.quality_score for m in recent_metrics if m.quality_score is not None]
        if quality_scores:
            summary['avg_quality_score'] = sum(quality_scores) / len(quality_scores)
        
        # Task type breakdown
        task_types = {}
        for m in recent_metrics:
            if m.task_type not in task_types:
                task_types[m.task_type] = {'count': 0, 'avg_duration': 0, 'success_rate': 0}
            task_types[m.task_type]['count'] += 1
        
        for task_type in task_types:
            type_metrics = [m for m in recent_metrics if m.task_type == task_type]
            task_types[task_type]['avg_duration'] = sum(m.duration_ms for m in type_metrics) / len(type_metrics)
            task_types[task_type]['success_rate'] = sum(1 for m in type_metrics if m.success) / len(type_metrics)
        
        summary['task_types'] = task_types
        
        return summary
    
    def get_component_health(self, component: str) -> Dict[str, Any]:
        """Get health status of a specific component."""
        if component not in self.component_metrics:
            return {'status': 'unknown', 'message': 'No metrics available'}
        
        recent_metrics = self.component_metrics[component][-100:]  # Last 100 tasks
        
        if not recent_metrics:
            return {'status': 'idle', 'message': 'No recent activity'}
        
        success_rate = sum(1 for m in recent_metrics if m.success) / len(recent_metrics)
        avg_duration = sum(m.duration_ms for m in recent_metrics) / len(recent_metrics)
        
        # Determine health status
        if success_rate >= 0.95 and avg_duration < 1000:
            status = 'healthy'
        elif success_rate >= 0.8 and avg_duration < 5000:
            status = 'degraded'
        else:
            status = 'unhealthy'
        
        return {
            'status': status,
            'success_rate': success_rate,
            'avg_duration_ms': avg_duration,
            'recent_tasks': len(recent_metrics),
            'last_activity': datetime.fromtimestamp(recent_metrics[-1].timestamp).isoformat()
        }
    
    def get_system_health(self) -> Dict[str, Any]:
        """Get overall system health."""
        resource_summary = self.get_resource_summary(window_seconds=60)
        cognitive_summary = self.get_cognitive_summary(window_seconds=60)
        
        # Determine overall health
        health_score = 1.0
        
        if resource_summary:
            if resource_summary['cpu']['avg'] > 90:
                health_score *= 0.7
            if resource_summary['memory']['avg_percent'] > 90:
                health_score *= 0.7
        
        if cognitive_summary:
            health_score *= cognitive_summary.get('success_rate', 1.0)
        
        if health_score >= 0.9:
            status = 'healthy'
        elif health_score >= 0.7:
            status = 'degraded'
        else:
            status = 'unhealthy'
        
        return {
            'status': status,
            'health_score': health_score,
            'uptime_seconds': time.time() - self.start_time,
            'resource_summary': resource_summary,
            'cognitive_summary': cognitive_summary,
            'component_health': {
                comp: self.get_component_health(comp)
                for comp in self.component_metrics.keys()
            },
            'active_alerts': len(self.alerts)
        }
    
    def _check_resource_alerts(self, snapshot: ResourceSnapshot):
        """Check for resource-related alerts."""
        if snapshot.cpu_percent > 95:
            self._add_alert('critical', 'cpu', f'CPU usage critical: {snapshot.cpu_percent:.1f}%')
        
        if snapshot.memory_percent > 95:
            self._add_alert('critical', 'memory', f'Memory usage critical: {snapshot.memory_percent:.1f}%')
        
        if snapshot.memory_available_mb < 100:
            self._add_alert('warning', 'memory', f'Low available memory: {snapshot.memory_available_mb:.1f} MB')
    
    def _add_alert(self, severity: str, category: str, message: str):
        """Add an alert."""
        alert = {
            'timestamp': time.time(),
            'severity': severity,
            'category': category,
            'message': message
        }
        self.alerts.append(alert)
        logger.warning(f"Alert: [{severity}] {category}: {message}")
    
    def export_report(self, output_path: Path):
        """Export comprehensive monitoring report."""
        report = {
            'generated_at': datetime.now().isoformat(),
            'system_health': self.get_system_health(),
            'resource_summary': self.get_resource_summary(window_seconds=300),
            'cognitive_summary': self.get_cognitive_summary(window_seconds=300),
            'alerts': self.alerts[-50:],  # Last 50 alerts
            'component_details': {
                comp: {
                    'health': self.get_component_health(comp),
                    'summary': self.get_cognitive_summary(component=comp, window_seconds=300)
                }
                for comp in self.component_metrics.keys()
            }
        }
        
        with open(output_path, 'w') as f:
            json.dump(report, f, indent=2)
        
        logger.info(f"Monitoring report exported to {output_path}")


def main():
    """Example usage of the Cognitive Monitor."""
    monitor = CognitiveMonitor()
    
    # Simulate monitoring
    for i in range(10):
        # Capture resources
        snapshot = monitor.capture_resource_snapshot()
        print(f"Snapshot {i+1}: CPU={snapshot.cpu_percent:.1f}%, Memory={snapshot.memory_percent:.1f}%")
        
        # Simulate cognitive tasks
        monitor.record_cognitive_task(
            component='atomspace',
            task_type='query',
            duration_ms=50 + i * 10,
            success=True,
            quality_score=0.9
        )
        
        time.sleep(0.5)
    
    # Get summaries
    print("\nSystem Health:")
    health = monitor.get_system_health()
    print(json.dumps(health, indent=2))
    
    # Export report
    output_path = Path('/tmp/cognitive_monitor_report.json')
    monitor.export_report(output_path)
    print(f"\nReport exported to {output_path}")


if __name__ == '__main__':
    main()
