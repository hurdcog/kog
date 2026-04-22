"""
Distributed Cognitive Shard Network

This module implements a network of specialized cognitive shards with shared
memory, enabling distributed cognition capabilities.

Emergent Capability: distributed_cognition
- Multiple specialized cognitive processes running in parallel
- Shared AtomSpace view for inter-shard communication
- Dynamic load balancing and task allocation
- Emergent problem-solving through shard collaboration

Architecture:
1. Shard specialization taxonomy
2. Shard-to-shard communication protocol
3. Shared AtomSpace view for shards
4. Shard orchestration policies

Based on: Autogenesis Roadmap Feature #5
Implementation Status: Phase 1
"""

import logging
import time
import threading
from typing import Dict, List, Set, Optional, Any, Callable
from dataclasses import dataclass, field
from enum import Enum
from collections import defaultdict
import queue
import uuid

logger = logging.getLogger(__name__)


class ShardSpecialization(Enum):
    """Types of cognitive shard specializations"""
    REASONING = "reasoning"          # PLN-based logical reasoning
    LEARNING = "learning"            # MOSES-based program synthesis
    PATTERN_MINING = "pattern_mining"  # Pattern discovery and extraction
    ATTENTION = "attention"          # Attention allocation and focus
    MEMORY = "memory"                # Long-term memory management
    PERCEPTION = "perception"        # Sensory input processing
    ACTION = "action"                # Motor/action planning
    META_COGNITION = "meta_cognition"  # Self-monitoring and regulation
    GENERAL = "general"              # General-purpose processing


class MessagePriority(Enum):
    """Priority levels for inter-shard messages"""
    CRITICAL = 0   # Immediate processing required
    HIGH = 1       # Important, process soon
    NORMAL = 2     # Standard priority
    LOW = 3        # Background processing


@dataclass
class ShardMessage:
    """Message exchanged between cognitive shards"""
    message_id: str
    sender_id: str
    receiver_id: str
    message_type: str
    priority: MessagePriority
    payload: Dict[str, Any]
    timestamp: float = field(default_factory=time.time)
    ttl: int = 10  # Time-to-live in hops


@dataclass(order=True)
class ShardTask:
    """Task to be processed by a cognitive shard"""
    priority: MessagePriority = field(compare=True)
    task_id: str = field(compare=False)
    task_type: str = field(compare=False)
    data: Dict[str, Any] = field(compare=False)
    required_specialization: Optional[ShardSpecialization] = field(default=None, compare=False)
    timestamp: float = field(default_factory=time.time, compare=False)
    deadline: Optional[float] = field(default=None, compare=False)


@dataclass
class ShardState:
    """Current state of a cognitive shard"""
    shard_id: str
    specialization: ShardSpecialization
    load: float  # 0.0 to 1.0
    active_tasks: int
    completed_tasks: int
    failed_tasks: int
    is_active: bool
    last_heartbeat: float = field(default_factory=time.time)


