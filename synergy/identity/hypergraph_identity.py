"""
Hypergraph Identity Refinement System

This module implements a system for transforming conversational interactions
into hypergraph format and using them for continuous identity refinement.

The system supports:
- Conversation-to-hypergraph transformation
- Identity refinement through tuple addition
- Core self visualization and evolution tracking
- Integration with the AAR Core for self-awareness

Based on the Deep Tree Echo architecture and cognitive synergy principles.
"""

import numpy as np
from typing import Dict, List, Tuple, Set, Optional, Any
from dataclasses import dataclass, field
from datetime import datetime
from collections import defaultdict
import json
import hashlib


@dataclass
class HypergraphNode:
    """Represents a node in the hypergraph"""
    id: str
    label: str
    node_type: str  # 'concept', 'relation', 'attribute', 'action'
    properties: Dict[str, Any] = field(default_factory=dict)
    timestamp: datetime = field(default_factory=datetime.now)
    
    def __hash__(self):
        return hash(self.id)
    
    def __eq__(self, other):
        return isinstance(other, HypergraphNode) and self.id == other.id


@dataclass
class HypergraphEdge:
    """Represents a hyperedge connecting multiple nodes"""
    id: str
    nodes: Set[str]  # Set of node IDs
    edge_type: str  # 'identity', 'relation', 'refinement', 'context'
    weight: float = 1.0
    properties: Dict[str, Any] = field(default_factory=dict)
    timestamp: datetime = field(default_factory=datetime.now)
    
    def __hash__(self):
        return hash(self.id)
    
    def __eq__(self, other):
        return isinstance(other, HypergraphEdge) and self.id == other.id


@dataclass
class IdentityRefinementTuple:
    """
    Represents a tuple that refines identity
    
    Format: (subject, predicate, object, context, confidence)
    """
    subject: str
    predicate: str
    object: str
    context: str = ""
    confidence: float = 1.0
    source: str = "conversation"
    timestamp: datetime = field(default_factory=datetime.now)
    
    def to_dict(self) -> Dict:
        return {
            'subject': self.subject,
            'predicate': self.predicate,
            'object': self.object,
            'context': self.context,
            'confidence': self.confidence,
            'source': self.source,
            'timestamp': self.timestamp.isoformat()
        }
    
    def get_hash(self) -> str:
        """Generate a unique hash for this tuple"""
        content = f"{self.subject}|{self.predicate}|{self.object}|{self.context}"
        return hashlib.sha256(content.encode()).hexdigest()[:16]


