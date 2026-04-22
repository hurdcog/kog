"""
metamodel_bridge.py - Bridge between Scheme Metamodel and Python Synergy Components

This module provides integration between the foundational Scheme metamodel
(bootstrap, AAR core, cognitive kernel) and the Python-based synergy components,
enabling true neural-symbolic cognitive synergy.
"""

import subprocess
import json
import numpy as np
from typing import Dict, List, Any, Optional, Tuple
from pathlib import Path


class SchemeMetamodelBridge:
    """
    Bridge to interact with Scheme metamodel from Python.
    
    Enables:
    - Invoking Scheme cognitive operations from Python
    - Converting between Python and Scheme data structures
    - Integrating symbolic (Scheme) and neural (Python) processing
    """
    
    def __init__(self, metamodel_path: Optional[str] = None):
        """
        Initialize the Scheme metamodel bridge.
        
        Args:
            metamodel_path: Path to metamodel directory (default: auto-detect)
        """
        if metamodel_path is None:
            # Auto-detect metamodel path
            current_dir = Path(__file__).parent
            repo_root = current_dir.parent.parent
            metamodel_path = repo_root / "metamodel"
        
        self.metamodel_path = Path(metamodel_path)
        self.guile_load_path = str(self.metamodel_path.parent)
        
        # Verify Guile is available
        try:
            result = subprocess.run(
                ["guile", "--version"],
                capture_output=True,
                text=True,
                timeout=5
            )
            self.guile_available = result.returncode == 0
        except (subprocess.TimeoutExpired, FileNotFoundError):
            self.guile_available = False
            print("Warning: Guile not available. Scheme integration disabled.")
    
    def eval_scheme(self, code: str) -> Any:
        """
        Evaluate Scheme code and return result.
        
        Args:
            code: Scheme code to evaluate
            
        Returns:
            Result of evaluation (converted to Python types)
        """
        if not self.guile_available:
            raise RuntimeError("Guile not available")
        
        try:
            result = subprocess.run(
                ["guile", "-L", self.guile_load_path, "-c", code],
                capture_output=True,
                text=True,
                timeout=30
            )
            
            if result.returncode != 0:
                raise RuntimeError(f"Scheme evaluation failed: {result.stderr}")
            
            return self._parse_scheme_output(result.stdout.strip())
        
        except subprocess.TimeoutExpired:
            raise RuntimeError("Scheme evaluation timed out")
    
    def _parse_scheme_output(self, output: str) -> Any:
        """Parse Scheme output to Python types."""
        # Simple parsing - can be enhanced
        output = output.strip()
        
        # Boolean
        if output == "#t":
            return True
        elif output == "#f":
            return False
        
        # Number
        try:
            if "." in output:
                return float(output)
            else:
                return int(output)
        except ValueError:
            pass
        
        # String
        if output.startswith('"') and output.endswith('"'):
            return output[1:-1]
        
        # Default: return as string
        return output
    
    def bootstrap_eval(self, expression: str) -> Any:
        """
        Evaluate expression using bootstrap metacircular evaluator.
        
        Args:
            expression: Scheme expression to evaluate
            
        Returns:
            Evaluation result
        """
        code = f"""
        (use-modules (metamodel bootstrap))
        (define env (make-environment))
        (display (bootstrap-eval '{expression} env))
        """
        return self.eval_scheme(code)
    
    def create_aar_core(self, dimension: int = 32, learning_rate: float = 0.01) -> str:
        """
        Create AAR core in Scheme and return handle.
        
        Args:
            dimension: State space dimension
            learning_rate: Learning rate for updates
            
        Returns:
            Core identifier
        """
        # For now, we'll use Python AAR core
        # Full Scheme integration would require persistent Guile process
        return f"aar-core-{dimension}-{learning_rate}"
    
    def vector_to_scheme(self, vector: np.ndarray) -> str:
        """
        Convert NumPy vector to Scheme vector literal.
        
        Args:
            vector: NumPy array
            
        Returns:
            Scheme vector literal string
        """
        values = " ".join(str(v) for v in vector)
        return f"#({values})"
    
    def scheme_to_vector(self, scheme_vec: str) -> np.ndarray:
        """
        Convert Scheme vector literal to NumPy array.
        
        Args:
            scheme_vec: Scheme vector literal (e.g., "#(1 2 3)")
            
        Returns:
            NumPy array
        """
        # Remove #( and )
        if scheme_vec.startswith("#(") and scheme_vec.endswith(")"):
            values_str = scheme_vec[2:-1]
            values = [float(v) for v in values_str.split()]
            return np.array(values)
        else:
            raise ValueError(f"Invalid Scheme vector: {scheme_vec}")


