#!/usr/bin/env python3
"""
Hypergraph Synergy Bridge

Enables seamless data flow between AtomSpace (C++) and Hyperon (Rust)
components through a unified hypergraph interface.

This bridge implements serialization/deserialization and provides
a Python API for cross-language hypergraph operations.
"""

import json
import logging
from typing import Dict, List, Any, Optional, Union
from dataclasses import dataclass, asdict
from enum import Enum
from pathlib import Path


logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


class AtomType(Enum):
    """Atom types in the hypergraph."""
    NODE = "Node"
    LINK = "Link"
    CONCEPT = "ConceptNode"
    PREDICATE = "PredicateNode"
    EVALUATION = "EvaluationLink"
    INHERITANCE = "InheritanceLink"
    SIMILARITY = "SimilarityLink"
    LIST = "ListLink"


@dataclass
class Atom:
    """
    Unified representation of an Atom.
    
    Compatible with both AtomSpace (C++) and Hyperon (Rust) formats.
    """
    atom_type: str
    name: Optional[str] = None
    outgoing: Optional[List['Atom']] = None
    truth_value: Optional[Dict[str, float]] = None
    attention_value: Optional[Dict[str, float]] = None
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary representation."""
        result = {'type': self.atom_type}
        if self.name:
            result['name'] = self.name
        if self.outgoing:
            result['outgoing'] = [atom.to_dict() for atom in self.outgoing]
        if self.truth_value:
            result['truth_value'] = self.truth_value
        if self.attention_value:
            result['attention_value'] = self.attention_value
        return result
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'Atom':
        """Create Atom from dictionary representation."""
        outgoing = None
        if 'outgoing' in data:
            outgoing = [cls.from_dict(atom_data) for atom_data in data['outgoing']]
        
        return cls(
            atom_type=data['type'],
            name=data.get('name'),
            outgoing=outgoing,
            truth_value=data.get('truth_value'),
            attention_value=data.get('attention_value')
        )
    
    def to_scheme(self) -> str:
        """Convert to Scheme representation for AtomSpace."""
        if self.name and not self.outgoing:
            # Node
            tv_str = ""
            if self.truth_value:
                strength = self.truth_value.get('strength', 1.0)
                confidence = self.truth_value.get('confidence', 1.0)
                tv_str = f" (stv {strength} {confidence})"
            return f"({self.atom_type} \"{self.name}\"{tv_str})"
        elif self.outgoing:
            # Link
            outgoing_str = " ".join(atom.to_scheme() for atom in self.outgoing)
            tv_str = ""
            if self.truth_value:
                strength = self.truth_value.get('strength', 1.0)
                confidence = self.truth_value.get('confidence', 1.0)
                tv_str = f" (stv {strength} {confidence})"
            return f"({self.atom_type} {outgoing_str}{tv_str})"
        else:
            return f"({self.atom_type})"
    
    def to_metta(self) -> str:
        """Convert to MeTTa representation for Hyperon."""
        if self.name and not self.outgoing:
            # Atom
            return f"({self.atom_type} {self.name})"
        elif self.outgoing:
            # Expression
            outgoing_str = " ".join(atom.to_metta() for atom in self.outgoing)
            return f"({self.atom_type} {outgoing_str})"
        else:
            return f"({self.atom_type})"


class HypergraphBridge:
    """
    Bridge between AtomSpace and Hyperon hypergraph representations.
    
    Provides unified API for cross-component hypergraph operations.
    """
    
    def __init__(self, atomspace_path: Optional[Path] = None, hyperon_path: Optional[Path] = None):
        self.atomspace_path = atomspace_path
        self.hyperon_path = hyperon_path
        self.atom_cache: Dict[str, Atom] = {}
        logger.info("Hypergraph Bridge initialized")
    
    def create_concept(self, name: str, truth_value: Optional[Dict[str, float]] = None) -> Atom:
        """Create a concept node."""
        atom = Atom(
            atom_type=AtomType.CONCEPT.value,
            name=name,
            truth_value=truth_value or {'strength': 1.0, 'confidence': 1.0}
        )
        self.atom_cache[name] = atom
        return atom
    
    def create_predicate(self, name: str) -> Atom:
        """Create a predicate node."""
        atom = Atom(
            atom_type=AtomType.PREDICATE.value,
            name=name
        )
        return atom
    
    def create_evaluation(self, predicate: Atom, *args: Atom, truth_value: Optional[Dict[str, float]] = None) -> Atom:
        """Create an evaluation link."""
        list_link = Atom(
            atom_type=AtomType.LIST.value,
            outgoing=list(args)
        )
        
        atom = Atom(
            atom_type=AtomType.EVALUATION.value,
            outgoing=[predicate, list_link],
            truth_value=truth_value
        )
        return atom
    
    def create_inheritance(self, child: Atom, parent: Atom, truth_value: Optional[Dict[str, float]] = None) -> Atom:
        """Create an inheritance link."""
        atom = Atom(
            atom_type=AtomType.INHERITANCE.value,
            outgoing=[child, parent],
            truth_value=truth_value
        )
        return atom
    
    def create_similarity(self, concept1: Atom, concept2: Atom, truth_value: Optional[Dict[str, float]] = None) -> Atom:
        """Create a similarity link."""
        atom = Atom(
            atom_type=AtomType.SIMILARITY.value,
            outgoing=[concept1, concept2],
            truth_value=truth_value
        )
        return atom
    
    def serialize_to_json(self, atoms: List[Atom], output_path: Path):
        """Serialize atoms to JSON format."""
        data = {
            'version': '1.0',
            'atoms': [atom.to_dict() for atom in atoms]
        }
        with open(output_path, 'w') as f:
            json.dump(data, f, indent=2)
        logger.info(f"Serialized {len(atoms)} atoms to {output_path}")
    
    def deserialize_from_json(self, input_path: Path) -> List[Atom]:
        """Deserialize atoms from JSON format."""
        with open(input_path, 'r') as f:
            data = json.load(f)
        
        atoms = [Atom.from_dict(atom_data) for atom_data in data['atoms']]
        logger.info(f"Deserialized {len(atoms)} atoms from {input_path}")
        return atoms
    
    def export_to_scheme(self, atoms: List[Atom], output_path: Path):
        """Export atoms to Scheme format for AtomSpace."""
        with open(output_path, 'w') as f:
            f.write(";; Generated by Hypergraph Bridge\n")
            f.write(";; Compatible with OpenCog AtomSpace\n\n")
            for atom in atoms:
                f.write(atom.to_scheme() + "\n")
        logger.info(f"Exported {len(atoms)} atoms to Scheme format: {output_path}")
    
    def export_to_metta(self, atoms: List[Atom], output_path: Path):
        """Export atoms to MeTTa format for Hyperon."""
        with open(output_path, 'w') as f:
            f.write("; Generated by Hypergraph Bridge\n")
            f.write("; Compatible with Hyperon MeTTa\n\n")
            for atom in atoms:
                f.write(atom.to_metta() + "\n")
        logger.info(f"Exported {len(atoms)} atoms to MeTTa format: {output_path}")
    
    def transfer_atomspace_to_hyperon(self, atoms: List[Atom]) -> List[Atom]:
        """
        Transfer atoms from AtomSpace format to Hyperon format.
        
        Performs any necessary transformations for compatibility.
        """
        # For now, atoms are already in unified format
        # In a real implementation, this would handle format-specific conversions
        logger.info(f"Transferring {len(atoms)} atoms from AtomSpace to Hyperon")
        return atoms
    
    def transfer_hyperon_to_atomspace(self, atoms: List[Atom]) -> List[Atom]:
        """
        Transfer atoms from Hyperon format to AtomSpace format.
        
        Performs any necessary transformations for compatibility.
        """
        logger.info(f"Transferring {len(atoms)} atoms from Hyperon to AtomSpace")
        return atoms
    
    def validate_atom(self, atom: Atom) -> bool:
        """Validate atom structure."""
        if not atom.atom_type:
            logger.error("Atom missing type")
            return False
        
        # Nodes must have names
        if atom.atom_type.endswith('Node') and not atom.name:
            logger.error(f"Node {atom.atom_type} missing name")
            return False
        
        # Links must have outgoing
        if atom.atom_type.endswith('Link') and not atom.outgoing:
            logger.error(f"Link {atom.atom_type} missing outgoing")
            return False
        
        return True
    
    def get_statistics(self, atoms: List[Atom]) -> Dict[str, Any]:
        """Get statistics about a collection of atoms."""
        stats = {
            'total_atoms': len(atoms),
            'nodes': 0,
            'links': 0,
            'types': {},
            'with_truth_values': 0,
            'with_attention_values': 0
        }
        
        for atom in atoms:
            if atom.atom_type.endswith('Node'):
                stats['nodes'] += 1
            elif atom.atom_type.endswith('Link'):
                stats['links'] += 1
            
            stats['types'][atom.atom_type] = stats['types'].get(atom.atom_type, 0) + 1
            
            if atom.truth_value:
                stats['with_truth_values'] += 1
            if atom.attention_value:
                stats['with_attention_values'] += 1
        
        return stats


def main():
    """Example usage of the Hypergraph Bridge."""
    bridge = HypergraphBridge()
    
    # Create some atoms
    cat = bridge.create_concept('cat', {'strength': 0.9, 'confidence': 0.8})
    animal = bridge.create_concept('animal', {'strength': 1.0, 'confidence': 1.0})
    mammal = bridge.create_concept('mammal', {'strength': 1.0, 'confidence': 0.9})
    
    # Create relationships
    cat_is_mammal = bridge.create_inheritance(cat, mammal, {'strength': 0.95, 'confidence': 0.9})
    mammal_is_animal = bridge.create_inheritance(mammal, animal, {'strength': 1.0, 'confidence': 1.0})
    
    # Create a predicate evaluation
    has_fur = bridge.create_predicate('has_fur')
    cat_has_fur = bridge.create_evaluation(has_fur, cat, truth_value={'strength': 0.99, 'confidence': 0.95})
    
    atoms = [cat, animal, mammal, cat_is_mammal, mammal_is_animal, has_fur, cat_has_fur]
    
    # Get statistics
    stats = bridge.get_statistics(atoms)
    print(f"Statistics: {json.dumps(stats, indent=2)}")
    
    # Export to different formats
    output_dir = Path('/tmp/hypergraph_bridge')
    output_dir.mkdir(exist_ok=True)
    
    bridge.serialize_to_json(atoms, output_dir / 'atoms.json')
    bridge.export_to_scheme(atoms, output_dir / 'atoms.scm')
    bridge.export_to_metta(atoms, output_dir / 'atoms.metta')
    
    print(f"\nExported atoms to {output_dir}")
    print("\nScheme representation:")
    for atom in atoms:
        print(f"  {atom.to_scheme()}")
    
    print("\nMeTTa representation:")
    for atom in atoms:
        print(f"  {atom.to_metta()}")


if __name__ == '__main__':
    main()
