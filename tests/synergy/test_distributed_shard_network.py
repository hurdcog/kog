"""
Test suite for Distributed Cognitive Shard Network

Tests the network of specialized cognitive shards with shared memory.
"""

import unittest
import sys
import os
import time
import threading

# Add parent directory to path for imports
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '../..'))

from synergy.bridges.distributed_shard_network import (
    DistributedCognitiveNetwork,
    SharedAtomSpaceView,
    ShardCommunicationBus,
    CognitiveShard,
    ShardOrchestrator,
    ShardSpecialization,
    ShardTask,
    ShardMessage,
    MessagePriority,
    create_distributed_network
)


class TestSharedAtomSpaceView(unittest.TestCase):
    """Test shared AtomSpace view"""
    
    def setUp(self):
        self.view = SharedAtomSpaceView()
    
    def test_write_and_read_atom(self):
        """Test writing and reading atoms"""
        atom_data = {'type': 'concept', 'value': 'test'}
        self.view.write_atom('shard1', 'atom1', atom_data)
        
        retrieved = self.view.read_atom('shard1', 'atom1')
        self.assertIsNotNone(retrieved)
        self.assertEqual(retrieved['type'], 'concept')
        self.assertEqual(retrieved['value'], 'test')
    
    def test_thread_safety(self):
        """Test thread-safe access"""
        def writer(shard_id, count):
            for i in range(count):
                self.view.write_atom(
                    shard_id,
                    f'{shard_id}_atom_{i}',
                    {'data': i}
                )
        
        threads = [
            threading.Thread(target=writer, args=(f'shard{i}', 10))
            for i in range(3)
        ]
        
        for t in threads:
            t.start()
        for t in threads:
            t.join()
        
        stats = self.view.get_statistics()
        self.assertEqual(stats['total_atoms'], 30)
    
    def test_query_atoms(self):
        """Test querying atoms with predicate"""
        self.view.write_atom('shard1', 'atom1', {'type': 'concept', 'value': 5})
        self.view.write_atom('shard1', 'atom2', {'type': 'concept', 'value': 10})
        self.view.write_atom('shard1', 'atom3', {'type': 'link', 'value': 3})
        
        concepts = self.view.query_atoms(
            'shard1',
            lambda atom: atom.get('type') == 'concept'
        )
        
        self.assertEqual(len(concepts), 2)
    
    def test_shard_view_tracking(self):
        """Test tracking of shard views"""
        self.view.write_atom('shard1', 'atom1', {'data': 1})
        self.view.read_atom('shard1', 'atom1')
        self.view.read_atom('shard2', 'atom1')
        
        shard1_view = self.view.get_shard_view('shard1')
        shard2_view = self.view.get_shard_view('shard2')
        
        self.assertIn('atom1', shard1_view)
        self.assertIn('atom1', shard2_view)


class TestShardCommunicationBus(unittest.TestCase):
    """Test shard communication bus"""
    
    def setUp(self):
        self.bus = ShardCommunicationBus()
    
    def test_register_and_send(self):
        """Test shard registration and message sending"""
        self.bus.register_shard('shard1')
        self.bus.register_shard('shard2')
        
        message = ShardMessage(
            message_id='msg1',
            sender_id='shard1',
            receiver_id='shard2',
            message_type='test',
            priority=MessagePriority.NORMAL,
            payload={'data': 'hello'}
        )
        
        result = self.bus.send_message(message)
        self.assertTrue(result)
        
        received = self.bus.receive_message('shard2', timeout=0.1)
        self.assertIsNotNone(received)
        self.assertEqual(received.message_id, 'msg1')
    
    def test_broadcast(self):
        """Test broadcasting messages"""
        for i in range(5):
            self.bus.register_shard(f'shard{i}')
        
        count = self.bus.broadcast_message(
            'shard0',
            'broadcast_test',
            {'data': 'broadcast'},
            MessagePriority.NORMAL
        )
        
        self.assertEqual(count, 4)  # All except sender
    
    def test_publish_subscribe(self):
        """Test publish-subscribe pattern"""
        self.bus.register_shard('publisher')
        self.bus.register_shard('subscriber1')
        self.bus.register_shard('subscriber2')
        
        self.bus.subscribe('subscriber1', 'topic_a')
        self.bus.subscribe('subscriber2', 'topic_a')
        
        count = self.bus.publish(
            'publisher',
            'topic_a',
            {'data': 'news'},
            MessagePriority.HIGH
        )
        
        self.assertEqual(count, 2)
        
        msg1 = self.bus.receive_message('subscriber1', timeout=0.1)
        msg2 = self.bus.receive_message('subscriber2', timeout=0.1)
        
        self.assertIsNotNone(msg1)
        self.assertIsNotNone(msg2)
    
    def test_priority_ordering(self):
        """Test that high priority messages are received first"""
        self.bus.register_shard('shard1')
        
        # Send low priority message first
        low_msg = ShardMessage(
            message_id='low',
            sender_id='shard0',
            receiver_id='shard1',
            message_type='test',
            priority=MessagePriority.LOW,
            payload={}
        )
        self.bus.send_message(low_msg)
        
        # Then send high priority message
        high_msg = ShardMessage(
            message_id='high',
            sender_id='shard0',
            receiver_id='shard1',
            message_type='test',
            priority=MessagePriority.HIGH,
            payload={}
        )
        self.bus.send_message(high_msg)
        
        # High priority should be received first
        first = self.bus.receive_message('shard1', timeout=0.1)
        self.assertEqual(first.message_id, 'high')


