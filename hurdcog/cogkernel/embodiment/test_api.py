#!/usr/bin/env python3
"""
Test suite for Distributed Cognitive Mesh API
Tests REST endpoints, WebSocket connections, and load handling
"""

import pytest
import asyncio
import requests
import json
from datetime import datetime
from typing import Dict, List

# Test configuration
API_BASE_URL = "http://localhost:8000"
WS_URL = "ws://localhost:8000/ws"

class TestRESTAPI:
    """Test REST API endpoints"""

    def test_api_root(self):
        """Test API root endpoint"""
        response = requests.get(f"{API_BASE_URL}/")
        assert response.status_code == 200
        data = response.json()
        assert data["service"] == "Distributed Cognitive Mesh API"
        assert data["version"] == "1.0.0"
        assert "endpoints" in data

    def test_get_cognitive_state(self):
        """Test getting cognitive state"""
        response = requests.get(f"{API_BASE_URL}/api/v1/cognitive/state")
        assert response.status_code == 200
        state = response.json()
        assert "timestamp" in state
        assert "active_processes" in state
        assert "cognitive_load" in state
        assert "status" in state

    def test_submit_cognitive_task(self):
        """Test submitting a cognitive task"""
        task = {
            "description": "Test cognitive processing task",
            "priority": 7,
            "context": {"test": True}
        }

        response = requests.post(
            f"{API_BASE_URL}/api/v1/cognitive/process",
            json=task
        )

        assert response.status_code == 200
        result = response.json()
        assert "task_id" in result
        assert result["status"] == "completed"
        assert "confidence" in result
        assert result["confidence"] > 0

    def test_get_task_result(self):
        """Test retrieving task results"""
        # First submit a task
        task = {
            "description": "Task to retrieve",
            "priority": 5
        }

        response = requests.post(
            f"{API_BASE_URL}/api/v1/cognitive/process",
            json=task
        )
        result = response.json()
        task_id = result["task_id"]

        # Now retrieve it
        response = requests.get(
            f"{API_BASE_URL}/api/v1/cognitive/task/{task_id}"
        )

        assert response.status_code == 200
        retrieved = response.json()
        assert retrieved["task_id"] == task_id

    def test_get_attention_allocation(self):
        """Test getting attention allocation"""
        response = requests.get(f"{API_BASE_URL}/api/v1/attention/allocation")
        assert response.status_code == 200
        attention = response.json()
        assert "timestamp" in attention
        assert "allocations" in attention
        assert "total_attention" in attention

    def test_set_attention_focus(self):
        """Test setting attention focus"""
        response = requests.post(
            f"{API_BASE_URL}/api/v1/attention/focus",
            params={"target": "test_target", "weight": 0.8}
        )

        assert response.status_code == 200
        result = response.json()
        assert result["status"] == "success"
        assert result["target"] == "test_target"

    def test_register_agent(self):
        """Test agent registration"""
        registration = {
            "agent_type": "test_agent",
            "capabilities": ["testing", "validation"],
            "metadata": {"test": True, "timestamp": datetime.now().isoformat()}
        }

        response = requests.post(
            f"{API_BASE_URL}/api/v1/agents/register",
            json=registration
        )

        assert response.status_code == 200
        agent = response.json()
        assert "agent_id" in agent
        assert agent["agent_type"] == "test_agent"
        assert agent["status"] == "active"
        assert len(agent["capabilities"]) == 2

        return agent["agent_id"]

    def test_get_agent(self):
        """Test retrieving agent information"""
        # First register an agent
        agent_id = self.test_register_agent()

        # Now retrieve it
        response = requests.get(f"{API_BASE_URL}/api/v1/agents/{agent_id}")
        assert response.status_code == 200
        agent = response.json()
        assert agent["agent_id"] == agent_id

    def test_list_agents(self):
        """Test listing all agents"""
        response = requests.get(f"{API_BASE_URL}/api/v1/agents")
        assert response.status_code == 200
        agents = response.json()
        assert isinstance(agents, list)

    def test_unregister_agent(self):
        """Test unregistering an agent"""
        # First register an agent
        agent_id = self.test_register_agent()

        # Now unregister it
        response = requests.delete(f"{API_BASE_URL}/api/v1/agents/{agent_id}")
        assert response.status_code == 200
        result = response.json()
        assert result["status"] == "unregistered"

    def test_health_check(self):
        """Test health check endpoint"""
        response = requests.get(f"{API_BASE_URL}/api/v1/health")
        assert response.status_code == 200
        health = response.json()
        assert health["status"] == "healthy"
        assert "timestamp" in health
        assert "agents" in health
        assert "tasks_processed" in health

    def test_nonexistent_task(self):
        """Test retrieving nonexistent task"""
        response = requests.get(
            f"{API_BASE_URL}/api/v1/cognitive/task/nonexistent_id"
        )
        assert response.status_code == 404

    def test_nonexistent_agent(self):
        """Test retrieving nonexistent agent"""
        response = requests.get(
            f"{API_BASE_URL}/api/v1/agents/nonexistent_id"
        )
        assert response.status_code == 404


