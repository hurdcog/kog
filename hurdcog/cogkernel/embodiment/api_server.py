#!/usr/bin/env python3
"""
Distributed Cognitive Mesh API Server
Phase 4: REST API and WebSocket implementation for cognitive network access

This server provides:
- REST endpoints for cognitive state access and task submission
- WebSocket real-time bidirectional communication
- Agent registration and management
- Attention allocation queries
"""

from fastapi import FastAPI, WebSocket, WebSocketDisconnect, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel, Field
from typing import Dict, List, Optional, Any
from datetime import datetime
import asyncio
import json
import logging
from enum import Enum

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# API Models
class CognitiveState(BaseModel):
    """Current state of the cognitive system"""
    timestamp: datetime = Field(default_factory=datetime.now)
    active_processes: int = 0
    memory_usage: float = 0.0
    attention_focus: List[str] = []
    cognitive_load: float = 0.0
    status: str = "operational"

class CognitiveTask(BaseModel):
    """Task submission for cognitive processing"""
    task_id: Optional[str] = None
    description: str
    priority: int = Field(default=5, ge=1, le=10)
    context: Dict[str, Any] = {}
    timeout: Optional[int] = None

class TaskResult(BaseModel):
    """Result of cognitive task processing"""
    task_id: str
    status: str
    result: Optional[Any] = None
    confidence: float = 0.0
    processing_time: float = 0.0
    timestamp: datetime = Field(default_factory=datetime.now)

class AttentionAllocation(BaseModel):
    """Current attention distribution across resources"""
    timestamp: datetime = Field(default_factory=datetime.now)
    allocations: Dict[str, float] = {}
    total_attention: float = 1.0
    focus_target: Optional[str] = None

class AgentInfo(BaseModel):
    """Cognitive agent information"""
    agent_id: str
    agent_type: str
    capabilities: List[str] = []
    status: str = "active"
    connection_time: datetime = Field(default_factory=datetime.now)
    metadata: Dict[str, Any] = {}

class AgentRegistration(BaseModel):
    """Agent registration request"""
    agent_type: str
    capabilities: List[str] = []
    metadata: Dict[str, Any] = {}

# WebSocket Event Types
class EventType(str, Enum):
    COGNITIVE_STATE_UPDATE = "cognitive.state.update"
    ATTENTION_ALLOCATION_CHANGE = "attention.allocation.change"
    AGENT_REGISTRATION = "agent.registration"
    TASK_COMPLETION = "task.completion"

# Application state
class CognitiveNetworkState:
    """Global state for the cognitive network"""
    def __init__(self):
        self.cognitive_state = CognitiveState()
        self.attention_allocation = AttentionAllocation()
        self.agents: Dict[str, AgentInfo] = {}
        self.tasks: Dict[str, TaskResult] = {}
        self.websocket_connections: List[WebSocket] = []
        self.task_counter = 0

    def generate_task_id(self) -> str:
        """Generate unique task ID"""
        self.task_counter += 1
        return f"task_{self.task_counter}_{datetime.now().timestamp()}"

    async def broadcast_event(self, event_type: EventType, data: Any):
        """Broadcast event to all WebSocket connections"""
        message = {
            "event": event_type.value,
            "data": data,
            "timestamp": datetime.now().isoformat()
        }

        disconnected = []
        for connection in self.websocket_connections:
            try:
                await connection.send_json(message)
            except Exception as e:
                logger.error(f"Error broadcasting to connection: {e}")
                disconnected.append(connection)

        # Remove disconnected clients
        for conn in disconnected:
            self.websocket_connections.remove(conn)

# Initialize FastAPI app
app = FastAPI(
    title="Distributed Cognitive Mesh API",
    description="REST and WebSocket API for cognitive network access and embodiment",
    version="1.0.0"
)

# Add CORS middleware for web agents
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Configure appropriately for production
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Initialize global state
network_state = CognitiveNetworkState()

# REST API Endpoints

@app.get("/")
async def root():
    """API root endpoint"""
    return {
        "service": "Distributed Cognitive Mesh API",
        "version": "1.0.0",
        "status": "operational",
        "endpoints": {
            "cognitive_state": "/api/v1/cognitive/state",
            "process_task": "/api/v1/cognitive/process",
            "attention": "/api/v1/attention/allocation",
            "register_agent": "/api/v1/agents/register",
            "websocket": "/ws"
        }
    }

@app.get("/api/v1/cognitive/state", response_model=CognitiveState)
async def get_cognitive_state():
    """Get current cognitive state"""
    return network_state.cognitive_state

@app.post("/api/v1/cognitive/process", response_model=TaskResult)
async def process_cognitive_task(task: CognitiveTask):
    """Submit cognitive task for processing"""
    # Generate task ID if not provided
    if not task.task_id:
        task.task_id = network_state.generate_task_id()

    # Simulate cognitive processing
    result = TaskResult(
        task_id=task.task_id,
        status="completed",
        result={"processed": True, "description": task.description},
        confidence=0.85,
        processing_time=0.042
    )

    # Store result
    network_state.tasks[task.task_id] = result

    # Update cognitive state
    network_state.cognitive_state.active_processes += 1
    network_state.cognitive_state.cognitive_load += 0.1

    # Broadcast task completion event
    await network_state.broadcast_event(
        EventType.TASK_COMPLETION,
        result.model_dump()
    )

    return result

