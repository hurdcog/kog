"""
Membrane Architecture Manager

Implements P-System membrane boundaries for cognitive process isolation and communication.
Based on the Deep Tree Echo hierarchical membrane architecture.

Membrane Hierarchy:
- Root Membrane (System Boundary)
  - Cognitive Membrane (Core Processing)
    - Memory Membrane
    - Reasoning Membrane
    - Grammar Membrane
  - Extension Membrane (Plugin Container)
    - Browser Membrane
    - ML Membrane
    - Introspection Membrane
  - Security Membrane (Validation & Control)
    - Authentication Membrane
    - Validation Membrane
    - Emergency Membrane

This implementation provides:
- Hierarchical membrane structure
- Membrane communication protocols
- Resource allocation and boundary enforcement
- Process isolation and coordination
"""

from typing import Dict, List, Set, Optional, Callable, Any
from dataclasses import dataclass, field
from datetime import datetime
from enum import Enum
import threading
import queue
import json


class MembraneType(Enum):
    """Types of membranes in the hierarchy"""
    ROOT = "root"
    COGNITIVE = "cognitive"
    MEMORY = "memory"
    REASONING = "reasoning"
    GRAMMAR = "grammar"
    EXTENSION = "extension"
    BROWSER = "browser"
    ML = "ml"
    INTROSPECTION = "introspection"
    SECURITY = "security"
    AUTHENTICATION = "authentication"
    VALIDATION = "validation"
    EMERGENCY = "emergency"


class MessageType(Enum):
    """Types of messages that can be passed between membranes"""
    DATA = "data"
    COMMAND = "command"
    QUERY = "query"
    RESPONSE = "response"
    EVENT = "event"
    RESOURCE_REQUEST = "resource_request"
    RESOURCE_GRANT = "resource_grant"


@dataclass
class MembraneMessage:
    """Message passed between membranes"""
    id: str
    source_membrane: str
    target_membrane: str
    message_type: MessageType
    payload: Any
    priority: int = 0
    timestamp: datetime = field(default_factory=datetime.now)
    metadata: Dict = field(default_factory=dict)
    
    def to_dict(self) -> Dict:
        return {
            'id': self.id,
            'source': self.source_membrane,
            'target': self.target_membrane,
            'type': self.message_type.value,
            'payload': self.payload,
            'priority': self.priority,
            'timestamp': self.timestamp.isoformat(),
            'metadata': self.metadata
        }


@dataclass
class MembraneResources:
    """Resources allocated to a membrane"""
    cpu_quota: float = 1.0  # Relative CPU allocation
    memory_quota: int = 1024 * 1024 * 100  # Memory in bytes (100MB default)
    message_queue_size: int = 1000
    max_child_membranes: int = 10
    
    cpu_used: float = 0.0
    memory_used: int = 0
    messages_processed: int = 0