class TestWebSocketAPI:
    """Test WebSocket real-time communication"""

    @pytest.mark.asyncio
    async def test_websocket_connection(self):
        """Test WebSocket connection establishment"""
        try:
            import websockets

            async with websockets.connect(WS_URL) as websocket:
                # Receive connection established message
                message = await websocket.recv()
                data = json.loads(message)
                assert data["event"] == "connection.established"
                assert "cognitive_state" in data["data"]

        except ImportError:
            pytest.skip("websockets library not available")

    @pytest.mark.asyncio
    async def test_websocket_ping_pong(self):
        """Test WebSocket ping/pong"""
        try:
            import websockets

            async with websockets.connect(WS_URL) as websocket:
                # Skip initial connection message
                await websocket.recv()

                # Send ping
                await websocket.send(json.dumps({"type": "ping"}))

                # Receive pong
                message = await websocket.recv()
                data = json.loads(message)
                assert data["type"] == "pong"

        except ImportError:
            pytest.skip("websockets library not available")

    @pytest.mark.asyncio
    async def test_websocket_task_submission(self):
        """Test submitting task via WebSocket"""
        try:
            import websockets

            async with websockets.connect(WS_URL) as websocket:
                # Skip initial connection message
                await websocket.recv()

                # Submit task
                task = {
                    "type": "task",
                    "data": {
                        "description": "WebSocket test task",
                        "priority": 5
                    }
                }
                await websocket.send(json.dumps(task))

                # Receive result
                message = await websocket.recv()
                data = json.loads(message)
                assert data["type"] == "task.result"
                assert "task_id" in data["data"]

        except ImportError:
            pytest.skip("websockets library not available")

    @pytest.mark.asyncio
    async def test_websocket_event_subscription(self):
        """Test subscribing to events"""
        try:
            import websockets

            async with websockets.connect(WS_URL) as websocket:
                # Skip initial connection message
                await websocket.recv()

                # Subscribe to events
                subscription = {
                    "type": "subscribe",
                    "events": ["cognitive.state.update", "attention.allocation.change"]
                }
                await websocket.send(json.dumps(subscription))

                # Receive subscription confirmation
                message = await websocket.recv()
                data = json.loads(message)
                assert data["type"] == "subscribed"
                assert len(data["events"]) == 2

        except ImportError:
            pytest.skip("websockets library not available")


class TestLoadHandling:
    """Test system load handling and performance"""

    def test_concurrent_task_submission(self):
        """Test submitting multiple tasks concurrently"""
        import concurrent.futures

        def submit_task(i):
            task = {
                "description": f"Concurrent task {i}",
                "priority": 5
            }
            response = requests.post(
                f"{API_BASE_URL}/api/v1/cognitive/process",
                json=task
            )
            return response.status_code == 200

        # Submit 100 concurrent tasks
        with concurrent.futures.ThreadPoolExecutor(max_workers=10) as executor:
            futures = [executor.submit(submit_task, i) for i in range(100)]
            results = [f.result() for f in concurrent.futures.as_completed(futures)]

        # All tasks should succeed
        assert all(results)

    def test_concurrent_agent_registration(self):
        """Test registering multiple agents concurrently"""
        import concurrent.futures

        def register_agent(i):
            registration = {
                "agent_type": f"load_test_agent_{i}",
                "capabilities": ["testing"],
                "metadata": {"index": i}
            }
            response = requests.post(
                f"{API_BASE_URL}/api/v1/agents/register",
                json=registration
            )
            return response.status_code == 200

        # Register 50 concurrent agents
        with concurrent.futures.ThreadPoolExecutor(max_workers=10) as executor:
            futures = [executor.submit(register_agent, i) for i in range(50)]
            results = [f.result() for f in concurrent.futures.as_completed(futures)]

        # All registrations should succeed
        assert all(results)

    def test_api_response_time(self):
        """Test API response times"""
        import time

        # Test state endpoint
        start = time.time()
        response = requests.get(f"{API_BASE_URL}/api/v1/cognitive/state")
        elapsed = time.time() - start

        assert response.status_code == 200
        assert elapsed < 0.1  # Should respond in less than 100ms

    def test_sustained_load(self):
        """Test system under sustained load"""
        import time

        success_count = 0
        total_time = 0
        iterations = 100

        for i in range(iterations):
            start = time.time()

            task = {
                "description": f"Sustained load task {i}",
                "priority": 5
            }

            response = requests.post(
                f"{API_BASE_URL}/api/v1/cognitive/process",
                json=task
            )

            if response.status_code == 200:
                success_count += 1

            total_time += time.time() - start

        # Should have >95% success rate
        assert success_count / iterations > 0.95

        # Average response time should be reasonable
        avg_time = total_time / iterations
        assert avg_time < 0.2  # Average under 200ms