class TestCognitiveShard(unittest.TestCase):
    """Test individual cognitive shard"""
    
    def setUp(self):
        self.view = SharedAtomSpaceView()
        self.bus = ShardCommunicationBus()
    
    def tearDown(self):
        # Clean up any running shards
        pass
    
    def test_shard_creation(self):
        """Test creating a cognitive shard"""
        shard = CognitiveShard(
            'test_shard',
            ShardSpecialization.REASONING,
            self.view,
            self.bus
        )
        
        self.assertEqual(shard.shard_id, 'test_shard')
        self.assertEqual(shard.specialization, ShardSpecialization.REASONING)
        self.assertFalse(shard.state.is_active)
    
    def test_shard_start_stop(self):
        """Test starting and stopping shard"""
        shard = CognitiveShard(
            'test_shard',
            ShardSpecialization.LEARNING,
            self.view,
            self.bus
        )
        
        shard.start()
        self.assertTrue(shard.state.is_active)
        time.sleep(0.1)  # Let it run briefly
        
        shard.stop()
        self.assertFalse(shard.state.is_active)
    
    def test_task_processing(self):
        """Test that shard processes tasks"""
        shard = CognitiveShard(
            'test_shard',
            ShardSpecialization.GENERAL,
            self.view,
            self.bus
        )
        
        shard.start()
        
        task = ShardTask(
            task_id='task1',
            task_type='test_task',
            priority=MessagePriority.NORMAL,
            data={'test': 'data'}
        )
        
        shard.submit_task(task)
        time.sleep(0.2)  # Wait for processing
        
        state = shard.get_state()
        self.assertGreater(state.completed_tasks, 0)
        
        shard.stop()


class TestShardOrchestrator(unittest.TestCase):
    """Test shard orchestrator"""
    
    def setUp(self):
        self.view = SharedAtomSpaceView()
        self.bus = ShardCommunicationBus()
        self.orchestrator = ShardOrchestrator(self.view, self.bus)
    
    def tearDown(self):
        self.orchestrator.stop_all_shards()
    
    def test_create_shards(self):
        """Test creating shards"""
        shard_id1 = self.orchestrator.create_shard(ShardSpecialization.REASONING)
        shard_id2 = self.orchestrator.create_shard(ShardSpecialization.LEARNING)
        
        self.assertEqual(len(self.orchestrator.shards), 2)
        self.assertIn(shard_id1, self.orchestrator.shards)
        self.assertIn(shard_id2, self.orchestrator.shards)
    
    def test_start_all_shards(self):
        """Test starting all shards"""
        for spec in [ShardSpecialization.REASONING, ShardSpecialization.LEARNING]:
            self.orchestrator.create_shard(spec)
        
        self.orchestrator.start_all_shards()
        time.sleep(0.1)
        
        for shard in self.orchestrator.shards.values():
            self.assertTrue(shard.state.is_active)
    
    def test_task_allocation(self):
        """Test task allocation"""
        for spec in [ShardSpecialization.REASONING, ShardSpecialization.LEARNING]:
            shard_id = self.orchestrator.create_shard(spec)
            self.orchestrator.start_shard(shard_id)
        
        time.sleep(0.1)
        
        task = ShardTask(
            task_id='task1',
            task_type='test',
            priority=MessagePriority.NORMAL,
            data={}
        )
        
        allocated_shard = self.orchestrator.submit_task(task)
        self.assertIsNotNone(allocated_shard)
    
    def test_network_state(self):
        """Test getting network state"""
        self.orchestrator.create_shard(ShardSpecialization.REASONING)
        self.orchestrator.create_shard(ShardSpecialization.LEARNING)
        
        state = self.orchestrator.get_network_state()
        
        self.assertEqual(state['total_shards'], 2)
        self.assertIn('specializations', state)
        self.assertIn('average_load', state)