class SharedAtomSpaceView:
    """
    Shared view of AtomSpace accessible by all shards
    
    Provides thread-safe access to shared knowledge representation
    with read/write synchronization and caching.
    """
    
    def __init__(self):
        self.atoms: Dict[str, Dict] = {}
        self.lock = threading.RLock()
        self.access_log: List[Dict] = []
        self.shard_views: Dict[str, Set[str]] = defaultdict(set)
        logger.info("SharedAtomSpaceView initialized")
    
    def read_atom(self, shard_id: str, atom_id: str) -> Optional[Dict]:
        """Thread-safe atom read"""
        with self.lock:
            if atom_id in self.atoms:
                self.shard_views[shard_id].add(atom_id)
                self.access_log.append({
                    'shard': shard_id,
                    'atom': atom_id,
                    'op': 'read',
                    'time': time.time()
                })
                return self.atoms[atom_id].copy()
            return None
    
    def write_atom(self, shard_id: str, atom_id: str, atom_data: Dict) -> None:
        """Thread-safe atom write"""
        with self.lock:
            self.atoms[atom_id] = {
                **atom_data,
                'last_writer': shard_id,
                'last_modified': time.time()
            }
            self.shard_views[shard_id].add(atom_id)
            self.access_log.append({
                'shard': shard_id,
                'atom': atom_id,
                'op': 'write',
                'time': time.time()
            })
            logger.debug(f"Shard {shard_id} wrote atom {atom_id}")
    
    def query_atoms(self, shard_id: str, predicate: Callable[[Dict], bool]) -> List[str]:
        """Query atoms matching a predicate"""
        with self.lock:
            matching = [
                atom_id for atom_id, atom in self.atoms.items()
                if predicate(atom)
            ]
            for atom_id in matching:
                self.shard_views[shard_id].add(atom_id)
            return matching
    
    def get_shard_view(self, shard_id: str) -> Set[str]:
        """Get all atoms accessed by a shard"""
        with self.lock:
            return self.shard_views[shard_id].copy()
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get view statistics"""
        with self.lock:
            return {
                'total_atoms': len(self.atoms),
                'total_shards': len(self.shard_views),
                'total_accesses': len(self.access_log),
                'atoms_per_shard': {
                    shard_id: len(atoms)
                    for shard_id, atoms in self.shard_views.items()
                }
            }


class ShardCommunicationBus:
    """
    Communication bus for inter-shard messaging
    
    Implements publish-subscribe and direct messaging patterns
    with priority queuing.
    """
    
    def __init__(self):
        self.message_queues: Dict[str, queue.PriorityQueue] = {}
        self.subscriptions: Dict[str, Set[str]] = defaultdict(set)
        self.message_history: List[ShardMessage] = []
        self.lock = threading.RLock()
        logger.info("ShardCommunicationBus initialized")
    
    def register_shard(self, shard_id: str) -> None:
        """Register a shard on the communication bus"""
        with self.lock:
            if shard_id not in self.message_queues:
                self.message_queues[shard_id] = queue.PriorityQueue()
                logger.info(f"Shard {shard_id} registered on communication bus")
    
    def send_message(self, message: ShardMessage) -> bool:
        """Send a message to a specific shard"""
        with self.lock:
            if message.receiver_id not in self.message_queues:
                logger.warning(f"Receiver {message.receiver_id} not registered")
                return False
            
            # Use priority as queue priority (lower = higher priority)
            priority = message.priority.value
            self.message_queues[message.receiver_id].put((priority, message))
            self.message_history.append(message)
            logger.debug(f"Message {message.message_id} sent: {message.sender_id} -> {message.receiver_id}")
            return True
    
    def broadcast_message(self, sender_id: str, message_type: str, 
                         payload: Dict, priority: MessagePriority) -> int:
        """Broadcast a message to all registered shards"""
        count = 0
        with self.lock:
            for receiver_id in self.message_queues.keys():
                if receiver_id != sender_id:
                    message = ShardMessage(
                        message_id=str(uuid.uuid4()),
                        sender_id=sender_id,
                        receiver_id=receiver_id,
                        message_type=message_type,
                        priority=priority,
                        payload=payload
                    )
                    if self.send_message(message):
                        count += 1
        return count
    
    def publish(self, sender_id: str, topic: str, payload: Dict, 
                priority: MessagePriority) -> int:
        """Publish a message to a topic (subscribers receive it)"""
        count = 0
        with self.lock:
            if topic in self.subscriptions:
                for subscriber_id in self.subscriptions[topic]:
                    if subscriber_id != sender_id:
                        message = ShardMessage(
                            message_id=str(uuid.uuid4()),
                            sender_id=sender_id,
                            receiver_id=subscriber_id,
                            message_type=f"topic:{topic}",
                            priority=priority,
                            payload=payload
                        )
                        if self.send_message(message):
                            count += 1
        return count
    
    def subscribe(self, shard_id: str, topic: str) -> None:
        """Subscribe a shard to a topic"""
        with self.lock:
            self.subscriptions[topic].add(shard_id)
            logger.info(f"Shard {shard_id} subscribed to topic '{topic}'")
    
    def receive_message(self, shard_id: str, timeout: float = 0.1) -> Optional[ShardMessage]:
        """Receive a message from the shard's queue"""
        if shard_id not in self.message_queues:
            return None
        
        try:
            priority, message = self.message_queues[shard_id].get(timeout=timeout)
            return message
        except queue.Empty:
            return None
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get communication statistics"""
        with self.lock:
            return {
                'registered_shards': len(self.message_queues),
                'total_messages': len(self.message_history),
                'active_subscriptions': sum(len(subs) for subs in self.subscriptions.values()),
                'topics': list(self.subscriptions.keys()),
                'queue_sizes': {
                    shard_id: q.qsize()
                    for shard_id, q in self.message_queues.items()
                }
            }


class CognitiveShard:
    """
    Individual cognitive shard with specialized processing capabilities
    
    A shard is an autonomous cognitive processing unit that:
    - Specializes in specific cognitive tasks
    - Communicates with other shards
    - Accesses shared knowledge representation
    - Processes tasks according to its specialization
    """
    
    def __init__(self, shard_id: str, specialization: ShardSpecialization,
                 shared_view: SharedAtomSpaceView,
                 comm_bus: ShardCommunicationBus):
        self.shard_id = shard_id
        self.specialization = specialization
        self.shared_view = shared_view
        self.comm_bus = comm_bus
        
        self.state = ShardState(
            shard_id=shard_id,
            specialization=specialization,
            load=0.0,
            active_tasks=0,
            completed_tasks=0,
            failed_tasks=0,
            is_active=False
        )
        
        self.task_queue: queue.PriorityQueue = queue.PriorityQueue()
        self.processing_thread: Optional[threading.Thread] = None
        self.stop_flag = threading.Event()
        
        # Register on communication bus
        self.comm_bus.register_shard(shard_id)
        
        logger.info(f"CognitiveShard {shard_id} created ({specialization.value})")
    
    def start(self) -> None:
        """Start the shard's processing thread"""
        if self.state.is_active:
            logger.warning(f"Shard {self.shard_id} already active")
            return
        
        self.state.is_active = True
        self.stop_flag.clear()
        self.processing_thread = threading.Thread(target=self._processing_loop)
        self.processing_thread.start()
        logger.info(f"Shard {self.shard_id} started")
    
    def stop(self) -> None:
        """Stop the shard's processing thread"""
        if not self.state.is_active:
            return
        
        self.stop_flag.set()
        if self.processing_thread:
            self.processing_thread.join(timeout=2.0)
        self.state.is_active = False
        logger.info(f"Shard {self.shard_id} stopped")
    
    def submit_task(self, task: ShardTask) -> None:
        """Submit a task to the shard"""
        priority = task.priority.value
        self.task_queue.put((priority, task))
        logger.debug(f"Task {task.task_id} submitted to shard {self.shard_id}")
    
    def _processing_loop(self) -> None:
        """Main processing loop for the shard"""
        while not self.stop_flag.is_set():
            # Process messages
            message = self.comm_bus.receive_message(self.shard_id, timeout=0.05)
            if message:
                self._handle_message(message)
            
            # Process tasks
            try:
                priority, task = self.task_queue.get(timeout=0.05)
                self._process_task(task)
            except queue.Empty:
                pass
            
            # Update heartbeat
            self.state.last_heartbeat = time.time()
            
            # Calculate load
            self.state.load = min(
                (self.task_queue.qsize() + self.state.active_tasks) / 10.0,
                1.0
            )
    
    def _handle_message(self, message: ShardMessage) -> None:
        """Handle incoming message"""
        logger.debug(f"Shard {self.shard_id} received message {message.message_id}")
        
        # Handle different message types
        if message.message_type == "task_request":
            task = ShardTask(**message.payload)
            self.submit_task(task)
        elif message.message_type == "query":
            self._handle_query(message)
        elif message.message_type.startswith("topic:"):
            self._handle_topic_message(message)
    
    def _handle_query(self, message: ShardMessage) -> None:
        """Handle a query message"""
        query = message.payload.get('query')
        if query:
            # Simple query handling - in real implementation would be more sophisticated
            results = self.shared_view.query_atoms(
                self.shard_id,
                lambda atom: query in str(atom)
            )
            
            # Send response
            response = ShardMessage(
                message_id=str(uuid.uuid4()),
                sender_id=self.shard_id,
                receiver_id=message.sender_id,
                message_type="query_response",
                priority=message.priority,
                payload={'results': results, 'query': query}
            )
            self.comm_bus.send_message(response)
    
    def _handle_topic_message(self, message: ShardMessage) -> None:
        """Handle a topic subscription message"""
        logger.debug(f"Shard {self.shard_id} received topic message: {message.message_type}")
        # Process topic-specific logic here
    
    def _process_task(self, task: ShardTask) -> None:
        """Process a cognitive task"""
        self.state.active_tasks += 1
        
        try:
            # Simulate task processing based on specialization
            logger.debug(f"Shard {self.shard_id} processing task {task.task_id}")
            
            # Read relevant atoms
            relevant_atoms = self.shared_view.query_atoms(
                self.shard_id,
                lambda atom: task.task_type in atom.get('tags', [])
            )
            
            # Perform specialized processing
            result = self._specialized_processing(task, relevant_atoms)
            
            # Write results back
            result_atom_id = f"result_{task.task_id}"
            self.shared_view.write_atom(
                self.shard_id,
                result_atom_id,
                {
                    'type': 'task_result',
                    'task_id': task.task_id,
                    'result': result,
                    'processor': self.shard_id,
                    'specialization': self.specialization.value
                }
            )
            
            self.state.completed_tasks += 1
            logger.debug(f"Shard {self.shard_id} completed task {task.task_id}")
            
        except Exception as e:
            logger.error(f"Shard {self.shard_id} failed task {task.task_id}: {e}")
            self.state.failed_tasks += 1
        
        finally:
            self.state.active_tasks -= 1
    
    def _specialized_processing(self, task: ShardTask, atoms: List[str]) -> Dict[str, Any]:
        """Perform specialized processing based on shard type"""
        # This is a simplified simulation
        # Real implementation would use actual cognitive algorithms
        
        processing_time = 0.01  # Simulate processing
        time.sleep(processing_time)
        
        return {
            'specialization': self.specialization.value,
            'processed_atoms': len(atoms),
            'processing_time': processing_time,
            'task_type': task.task_type
        }
    
    def get_state(self) -> ShardState:
        """Get current shard state"""
        return self.state


