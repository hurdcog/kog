#!/usr/bin/env python3
"""
Synergy Metrics Collection Tool

Collects cognitive synergy metrics from all components and generates
comprehensive reports for CI/CD integration and historical tracking.

Part of the Cognitive Synergy Enhancement initiative.
"""

import json
import os
import sys
import time
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Any, Optional

# Add synergy module to path
sys.path.insert(0, str(Path(__file__).parent.parent))

try:
    from synergy.core.aar_core import AARCore
    from synergy.identity.hypergraph_identity import HypergraphIdentitySystem
    from synergy.membranes.membrane_manager import MembraneManager
    AAR_AVAILABLE = True
except ImportError as e:
    print(f"Warning: Could not import synergy components: {e}")
    AAR_AVAILABLE = False


class SynergyMetricsCollector:
    """Collects and aggregates cognitive synergy metrics."""
    
    def __init__(self, output_dir: str = "synergy_reports"):
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
        self.metrics = {
            "timestamp": datetime.utcnow().isoformat(),
            "collection_time_ms": 0,
            "components": {},
            "synergy_score": 0.0,
            "health_status": "unknown"
        }
    
    def collect_aar_metrics(self) -> Dict[str, Any]:
        """Collect metrics from AAR Core component."""
        if not AAR_AVAILABLE:
            return {"status": "unavailable", "error": "AAR Core not imported"}
        
        try:
            aar = AARCore(dimension=32, learning_rate=0.01)
            
            # Simulate a cognitive cycle
            import numpy as np
            perception = np.random.randn(32)
            aar.perceive(perception)
            action = aar.act()
            reflection = aar.reflect()
            
            # Get state
            state = aar.get_self_state()
            
            return {
                "status": "available",
                "emergence_score": float(state.get("emergence_score", 0.0)),
                "coherence": float(state.get("coherence", 0.0)),
                "dimension": 32,
                "cycles_completed": 1,
                "health": "healthy" if state.get("emergence_score", 0) > 0.3 else "degraded"
            }
        except Exception as e:
            return {"status": "error", "error": str(e)}
    
    def collect_hypergraph_metrics(self) -> Dict[str, Any]:
        """Collect metrics from Hypergraph Identity System."""
        if not AAR_AVAILABLE:
            return {"status": "unavailable", "error": "Hypergraph system not imported"}
        
        try:
            identity = HypergraphIdentitySystem()
            
            # Add sample conversation for testing
            sample_conversation = [
                {"role": "assistant", "content": "I am a cognitive synergy system."},
                {"role": "user", "content": "What is your purpose?"},
                {"role": "assistant", "content": "To enable emergent intelligence through component integration."}
            ]
            identity.add_conversation(sample_conversation)
            
            # Get summary
            summary = identity.get_identity_summary()
            
            return {
                "status": "available",
                "total_nodes": summary.get("total_nodes", 0),
                "total_edges": summary.get("total_edges", 0),
                "core_attributes": len(summary.get("core_attributes", [])),
                "identity_strength": summary.get("identity_strength", 0.0),
                "health": "healthy" if summary.get("total_nodes", 0) > 0 else "empty"
            }
        except Exception as e:
            return {"status": "error", "error": str(e)}
    
    def collect_membrane_metrics(self) -> Dict[str, Any]:
        """Collect metrics from Membrane Manager."""
        if not AAR_AVAILABLE:
            return {"status": "unavailable", "error": "Membrane Manager not imported"}
        
        try:
            manager = MembraneManager()
            
            # Get hierarchy state
            hierarchy = manager.get_hierarchy_state()
            metrics = manager.get_metrics()
            
            return {
                "status": "available",
                "total_membranes": len(hierarchy.get("membranes", [])),
                "messages_processed": metrics.get("messages_processed", 0),
                "active_processes": metrics.get("active_processes", 0),
                "resource_utilization": metrics.get("resource_utilization", 0.0),
                "health": "healthy" if len(hierarchy.get("membranes", [])) > 0 else "uninitialized"
            }
        except Exception as e:
            return {"status": "error", "error": str(e)}
    
    def collect_repository_metrics(self) -> Dict[str, Any]:
        """Collect repository-level metrics."""
        repo_root = Path(__file__).parent.parent
        
        metrics = {
            "status": "available",
            "synergy_components": 0,
            "test_files": 0,
            "documentation_files": 0
        }
        
        # Count synergy components
        synergy_dir = repo_root / "synergy"
        if synergy_dir.exists():
            metrics["synergy_components"] = len(list(synergy_dir.rglob("*.py")))
        
        # Count test files
        tests_dir = repo_root / "tests" / "synergy"
        if tests_dir.exists():
            metrics["test_files"] = len(list(tests_dir.rglob("test_*.py")))
        
        # Count documentation
        docs = list(repo_root.glob("*SYNERGY*.md")) + list(repo_root.glob("*COGNITIVE*.md"))
        metrics["documentation_files"] = len(docs)
        
        return metrics
    
    def calculate_synergy_score(self) -> float:
        """Calculate overall synergy score based on component metrics."""
        score = 0.0
        max_score = 100.0
        
        components = self.metrics.get("components", {})
        
        # AAR contribution (30 points)
        aar = components.get("aar_core", {})
        if aar.get("status") == "available":
            score += aar.get("emergence_score", 0) * 30
        
        # Hypergraph contribution (30 points)
        hypergraph = components.get("hypergraph_identity", {})
        if hypergraph.get("status") == "available":
            identity_strength = hypergraph.get("identity_strength", 0)
            score += identity_strength * 30
        
        # Membrane contribution (20 points)
        membrane = components.get("membrane_manager", {})
        if membrane.get("status") == "available":
            resource_util = membrane.get("resource_utilization", 0)
            score += min(resource_util * 20, 20)
        
        # Repository health (20 points)
        repo = components.get("repository", {})
        if repo.get("status") == "available":
            component_score = min(repo.get("synergy_components", 0) / 20 * 10, 10)
            test_score = min(repo.get("test_files", 0) / 5 * 5, 5)
            doc_score = min(repo.get("documentation_files", 0) / 5 * 5, 5)
            score += component_score + test_score + doc_score
        
        return min(score, max_score)
    
    def determine_health_status(self) -> str:
        """Determine overall system health."""
        score = self.metrics.get("synergy_score", 0)
        
        if score >= 80:
            return "excellent"
        elif score >= 60:
            return "healthy"
        elif score >= 40:
            return "degraded"
        elif score >= 20:
            return "poor"
        else:
            return "critical"
    
    def collect_all_metrics(self) -> Dict[str, Any]:
        """Collect metrics from all components."""
        start_time = time.time()
        
        print("Collecting cognitive synergy metrics...")
        
        # Collect from each component
        print("  - AAR Core...")
        self.metrics["components"]["aar_core"] = self.collect_aar_metrics()
        
        print("  - Hypergraph Identity...")
        self.metrics["components"]["hypergraph_identity"] = self.collect_hypergraph_metrics()
        
        print("  - Membrane Manager...")
        self.metrics["components"]["membrane_manager"] = self.collect_membrane_metrics()
        
        print("  - Repository...")
        self.metrics["components"]["repository"] = self.collect_repository_metrics()
        
        # Calculate overall metrics
        self.metrics["synergy_score"] = self.calculate_synergy_score()
        self.metrics["health_status"] = self.determine_health_status()
        self.metrics["collection_time_ms"] = int((time.time() - start_time) * 1000)
        
        print(f"\nCollection complete in {self.metrics['collection_time_ms']}ms")
        print(f"Synergy Score: {self.metrics['synergy_score']:.2f}/100")
        print(f"Health Status: {self.metrics['health_status']}")
        
        return self.metrics
    
    def save_metrics(self, filename: Optional[str] = None) -> Path:
        """Save metrics to JSON file."""
        if filename is None:
            timestamp = datetime.utcnow().strftime("%Y%m%d_%H%M%S")
            filename = f"synergy_metrics_{timestamp}.json"
        
        output_path = self.output_dir / filename
        
        with open(output_path, 'w') as f:
            json.dump(self.metrics, f, indent=2)
        
        print(f"\nMetrics saved to: {output_path}")
        return output_path
    
    def save_latest(self) -> Path:
        """Save metrics as latest.json for CI/CD."""
        return self.save_metrics("latest.json")
    
    def generate_markdown_report(self) -> Path:
        """Generate human-readable markdown report."""
        timestamp = datetime.utcnow().strftime("%Y-%m-%d %H:%M:%S UTC")
        
        report = f"""# Cognitive Synergy Metrics Report

**Generated**: {timestamp}  
**Synergy Score**: {self.metrics['synergy_score']:.2f}/100  
**Health Status**: {self.metrics['health_status'].upper()}  
**Collection Time**: {self.metrics['collection_time_ms']}ms

## Component Status

### AAR Core (Agent-Arena-Relation)
"""
        
        aar = self.metrics["components"].get("aar_core", {})
        if aar.get("status") == "available":
            report += f"""
- **Status**: ✅ Available
- **Emergence Score**: {aar.get('emergence_score', 0):.4f}
- **Coherence**: {aar.get('coherence', 0):.4f}
- **Dimension**: {aar.get('dimension', 0)}
- **Health**: {aar.get('health', 'unknown')}
"""
        else:
            report += f"\n- **Status**: ❌ {aar.get('status', 'unknown')}\n"
        
        report += "\n### Hypergraph Identity System\n"
        hypergraph = self.metrics["components"].get("hypergraph_identity", {})
        if hypergraph.get("status") == "available":
            report += f"""
- **Status**: ✅ Available
- **Total Nodes**: {hypergraph.get('total_nodes', 0)}
- **Total Edges**: {hypergraph.get('total_edges', 0)}
- **Core Attributes**: {hypergraph.get('core_attributes', 0)}
- **Identity Strength**: {hypergraph.get('identity_strength', 0):.4f}
- **Health**: {hypergraph.get('health', 'unknown')}
"""
        else:
            report += f"\n- **Status**: ❌ {hypergraph.get('status', 'unknown')}\n"
        
        report += "\n### Membrane Manager\n"
        membrane = self.metrics["components"].get("membrane_manager", {})
        if membrane.get("status") == "available":
            report += f"""
- **Status**: ✅ Available
- **Total Membranes**: {membrane.get('total_membranes', 0)}
- **Messages Processed**: {membrane.get('messages_processed', 0)}
- **Active Processes**: {membrane.get('active_processes', 0)}
- **Resource Utilization**: {membrane.get('resource_utilization', 0):.2f}%
- **Health**: {membrane.get('health', 'unknown')}
"""
        else:
            report += f"\n- **Status**: ❌ {membrane.get('status', 'unknown')}\n"
        
        report += "\n### Repository Metrics\n"
        repo = self.metrics["components"].get("repository", {})
        report += f"""
- **Synergy Components**: {repo.get('synergy_components', 0)} files
- **Test Files**: {repo.get('test_files', 0)} files
- **Documentation Files**: {repo.get('documentation_files', 0)} files
"""
        
        report += f"""
## Interpretation

### Synergy Score Breakdown
- **AAR Core**: Contributes up to 30 points based on emergence score
- **Hypergraph Identity**: Contributes up to 30 points based on identity strength
- **Membrane Manager**: Contributes up to 20 points based on resource utilization
- **Repository Health**: Contributes up to 20 points based on component/test/doc counts

### Health Status Levels
- **Excellent** (80-100): All systems operating optimally
- **Healthy** (60-79): Normal operation with minor issues
- **Degraded** (40-59): Reduced functionality, needs attention
- **Poor** (20-39): Significant issues, immediate action required
- **Critical** (0-19): System failure, emergency intervention needed

## Recommendations

"""
        
        score = self.metrics['synergy_score']
        if score >= 80:
            report += "✅ System is performing excellently. Continue monitoring and maintain current practices.\n"
        elif score >= 60:
            report += "⚠️ System is healthy but has room for improvement. Review component metrics for optimization opportunities.\n"
        elif score >= 40:
            report += "⚠️ System performance is degraded. Investigate failing components and address issues promptly.\n"
        else:
            report += "🚨 System is in poor health. Immediate investigation and remediation required.\n"
        
        report += f"""
---

*This report was automatically generated by the Synergy Metrics Collector.*  
*For more information, see `tools/collect_synergy_metrics.py`*
"""
        
        report_path = self.output_dir / f"synergy_report_{datetime.utcnow().strftime('%Y%m%d_%H%M%S')}.md"
        with open(report_path, 'w') as f:
            f.write(report)
        
        # Also save as latest
        latest_path = self.output_dir / "synergy_report_latest.md"
        with open(latest_path, 'w') as f:
            f.write(report)
        
        print(f"Report saved to: {report_path}")
        print(f"Latest report: {latest_path}")
        
        return report_path


def main():
    """Main entry point."""
    print("=" * 60)
    print("Cognitive Synergy Metrics Collector")
    print("=" * 60)
    print()
    
    collector = SynergyMetricsCollector()
    
    # Collect all metrics
    metrics = collector.collect_all_metrics()
    
    # Save outputs
    collector.save_metrics()
    collector.save_latest()
    collector.generate_markdown_report()
    
    print("\n" + "=" * 60)
    print("Collection Complete")
    print("=" * 60)
    
    # Exit with appropriate code based on health
    health = metrics.get("health_status", "unknown")
    if health in ["excellent", "healthy"]:
        sys.exit(0)
    elif health == "degraded":
        sys.exit(0)  # Warning but not failure
    else:
        sys.exit(1)  # Failure for poor/critical


if __name__ == "__main__":
    main()