@app.get("/api/v1/cognitive/task/{task_id}", response_model=TaskResult)
async def get_task_result(task_id: str):
    """Get result of a specific cognitive task"""
    if task_id not in network_state.tasks:
        raise HTTPException(status_code=404, detail="Task not found")
    return network_state.tasks[task_id]

@app.get("/api/v1/attention/allocation", response_model=AttentionAllocation)
async def get_attention_allocation():
    """Get current attention distribution"""
    return network_state.attention_allocation

@app.post("/api/v1/attention/focus")
async def set_attention_focus(target: str, weight: float = 1.0):
    """Set attention focus target"""
    # Update attention allocation
    network_state.attention_allocation.focus_target = target
    network_state.attention_allocation.allocations[target] = weight

    # Normalize allocations
    total = sum(network_state.attention_allocation.allocations.values())
    for key in network_state.attention_allocation.allocations:
        network_state.attention_allocation.allocations[key] /= total

    # Broadcast attention change event
    await network_state.broadcast_event(
        EventType.ATTENTION_ALLOCATION_CHANGE,
        network_state.attention_allocation.model_dump()
    )

    return {"status": "success", "target": target, "weight": weight}

@app.post("/api/v1/agents/register", response_model=AgentInfo)
async def register_agent(registration: AgentRegistration):
    """Register new cognitive agent"""
    import uuid

    # Generate agent ID
    agent_id = f"agent_{uuid.uuid4().hex[:8]}"

    # Create agent info
    agent = AgentInfo(
        agent_id=agent_id,
        agent_type=registration.agent_type,
        capabilities=registration.capabilities,
        metadata=registration.metadata
    )

    # Store agent
    network_state.agents[agent_id] = agent

    # Broadcast agent registration event
    await network_state.broadcast_event(
        EventType.AGENT_REGISTRATION,
        agent.model_dump()
    )

    return agent

@app.get("/api/v1/agents/{agent_id}", response_model=AgentInfo)
async def get_agent(agent_id: str):
    """Get agent information"""
    if agent_id not in network_state.agents:
        raise HTTPException(status_code=404, detail="Agent not found")
    return network_state.agents[agent_id]

@app.get("/api/v1/agents", response_model=List[AgentInfo])
async def list_agents():
    """List all registered agents"""
    return list(network_state.agents.values())

@app.delete("/api/v1/agents/{agent_id}")
async def unregister_agent(agent_id: str):
    """Unregister cognitive agent"""
    if agent_id not in network_state.agents:
        raise HTTPException(status_code=404, detail="Agent not found")

    agent = network_state.agents.pop(agent_id)
    return {"status": "unregistered", "agent_id": agent_id}

@app.get("/api/v1/health")
async def health_check():
    """Health check endpoint"""
    return {
        "status": "healthy",
        "timestamp": datetime.now().isoformat(),
        "agents": len(network_state.agents),
        "active_connections": len(network_state.websocket_connections),
        "tasks_processed": len(network_state.tasks)
    }

# WebSocket endpoint for real-time communication
@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    """WebSocket endpoint for real-time bidirectional communication"""
    await websocket.accept()
    network_state.websocket_connections.append(websocket)

    logger.info(f"WebSocket connection established. Total connections: {len(network_state.websocket_connections)}")

    try:
        # Send initial state
        await websocket.send_json({
            "event": "connection.established",
            "data": {
                "cognitive_state": network_state.cognitive_state.model_dump(),
                "attention_allocation": network_state.attention_allocation.model_dump()
            },
            "timestamp": datetime.now().isoformat()
        })

        # Listen for messages from client
        while True:
            data = await websocket.receive_text()
            message = json.loads(data)

            # Handle different message types
            if message.get("type") == "ping":
                await websocket.send_json({"type": "pong", "timestamp": datetime.now().isoformat()})

            elif message.get("type") == "subscribe":
                # Handle event subscription
                events = message.get("events", [])
                await websocket.send_json({
                    "type": "subscribed",
                    "events": events,
                    "timestamp": datetime.now().isoformat()
                })

            elif message.get("type") == "task":
                # Handle task submission via WebSocket
                task_data = message.get("data", {})
                task = CognitiveTask(**task_data)
                result = await process_cognitive_task(task)
                await websocket.send_json({
                    "type": "task.result",
                    "data": result.model_dump(),
                    "timestamp": datetime.now().isoformat()
                })

    except WebSocketDisconnect:
        logger.info("WebSocket connection closed")
    except Exception as e:
        logger.error(f"WebSocket error: {e}")
    finally:
        if websocket in network_state.websocket_connections:
            network_state.websocket_connections.remove(websocket)
        logger.info(f"WebSocket connection removed. Total connections: {len(network_state.websocket_connections)}")

# Background task to simulate cognitive state updates
@app.on_event("startup")
async def startup_event():
    """Initialize background tasks on startup"""
    asyncio.create_task(simulate_cognitive_activity())

async def simulate_cognitive_activity():
    """Simulate periodic cognitive state updates"""
    while True:
        await asyncio.sleep(5)  # Update every 5 seconds

        # Update cognitive state
        network_state.cognitive_state.timestamp = datetime.now()
        network_state.cognitive_state.cognitive_load = max(0, network_state.cognitive_state.cognitive_load - 0.05)

        # Broadcast state update
        await network_state.broadcast_event(
            EventType.COGNITIVE_STATE_UPDATE,
            network_state.cognitive_state.model_dump()
        )

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