class ShardOrchestrator:
    """
    Orchestrator for managing cognitive shard network
    
    Implements policies for:
    - Task allocation and load balancing
    - Shard lifecycle management
    - Performance monitoring
    - Dynamic scaling
    """
    
    def __init__(self, shared_view: SharedAtomSpaceView,
                 comm_bus: ShardCommunicationBus):
        self.shared_view = shared_view
        self.comm_bus = comm_bus
        self.shards: Dict[str, CognitiveShard] = {}
        self.allocation_policy = "load_balanced"  # or "specialized", "random"
        logger.info("ShardOrchestrator initialized")
    
    def create_shard(self, specialization: ShardSpecialization) -> str:
        """Create a new cognitive shard"""
        shard_id = f"shard_{specialization.value}_{uuid.uuid4().hex[:8]}"
        shard = CognitiveShard(shard_id, specialization, self.shared_view, self.comm_bus)
        self.shards[shard_id] = shard
        logger.info(f"Created shard {shard_id} ({specialization.value})")
        return shard_id
    
    def start_shard(self, shard_id: str) -> None:
        """Start a specific shard"""
        if shard_id in self.shards:
            self.shards[shard_id].start()
    
    def stop_shard(self, shard_id: str) -> None:
        """Stop a specific shard"""
        if shard_id in self.shards:
            self.shards[shard_id].stop()
    
    def start_all_shards(self) -> None:
        """Start all shards"""
        for shard in self.shards.values():
            shard.start()
        logger.info(f"Started {len(self.shards)} shards")
    
    def stop_all_shards(self) -> None:
        """Stop all shards"""
        for shard in self.shards.values():
            shard.stop()
        logger.info(f"Stopped {len(self.shards)} shards")
    
    def submit_task(self, task: ShardTask) -> Optional[str]:
        """Submit a task and allocate it to appropriate shard"""
        target_shard = self._allocate_task(task)
        
        if target_shard:
            target_shard.submit_task(task)
            logger.info(f"Task {task.task_id} allocated to {target_shard.shard_id}")
            return target_shard.shard_id
        
        logger.warning(f"No suitable shard found for task {task.task_id}")
        return None
    
    def _allocate_task(self, task: ShardTask) -> Optional[CognitiveShard]:
        """Allocate task to appropriate shard based on policy"""
        active_shards = [s for s in self.shards.values() if s.state.is_active]
        
        if not active_shards:
            return None
        
        if self.allocation_policy == "specialized":
            # Prefer shards with matching specialization
            specialized = [
                s for s in active_shards
                if task.required_specialization is None or
                   s.specialization == task.required_specialization
            ]
            if specialized:
                return min(specialized, key=lambda s: s.state.load)
            return min(active_shards, key=lambda s: s.state.load)
        
        elif self.allocation_policy == "load_balanced":
            # Always choose least loaded shard
            return min(active_shards, key=lambda s: s.state.load)
        
        else:  # random
            import random
            return random.choice(active_shards)
    
    def get_network_state(self) -> Dict[str, Any]:
        """Get state of entire shard network"""
        return {
            'total_shards': len(self.shards),
            'active_shards': sum(1 for s in self.shards.values() if s.state.is_active),
            'specializations': {
                spec.value: sum(1 for s in self.shards.values() 
                               if s.specialization == spec)
                for spec in ShardSpecialization
            },
            'total_completed_tasks': sum(s.state.completed_tasks for s in self.shards.values()),
            'total_failed_tasks': sum(s.state.failed_tasks for s in self.shards.values()),
            'average_load': sum(s.state.load for s in self.shards.values()) / len(self.shards) if self.shards else 0.0
        }
    
    def get_shard_states(self) -> Dict[str, ShardState]:
        """Get states of all shards"""
        return {
            shard_id: shard.get_state()
            for shard_id, shard in self.shards.items()
        }


