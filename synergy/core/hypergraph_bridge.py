#!/usr/bin/env python3
"""
Hypergraph Synergy Bridge

Provides seamless data flow between AtomSpace (C++) and Hyperon (Rust)
components, enabling true cognitive synergy through unified hypergraph operations.

This module implements the core data structure integration for the
OpenCog Collection's cognitive architecture.
"""

import json
import logging
from typing import Dict, List, Any, Optional, Union
from dataclasses import dataclass, asdict
from enum import Enum


logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


class AtomType(Enum):
    """Common atom types across AtomSpace and Hyperon"""
    CONCEPT = "ConceptNode"
    PREDICATE = "PredicateNode"
    LINK = "Link"
    EVALUATION = "EvaluationLink"
    INHERITANCE = "InheritanceLink"
    SIMILARITY = "SimilarityLink"
    EXECUTION = "ExecutionLink"
    VARIABLE = "VariableNode"
    NUMBER = "NumberNode"
    
    def __str__(self):
        return self.value


@dataclass
class Atom:
    """
    Unified representation of an atom that can be serialized
    between AtomSpace (C++) and Hyperon (Rust).
    """
    atom_type: AtomType
    name: Optional[str] = None
    value: Optional[Union[str, float, int]] = None
    outgoing: List['Atom'] = None
    truth_value: Optional[Dict[str, float]] = None
    attention_value: Optional[Dict[str, float]] = None
    
    def __post_init__(self):
        if self.outgoing is None:
            self.outgoing = []
        if self.truth_value is None:
            self.truth_value = {'strength': 1.0, 'confidence': 1.0}
        if self.attention_value is None:
            self.attention_value = {'sti': 0.0, 'lti': 0.0, 'vlti': 0.0}
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert atom to dictionary for serialization"""
        return {
            'atom_type': self.atom_type.value,
            'name': self.name,
            'value': self.value,
            'outgoing': [a.to_dict() for a in self.outgoing],
            'truth_value': self.truth_value,
            'attention_value': self.attention_value
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'Atom':
        """Create atom from dictionary"""
        return cls(
            atom_type=AtomType(data['atom_type']),
            name=data.get('name'),
            value=data.get('value'),
            outgoing=[cls.from_dict(a) for a in data.get('outgoing', [])],
            truth_value=data.get('truth_value'),
            attention_value=data.get('attention_value')
        )
    
    def to_scheme(self) -> str:
        """Convert atom to Scheme representation for AtomSpace"""
        if self.name:
            # Node
            tv_str = ""
            if self.truth_value:
                s = self.truth_value.get('strength', 1.0)
                c = self.truth_value.get('confidence', 1.0)
                tv_str = f" (stv {s} {c})"
            return f"({self.atom_type} \"{self.name}\"{tv_str})"
        else:
            # Link
            outgoing_str = " ".join(a.to_scheme() for a in self.outgoing)
            return f"({self.atom_type} {outgoing_str})"
    
    def to_metta(self) -> str:
        """Convert atom to MeTTa representation for Hyperon"""
        if self.name:
            # Node/Symbol
            return f"{self.name}"
        else:
            # Expression
            if self.atom_type == AtomType.LINK:
                elements = " ".join(a.to_metta() for a in self.outgoing)
                return f"({elements})"
            else:
                # Typed expression
                elements = " ".join(a.to_metta() for a in self.outgoing)
                return f"({self.atom_type.value} {elements})"


class HypergraphBridge:
    """
    Bridge for seamless data flow between AtomSpace and Hyperon.
    
    Provides:
    - Serialization/deserialization of atoms
    - Format conversion (Scheme <-> MeTTa)
    - Data consistency validation
    - Synchronization support
    """
    
    def __init__(self):
        self.atom_cache: Dict[str, Atom] = {}
        self.conversion_count = 0
        logger.info("Hypergraph Bridge initialized")
    
    def create_concept(self, name: str, truth_value: Dict[str, float] = None) -> Atom:
        """Create a concept node"""
        return Atom(
            atom_type=AtomType.CONCEPT,
            name=name,
            truth_value=truth_value
        )
    
    def create_predicate(self, name: str) -> Atom:
        """Create a predicate node"""
        return Atom(
            atom_type=AtomType.PREDICATE,
            name=name
        )
    
    def create_link(
        self,
        link_type: AtomType,
        outgoing: List[Atom],
        truth_value: Dict[str, float] = None
    ) -> Atom:
        """Create a link between atoms"""
        return Atom(
            atom_type=link_type,
            outgoing=outgoing,
            truth_value=truth_value
        )
    
    def create_evaluation(
        self,
        predicate: Atom,
        arguments: List[Atom],
        truth_value: Dict[str, float] = None
    ) -> Atom:
        """Create an evaluation link"""
        return Atom(
            atom_type=AtomType.EVALUATION,
            outgoing=[predicate] + arguments,
            truth_value=truth_value
        )
    
    def create_inheritance(
        self,
        child: Atom,
        parent: Atom,
        truth_value: Dict[str, float] = None
    ) -> Atom:
        """Create an inheritance link"""
        return Atom(
            atom_type=AtomType.INHERITANCE,
            outgoing=[child, parent],
            truth_value=truth_value
        )
    
    def serialize_to_json(self, atom: Atom) -> str:
        """Serialize atom to JSON string"""
        return json.dumps(atom.to_dict(), indent=2)
    
    def deserialize_from_json(self, json_str: str) -> Atom:
        """Deserialize atom from JSON string"""
        data = json.loads(json_str)
        return Atom.from_dict(data)
    
    def convert_to_atomspace(self, atom: Atom) -> str:
        """
        Convert atom to AtomSpace Scheme format.
        
        Returns Scheme code that can be executed in AtomSpace.
        """
        self.conversion_count += 1
        scheme_code = atom.to_scheme()
        logger.debug(f"Converted to AtomSpace: {scheme_code}")
        return scheme_code
    
    def convert_to_hyperon(self, atom: Atom) -> str:
        """
        Convert atom to Hyperon MeTTa format.
        
        Returns MeTTa code that can be executed in Hyperon.
        """
        self.conversion_count += 1
        metta_code = atom.to_metta()
        logger.debug(f"Converted to Hyperon: {metta_code}")
        return metta_code
    
    def validate_atom(self, atom: Atom) -> bool:
        """
        Validate atom structure and consistency.
        
        Checks:
        - Nodes have names
        - Links have outgoing atoms
        - Truth values are in valid range [0, 1]
        """
        # Nodes must have names
        if atom.name is None and not atom.outgoing:
            logger.error("Invalid atom: neither name nor outgoing set")
            return False
        
        # Validate truth value
        if atom.truth_value:
            s = atom.truth_value.get('strength', 1.0)
            c = atom.truth_value.get('confidence', 1.0)
            if not (0 <= s <= 1 and 0 <= c <= 1):
                logger.error(f"Invalid truth value: strength={s}, confidence={c}")
                return False
        
        # Recursively validate outgoing atoms
        for outgoing_atom in atom.outgoing:
            if not self.validate_atom(outgoing_atom):
                return False
        
        return True
    
    def cache_atom(self, key: str, atom: Atom):
        """Cache an atom for later retrieval"""
        self.atom_cache[key] = atom
        logger.debug(f"Cached atom: {key}")
    
    def get_cached_atom(self, key: str) -> Optional[Atom]:
        """Retrieve a cached atom"""
        return self.atom_cache.get(key)
    
    def clear_cache(self):
        """Clear the atom cache"""
        self.atom_cache.clear()
        logger.info("Atom cache cleared")
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get bridge statistics"""
        return {
            'conversion_count': self.conversion_count,
            'cached_atoms': len(self.atom_cache),
            'cache_keys': list(self.atom_cache.keys())
        }


