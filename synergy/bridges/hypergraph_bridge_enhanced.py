#!/usr/bin/env python3
"""
Enhanced Hypergraph Bridge

Improved bridge for seamless data flow between AtomSpace (C++) and Hyperon (Rust)
components with better error handling, validation, and performance monitoring.
"""

import json
import time
import logging
from typing import Dict, List, Any, Optional, Tuple
from dataclasses import dataclass, asdict
from enum import Enum
from pathlib import Path

logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


class AtomType(Enum):
    """Atom types for hypergraph nodes."""
    CONCEPT = "ConceptNode"
    PREDICATE = "PredicateNode"
    VARIABLE = "VariableNode"
    NUMBER = "NumberNode"
    LINK = "Link"
    EVALUATION = "EvaluationLink"
    INHERITANCE = "InheritanceLink"
    SIMILARITY = "SimilarityLink"


@dataclass
class Atom:
    """Represents a hypergraph atom."""
    atom_type: str
    name: str
    truth_value: Optional[Tuple[float, float]] = None  # (strength, confidence)
    attention_value: Optional[Tuple[float, float, float]] = None  # (sti, lti, vlti)
    outgoing: Optional[List['Atom']] = None
    metadata: Optional[Dict[str, Any]] = None
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary representation."""
        result = {
            'type': self.atom_type,
            'name': self.name
        }
        if self.truth_value:
            result['tv'] = {'strength': self.truth_value[0], 'confidence': self.truth_value[1]}
        if self.attention_value:
            result['av'] = {
                'sti': self.attention_value[0],
                'lti': self.attention_value[1],
                'vlti': self.attention_value[2]
            }
        if self.outgoing:
            result['outgoing'] = [atom.to_dict() for atom in self.outgoing]
        if self.metadata:
            result['metadata'] = self.metadata
        return result
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'Atom':
        """Create Atom from dictionary representation."""
        tv = None
        if 'tv' in data:
            tv = (data['tv']['strength'], data['tv']['confidence'])
        
        av = None
        if 'av' in data:
            av = (data['av']['sti'], data['av']['lti'], data['av']['vlti'])
        
        outgoing = None
        if 'outgoing' in data:
            outgoing = [cls.from_dict(atom_data) for atom_data in data['outgoing']]
        
        return cls(
            atom_type=data['type'],
            name=data['name'],
            truth_value=tv,
            attention_value=av,
            outgoing=outgoing,
            metadata=data.get('metadata')
        )
    
    def to_scheme(self) -> str:
        """Convert to Scheme representation for AtomSpace."""
        if self.outgoing:
            # Link with outgoing atoms
            outgoing_scheme = ' '.join(atom.to_scheme() for atom in self.outgoing)
            scheme = f"({self.atom_type} {outgoing_scheme})"
        else:
            # Node
            scheme = f'({self.atom_type} "{self.name}")'
        
        # Add truth value if present
        if self.truth_value:
            scheme += f" (stv {self.truth_value[0]} {self.truth_value[1]})"
        
        return scheme
    
    def to_metta(self) -> str:
        """Convert to MeTTa representation for Hyperon."""
        if self.outgoing:
            # Expression with outgoing atoms
            outgoing_metta = ' '.join(atom.to_metta() for atom in self.outgoing)
            return f"({self.atom_type} {outgoing_metta})"
        else:
            # Symbol or value
            return f"{self.name}"


class HypergraphBridge:
    """
    Enhanced bridge for cross-component hypergraph operations.
    
    Provides seamless data flow between AtomSpace (C++) and Hyperon (Rust)
    with validation, error handling, and performance monitoring.
    """
    
    def __init__(self, cache_dir: Optional[Path] = None):
        self.cache_dir = cache_dir or Path("/tmp/hypergraph_bridge")
        self.cache_dir.mkdir(parents=True, exist_ok=True)
        
        self.transfer_count = 0
        self.error_count = 0
        self.total_latency = 0.0
        
        logger.info(f"Hypergraph Bridge initialized with cache: {self.cache_dir}")
    
    def atomspace_to_hyperon(self, atoms: List[Atom], validate: bool = True) -> Tuple[List[str], Dict[str, Any]]:
        """
        Convert AtomSpace atoms to Hyperon MeTTa format.
        
        Args:
            atoms: List of Atom objects
            validate: Whether to validate the conversion
            
        Returns:
            Tuple of (metta_expressions, metadata)
        """
        start_time = time.time()
        
        try:
            # Convert to MeTTa
            metta_expressions = []
            for atom in atoms:
                try:
                    metta_expr = atom.to_metta()
                    metta_expressions.append(metta_expr)
                except Exception as e:
                    logger.error(f"Error converting atom to MeTTa: {e}")
                    self.error_count += 1
                    if validate:
                        raise
            
            # Validation
            if validate:
                self._validate_metta_expressions(metta_expressions)
            
            # Update metrics
            latency = time.time() - start_time
            self.transfer_count += 1
            self.total_latency += latency
            
            metadata = {
                'atom_count': len(atoms),
                'latency_ms': latency * 1000,
                'timestamp': time.time(),
                'direction': 'atomspace_to_hyperon'
            }
            
            logger.info(f"Converted {len(atoms)} atoms to MeTTa in {latency*1000:.2f}ms")
            
            return metta_expressions, metadata
            
        except Exception as e:
            logger.error(f"Error in atomspace_to_hyperon: {e}")
            self.error_count += 1
            raise
    
    def hyperon_to_atomspace(self, metta_expressions: List[str], validate: bool = True) -> Tuple[List[Atom], Dict[str, Any]]:
        """
        Convert Hyperon MeTTa expressions to AtomSpace atoms.
        
        Args:
            metta_expressions: List of MeTTa expression strings
            validate: Whether to validate the conversion
            
        Returns:
            Tuple of (atoms, metadata)
        """
        start_time = time.time()
        
        try:
            # Parse MeTTa expressions to Atoms
            atoms = []
            for expr in metta_expressions:
                try:
                    atom = self._parse_metta_to_atom(expr)
                    atoms.append(atom)
                except Exception as e:
                    logger.error(f"Error parsing MeTTa expression: {e}")
                    self.error_count += 1
                    if validate:
                        raise
            
            # Validation
            if validate:
                self._validate_atoms(atoms)
            
            # Update metrics
            latency = time.time() - start_time
            self.transfer_count += 1
            self.total_latency += latency
            
            metadata = {
                'atom_count': len(atoms),
                'latency_ms': latency * 1000,
                'timestamp': time.time(),
                'direction': 'hyperon_to_atomspace'
            }
            
            logger.info(f"Converted {len(metta_expressions)} MeTTa expressions to atoms in {latency*1000:.2f}ms")
            
            return atoms, metadata
            
        except Exception as e:
            logger.error(f"Error in hyperon_to_atomspace: {e}")
            self.error_count += 1
            raise
    
    def save_to_cache(self, atoms: List[Atom], cache_key: str) -> Path:
        """Save atoms to cache for later retrieval."""
        cache_file = self.cache_dir / f"{cache_key}.json"
        
        try:
            data = {
                'atoms': [atom.to_dict() for atom in atoms],
                'timestamp': time.time(),
                'count': len(atoms)
            }
            
            with open(cache_file, 'w') as f:
                json.dump(data, f, indent=2)
            
            logger.info(f"Saved {len(atoms)} atoms to cache: {cache_key}")
            return cache_file
            
        except Exception as e:
            logger.error(f"Error saving to cache: {e}")
            raise
    
    def load_from_cache(self, cache_key: str) -> Optional[List[Atom]]:
        """Load atoms from cache."""
        cache_file = self.cache_dir / f"{cache_key}.json"
        
        if not cache_file.exists():
            logger.warning(f"Cache file not found: {cache_key}")
            return None
        
        try:
            with open(cache_file, 'r') as f:
                data = json.load(f)
            
            atoms = [Atom.from_dict(atom_data) for atom_data in data['atoms']]
            logger.info(f"Loaded {len(atoms)} atoms from cache: {cache_key}")
            return atoms
            
        except Exception as e:
            logger.error(f"Error loading from cache: {e}")
            return None
    
    def get_metrics(self) -> Dict[str, Any]:
        """Get bridge performance metrics."""
        avg_latency = self.total_latency / self.transfer_count if self.transfer_count > 0 else 0
        
        return {
            'transfer_count': self.transfer_count,
            'error_count': self.error_count,
            'avg_latency_ms': avg_latency * 1000,
            'total_latency_s': self.total_latency,
            'error_rate': self.error_count / max(self.transfer_count, 1)
        }
    
    def _parse_metta_to_atom(self, expr: str) -> Atom:
        """Parse a MeTTa expression to an Atom (simplified parser)."""
        # This is a simplified parser - a full implementation would use proper parsing
        expr = expr.strip()
        
        if expr.startswith('(') and expr.endswith(')'):
            # Link expression
            inner = expr[1:-1].strip()
            parts = inner.split(None, 1)
            
            if len(parts) == 2:
                atom_type, rest = parts
                # For now, create a simple link
                return Atom(
                    atom_type=atom_type,
                    name=rest,
                    metadata={'source': 'metta'}
                )
            else:
                return Atom(
                    atom_type='Expression',
                    name=inner,
                    metadata={'source': 'metta'}
                )
        else:
            # Simple symbol
            return Atom(
                atom_type='Symbol',
                name=expr,
                metadata={'source': 'metta'}
            )
    
    def _validate_metta_expressions(self, expressions: List[str]):
        """Validate MeTTa expressions."""
        for expr in expressions:
            if not expr or not isinstance(expr, str):
                raise ValueError(f"Invalid MeTTa expression: {expr}")
    
    def _validate_atoms(self, atoms: List[Atom]):
        """Validate Atom objects."""
        for atom in atoms:
            if not atom.atom_type or not atom.name:
                raise ValueError(f"Invalid atom: missing type or name")


def main():
    """Example usage of the enhanced hypergraph bridge."""
    bridge = HypergraphBridge()
    
    # Create some example atoms
    atoms = [
        Atom(
            atom_type=AtomType.CONCEPT.value,
            name="cognitive-synergy",
            truth_value=(0.9, 0.95),
            metadata={'created': time.time()}
        ),
        Atom(
            atom_type=AtomType.CONCEPT.value,
            name="hypergraph",
            truth_value=(0.85, 0.9)
        ),
        Atom(
            atom_type=AtomType.INHERITANCE.value,
            name="isa",
            outgoing=[
                Atom(AtomType.CONCEPT.value, "atomspace"),
                Atom(AtomType.CONCEPT.value, "hypergraph")
            ]
        )
    ]
    
    # Convert to MeTTa
    metta_exprs, metadata = bridge.atomspace_to_hyperon(atoms)
    print(f"\nMeTTa expressions:")
    for expr in metta_exprs:
        print(f"  {expr}")
    
    # Convert to Scheme
    print(f"\nScheme expressions:")
    for atom in atoms:
        print(f"  {atom.to_scheme()}")
    
    # Save to cache
    cache_file = bridge.save_to_cache(atoms, "example_atoms")
    print(f"\nSaved to cache: {cache_file}")
    
    # Load from cache
    loaded_atoms = bridge.load_from_cache("example_atoms")
    print(f"\nLoaded {len(loaded_atoms)} atoms from cache")
    
    # Get metrics
    metrics = bridge.get_metrics()
    print(f"\nBridge metrics:")
    for key, value in metrics.items():
        print(f"  {key}: {value}")


if __name__ == '__main__':
    main()
