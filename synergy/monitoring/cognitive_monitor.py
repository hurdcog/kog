#!/usr/bin/env python3
"""
Cognitive Monitoring System

Provides system-wide monitoring and self-awareness capabilities for the
OpenCog Collection. Implements the Introspection Membrane from the
Deep Tree Echo architecture.
"""

import psutil
import time
import json
import logging
from typing import Dict, List, Any, Optional
from dataclasses import dataclass, asdict
from datetime import datetime
import threading
from collections import deque


logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


@dataclass
class ResourceSnapshot:
    """Snapshot of system resources at a point in time"""
    timestamp: float
    cpu_percent: float
    memory_percent: float
    memory_mb: float
    disk_io_read_mb: float
    disk_io_write_mb: float
    network_sent_mb: float
    network_recv_mb: float
    
    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)


@dataclass
class PerformanceMetric:
    """Performance metric for a cognitive operation"""
    operation_name: str
    timestamp: float
    duration_ms: float
    success: bool
    component: str
    metadata: Dict[str, Any]
    
    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)


class CognitiveMonitor:
    """
    System-wide monitoring and self-awareness for cognitive operations.
    
    Tracks:
    - Resource utilization (CPU, memory, I/O)
    - Performance metrics for cognitive operations
    - Component health and activity
    - System-level cognitive state
    """
    
    def __init__(self, history_size: int = 1000):
        self.history_size = history_size
        self.resource_history: deque = deque(maxlen=history_size)
        self.performance_metrics: deque = deque(maxlen=history_size)
        self.monitoring = False
        self.monitor_thread: Optional[threading.Thread] = None
        self.monitor_interval = 5.0  # seconds
        
        # Baseline measurements
        self._baseline_io = psutil.disk_io_counters()
        self._baseline_net = psutil.net_io_counters()
        
        logger.info("Cognitive Monitor initialized")
    
    def start_monitoring(self, interval: float = 5.0):
        """Start continuous resource monitoring"""
        if self.monitoring:
            logger.warning("Monitoring already active")
            return
        
        self.monitor_interval = interval
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop, daemon=True)
        self.monitor_thread.start()
        logger.info(f"Started continuous monitoring (interval: {interval}s)")
    
    def stop_monitoring(self):
        """Stop continuous monitoring"""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=10.0)
        logger.info("Stopped monitoring")
    
    def _monitor_loop(self):
        """Continuous monitoring loop"""
        while self.monitoring:
            try:
                snapshot = self.capture_resource_snapshot()
                self.resource_history.append(snapshot)
            except Exception as e:
                logger.error(f"Error in monitoring loop: {e}")
            
            time.sleep(self.monitor_interval)
    
    def capture_resource_snapshot(self) -> ResourceSnapshot:
        """Capture current system resource utilization"""
        # CPU and memory
        cpu_percent = psutil.cpu_percent(interval=0.1)
        memory = psutil.virtual_memory()
        memory_percent = memory.percent
        memory_mb = memory.used / (1024 * 1024)
        
        # Disk I/O
        disk_io = psutil.disk_io_counters()
        disk_read_mb = (disk_io.read_bytes - self._baseline_io.read_bytes) / (1024 * 1024)
        disk_write_mb = (disk_io.write_bytes - self._baseline_io.write_bytes) / (1024 * 1024)
        
        # Network I/O
        net_io = psutil.net_io_counters()
        net_sent_mb = (net_io.bytes_sent - self._baseline_net.bytes_sent) / (1024 * 1024)
        net_recv_mb = (net_io.bytes_recv - self._baseline_net.bytes_recv) / (1024 * 1024)
        
        return ResourceSnapshot(
            timestamp=time.time(),
            cpu_percent=cpu_percent,
            memory_percent=memory_percent,
            memory_mb=memory_mb,
            disk_io_read_mb=disk_read_mb,
            disk_io_write_mb=disk_write_mb,
            network_sent_mb=net_sent_mb,
            network_recv_mb=net_recv_mb
        )
    
    def record_performance(
        self,
        operation_name: str,
        duration_ms: float,
        success: bool,
        component: str,
        metadata: Dict[str, Any] = None
    ):
        """Record performance metric for a cognitive operation"""
        metric = PerformanceMetric(
            operation_name=operation_name,
            timestamp=time.time(),
            duration_ms=duration_ms,
            success=success,
            component=component,
            metadata=metadata or {}
        )
        self.performance_metrics.append(metric)
        logger.debug(f"Recorded performance: {operation_name} ({duration_ms:.2f}ms)")
    
    def get_resource_statistics(self) -> Dict[str, Any]:
        """Get statistical summary of resource utilization"""
        if not self.resource_history:
            return {}
        
        cpu_values = [s.cpu_percent for s in self.resource_history]
        memory_values = [s.memory_percent for s in self.resource_history]
        
        return {
            'cpu': {
                'current': cpu_values[-1] if cpu_values else 0,
                'average': sum(cpu_values) / len(cpu_values),
                'max': max(cpu_values),
                'min': min(cpu_values)
            },
            'memory': {
                'current': memory_values[-1] if memory_values else 0,
                'average': sum(memory_values) / len(memory_values),
                'max': max(memory_values),
                'min': min(memory_values)
            },
            'samples': len(self.resource_history)
        }
    
    def get_performance_statistics(self) -> Dict[str, Any]:
        """Get statistical summary of performance metrics"""
        if not self.performance_metrics:
            return {}
        
        # Group by component
        by_component: Dict[str, List[PerformanceMetric]] = {}
        for metric in self.performance_metrics:
            if metric.component not in by_component:
                by_component[metric.component] = []
            by_component[metric.component].append(metric)
        
        # Calculate statistics per component
        component_stats = {}
        for component, metrics in by_component.items():
            durations = [m.duration_ms for m in metrics]
            successes = sum(1 for m in metrics if m.success)
            
            component_stats[component] = {
                'total_operations': len(metrics),
                'success_rate': (successes / len(metrics) * 100) if metrics else 0,
                'avg_duration_ms': sum(durations) / len(durations) if durations else 0,
                'max_duration_ms': max(durations) if durations else 0,
                'min_duration_ms': min(durations) if durations else 0
            }
        
        return {
            'total_operations': len(self.performance_metrics),
            'by_component': component_stats
        }
    
    def get_cognitive_state(self) -> Dict[str, Any]:
        """
        Assess overall cognitive state of the system.
        
        Returns a comprehensive view of system health, performance,
        and resource utilization.
        """
        resource_stats = self.get_resource_statistics()
        performance_stats = self.get_performance_statistics()
        
        # Assess cognitive load
        cognitive_load = "unknown"
        if resource_stats:
            avg_cpu = resource_stats['cpu']['average']
            avg_memory = resource_stats['memory']['average']
            
            if avg_cpu < 30 and avg_memory < 50:
                cognitive_load = "light"
            elif avg_cpu < 60 and avg_memory < 75:
                cognitive_load = "moderate"
            elif avg_cpu < 85 and avg_memory < 90:
                cognitive_load = "heavy"
            else:
                cognitive_load = "critical"
        
        return {
            'timestamp': datetime.now().isoformat(),
            'cognitive_load': cognitive_load,
            'resources': resource_stats,
            'performance': performance_stats,
            'monitoring_active': self.monitoring
        }
    
    def generate_report(self) -> Dict[str, Any]:
        """Generate comprehensive monitoring report"""
        return {
            'timestamp': datetime.now().isoformat(),
            'cognitive_state': self.get_cognitive_state(),
            'resource_history_size': len(self.resource_history),
            'performance_history_size': len(self.performance_metrics)
        }
    
    def save_report(self, filepath: str):
        """Save monitoring report to file"""
        report = self.generate_report()
        with open(filepath, 'w') as f:
            json.dump(report, f, indent=2)
        logger.info(f"Monitoring report saved to {filepath}")
    
    def export_metrics(self, filepath: str):
        """Export all metrics to JSON file"""
        data = {
            'resource_history': [s.to_dict() for s in self.resource_history],
            'performance_metrics': [m.to_dict() for m in self.performance_metrics]
        }
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
        logger.info(f"Metrics exported to {filepath}")