class CognitiveSynergyOrchestrator:
    """
    Orchestrates cognitive synergy between Scheme metamodel and Python components.
    
    Integrates:
    - Scheme bootstrap (symbolic primitives)
    - Scheme AAR core (symbolic self-awareness)
    - Python AAR core (numerical self-awareness)
    - Python hypergraph identity (knowledge representation)
    - Python membrane manager (process isolation)
    """
    
    def __init__(
        self,
        dimension: int = 32,
        learning_rate: float = 0.01,
        enable_scheme: bool = True
    ):
        """
        Initialize cognitive synergy orchestrator.
        
        Args:
            dimension: State space dimension
            learning_rate: Learning rate
            enable_scheme: Enable Scheme metamodel integration
        """
        self.dimension = dimension
        self.learning_rate = learning_rate
        
        # Initialize Python components
        try:
            from synergy.core.aar_core import AARCore
            self.python_aar = AARCore(dimension=dimension, learning_rate=learning_rate)
        except ImportError:
            self.python_aar = None
        
        try:
            from synergy.identity.hypergraph_identity import HypergraphIdentitySystem
            self.identity_system = HypergraphIdentitySystem()
        except ImportError:
            self.identity_system = None
        
        # Initialize Scheme bridge
        self.scheme_bridge = None
        if enable_scheme:
            try:
                self.scheme_bridge = SchemeMetamodelBridge()
                if not self.scheme_bridge.guile_available:
                    self.scheme_bridge = None
            except Exception as e:
                print(f"Warning: Could not initialize Scheme bridge: {e}")
                self.scheme_bridge = None
        
        # Synergy state
        self.cycle_count = 0
        self.synergy_history = []
    
    def cognitive_cycle(
        self,
        perception: np.ndarray,
        symbolic_input: Optional[str] = None
    ) -> Dict[str, Any]:
        """
        Execute one cognitive cycle integrating symbolic and neural processing.
        
        Args:
            perception: Neural perception vector
            symbolic_input: Optional symbolic input for Scheme processing
            
        Returns:
            Dictionary with cycle results
        """
        results = {
            "cycle": self.cycle_count,
            "neural": {},
            "symbolic": {},
            "synergy": {}
        }
        
        # Neural processing (Python AAR)
        if self.python_aar is not None:
            self.python_aar.perceive(perception)
            action = self.python_aar.act()
            self.python_aar.reflect()
            
            results["neural"] = {
                "action": action,
                "coherence": self.python_aar.get_self_state()["coherence_score"],
                "emergence": self.python_aar.get_self_state()["emergence_score"]
            }
        
        # Symbolic processing (Scheme metamodel)
        if self.scheme_bridge is not None and symbolic_input is not None:
            try:
                # Evaluate symbolic input
                symbolic_result = self.scheme_bridge.bootstrap_eval(symbolic_input)
                results["symbolic"] = {
                    "result": symbolic_result,
                    "input": symbolic_input
                }
            except Exception as e:
                results["symbolic"] = {"error": str(e)}
        
        # Identity refinement
        if self.identity_system is not None:
            conversation = [{
                "role": "system",
                "content": f"Cycle {self.cycle_count}: Neural coherence {results['neural'].get('coherence', 0):.3f}"
            }]
            self.identity_system.add_conversation(conversation)
            
            results["synergy"]["identity_nodes"] = len(self.identity_system.graph.nodes)
        
        # Compute synergy metrics
        if "neural" in results and "coherence" in results["neural"]:
            neural_coherence = results["neural"]["coherence"]
            neural_emergence = results["neural"]["emergence"]
            
            # Synergy score: product of coherence and emergence
            synergy_score = neural_coherence * neural_emergence
            
            results["synergy"]["score"] = synergy_score
            results["synergy"]["neural_coherence"] = neural_coherence
            results["synergy"]["neural_emergence"] = neural_emergence
        
        self.synergy_history.append(results)
        self.cycle_count += 1
        
        return results
    
    def get_synergy_metrics(self) -> Dict[str, Any]:
        """
        Get comprehensive synergy metrics.
        
        Returns:
            Dictionary of synergy metrics
        """
        if not self.synergy_history:
            return {"error": "No cycles executed"}
        
        # Extract synergy scores
        scores = [
            h["synergy"].get("score", 0)
            for h in self.synergy_history
            if "synergy" in h and "score" in h["synergy"]
        ]
        
        if not scores:
            return {"error": "No synergy scores available"}
        
        return {
            "cycle_count": self.cycle_count,
            "mean_synergy": np.mean(scores),
            "max_synergy": np.max(scores),
            "min_synergy": np.min(scores),
            "synergy_trend": np.polyfit(range(len(scores)), scores, 1)[0] if len(scores) > 1 else 0,
            "current_synergy": scores[-1],
            "history_length": len(self.synergy_history)
        }
    
    def demonstrate(self, num_cycles: int = 10):
        """
        Demonstrate cognitive synergy orchestration.
        
        Args:
            num_cycles: Number of cycles to run
        """
        print("=== Cognitive Synergy Orchestration Demonstration ===\n")
        
        print(f"Configuration:")
        print(f"  Dimension: {self.dimension}")
        print(f"  Learning rate: {self.learning_rate}")
        print(f"  Python AAR: {'✓' if self.python_aar else '✗'}")
        print(f"  Scheme bridge: {'✓' if self.scheme_bridge else '✗'}")
        print(f"  Identity system: {'✓' if self.identity_system else '✗'}")
        print()
        
        print(f"Running {num_cycles} cognitive cycles...\n")
        
        for i in range(num_cycles):
            # Generate random perception
            perception = np.random.randn(self.dimension)
            
            # Symbolic input (alternating)
            symbolic_input = None
            if i % 3 == 0 and self.scheme_bridge:
                symbolic_input = f"(+ {i} 1)"
            
            # Execute cycle
            results = self.cognitive_cycle(perception, symbolic_input)
            
            # Print progress
            if "synergy" in results and "score" in results["synergy"]:
                print(f"  Cycle {i}: Synergy = {results['synergy']['score']:.4f}")
        
        print("\nFinal metrics:")
        metrics = self.get_synergy_metrics()
        for key, value in metrics.items():
            if isinstance(value, float):
                print(f"  {key}: {value:.4f}")
            else:
                print(f"  {key}: {value}")
        
        print("\n=== Demonstration Complete ===")


def main():
    """Main demonstration function."""
    orchestrator = CognitiveSynergyOrchestrator(
        dimension=16,
        learning_rate=0.01,
        enable_scheme=True
    )
    
    orchestrator.demonstrate(num_cycles=10)


if __name__ == "__main__":
    main()