class ConversationToHypergraphTransformer:
    """
    Transforms conversational data into hypergraph format
    
    Extracts identity-relevant information from conversations and
    creates hypergraph structures for identity refinement.
    """
    
    def __init__(self):
        self.identity_keywords = {
            'is', 'am', 'are', 'was', 'were', 'been',
            'have', 'has', 'had',
            'can', 'could', 'should', 'would',
            'prefer', 'like', 'want', 'need',
            'believe', 'think', 'feel', 'know'
        }
        
        self.relation_keywords = {
            'relates', 'connects', 'links', 'associates',
            'causes', 'enables', 'requires', 'depends'
        }
    
    def extract_tuples(self, conversation: List[Dict[str, str]]) -> List[IdentityRefinementTuple]:
        """
        Extract identity refinement tuples from a conversation
        
        Args:
            conversation: List of messages with 'role' and 'content' keys
            
        Returns:
            List of identity refinement tuples
        """
        tuples = []
        
        for i, message in enumerate(conversation):
            role = message.get('role', 'unknown')
            content = message.get('content', '')
            
            # Extract simple subject-predicate-object patterns
            # This is a simplified extraction; real implementation would use NLP
            sentences = content.split('.')
            
            for sentence in sentences:
                sentence = sentence.strip().lower()
                if not sentence:
                    continue
                
                # Look for identity patterns
                for keyword in self.identity_keywords:
                    if keyword in sentence:
                        # Simple pattern matching
                        parts = sentence.split(keyword, 1)
                        if len(parts) == 2:
                            subject = parts[0].strip() or role
                            obj = parts[1].strip()
                            
                            if obj:
                                tuple_obj = IdentityRefinementTuple(
                                    subject=subject,
                                    predicate=keyword,
                                    object=obj,
                                    context=f"message_{i}",
                                    confidence=0.8,
                                    source="conversation"
                                )
                                tuples.append(tuple_obj)
        
        return tuples
    
    def create_hypergraph_from_tuples(
        self,
        tuples: List[IdentityRefinementTuple]
    ) -> Tuple[Dict[str, HypergraphNode], Dict[str, HypergraphEdge]]:
        """
        Create hypergraph nodes and edges from identity tuples
        
        Returns:
            Tuple of (nodes_dict, edges_dict)
        """
        nodes = {}
        edges = {}
        
        for tuple_obj in tuples:
            # Create nodes for subject, predicate, and object
            subj_id = f"node_{hashlib.sha256(tuple_obj.subject.encode()).hexdigest()[:8]}"
            pred_id = f"node_{hashlib.sha256(tuple_obj.predicate.encode()).hexdigest()[:8]}"
            obj_id = f"node_{hashlib.sha256(tuple_obj.object.encode()).hexdigest()[:8]}"
            
            if subj_id not in nodes:
                nodes[subj_id] = HypergraphNode(
                    id=subj_id,
                    label=tuple_obj.subject,
                    node_type='concept'
                )
            
            if pred_id not in nodes:
                nodes[pred_id] = HypergraphNode(
                    id=pred_id,
                    label=tuple_obj.predicate,
                    node_type='relation'
                )
            
            if obj_id not in nodes:
                nodes[obj_id] = HypergraphNode(
                    id=obj_id,
                    label=tuple_obj.object,
                    node_type='attribute'
                )
            
            # Create hyperedge connecting the three nodes
            edge_id = f"edge_{tuple_obj.get_hash()}"
            edges[edge_id] = HypergraphEdge(
                id=edge_id,
                nodes={subj_id, pred_id, obj_id},
                edge_type='identity',
                weight=tuple_obj.confidence,
                properties={
                    'context': tuple_obj.context,
                    'source': tuple_obj.source
                }
            )
        
        return nodes, edges