class TestDistributedCognitiveNetwork(unittest.TestCase):
    """Test the complete distributed cognitive network"""
    
    def setUp(self):
        self.network = DistributedCognitiveNetwork()
    
    def tearDown(self):
        self.network.shutdown()
    
    def test_network_creation(self):
        """Test creating network"""
        self.assertIsNotNone(self.network.shared_view)
        self.assertIsNotNone(self.network.comm_bus)
        self.assertIsNotNone(self.network.orchestrator)
    
    def test_standard_network_creation(self):
        """Test creating standard network"""
        self.network.create_standard_network()
        time.sleep(0.2)
        
        stats = self.network.get_statistics()
        self.assertGreater(stats['network']['total_shards'], 0)
        self.assertGreater(stats['network']['active_shards'], 0)
    
    def test_task_submission(self):
        """Test submitting tasks to network"""
        self.network.create_standard_network()
        time.sleep(0.2)
        
        task = ShardTask(
            task_id='network_task',
            task_type='test',
            priority=MessagePriority.NORMAL,
            data={'test': 'data'}
        )
        
        shard_id = self.network.submit_task(task)
        self.assertIsNotNone(shard_id)
        
        time.sleep(0.3)  # Wait for processing
        
        stats = self.network.get_statistics()
        self.assertGreater(stats['network']['total_completed_tasks'], 0)
    
    def test_multiple_tasks(self):
        """Test processing multiple tasks"""
        self.network.create_standard_network()
        time.sleep(0.2)
        
        # Submit multiple tasks
        tasks = [
            ShardTask(
                task_id=f'task_{i}',
                task_type='test',
                priority=MessagePriority.NORMAL,
                data={'index': i}
            )
            for i in range(10)
        ]
        
        for task in tasks:
            self.network.submit_task(task)
        
        time.sleep(0.5)  # Wait for processing
        
        stats = self.network.get_statistics()
        self.assertGreaterEqual(stats['network']['total_completed_tasks'], 5)
    
    def test_comprehensive_statistics(self):
        """Test getting comprehensive statistics"""
        self.network.create_standard_network()
        time.sleep(0.2)
        
        stats = self.network.get_statistics()
        
        self.assertIn('network', stats)
        self.assertIn('shared_view', stats)
        self.assertIn('communication', stats)
        
        self.assertIn('total_shards', stats['network'])
        self.assertIn('total_atoms', stats['shared_view'])
        self.assertIn('registered_shards', stats['communication'])


class TestIntegration(unittest.TestCase):
    """Integration tests for distributed cognition"""
    
    def test_create_distributed_network(self):
        """Test convenience function"""
        network = create_distributed_network()
        time.sleep(0.2)
        
        self.assertIsInstance(network, DistributedCognitiveNetwork)
        
        stats = network.get_statistics()
        self.assertGreater(stats['network']['active_shards'], 0)
        
        network.shutdown()
    
    def test_inter_shard_communication(self):
        """Test that shards can communicate"""
        network = DistributedCognitiveNetwork()
        network.create_standard_network()
        time.sleep(0.2)
        
        # Get two shard IDs
        shard_ids = list(network.orchestrator.shards.keys())
        if len(shard_ids) >= 2:
            sender_id = shard_ids[0]
            receiver_id = shard_ids[1]
            
            # Send a message
            message = ShardMessage(
                message_id='test_msg',
                sender_id=sender_id,
                receiver_id=receiver_id,
                message_type='test',
                priority=MessagePriority.NORMAL,
                payload={'data': 'test'}
            )
            
            result = network.comm_bus.send_message(message)
            self.assertTrue(result)
        
        network.shutdown()
    
    def test_shared_knowledge(self):
        """Test that shards share knowledge via AtomSpace"""
        network = DistributedCognitiveNetwork()
        network.create_standard_network()
        time.sleep(0.2)
        
        shard_ids = list(network.orchestrator.shards.keys())
        if len(shard_ids) >= 2:
            # First shard writes
            network.shared_view.write_atom(
                shard_ids[0],
                'shared_atom',
                {'data': 'shared_knowledge'}
            )
            
            # Second shard reads
            atom = network.shared_view.read_atom(shard_ids[1], 'shared_atom')
            self.assertIsNotNone(atom)
            self.assertEqual(atom['data'], 'shared_knowledge')
        
        network.shutdown()


def run_tests():
    """Run all tests"""
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    
    # Add all test classes
    suite.addTests(loader.loadTestsFromTestCase(TestSharedAtomSpaceView))
    suite.addTests(loader.loadTestsFromTestCase(TestShardCommunicationBus))
    suite.addTests(loader.loadTestsFromTestCase(TestCognitiveShard))
    suite.addTests(loader.loadTestsFromTestCase(TestShardOrchestrator))
    suite.addTests(loader.loadTestsFromTestCase(TestDistributedCognitiveNetwork))
    suite.addTests(loader.loadTestsFromTestCase(TestIntegration))
    
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    return result.wasSuccessful()


if __name__ == '__main__':
    success = run_tests()
    sys.exit(0 if success else 1)