# Singleton instance
_bridge_instance: Optional[HypergraphBridge] = None


def get_bridge() -> HypergraphBridge:
    """Get the global bridge instance"""
    global _bridge_instance
    if _bridge_instance is None:
        _bridge_instance = HypergraphBridge()
    return _bridge_instance


if __name__ == "__main__":
    # Example usage
    bridge = get_bridge()
    
    # Create some atoms
    cat = bridge.create_concept("cat", truth_value={'strength': 0.9, 'confidence': 0.8})
    animal = bridge.create_concept("animal")
    
    # Create inheritance link
    inheritance = bridge.create_inheritance(
        cat,
        animal,
        truth_value={'strength': 0.95, 'confidence': 0.9}
    )
    
    # Convert to different formats
    print("=== Scheme (AtomSpace) ===")
    print(bridge.convert_to_atomspace(inheritance))
    print()
    
    print("=== MeTTa (Hyperon) ===")
    print(bridge.convert_to_hyperon(inheritance))
    print()
    
    # Serialize to JSON
    print("=== JSON ===")
    json_str = bridge.serialize_to_json(inheritance)
    print(json_str)
    print()
    
    # Deserialize and validate
    restored = bridge.deserialize_from_json(json_str)
    print(f"Validation: {bridge.validate_atom(restored)}")
    print()
    
    # Statistics
    print("=== Statistics ===")
    print(json.dumps(bridge.get_statistics(), indent=2))