class DistributedCognitiveNetwork:
    """
    Main interface for distributed cognitive shard network
    
    Coordinates all components to provide distributed cognition capabilities.
    """
    
    def __init__(self):
        self.shared_view = SharedAtomSpaceView()
        self.comm_bus = ShardCommunicationBus()
        self.orchestrator = ShardOrchestrator(self.shared_view, self.comm_bus)
        logger.info("DistributedCognitiveNetwork initialized")
    
    def create_standard_network(self) -> None:
        """Create a standard network with common specializations"""
        specializations = [
            ShardSpecialization.REASONING,
            ShardSpecialization.LEARNING,
            ShardSpecialization.PATTERN_MINING,
            ShardSpecialization.ATTENTION,
            ShardSpecialization.MEMORY,
        ]
        
        for spec in specializations:
            shard_id = self.orchestrator.create_shard(spec)
            self.orchestrator.start_shard(shard_id)
        
        logger.info("Standard cognitive network created and started")
    
    def submit_task(self, task: ShardTask) -> Optional[str]:
        """Submit a task to the network"""
        return self.orchestrator.submit_task(task)
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get comprehensive network statistics"""
        return {
            'network': self.orchestrator.get_network_state(),
            'shared_view': self.shared_view.get_statistics(),
            'communication': self.comm_bus.get_statistics()
        }
    
    def shutdown(self) -> None:
        """Shutdown the entire network"""
        self.orchestrator.stop_all_shards()
        logger.info("DistributedCognitiveNetwork shutdown complete")


# Convenience functions

def create_distributed_network() -> DistributedCognitiveNetwork:
    """Create and initialize a distributed cognitive network"""
    network = DistributedCognitiveNetwork()
    network.create_standard_network()
    return network


def demonstrate_distributed_cognition():
    """Demonstrate distributed cognitive network capabilities"""
    print("=" * 80)
    print("Distributed Cognitive Shard Network Demonstration")
    print("=" * 80)
    
    # Create network
    network = create_distributed_network()
    
    # Give shards time to start
    time.sleep(0.2)
    
    # Submit some tasks
    print("\nSubmitting tasks...")
    tasks = [
        ShardTask(
            task_id=f"task_{i}",
            task_type="cognitive_task",
            priority=MessagePriority.NORMAL,
            data={'content': f"Task {i}"}
        )
        for i in range(10)
    ]
    
    for task in tasks:
        shard_id = network.submit_task(task)
        if shard_id:
            print(f"  Task {task.task_id} -> {shard_id}")
    
    # Wait for processing
    print("\nProcessing tasks...")
    time.sleep(0.5)
    
    # Get statistics
    print("\nNetwork Statistics:")
    stats = network.get_statistics()
    
    print(f"\n  Network:")
    for key, value in stats['network'].items():
        print(f"    {key}: {value}")
    
    print(f"\n  Shared View:")
    for key, value in stats['shared_view'].items():
        print(f"    {key}: {value}")
    
    print(f"\n  Communication:")
    for key, value in stats['communication'].items():
        print(f"    {key}: {value}")
    
    # Shutdown
    print("\nShutting down network...")
    network.shutdown()
    
    print("\n" + "=" * 80)
    print("Demonstration complete!")
    print("=" * 80)


if __name__ == "__main__":
    logging.basicConfig(
        level=logging.INFO,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    )
    demonstrate_distributed_cognition()