class HypergraphIdentitySystem:
    """
    Complete system for hypergraph-based identity refinement
    
    Manages the hypergraph representation of identity, supports
    continuous refinement through tuple addition, and provides
    visualization and analysis capabilities.
    """
    
    def __init__(self, core_self_id: str = "core_self"):
        self.core_self_id = core_self_id
        self.nodes: Dict[str, HypergraphNode] = {}
        self.edges: Dict[str, HypergraphEdge] = {}
        self.refinement_history: List[IdentityRefinementTuple] = []
        self.transformer = ConversationToHypergraphTransformer()
        
        # Initialize core self node
        self._initialize_core_self()
    
    def _initialize_core_self(self):
        """Initialize the core self node"""
        core_node = HypergraphNode(
            id=self.core_self_id,
            label="Core Self",
            node_type='concept',
            properties={
                'is_core': True,
                'centrality': 1.0
            }
        )
        self.nodes[self.core_self_id] = core_node
    
    def add_conversation(self, conversation: List[Dict[str, str]]):
        """
        Add a conversation and refine identity based on it
        
        Args:
            conversation: List of messages with 'role' and 'content' keys
        """
        # Extract tuples from conversation
        tuples = self.transformer.extract_tuples(conversation)
        
        # Add tuples to the system
        for tuple_obj in tuples:
            self.add_refinement_tuple(tuple_obj)
    
    def add_refinement_tuple(self, tuple_obj: IdentityRefinementTuple):
        """
        Add a single refinement tuple to the hypergraph
        
        This is the core operation for identity refinement.
        """
        # Create hypergraph structures from tuple
        new_nodes, new_edges = self.transformer.create_hypergraph_from_tuples([tuple_obj])
        
        # Merge new nodes
        for node_id, node in new_nodes.items():
            if node_id not in self.nodes:
                self.nodes[node_id] = node
            else:
                # Update existing node properties
                self.nodes[node_id].properties.update(node.properties)
        
        # Merge new edges
        for edge_id, edge in new_edges.items():
            if edge_id not in self.edges:
                self.edges[edge_id] = edge
            else:
                # Update edge weight (increase confidence)
                self.edges[edge_id].weight = min(
                    1.0,
                    self.edges[edge_id].weight + 0.1
                )
        
        # Connect to core self if relevant
        self._connect_to_core_self(new_nodes, tuple_obj)
        
        # Store in history
        self.refinement_history.append(tuple_obj)
    
    def _connect_to_core_self(
        self,
        new_nodes: Dict[str, HypergraphNode],
        tuple_obj: IdentityRefinementTuple
    ):
        """Connect new nodes to core self if they represent identity"""
        # If the subject is "I", "me", or similar, connect to core self
        identity_pronouns = {'i', 'me', 'my', 'mine', 'myself', 'self'}
        
        if tuple_obj.subject.lower() in identity_pronouns:
            for node_id in new_nodes:
                if new_nodes[node_id].node_type in ['attribute', 'concept']:
                    # Create edge connecting core self to this node
                    edge_id = f"edge_core_{node_id}"
                    if edge_id not in self.edges:
                        self.edges[edge_id] = HypergraphEdge(
                            id=edge_id,
                            nodes={self.core_self_id, node_id},
                            edge_type='identity',
                            weight=tuple_obj.confidence,
                            properties={'refinement': True}
                        )
    
    def get_core_self_attributes(self) -> List[Tuple[str, float]]:
        """
        Get attributes directly connected to core self
        
        Returns:
            List of (attribute_label, confidence) tuples
        """
        attributes = []
        
        for edge in self.edges.values():
            if self.core_self_id in edge.nodes and edge.edge_type == 'identity':
                # Find the other node(s)
                other_nodes = edge.nodes - {self.core_self_id}
                for node_id in other_nodes:
                    if node_id in self.nodes:
                        node = self.nodes[node_id]
                        attributes.append((node.label, edge.weight))
        
        return sorted(attributes, key=lambda x: x[1], reverse=True)
    
    def compute_node_centrality(self) -> Dict[str, float]:
        """
        Compute centrality scores for all nodes
        
        Centrality indicates importance in the identity network.
        """
        centrality = defaultdict(float)
        
        # Simple degree centrality: count connections
        for edge in self.edges.values():
            for node_id in edge.nodes:
                centrality[node_id] += edge.weight
        
        # Normalize
        if centrality:
            max_centrality = max(centrality.values())
            if max_centrality > 0:
                for node_id in centrality:
                    centrality[node_id] /= max_centrality
        
        return dict(centrality)
    
    def get_identity_evolution(self) -> Dict[str, List[float]]:
        """
        Track how identity has evolved over time
        
        Returns:
            Dictionary mapping time periods to identity metrics
        """
        evolution = {
            'timestamps': [],
            'node_count': [],
            'edge_count': [],
            'core_connections': []
        }
        
        # Simulate evolution by replaying history
        temp_nodes = {self.core_self_id: self.nodes[self.core_self_id]}
        temp_edges = {}
        
        for i, tuple_obj in enumerate(self.refinement_history):
            # Add tuple
            new_nodes, new_edges = self.transformer.create_hypergraph_from_tuples([tuple_obj])
            temp_nodes.update(new_nodes)
            temp_edges.update(new_edges)
            
            # Record metrics every 10 tuples
            if i % 10 == 0:
                evolution['timestamps'].append(tuple_obj.timestamp.isoformat())
                evolution['node_count'].append(len(temp_nodes))
                evolution['edge_count'].append(len(temp_edges))
                
                # Count core connections
                core_connections = sum(
                    1 for edge in temp_edges.values()
                    if self.core_self_id in edge.nodes
                )
                evolution['core_connections'].append(core_connections)
        
        return evolution
    
    def get_identity_summary(self) -> Dict:
        """Get a comprehensive summary of the current identity state"""
        centrality = self.compute_node_centrality()
        core_attributes = self.get_core_self_attributes()
        
        return {
            'node_count': len(self.nodes),
            'edge_count': len(self.edges),
            'refinement_count': len(self.refinement_history),
            'core_self_id': self.core_self_id,
            'core_attributes': core_attributes[:10],  # Top 10
            'most_central_nodes': sorted(
                [(self.nodes[nid].label, score) for nid, score in centrality.items()],
                key=lambda x: x[1],
                reverse=True
            )[:10],
            'last_refinement': (
                self.refinement_history[-1].to_dict()
                if self.refinement_history else None
            )
        }
    
    def visualize_core_self(self) -> Dict:
        """
        Generate visualization data for the core self and its connections
        
        Returns:
            Dictionary with nodes and edges for visualization
        """
        viz_nodes = []
        viz_edges = []
        
        # Add core self
        viz_nodes.append({
            'id': self.core_self_id,
            'label': 'Core Self',
            'type': 'core',
            'size': 20
        })
        
        # Add directly connected nodes
        connected_node_ids = set()
        for edge in self.edges.values():
            if self.core_self_id in edge.nodes:
                other_nodes = edge.nodes - {self.core_self_id}
                connected_node_ids.update(other_nodes)
                
                # Add edge
                for other_id in other_nodes:
                    viz_edges.append({
                        'source': self.core_self_id,
                        'target': other_id,
                        'weight': edge.weight
                    })
        
        # Add connected nodes
        for node_id in connected_node_ids:
            if node_id in self.nodes:
                node = self.nodes[node_id]
                viz_nodes.append({
                    'id': node_id,
                    'label': node.label,
                    'type': node.node_type,
                    'size': 10
                })
        
        return {
            'nodes': viz_nodes,
            'edges': viz_edges
        }
    
    def save_to_file(self, filepath: str):
        """Save the hypergraph identity system to a file"""
        data = {
            'core_self_id': self.core_self_id,
            'nodes': {
                nid: {
                    'id': n.id,
                    'label': n.label,
                    'node_type': n.node_type,
                    'properties': n.properties,
                    'timestamp': n.timestamp.isoformat()
                }
                for nid, n in self.nodes.items()
            },
            'edges': {
                eid: {
                    'id': e.id,
                    'nodes': list(e.nodes),
                    'edge_type': e.edge_type,
                    'weight': e.weight,
                    'properties': e.properties,
                    'timestamp': e.timestamp.isoformat()
                }
                for eid, e in self.edges.items()
            },
            'refinement_history': [t.to_dict() for t in self.refinement_history]
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
    
    def load_from_file(self, filepath: str):
        """Load the hypergraph identity system from a file"""
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        self.core_self_id = data['core_self_id']
        
        # Reconstruct nodes
        self.nodes = {}
        for nid, ndata in data['nodes'].items():
            self.nodes[nid] = HypergraphNode(
                id=ndata['id'],
                label=ndata['label'],
                node_type=ndata['node_type'],
                properties=ndata['properties'],
                timestamp=datetime.fromisoformat(ndata['timestamp'])
            )
        
        # Reconstruct edges
        self.edges = {}
        for eid, edata in data['edges'].items():
            self.edges[eid] = HypergraphEdge(
                id=edata['id'],
                nodes=set(edata['nodes']),
                edge_type=edata['edge_type'],
                weight=edata['weight'],
                properties=edata['properties'],
                timestamp=datetime.fromisoformat(edata['timestamp'])
            )
        
        # Reconstruct history
        self.refinement_history = [
            IdentityRefinementTuple(
                subject=t['subject'],
                predicate=t['predicate'],
                object=t['object'],
                context=t['context'],
                confidence=t['confidence'],
                source=t['source'],
                timestamp=datetime.fromisoformat(t['timestamp'])
            )
            for t in data['refinement_history']
        ]


# Example usage and testing
if __name__ == "__main__":
    print("Initializing Hypergraph Identity System...")
    identity_system = HypergraphIdentitySystem()
    
    # Example conversation
    conversation = [
        {"role": "assistant", "content": "I am a cognitive AI system designed for AGI research."},
        {"role": "user", "content": "What can you do?"},
        {"role": "assistant", "content": "I can reason about complex problems. I prefer symbolic approaches."},
        {"role": "user", "content": "Tell me about your architecture."},
        {"role": "assistant", "content": "I use hypergraph memory. I believe in cognitive synergy."}
    ]
    
    print("\nProcessing conversation...")
    identity_system.add_conversation(conversation)
    
    print("\nIdentity Summary:")
    summary = identity_system.get_identity_summary()
    print(f"Nodes: {summary['node_count']}, Edges: {summary['edge_count']}")
    print(f"Refinements: {summary['refinement_count']}")
    
    print("\nCore Self Attributes:")
    for attr, conf in summary['core_attributes']:
        print(f"  - {attr} (confidence: {conf:.2f})")
    
    print("\nHypergraph Identity System implementation complete!")