class Membrane:
    """
    Represents a single membrane in the P-System architecture
    
    A membrane is a boundary that isolates cognitive processes while
    allowing controlled communication and resource sharing.
    """
    
    def __init__(
        self,
        membrane_id: str,
        membrane_type: MembraneType,
        parent: Optional['Membrane'] = None,
        resources: Optional[MembraneResources] = None
    ):
        self.membrane_id = membrane_id
        self.membrane_type = membrane_type
        self.parent = parent
        self.children: Dict[str, 'Membrane'] = {}
        self.resources = resources or MembraneResources()
        
        # Communication
        self.message_queue: queue.PriorityQueue = queue.PriorityQueue(
            maxsize=self.resources.message_queue_size
        )
        self.message_handlers: Dict[MessageType, List[Callable]] = {}
        
        # State
        self.is_active = True
        self.state: Dict = {}
        self.created_at = datetime.now()
        self.last_activity = datetime.now()
        
        # Thread safety
        self.lock = threading.RLock()
        
        # Metrics
        self.metrics = {
            'messages_sent': 0,
            'messages_received': 0,
            'messages_processed': 0,
            'errors': 0
        }
    
    def add_child(self, child: 'Membrane'):
        """Add a child membrane"""
        with self.lock:
            if len(self.children) >= self.resources.max_child_membranes:
                raise ValueError(f"Maximum child membranes ({self.resources.max_child_membranes}) reached")
            
            self.children[child.membrane_id] = child
            child.parent = self
    
    def remove_child(self, child_id: str):
        """Remove a child membrane"""
        with self.lock:
            if child_id in self.children:
                child = self.children[child_id]
                child.is_active = False
                del self.children[child_id]
    
    def register_handler(self, message_type: MessageType, handler: Callable):
        """Register a message handler for a specific message type"""
        with self.lock:
            if message_type not in self.message_handlers:
                self.message_handlers[message_type] = []
            self.message_handlers[message_type].append(handler)
    
    def send_message(self, message: MembraneMessage):
        """Send a message to another membrane"""
        with self.lock:
            self.metrics['messages_sent'] += 1
            self.last_activity = datetime.now()
        
        # Route message through parent if needed
        if message.target_membrane == self.membrane_id:
            self._receive_message(message)
        elif message.target_membrane in self.children:
            self.children[message.target_membrane]._receive_message(message)
        elif self.parent:
            self.parent.send_message(message)
        else:
            # Message cannot be routed
            with self.lock:
                self.metrics['errors'] += 1
    
    def _receive_message(self, message: MembraneMessage):
        """Receive a message (internal method)"""
        with self.lock:
            self.metrics['messages_received'] += 1
            self.last_activity = datetime.now()
        
        try:
            # Add to queue with priority (negative for proper ordering)
            # Use timestamp as tiebreaker to avoid comparing MembraneMessage objects
            self.message_queue.put((-message.priority, message.timestamp.timestamp(), message))
        except queue.Full:
            with self.lock:
                self.metrics['errors'] += 1
    
    def process_messages(self, max_messages: int = 10) -> int:
        """
        Process messages from the queue
        
        Returns:
            Number of messages processed
        """
        processed = 0
        
        for _ in range(max_messages):
            try:
                # Non-blocking get
                priority, timestamp, message = self.message_queue.get_nowait()
                
                # Process message
                self._handle_message(message)
                processed += 1
                
                with self.lock:
                    self.metrics['messages_processed'] += 1
                    self.resources.messages_processed += 1
                
            except queue.Empty:
                break
            except Exception as e:
                with self.lock:
                    self.metrics['errors'] += 1
        
        return processed
    
    def _handle_message(self, message: MembraneMessage):
        """Handle a single message"""
        handlers = self.message_handlers.get(message.message_type, [])
        
        for handler in handlers:
            try:
                handler(message)
            except Exception as e:
                with self.lock:
                    self.metrics['errors'] += 1
    
    def get_state(self) -> Dict:
        """Get the current state of the membrane"""
        with self.lock:
            return {
                'membrane_id': self.membrane_id,
                'membrane_type': self.membrane_type.value,
                'is_active': self.is_active,
                'children_count': len(self.children),
                'queue_size': self.message_queue.qsize(),
                'metrics': self.metrics.copy(),
                'resources': {
                    'cpu_quota': self.resources.cpu_quota,
                    'memory_quota': self.resources.memory_quota,
                    'cpu_used': self.resources.cpu_used,
                    'memory_used': self.resources.memory_used
                },
                'created_at': self.created_at.isoformat(),
                'last_activity': self.last_activity.isoformat()
            }
    
    def shutdown(self):
        """Shutdown the membrane and all children"""
        with self.lock:
            self.is_active = False
            
            # Shutdown children
            for child in list(self.children.values()):
                child.shutdown()
            
            self.children.clear()


