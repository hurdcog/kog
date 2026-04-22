#!/usr/bin/env python3
"""
Scheme-Python FFI Bridge

Provides Foreign Function Interface between:
- Guile Scheme (metamodel implementation)
- Python (synergy components)

This enables the Scheme metamodel to interact with Python-based
AGI components and vice versa, creating neural-symbolic synergy.
"""

import os
import sys
import json
import subprocess
import tempfile
import logging
from typing import Any, Dict, List, Optional, Union
from pathlib import Path

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


class SchemeInterpreter:
    """
    Interface to Guile Scheme interpreter
    
    Allows Python code to execute Scheme code and retrieve results.
    """
    
    def __init__(self, guile_path: str = "guile"):
        """
        Initialize Scheme interpreter
        
        Args:
            guile_path: Path to guile executable
        """
        self.guile_path = guile_path
        self.logger = logging.getLogger(f"{__name__}.SchemeInterpreter")
        
        # Check if Guile is available
        try:
            result = subprocess.run(
                [self.guile_path, "--version"],
                capture_output=True,
                text=True,
                timeout=5
            )
            if result.returncode == 0:
                self.logger.info(f"Guile found: {result.stdout.split()[0]}")
            else:
                self.logger.warning("Guile not found or not working")
        except Exception as e:
            self.logger.error(f"Failed to check Guile: {e}")
    
    def eval_scheme(self, scheme_code: str, 
                   load_files: Optional[List[str]] = None) -> Optional[str]:
        """
        Evaluate Scheme code and return result
        
        Args:
            scheme_code: Scheme code to evaluate
            load_files: Optional list of Scheme files to load first
            
        Returns:
            String result or None if error
        """
        try:
            # Build command
            cmd = [self.guile_path, "--no-auto-compile"]
            
            # Add load files
            if load_files:
                for file in load_files:
                    cmd.extend(["-l", file])
            
            # Add code to evaluate
            cmd.extend(["-c", scheme_code])
            
            # Execute
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=30
            )
            
            if result.returncode == 0:
                return result.stdout.strip()
            else:
                self.logger.error(f"Scheme error: {result.stderr}")
                return None
                
        except Exception as e:
            self.logger.error(f"Failed to evaluate Scheme: {e}")
            return None
    
    def call_scheme_function(self, function_name: str, 
                            args: List[Any],
                            module_file: Optional[str] = None) -> Optional[Any]:
        """
        Call a Scheme function with arguments
        
        Args:
            function_name: Name of Scheme function
            args: List of arguments
            module_file: Optional Scheme file to load
            
        Returns:
            Result of function call
        """
        # Convert Python args to Scheme representation
        scheme_args = []
        for arg in args:
            if isinstance(arg, str):
                scheme_args.append(f'"{arg}"')
            elif isinstance(arg, (int, float)):
                scheme_args.append(str(arg))
            elif isinstance(arg, bool):
                scheme_args.append('#t' if arg else '#f')
            elif isinstance(arg, list):
                scheme_args.append(self._python_list_to_scheme(arg))
            else:
                scheme_args.append(str(arg))
        
        # Build function call
        scheme_code = f"({function_name} {' '.join(scheme_args)})"
        
        # Evaluate
        load_files = [module_file] if module_file else None
        return self.eval_scheme(scheme_code, load_files)
    
    def _python_list_to_scheme(self, lst: List[Any]) -> str:
        """Convert Python list to Scheme list notation"""
        elements = []
        for item in lst:
            if isinstance(item, str):
                elements.append(f'"{item}"')
            elif isinstance(item, (int, float)):
                elements.append(str(item))
            elif isinstance(item, bool):
                elements.append('#t' if item else '#f')
            else:
                elements.append(str(item))
        return f"'({' '.join(elements)})"


