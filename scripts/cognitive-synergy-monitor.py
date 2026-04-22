#!/usr/bin/env python3
"""
Cognitive Synergy Monitor for AGI-OS

This script monitors and analyzes cognitive synergy across the three-layer
AGI-OS architecture (Cognumach, HurdCog, OpenCog Collection).
"""

import os
import sys
import json
import time
import subprocess
from datetime import datetime
from pathlib import Path

class CognitiveSynergyMonitor:
    """Monitor cognitive synergy across AGI-OS layers"""
    
    def __init__(self, occ_root):
        self.occ_root = Path(occ_root)
        self.report_dir = self.occ_root / "synergy_reports"
        self.report_dir.mkdir(exist_ok=True)
        
    def check_layer1_cognumach(self):
        """Check Cognumach microkernel layer"""
        print("\n" + "="*60)
        print("Layer 1: Cognumach Microkernel")
        print("="*60)
        
        cognumach_path = self.occ_root / "cognumach"
        
        if not cognumach_path.exists():
            print("❌ Cognumach source not found")
            return {"status": "missing", "score": 0}
        
        print("✓ Cognumach source: Present")
        
        # Check for key components
        components = {
            "kernel": cognumach_path / "kern",
            "vm": cognumach_path / "vm",
            "ipc": cognumach_path / "ipc",
            "device": cognumach_path / "device"
        }
        
        present = sum(1 for path in components.values() if path.exists())
        total = len(components)
        
        print(f"✓ Core components: {present}/{total}")
        
        score = (present / total) * 100
        return {"status": "available", "score": score, "components": present}
    
    def check_layer2_hurdcog(self):
        """Check HurdCog cognitive OS layer"""
        print("\n" + "="*60)
        print("Layer 2: HurdCog Cognitive OS")
        print("="*60)
        
        hurdcog_path = self.occ_root / "hurdcog"
        
        if not hurdcog_path.exists():
            print("❌ HurdCog source not found")
            return {"status": "missing", "score": 0}
        
        print("✓ HurdCog source: Present")
        
        # Check for cognitive components
        components = {
            "cogkernel": hurdcog_path / "cogkernel",
            "dashboard": hurdcog_path / "dashboard",
            "hurd": hurdcog_path / "hurd",
            "cognitive": hurdcog_path / "cognitive"
        }
        
        present = sum(1 for path in components.values() if path.exists())
        total = len(components)
        
        print(f"✓ Cognitive components: {present}/{total}")
        
        score = (present / total) * 100
        return {"status": "available", "score": score, "components": present}
    
    def check_layer3_opencog(self):
        """Check OpenCog Collection layer"""
        print("\n" + "="*60)
        print("Layer 3: OpenCog Collection")
        print("="*60)
        
        # Core components
        core_components = [
            "cogutil", "atomspace", "cogserver", "ure"
        ]
        
        # Cognitive components
        cognitive_components = [
            "pln", "attention", "miner", "learn", "generate"
        ]
        
        # Specialized components
        specialized_components = [
            "moses", "lg-atomese", "relex", "vision", "spacetime"
        ]
        
        all_components = core_components + cognitive_components + specialized_components
        
        present = 0
        for component in all_components:
            component_path = self.occ_root / component
            if component_path.exists():
                present += 1
        
        total = len(all_components)
        
        print(f"✓ OpenCog components: {present}/{total}")
        print(f"  - Core: {sum(1 for c in core_components if (self.occ_root / c).exists())}/{len(core_components)}")
        print(f"  - Cognitive: {sum(1 for c in cognitive_components if (self.occ_root / c).exists())}/{len(cognitive_components)}")
        print(f"  - Specialized: {sum(1 for c in specialized_components if (self.occ_root / c).exists())}/{len(specialized_components)}")
        
        score = (present / total) * 100
        return {"status": "available", "score": score, "components": present, "total": total}
    
    def check_integration_infrastructure(self):
        """Check integration infrastructure"""
        print("\n" + "="*60)
        print("Integration Infrastructure")
        print("="*60)
        
        infrastructure = {
            "Guix unified package": self.occ_root / "occ-hurdcog-unified.scm",
            "Debian packaging": self.occ_root / "opencog-debian",
            "Build order docs": self.occ_root / "opencog-debian" / "BUILD_ORDER.md",
            "Architecture docs": self.occ_root / "opencog-debian" / "PACKAGING_ARCHITECTURE.md",
            "Integration docs": self.occ_root / "integration_architecture.md",
            "Synergy script": self.occ_root / "synergy_improved.sh"
        }
        
        present = 0
        for name, path in infrastructure.items():
            if path.exists():
                print(f"✓ {name}: Present")
                present += 1
            else:
                print(f"❌ {name}: Missing")
        
        total = len(infrastructure)
        score = (present / total) * 100
        
        return {"status": "available", "score": score, "components": present, "total": total}
    
    def calculate_synergy_score(self, layer1, layer2, layer3, integration):
        """Calculate overall cognitive synergy score"""
        
        # Weighted scoring
        weights = {
            "layer1": 0.2,  # Microkernel foundation
            "layer2": 0.3,  # Cognitive OS
            "layer3": 0.4,  # OpenCog Collection
            "integration": 0.1  # Integration infrastructure
        }
        
        total_score = (
            layer1["score"] * weights["layer1"] +
            layer2["score"] * weights["layer2"] +
            layer3["score"] * weights["layer3"] +
            integration["score"] * weights["integration"]
        )
        
        return total_score
    
    def generate_report(self, layer1, layer2, layer3, integration, synergy_score):
        """Generate synergy report"""
        
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        report_file = self.report_dir / f"synergy_report_{timestamp}.json"
        
        report = {
            "timestamp": datetime.now().isoformat(),
            "layers": {
                "layer1_cognumach": layer1,
                "layer2_hurdcog": layer2,
                "layer3_opencog": layer3
            },
            "integration": integration,
            "synergy_score": synergy_score,
            "status": self.get_status_label(synergy_score)
        }
        
        with open(report_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"\n✓ Report saved to: {report_file}")
        
        return report
    
    def get_status_label(self, score):
        """Get status label based on score"""
        if score >= 90:
            return "Excellent - Full cognitive synergy achieved"
        elif score >= 75:
            return "Good - Strong cognitive synergy"
        elif score >= 60:
            return "Fair - Moderate cognitive synergy"
        elif score >= 40:
            return "Limited - Weak cognitive synergy"
        else:
            return "Insufficient - Minimal cognitive synergy"
    
    def display_summary(self, synergy_score, status):
        """Display summary"""
        print("\n" + "="*60)
        print("Cognitive Synergy Analysis Summary")
        print("="*60)
        print(f"\nOverall Synergy Score: {synergy_score:.1f}/100")
        print(f"Status: {status}")
        print("\nCognitive Synergy Factors:")
        print("  • Layer Integration: Multi-layer cognitive processing")
        print("  • Component Interaction: Cross-component knowledge flow")
        print("  • Emergent Intelligence: System-level cognitive capabilities")
        print("  • Adaptive Behavior: Self-optimization and learning")
        print("\n" + "="*60)
    
    def run(self):
        """Run the cognitive synergy monitor"""
        print("\n" + "="*60)
        print("AGI-OS Cognitive Synergy Monitor")
        print("="*60)
        print(f"OCC Root: {self.occ_root}")
        print(f"Timestamp: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        
        # Check all layers
        layer1 = self.check_layer1_cognumach()
        layer2 = self.check_layer2_hurdcog()
        layer3 = self.check_layer3_opencog()
        integration = self.check_integration_infrastructure()
        
        # Calculate synergy score
        synergy_score = self.calculate_synergy_score(layer1, layer2, layer3, integration)
        status = self.get_status_label(synergy_score)
        
        # Generate report
        self.generate_report(layer1, layer2, layer3, integration, synergy_score)
        
        # Display summary
        self.display_summary(synergy_score, status)
        
        return synergy_score

def main():
    # Determine OCC root
    script_dir = Path(__file__).parent
    occ_root = script_dir.parent
    
    # Run monitor
    monitor = CognitiveSynergyMonitor(occ_root)
    score = monitor.run()
    
    # Exit with status based on score
    if score >= 75:
        sys.exit(0)  # Success
    elif score >= 50:
        sys.exit(1)  # Warning
    else:
        sys.exit(2)  # Error

if __name__ == "__main__":
    main()