class MembraneManager:
    """
    Manages the complete membrane hierarchy
    
    Provides centralized control over membrane creation, communication,
    resource allocation, and monitoring.
    """
    
    def __init__(self):
        self.membranes: Dict[str, Membrane] = {}
        self.root_membrane: Optional[Membrane] = None
        self.lock = threading.RLock()
        self.message_counter = 0
        
        # Initialize the hierarchy
        self._initialize_hierarchy()
    
    def _initialize_hierarchy(self):
        """Initialize the Deep Tree Echo membrane hierarchy"""
        # Create root membrane
        self.root_membrane = self.create_membrane(
            "root",
            MembraneType.ROOT,
            resources=MembraneResources(
                cpu_quota=1.0,
                memory_quota=1024 * 1024 * 1000,  # 1GB
                max_child_membranes=3
            )
        )
        
        # Create Cognitive Membrane
        cognitive = self.create_membrane(
            "cognitive",
            MembraneType.COGNITIVE,
            parent_id="root",
            resources=MembraneResources(
                cpu_quota=0.5,
                memory_quota=1024 * 1024 * 500,  # 500MB
                max_child_membranes=3
            )
        )
        
        # Create cognitive sub-membranes
        self.create_membrane("memory", MembraneType.MEMORY, parent_id="cognitive")
        self.create_membrane("reasoning", MembraneType.REASONING, parent_id="cognitive")
        self.create_membrane("grammar", MembraneType.GRAMMAR, parent_id="cognitive")
        
        # Create Extension Membrane
        extension = self.create_membrane(
            "extension",
            MembraneType.EXTENSION,
            parent_id="root",
            resources=MembraneResources(
                cpu_quota=0.3,
                memory_quota=1024 * 1024 * 300,  # 300MB
                max_child_membranes=3
            )
        )
        
        # Create extension sub-membranes
        self.create_membrane("browser", MembraneType.BROWSER, parent_id="extension")
        self.create_membrane("ml", MembraneType.ML, parent_id="extension")
        self.create_membrane("introspection", MembraneType.INTROSPECTION, parent_id="extension")
        
        # Create Security Membrane
        security = self.create_membrane(
            "security",
            MembraneType.SECURITY,
            parent_id="root",
            resources=MembraneResources(
                cpu_quota=0.2,
                memory_quota=1024 * 1024 * 200,  # 200MB
                max_child_membranes=3
            )
        )
        
        # Create security sub-membranes
        self.create_membrane("authentication", MembraneType.AUTHENTICATION, parent_id="security")
        self.create_membrane("validation", MembraneType.VALIDATION, parent_id="security")
        self.create_membrane("emergency", MembraneType.EMERGENCY, parent_id="security")
    
    def create_membrane(
        self,
        membrane_id: str,
        membrane_type: MembraneType,
        parent_id: Optional[str] = None,
        resources: Optional[MembraneResources] = None
    ) -> Membrane:
        """Create a new membrane"""
        with self.lock:
            if membrane_id in self.membranes:
                raise ValueError(f"Membrane {membrane_id} already exists")
            
            parent = None
            if parent_id:
                parent = self.membranes.get(parent_id)
                if not parent:
                    raise ValueError(f"Parent membrane {parent_id} not found")
            
            membrane = Membrane(
                membrane_id=membrane_id,
                membrane_type=membrane_type,
                parent=parent,
                resources=resources
            )
            
            if parent:
                parent.add_child(membrane)
            
            self.membranes[membrane_id] = membrane
            return membrane
    
    def get_membrane(self, membrane_id: str) -> Optional[Membrane]:
        """Get a membrane by ID"""
        with self.lock:
            return self.membranes.get(membrane_id)
    
    def send_message(
        self,
        source_id: str,
        target_id: str,
        message_type: MessageType,
        payload: Any,
        priority: int = 0,
        metadata: Optional[Dict] = None
    ) -> str:
        """
        Send a message between membranes
        
        Returns:
            Message ID
        """
        with self.lock:
            self.message_counter += 1
            message_id = f"msg_{self.message_counter}"
        
        source = self.get_membrane(source_id)
        if not source:
            raise ValueError(f"Source membrane {source_id} not found")
        
        message = MembraneMessage(
            id=message_id,
            source_membrane=source_id,
            target_membrane=target_id,
            message_type=message_type,
            payload=payload,
            priority=priority,
            metadata=metadata or {}
        )
        
        source.send_message(message)
        return message_id
    
    def process_all_messages(self, max_per_membrane: int = 10) -> Dict[str, int]:
        """
        Process messages in all membranes
        
        Returns:
            Dictionary mapping membrane IDs to number of messages processed
        """
        results = {}
        
        with self.lock:
            membranes = list(self.membranes.values())
        
        for membrane in membranes:
            if membrane.is_active:
                processed = membrane.process_messages(max_per_membrane)
                results[membrane.membrane_id] = processed
        
        return results
    
    def get_hierarchy_state(self) -> Dict:
        """Get the state of the entire membrane hierarchy"""
        def build_tree(membrane: Membrane) -> Dict:
            state = membrane.get_state()
            state['children'] = {
                child_id: build_tree(child)
                for child_id, child in membrane.children.items()
            }
            return state
        
        if self.root_membrane:
            return build_tree(self.root_membrane)
        return {}
    
    def get_resource_usage(self) -> Dict:
        """Get resource usage across all membranes"""
        usage = {
            'total_cpu': 0.0,
            'total_memory': 0,
            'total_messages': 0,
            'by_membrane': {}
        }
        
        with self.lock:
            for membrane_id, membrane in self.membranes.items():
                state = membrane.get_state()
                resources = state['resources']
                
                usage['total_cpu'] += resources['cpu_used']
                usage['total_memory'] += resources['memory_used']
                usage['total_messages'] += state['metrics']['messages_processed']
                
                usage['by_membrane'][membrane_id] = {
                    'cpu': resources['cpu_used'],
                    'memory': resources['memory_used'],
                    'messages': state['metrics']['messages_processed']
                }
        
        return usage
    
    def get_communication_graph(self) -> Dict:
        """
        Get a graph representation of membrane communication
        
        Returns:
            Dictionary with nodes and edges for visualization
        """
        nodes = []
        edges = []
        
        with self.lock:
            for membrane_id, membrane in self.membranes.items():
                nodes.append({
                    'id': membrane_id,
                    'type': membrane.membrane_type.value,
                    'active': membrane.is_active,
                    'messages_processed': membrane.metrics['messages_processed']
                })
                
                # Add edges to children
                for child_id in membrane.children:
                    edges.append({
                        'source': membrane_id,
                        'target': child_id,
                        'type': 'parent-child'
                    })
        
        return {
            'nodes': nodes,
            'edges': edges
        }
    
    def shutdown(self):
        """Shutdown all membranes"""
        if self.root_membrane:
            self.root_membrane.shutdown()
        
        with self.lock:
            self.membranes.clear()
            self.root_membrane = None
    
    def save_state(self, filepath: str):
        """Save the membrane manager state to a file"""
        state = {
            'hierarchy': self.get_hierarchy_state(),
            'resource_usage': self.get_resource_usage(),
            'communication_graph': self.get_communication_graph()
        }
        
        with open(filepath, 'w') as f:
            json.dump(state, f, indent=2)