class MetamodelBridge:
    """
    Bridge between Scheme metamodel and Python synergy components
    
    Provides high-level interface to metamodel functionality.
    """
    
    def __init__(self, metamodel_path: str = "/home/ubuntu/occ/metamodel"):
        """
        Initialize metamodel bridge
        
        Args:
            metamodel_path: Path to metamodel directory
        """
        self.metamodel_path = Path(metamodel_path)
        self.scheme = SchemeInterpreter()
        self.logger = logging.getLogger(f"{__name__}.MetamodelBridge")
        
        # Metamodel module files
        self.bootstrap_file = self.metamodel_path / "bootstrap.scm"
        self.aar_core_file = self.metamodel_path / "aar-core.scm"
        self.cognitive_kernel_file = self.metamodel_path / "cognitive-kernel.scm"
        
        self.logger.info(f"Metamodel bridge initialized: {metamodel_path}")
    
    def create_aar_core(self, dimension: int = 32, 
                       learning_rate: float = 0.01) -> bool:
        """
        Create AAR (Agent-Arena-Relation) core
        
        Args:
            dimension: State vector dimension
            learning_rate: Learning rate for adaptation
            
        Returns:
            Success status
        """
        if not self.aar_core_file.exists():
            self.logger.warning(f"AAR core file not found: {self.aar_core_file}")
            return False
        
        scheme_code = f"""
        (use-modules (metamodel aar-core))
        (define aar-core (make-aar-core #:dimension {dimension} 
                                        #:learning-rate {learning_rate}))
        (display "AAR core created")
        """
        
        result = self.scheme.eval_scheme(
            scheme_code,
            load_files=[str(self.aar_core_file)]
        )
        
        return result is not None
    
    def aar_perceive(self, perception: List[float]) -> bool:
        """
        Send perception to AAR core
        
        Args:
            perception: Perception vector
            
        Returns:
            Success status
        """
        # Convert perception to Scheme vector notation
        perception_str = f"#({' '.join(map(str, perception))})"
        
        scheme_code = f"""
        (use-modules (metamodel aar-core))
        (aar-perceive aar-core {perception_str})
        (display "Perception processed")
        """
        
        result = self.scheme.eval_scheme(scheme_code)
        return result is not None
    
    def aar_get_state(self) -> Optional[Dict[str, float]]:
        """
        Get current AAR state
        
        Returns:
            Dictionary with coherence, emergence, etc.
        """
        scheme_code = """
        (use-modules (metamodel aar-core))
        (define state (aar-self-state aar-core))
        (display (format #f "coherence:~a emergence:~a" 
                        (assoc-ref state 'coherence)
                        (assoc-ref state 'emergence)))
        """
        
        result = self.scheme.eval_scheme(scheme_code)
        
        if result:
            # Parse result (simple format: "coherence:0.5 emergence:0.3")
            parts = result.split()
            state = {}
            for part in parts:
                key, value = part.split(':')
                state[key] = float(value)
            return state
        
        return None
    
    def create_cognitive_kernel(self, dimension: int = 32) -> bool:
        """
        Create cognitive kernel
        
        Args:
            dimension: State vector dimension
            
        Returns:
            Success status
        """
        if not self.cognitive_kernel_file.exists():
            self.logger.warning(
                f"Cognitive kernel file not found: {self.cognitive_kernel_file}"
            )
            return False
        
        scheme_code = f"""
        (use-modules (metamodel cognitive-kernel))
        (define kernel (make-cognitive-kernel #:dimension {dimension}))
        (display "Cognitive kernel created")
        """
        
        result = self.scheme.eval_scheme(
            scheme_code,
            load_files=[str(self.cognitive_kernel_file)]
        )
        
        return result is not None
    
    def kernel_perceive(self, input_text: str) -> bool:
        """
        Send perception to cognitive kernel
        
        Args:
            input_text: Text input to perceive
            
        Returns:
            Success status
        """
        scheme_code = f"""
        (use-modules (metamodel cognitive-kernel))
        (perceive kernel "{input_text}")
        (display "Kernel perception processed")
        """
        
        result = self.scheme.eval_scheme(scheme_code)
        return result is not None
    
    def kernel_recall(self, query: str) -> Optional[List[str]]:
        """
        Recall memories from cognitive kernel
        
        Args:
            query: Query string
            
        Returns:
            List of recalled memories
        """
        scheme_code = f"""
        (use-modules (metamodel cognitive-kernel))
        (define memories (recall-memory kernel "{query}"))
        (display memories)
        """
        
        result = self.scheme.eval_scheme(scheme_code)
        
        if result:
            # Parse Scheme list result
            # This is simplified - real implementation needs proper parsing
            return result.strip('()').split()
        
        return None


def demonstrate_ffi_bridge():
    """Demonstrate Scheme-Python FFI bridge"""
    print("=" * 60)
    print("Scheme-Python FFI Bridge Demonstration")
    print("=" * 60)
    print()
    
    # Create bridge
    bridge = MetamodelBridge()
    
    # Test Scheme interpreter
    print("1. Testing Scheme interpreter...")
    scheme = SchemeInterpreter()
    result = scheme.eval_scheme("(+ 1 2 3)")
    print(f"   (+ 1 2 3) = {result}")
    print()
    
    # Test function call
    print("2. Testing Scheme function call...")
    result = scheme.call_scheme_function("*", [6, 7])
    print(f"   (* 6 7) = {result}")
    print()
    
    # Test metamodel integration
    print("3. Testing metamodel integration...")
    
    # Check if metamodel files exist
    metamodel_path = Path("/home/ubuntu/occ/metamodel")
    if metamodel_path.exists():
        print(f"   Metamodel path: {metamodel_path}")
        print(f"   Files found:")
        for file in metamodel_path.glob("*.scm"):
            print(f"     - {file.name}")
    else:
        print(f"   Warning: Metamodel path not found: {metamodel_path}")
    
    print()
    print("=" * 60)
    print("FFI Bridge demonstration complete!")
    print("=" * 60)


if __name__ == '__main__':
    demonstrate_ffi_bridge()