class TestDataValidation:
    """Test data validation and error handling"""

    def test_invalid_task_priority(self):
        """Test task with invalid priority"""
        task = {
            "description": "Test task",
            "priority": 15  # Invalid, should be 1-10
        }

        response = requests.post(
            f"{API_BASE_URL}/api/v1/cognitive/process",
            json=task
        )

        # Should return validation error
        assert response.status_code == 422

    def test_missing_required_fields(self):
        """Test task submission without required fields"""
        task = {
            "priority": 5
            # Missing description
        }

        response = requests.post(
            f"{API_BASE_URL}/api/v1/cognitive/process",
            json=task
        )

        assert response.status_code == 422

    def test_invalid_agent_registration(self):
        """Test agent registration with invalid data"""
        registration = {
            # Missing agent_type
            "capabilities": ["testing"]
        }

        response = requests.post(
            f"{API_BASE_URL}/api/v1/agents/register",
            json=registration
        )

        assert response.status_code == 422


def run_integration_tests():
    """Run all integration tests"""
    print("Running Phase 4 API Integration Tests...")
    print("=" * 60)

    # Test REST API
    print("\n[1/4] Testing REST API Endpoints...")
    test_rest = TestRESTAPI()
    try:
        test_rest.test_api_root()
        print("  ✓ API root")
        test_rest.test_get_cognitive_state()
        print("  ✓ Get cognitive state")
        test_rest.test_submit_cognitive_task()
        print("  ✓ Submit cognitive task")
        test_rest.test_get_attention_allocation()
        print("  ✓ Get attention allocation")
        test_rest.test_register_agent()
        print("  ✓ Register agent")
        test_rest.test_health_check()
        print("  ✓ Health check")
        print("  ✅ REST API tests passed")
    except Exception as e:
        print(f"  ❌ REST API tests failed: {e}")
        return False

    # Test Load Handling
    print("\n[2/4] Testing Load Handling...")
    test_load = TestLoadHandling()
    try:
        test_load.test_concurrent_task_submission()
        print("  ✓ Concurrent task submission (100 tasks)")
        test_load.test_concurrent_agent_registration()
        print("  ✓ Concurrent agent registration (50 agents)")
        test_load.test_api_response_time()
        print("  ✓ API response time")
        print("  ✅ Load handling tests passed")
    except Exception as e:
        print(f"  ❌ Load handling tests failed: {e}")
        return False

    # Test Data Validation
    print("\n[3/4] Testing Data Validation...")
    test_validation = TestDataValidation()
    try:
        test_validation.test_invalid_task_priority()
        print("  ✓ Invalid task priority validation")
        test_validation.test_missing_required_fields()
        print("  ✓ Missing required fields validation")
        test_validation.test_invalid_agent_registration()
        print("  ✓ Invalid agent registration validation")
        print("  ✅ Data validation tests passed")
    except Exception as e:
        print(f"  ❌ Data validation tests failed: {e}")
        return False

    print("\n[4/4] WebSocket tests require async runtime (use pytest)")

    print("\n" + "=" * 60)
    print("✅ All integration tests passed!")
    print("\nPhase 4 API Implementation: SUCCESS")
    return True


if __name__ == "__main__":
    # Check if server is running
    try:
        response = requests.get(f"{API_BASE_URL}/", timeout=2)
        if response.status_code == 200:
            success = run_integration_tests()
            exit(0 if success else 1)
        else:
            print("❌ API server returned unexpected status code")
            exit(1)
    except requests.exceptions.ConnectionError:
        print("❌ API server is not running!")
        print(f"   Start the server with: python3 cogkernel/embodiment/api_server.py")
        print(f"   Or: uvicorn cogkernel.embodiment.api_server:app --host 0.0.0.0 --port 8000")
        exit(1)
    except Exception as e:
        print(f"❌ Error connecting to API server: {e}")
        exit(1)
