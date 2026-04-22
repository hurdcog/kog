"""
AtomSpace Bridge for GNN Integration
Provides interface between Python GNN module and C++ AtomSpace

This bridge supports:
- Extracting atoms from AtomSpace
- Converting atom data to GNN-compatible format
- Writing GNN results back to AtomSpace
- Integration with CogGML shards
"""

import logging
from typing import Dict, List, Any, Optional, Callable
from dataclasses import dataclass
from enum import Enum

logger = logging.getLogger(__name__)


class AtomType(Enum):
    """Types of atoms in the AtomSpace"""
    NODE = "Node"
    LINK = "Link"
    CONCEPT = "ConceptNode"
    PREDICATE = "PredicateNode"
    EVALUATION = "EvaluationLink"
    INHERITANCE = "InheritanceLink"
    SIMILARITY = "SimilarityLink"
    EXECUTION = "ExecutionLink"
    LIST = "ListLink"
    SET = "SetLink"
    # Diagnostic types for autognostic integration
    DIAGNOSTIC = "DiagnosticNode"
    ERROR = "ErrorNode"
    PREDICTION_MISMATCH = "PredictionMismatchNode"
    STALL = "StallNode"


@dataclass
class AtomData:
    """Represents atom data extracted from AtomSpace"""
    atom_id: str
    atom_type: str
    name: str
    truth_value: float = 1.0
    confidence: float = 1.0
    attention_value: float = 0.0
    incoming: List[str] = None
    outgoing: List[str] = None
    
    def __post_init__(self):
        if self.incoming is None:
            self.incoming = []
        if self.outgoing is None:
            self.outgoing = []
            
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary format for GNN"""
        return {
            'type': self.atom_type,
            'name': self.name,
            'truth_value': self.truth_value,
            'confidence': self.confidence,
            'attention_value': self.attention_value,
            'incoming': self.incoming,
            'outgoing': self.outgoing
        }


class AtomSpaceBridge:
    """
    Bridge between Python GNN module and C++ AtomSpace
    
    Supports multiple backend modes:
    - 'mock': In-memory mock AtomSpace for testing
    - 'cython': Cython bindings to C++ AtomSpace
    - 'grpc': gRPC connection to AtomSpace server
    - 'cogserver': Connection via CogServer REST API
    """
    
    def __init__(self, backend: str = 'mock', config: Optional[Dict[str, Any]] = None):
        self.backend = backend
        self.config = config or {}
        
        # Mock AtomSpace for testing
        self._mock_atomspace: Dict[str, AtomData] = {}
        
        # Backend connection
        self._connection = None
        
        # Callbacks for CogGML integration
        self._on_atom_added: Optional[Callable] = None
        self._on_atom_removed: Optional[Callable] = None
        
        self._initialize_backend()
        logger.info(f"AtomSpaceBridge initialized with backend: {backend}")
        
    def _initialize_backend(self):
        """Initialize the backend connection"""
        if self.backend == 'mock':
            logger.info("Using mock AtomSpace backend")
        elif self.backend == 'cython':
            self._init_cython_backend()
        elif self.backend == 'grpc':
            self._init_grpc_backend()
        elif self.backend == 'cogserver':
            self._init_cogserver_backend()
        else:
            logger.warning(f"Unknown backend: {self.backend}, using mock")
            self.backend = 'mock'
            
    def _init_cython_backend(self):
        """Initialize Cython bindings to C++ AtomSpace"""
        try:
            # Try to import OpenCog Python bindings
            from opencog.atomspace import AtomSpace, TruthValue
            from opencog.type_constructors import ConceptNode, PredicateNode
            self._connection = AtomSpace()
            logger.info("Cython AtomSpace backend initialized")
        except ImportError:
            logger.warning("OpenCog Python bindings not available, falling back to mock")
            self.backend = 'mock'
            
    def _init_grpc_backend(self):
        """Initialize gRPC connection to AtomSpace server"""
        host = self.config.get('grpc_host', 'localhost')
        port = self.config.get('grpc_port', 50051)
        logger.info(f"gRPC backend configured for {host}:{port}")
        # gRPC client would be initialized here
        
    def _init_cogserver_backend(self):
        """Initialize CogServer REST API connection"""
        url = self.config.get('cogserver_url', 'http://localhost:17001')
        logger.info(f"CogServer backend configured for {url}")
        # REST client would be initialized here
        
    def add_atom(self, atom_type: str, name: str, **kwargs) -> str:
        """
        Add an atom to the AtomSpace
        
        Args:
            atom_type: Type of atom (e.g., 'ConceptNode')
            name: Name of the atom
            **kwargs: Additional properties (truth_value, confidence, etc.)
            
        Returns:
            Atom ID
        """
        atom_id = f"{atom_type}_{name}_{id(name)}"
        
        atom_data = AtomData(
            atom_id=atom_id,
            atom_type=atom_type,
            name=name,
            truth_value=kwargs.get('truth_value', 1.0),
            confidence=kwargs.get('confidence', 1.0),
            attention_value=kwargs.get('attention_value', 0.0)
        )
        
        if self.backend == 'mock':
            self._mock_atomspace[atom_id] = atom_data
        elif self.backend == 'cython' and self._connection:
            # Use Cython bindings
            pass
            
        if self._on_atom_added:
            self._on_atom_added(atom_id, atom_data)
            
        logger.debug(f"Added atom: {atom_id}")
        return atom_id
        
    def create_link(self, link_type: str, source_id: str, target_id: str,
                   name: Optional[str] = None, **kwargs) -> Optional[str]:
        """
        Create a link between two atoms
        
        Args:
            link_type: Type of link (e.g., 'InheritanceLink')
            source_id: Source atom ID
            target_id: Target atom ID
            name: Optional link name
            **kwargs: Additional properties
            
        Returns:
            Link ID or None if failed
        """
        if self.backend == 'mock':
            if source_id not in self._mock_atomspace or target_id not in self._mock_atomspace:
                logger.error(f"Cannot create link: source or target not found")
                return None
                
        link_name = name or f"link_{source_id}_to_{target_id}"
        link_id = self.add_atom(link_type, link_name, **kwargs)
        
        if self.backend == 'mock':
            # Update outgoing/incoming references
            link = self._mock_atomspace[link_id]
            link.outgoing = [target_id]
            
            source = self._mock_atomspace[source_id]
            source.outgoing.append(link_id)
            
            target = self._mock_atomspace[target_id]
            target.incoming.append(link_id)
            
        logger.debug(f"Created link: {link_id}")
        return link_id
        
    def get_atom(self, atom_id: str) -> Optional[AtomData]:
        """Get atom by ID"""
        if self.backend == 'mock':
            return self._mock_atomspace.get(atom_id)
        return None
        
    def extract_atoms(self, filter_type: Optional[str] = None) -> Dict[str, Dict[str, Any]]:
        """
        Extract all atoms from AtomSpace
        
        Args:
            filter_type: Optional type filter
            
        Returns:
            Dictionary of atom_id -> atom_data
        """
        result = {}
        
        if self.backend == 'mock':
            for atom_id, atom_data in self._mock_atomspace.items():
                if filter_type is None or atom_data.atom_type == filter_type:
                    result[atom_id] = atom_data.to_dict()
        elif self.backend == 'cython' and self._connection:
            # Extract from Cython AtomSpace
            pass
            
        logger.info(f"Extracted {len(result)} atoms")
        return result
        
    def update_attention(self, atom_id: str, attention_value: float):
        """Update attention value of an atom"""
        if self.backend == 'mock':
            if atom_id in self._mock_atomspace:
                self._mock_atomspace[atom_id].attention_value = attention_value
                
    def write_embeddings(self, embeddings: Dict[str, Any]):
        """
        Write GNN embeddings back to AtomSpace as atom properties
        
        Args:
            embeddings: Dictionary of node_id -> embedding vector
        """
        for atom_id, embedding in embeddings.items():
            if self.backend == 'mock':
                if atom_id in self._mock_atomspace:
                    # Store embedding as attention value (simplified)
                    # In full implementation, would store as separate atom
                    self._mock_atomspace[atom_id].attention_value = float(embedding.mean())
                    
        logger.info(f"Wrote {len(embeddings)} embeddings to AtomSpace")
        
    def add_diagnostic_atom(self, diagnostic_type: str, data: Dict[str, Any]) -> str:
        """
        Add a diagnostic atom for autognostic integration
        
        Args:
            diagnostic_type: Type of diagnostic (error, prediction-mismatch, stall)
            data: Diagnostic data
            
        Returns:
            Diagnostic atom ID
        """
        atom_type = AtomType.DIAGNOSTIC.value
        if diagnostic_type == 'error':
            atom_type = AtomType.ERROR.value
        elif diagnostic_type == 'prediction_mismatch':
            atom_type = AtomType.PREDICTION_MISMATCH.value
        elif diagnostic_type == 'stall':
            atom_type = AtomType.STALL.value
            
        name = f"diag_{diagnostic_type}_{id(data)}"
        atom_id = self.add_atom(
            atom_type=atom_type,
            name=name,
            truth_value=data.get('severity_score', 0.5),
            confidence=data.get('confidence', 0.8)
        )
        
        logger.info(f"Added diagnostic atom: {atom_id}")
        return atom_id
        
    def get_statistics(self) -> Dict[str, Any]:
        """Get AtomSpace statistics"""
        if self.backend == 'mock':
            type_counts = {}
            for atom in self._mock_atomspace.values():
                type_counts[atom.atom_type] = type_counts.get(atom.atom_type, 0) + 1
                
            return {
                'backend': self.backend,
                'total_atoms': len(self._mock_atomspace),
                'atoms_by_type': type_counts
            }
        return {'backend': self.backend, 'total_atoms': 0}
        
    def clear(self):
        """Clear the AtomSpace"""
        if self.backend == 'mock':
            self._mock_atomspace.clear()
        logger.info("AtomSpace cleared")
        
    def set_callbacks(self, on_added: Optional[Callable] = None,
                     on_removed: Optional[Callable] = None):
        """Set callbacks for CogGML integration"""
        self._on_atom_added = on_added
        self._on_atom_removed = on_removed
        
    def create_sample_knowledge_graph(self):
        """Create a sample knowledge graph for testing"""
        # Create concept nodes
        ai = self.add_atom('ConceptNode', 'artificial_intelligence', truth_value=0.95)
        ml = self.add_atom('ConceptNode', 'machine_learning', truth_value=0.9)
        dl = self.add_atom('ConceptNode', 'deep_learning', truth_value=0.85)
        nn = self.add_atom('ConceptNode', 'neural_networks', truth_value=0.9)
        gnn = self.add_atom('ConceptNode', 'graph_neural_networks', truth_value=0.8)
        nlp = self.add_atom('ConceptNode', 'natural_language_processing', truth_value=0.85)
        cv = self.add_atom('ConceptNode', 'computer_vision', truth_value=0.85)
        agi = self.add_atom('ConceptNode', 'artificial_general_intelligence', truth_value=0.7)
        opencog = self.add_atom('ConceptNode', 'opencog', truth_value=0.9)
        atomspace = self.add_atom('ConceptNode', 'atomspace', truth_value=0.95)
        
        # Create inheritance links
        self.create_link('InheritanceLink', ml, ai)
        self.create_link('InheritanceLink', dl, ml)
        self.create_link('InheritanceLink', nn, dl)
        self.create_link('InheritanceLink', gnn, nn)
        self.create_link('InheritanceLink', nlp, ai)
        self.create_link('InheritanceLink', cv, ai)
        self.create_link('InheritanceLink', agi, ai)
        self.create_link('InheritanceLink', opencog, agi)
        self.create_link('InheritanceLink', atomspace, opencog)
        
        # Create similarity links
        self.create_link('SimilarityLink', dl, nn, truth_value=0.9)
        self.create_link('SimilarityLink', nlp, ml, truth_value=0.7)
        self.create_link('SimilarityLink', cv, dl, truth_value=0.8)
        
        logger.info("Created sample knowledge graph with 10 nodes and 12 links")