# Example usage and testing
if __name__ == "__main__":
    print("Initializing Membrane Manager...")
    manager = MembraneManager()
    
    print("\nMembrane Hierarchy:")
    hierarchy = manager.get_hierarchy_state()
    print(f"Root: {hierarchy['membrane_id']}")
    for child_id, child in hierarchy['children'].items():
        print(f"  - {child_id} ({child['membrane_type']})")
        for subchild_id in child.get('children', {}):
            print(f"    - {subchild_id}")
    
    print("\nTesting message passing...")
    # Register a simple handler
    memory_membrane = manager.get_membrane("memory")
    if memory_membrane:
        def handle_data(msg: MembraneMessage):
            print(f"Memory membrane received: {msg.payload}")
        
        memory_membrane.register_handler(MessageType.DATA, handle_data)
    
    # Send a message
    manager.send_message(
        source_id="reasoning",
        target_id="memory",
        message_type=MessageType.DATA,
        payload={"concept": "cognitive_synergy", "value": 0.95}
    )
    
    # Process messages
    print("\nProcessing messages...")
    results = manager.process_all_messages()
    print(f"Messages processed: {sum(results.values())}")
    
    print("\nResource Usage:")
    usage = manager.get_resource_usage()
    print(f"Total messages processed: {usage['total_messages']}")
    
    print("\nMembrane Manager implementation complete!")