class PerformanceTimer:
    """Context manager for timing cognitive operations"""
    
    def __init__(
        self,
        monitor: CognitiveMonitor,
        operation_name: str,
        component: str,
        metadata: Dict[str, Any] = None
    ):
        self.monitor = monitor
        self.operation_name = operation_name
        self.component = component
        self.metadata = metadata or {}
        self.start_time = None
        self.success = True
    
    def __enter__(self):
        self.start_time = time.time()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        duration_ms = (time.time() - self.start_time) * 1000
        self.success = exc_type is None
        
        self.monitor.record_performance(
            operation_name=self.operation_name,
            duration_ms=duration_ms,
            success=self.success,
            component=self.component,
            metadata=self.metadata
        )
        
        return False  # Don't suppress exceptions


# Singleton instance
_monitor_instance: Optional[CognitiveMonitor] = None


def get_monitor() -> CognitiveMonitor:
    """Get the global monitor instance"""
    global _monitor_instance
    if _monitor_instance is None:
        _monitor_instance = CognitiveMonitor()
    return _monitor_instance


if __name__ == "__main__":
    # Example usage
    monitor = get_monitor()
    monitor.start_monitoring(interval=2.0)
    
    # Simulate some cognitive operations
    for i in range(5):
        with PerformanceTimer(monitor, f"test_operation_{i}", "test_component"):
            time.sleep(0.1 + i * 0.05)
    
    # Wait for some monitoring data
    time.sleep(10)
    
    # Generate and print report
    report = monitor.generate_report()
    print(json.dumps(report, indent=2))
    
    monitor.stop_monitoring()
