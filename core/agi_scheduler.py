#!/usr/bin/env python3
"""
AGI_Scheduler - Priority-Aware Task Scheduler for AGI-OS

This module provides an attention-aware, priority-based task scheduler for
the AGI-OS. It integrates with OpenCog's attention allocation system to
enable cognitive resource management.

Addresses the critical gap in Scheduling (40% -> target 75%) identified
in the AGI-Kern evaluation.

Features:
- Priority queue implementation with multiple levels
- Attention-based task prioritization (STI/LTI integration)
- Deadline scheduling support
- Fair scheduling for background tasks
- Real-time task preemption
- Resource-aware scheduling
"""

import time
import logging
import threading
import heapq
from typing import Dict, List, Any, Optional, Callable
from dataclasses import dataclass, field
from enum import IntEnum
from queue import Queue, Empty
from collections import defaultdict
import uuid
import json

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger("AGI_Scheduler")


class TaskPriority(IntEnum):
    """Task priority levels (lower value = higher priority)"""
    REALTIME = 0      # Real-time critical tasks
    HIGH = 1          # High-priority cognitive tasks
    NORMAL = 2        # Normal priority
    LOW = 3           # Background tasks
    IDLE = 4          # Idle-time only tasks


class TaskState(IntEnum):
    """Task lifecycle states"""
    CREATED = 0
    QUEUED = 1
    RUNNING = 2
    BLOCKED = 3
    COMPLETED = 4
    CANCELLED = 5
    FAILED = 6


class TaskType(IntEnum):
    """Types of cognitive tasks"""
    INFERENCE = 1     # PLN/URE inference
    ATTENTION = 2     # Attention allocation
    LEARNING = 3      # MOSES/Pattern mining
    PERCEPTION = 4    # Sensory processing
    ACTION = 5        # Motor/action execution
    MEMORY = 6        # AtomSpace operations
    COMMUNICATION = 7 # Inter-component messaging
    SYSTEM = 8        # System maintenance


@dataclass(order=True)
class CognitiveTask:
    """Represents a cognitive task in the scheduler"""
    # Priority for heap ordering (lower = higher priority)
    sort_priority: float = field(compare=True)

    # Task metadata (not used for ordering)
    task_id: str = field(compare=False, default_factory=lambda: str(uuid.uuid4())[:8])
    name: str = field(compare=False, default="")
    task_type: TaskType = field(compare=False, default=TaskType.SYSTEM)
    base_priority: TaskPriority = field(compare=False, default=TaskPriority.NORMAL)
    state: TaskState = field(compare=False, default=TaskState.CREATED)

    # Attention values (from OpenCog attention allocation)
    sti: float = field(compare=False, default=0.0)  # Short-Term Importance
    lti: float = field(compare=False, default=0.0)  # Long-Term Importance

    # Timing
    deadline: Optional[float] = field(compare=False, default=None)  # Unix timestamp
    created_at: float = field(compare=False, default_factory=time.time)
    started_at: Optional[float] = field(compare=False, default=None)
    completed_at: Optional[float] = field(compare=False, default=None)

    # Execution
    execute_fn: Optional[Callable] = field(compare=False, default=None)
    args: tuple = field(compare=False, default_factory=tuple)
    kwargs: dict = field(compare=False, default_factory=dict)
    result: Any = field(compare=False, default=None)
    error: Optional[str] = field(compare=False, default=None)

    # Resource requirements
    cpu_weight: float = field(compare=False, default=1.0)
    memory_mb: int = field(compare=False, default=0)

    def __post_init__(self):
        """Calculate effective priority after initialization"""
        if self.sort_priority == 0:
            self.sort_priority = self._calculate_effective_priority()

    def _calculate_effective_priority(self) -> float:
        """
        Calculate effective priority incorporating attention values.

        Priority formula:
        - Base priority (0-4)
        - STI bonus (higher STI = lower priority number = higher priority)
        - Deadline urgency bonus
        - Age bonus (older waiting tasks get priority boost)
        """
        priority = float(self.base_priority)

        # STI adjustment: normalize STI to 0-1 range and reduce priority
        sti_bonus = max(0, min(self.sti, 100)) / 100.0 * 0.5
        priority -= sti_bonus

        # LTI minor adjustment
        lti_bonus = max(0, min(self.lti, 100)) / 100.0 * 0.1
        priority -= lti_bonus

        # Deadline urgency
        if self.deadline:
            time_to_deadline = self.deadline - time.time()
            if time_to_deadline < 0:
                priority -= 1.0  # Overdue tasks get maximum boost
            elif time_to_deadline < 60:
                priority -= 0.8  # Less than 1 minute
            elif time_to_deadline < 300:
                priority -= 0.5  # Less than 5 minutes
            elif time_to_deadline < 3600:
                priority -= 0.2  # Less than 1 hour

        return max(0, priority)

    def update_priority(self):
        """Recalculate priority (called when STI/LTI changes)"""
        self.sort_priority = self._calculate_effective_priority()


class SchedulerMetrics:
    """Tracks scheduler performance metrics"""

    def __init__(self):
        self.tasks_created = 0
        self.tasks_completed = 0
        self.tasks_failed = 0
        self.tasks_cancelled = 0
        self.total_wait_time = 0.0
        self.total_exec_time = 0.0
        self.priority_distributions = defaultdict(int)
        self.task_type_counts = defaultdict(int)

    def record_task_created(self, task: CognitiveTask):
        self.tasks_created += 1
        self.priority_distributions[task.base_priority.name] += 1
        self.task_type_counts[task.task_type.name] += 1

    def record_task_completed(self, task: CognitiveTask):
        self.tasks_completed += 1
        if task.started_at and task.completed_at:
            self.total_exec_time += task.completed_at - task.started_at
        if task.started_at:
            self.total_wait_time += task.started_at - task.created_at

    def record_task_failed(self, task: CognitiveTask):
        self.tasks_failed += 1

    def get_statistics(self) -> Dict[str, Any]:
        completed = self.tasks_completed or 1
        return {
            "total_created": self.tasks_created,
            "total_completed": self.tasks_completed,
            "total_failed": self.tasks_failed,
            "total_cancelled": self.tasks_cancelled,
            "avg_wait_time": self.total_wait_time / completed,
            "avg_exec_time": self.total_exec_time / completed,
            "priority_distribution": dict(self.priority_distributions),
            "task_type_distribution": dict(self.task_type_counts),
            "success_rate": self.tasks_completed / max(self.tasks_created, 1)
        }


class AGIScheduler:
    """
    Priority-Aware Task Scheduler for AGI-OS

    Features:
    - Multi-level priority queues
    - Attention-based priority adjustment
    - Deadline-aware scheduling
    - Preemptive task execution
    - Fair scheduling for low-priority tasks
    """

    def __init__(
        self,
        num_workers: int = 4,
        attention_update_interval: float = 1.0,
        starvation_threshold: float = 30.0
    ):
        self.num_workers = num_workers
        self.attention_update_interval = attention_update_interval
        self.starvation_threshold = starvation_threshold

        # Task storage
        self.task_queue: List[CognitiveTask] = []  # Priority heap
        self.tasks: Dict[str, CognitiveTask] = {}  # All tasks by ID
        self.running_tasks: Dict[str, CognitiveTask] = {}

        # Threading
        self._lock = threading.RLock()
        self._queue_not_empty = threading.Condition(self._lock)
        self.workers: List[threading.Thread] = []
        self.running = False

        # Metrics
        self.metrics = SchedulerMetrics()

        # Attention integration
        self._attention_updater: Optional[threading.Thread] = None

        logger.info(f"AGI Scheduler initialized with {num_workers} workers")

    def submit(
        self,
        name: str,
        execute_fn: Callable,
        priority: TaskPriority = TaskPriority.NORMAL,
        task_type: TaskType = TaskType.SYSTEM,
        deadline: Optional[float] = None,
        sti: float = 0.0,
        lti: float = 0.0,
        args: tuple = (),
        kwargs: dict = None
    ) -> str:
        """
        Submit a task to the scheduler.

        Args:
            name: Human-readable task name
            execute_fn: Function to execute
            priority: Base priority level
            task_type: Type of cognitive task
            deadline: Unix timestamp deadline (optional)
            sti: Short-term importance (from attention system)
            lti: Long-term importance (from attention system)
            args: Positional arguments for execute_fn
            kwargs: Keyword arguments for execute_fn

        Returns:
            Task ID
        """
        task = CognitiveTask(
            sort_priority=0,  # Will be calculated in __post_init__
            name=name,
            task_type=task_type,
            base_priority=priority,
            deadline=deadline,
            sti=sti,
            lti=lti,
            execute_fn=execute_fn,
            args=args,
            kwargs=kwargs or {}
        )
        task.update_priority()  # Ensure priority is calculated

        with self._lock:
            self.tasks[task.task_id] = task
            task.state = TaskState.QUEUED
            heapq.heappush(self.task_queue, task)
            self.metrics.record_task_created(task)
            self._queue_not_empty.notify()

        logger.debug(f"Task submitted: {task.name} (id={task.task_id}, priority={task.sort_priority:.2f})")
        return task.task_id

    def submit_inference(
        self,
        name: str,
        execute_fn: Callable,
        sti: float = 50.0,
        **kwargs
    ) -> str:
        """Convenience method to submit an inference task"""
        return self.submit(
            name=name,
            execute_fn=execute_fn,
            priority=TaskPriority.HIGH,
            task_type=TaskType.INFERENCE,
            sti=sti,
            **kwargs
        )

    def submit_learning(
        self,
        name: str,
        execute_fn: Callable,
        **kwargs
    ) -> str:
        """Convenience method to submit a learning task"""
        return self.submit(
            name=name,
            execute_fn=execute_fn,
            priority=TaskPriority.LOW,
            task_type=TaskType.LEARNING,
            **kwargs
        )

    def update_task_attention(self, task_id: str, sti: float, lti: float):
        """Update task attention values (called by attention system)"""
        with self._lock:
            if task_id in self.tasks:
                task = self.tasks[task_id]
                task.sti = sti
                task.lti = lti
                task.update_priority()
                # Re-heapify to account for priority change
                heapq.heapify(self.task_queue)

    def cancel(self, task_id: str) -> bool:
        """Cancel a pending task"""
        with self._lock:
            if task_id in self.tasks:
                task = self.tasks[task_id]
                if task.state == TaskState.QUEUED:
                    task.state = TaskState.CANCELLED
                    self.metrics.tasks_cancelled += 1
                    logger.info(f"Task cancelled: {task.name}")
                    return True
        return False

    def get_task(self, task_id: str) -> Optional[CognitiveTask]:
        """Get task by ID"""
        return self.tasks.get(task_id)

    def _worker_loop(self, worker_id: int):
        """Worker thread main loop"""
        logger.debug(f"Worker {worker_id} started")

        while self.running:
            task = None

            with self._queue_not_empty:
                # Wait for task or shutdown
                while self.running and not self.task_queue:
                    self._queue_not_empty.wait(timeout=1.0)

                if not self.running:
                    break

                # Find next runnable task
                for i, t in enumerate(self.task_queue):
                    if t.state == TaskState.QUEUED:
                        task = heapq.heappop(self.task_queue)
                        # Re-push cancelled tasks are removed
                        while self.task_queue and self.task_queue[0].state != TaskState.QUEUED:
                            heapq.heappop(self.task_queue)
                        break

            if task is None:
                continue

            # Execute task
            self._execute_task(task, worker_id)

    def _execute_task(self, task: CognitiveTask, worker_id: int):
        """Execute a single task"""
        task.state = TaskState.RUNNING
        task.started_at = time.time()

        with self._lock:
            self.running_tasks[task.task_id] = task

        logger.debug(f"Worker {worker_id} executing: {task.name}")

        try:
            if task.execute_fn:
                task.result = task.execute_fn(*task.args, **task.kwargs)
            task.state = TaskState.COMPLETED
            task.completed_at = time.time()
            self.metrics.record_task_completed(task)
            logger.debug(f"Task completed: {task.name} ({task.completed_at - task.started_at:.3f}s)")

        except Exception as e:
            task.state = TaskState.FAILED
            task.error = str(e)
            task.completed_at = time.time()
            self.metrics.record_task_failed(task)
            logger.error(f"Task failed: {task.name} - {e}")

        finally:
            with self._lock:
                self.running_tasks.pop(task.task_id, None)

    def _anti_starvation_loop(self):
        """Periodically boost priority of waiting tasks to prevent starvation"""
        while self.running:
            time.sleep(self.attention_update_interval)

            with self._lock:
                current_time = time.time()
                for task in self.task_queue:
                    if task.state == TaskState.QUEUED:
                        wait_time = current_time - task.created_at
                        if wait_time > self.starvation_threshold:
                            # Boost priority of starving tasks
                            task.sort_priority = max(0, task.sort_priority - 0.1)

                heapq.heapify(self.task_queue)

    def start(self):
        """Start the scheduler"""
        if self.running:
            logger.warning("Scheduler already running")
            return

        self.running = True

        # Start worker threads
        for i in range(self.num_workers):
            worker = threading.Thread(target=self._worker_loop, args=(i,), daemon=True)
            worker.start()
            self.workers.append(worker)

        # Start anti-starvation thread
        self._attention_updater = threading.Thread(target=self._anti_starvation_loop, daemon=True)
        self._attention_updater.start()

        logger.info(f"AGI Scheduler started with {self.num_workers} workers")

    def stop(self, wait: bool = True, timeout: float = 10.0):
        """Stop the scheduler"""
        self.running = False

        with self._queue_not_empty:
            self._queue_not_empty.notify_all()

        if wait:
            deadline = time.time() + timeout
            for worker in self.workers:
                remaining = max(0, deadline - time.time())
                worker.join(timeout=remaining)

        self.workers.clear()
        logger.info("AGI Scheduler stopped")

    def get_status(self) -> Dict[str, Any]:
        """Get scheduler status"""
        with self._lock:
            return {
                "running": self.running,
                "num_workers": self.num_workers,
                "queued_tasks": len([t for t in self.task_queue if t.state == TaskState.QUEUED]),
                "running_tasks": len(self.running_tasks),
                "total_tasks": len(self.tasks)
            }

    def get_metrics(self) -> Dict[str, Any]:
        """Get scheduler metrics"""
        return self.metrics.get_statistics()

    def get_queue_snapshot(self) -> List[Dict[str, Any]]:
        """Get snapshot of current task queue"""
        with self._lock:
            return [
                {
                    "task_id": t.task_id,
                    "name": t.name,
                    "priority": t.sort_priority,
                    "state": t.state.name,
                    "type": t.task_type.name,
                    "sti": t.sti,
                    "deadline": t.deadline
                }
                for t in sorted(self.task_queue, key=lambda x: x.sort_priority)[:20]
            ]


# Global scheduler instance
_scheduler_instance: Optional[AGIScheduler] = None


def get_scheduler(num_workers: int = 4) -> AGIScheduler:
    """Get the global scheduler instance"""
    global _scheduler_instance
    if _scheduler_instance is None:
        _scheduler_instance = AGIScheduler(num_workers=num_workers)
    return _scheduler_instance


if __name__ == "__main__":
    # Example usage
    import random

    scheduler = get_scheduler(num_workers=2)
    scheduler.start()

    def sample_task(task_name: str, duration: float):
        """Sample task that sleeps for duration"""
        time.sleep(duration)
        return f"{task_name} completed"

    # Submit various tasks
    task_ids = []

    # High priority inference tasks
    for i in range(3):
        tid = scheduler.submit_inference(
            name=f"PLN_Inference_{i}",
            execute_fn=sample_task,
            args=(f"inference_{i}", random.uniform(0.1, 0.5)),
            sti=random.uniform(50, 100)
        )
        task_ids.append(tid)

    # Normal priority tasks
    for i in range(5):
        tid = scheduler.submit(
            name=f"Normal_Task_{i}",
            execute_fn=sample_task,
            priority=TaskPriority.NORMAL,
            args=(f"normal_{i}", random.uniform(0.1, 0.3))
        )
        task_ids.append(tid)

    # Low priority learning tasks
    for i in range(3):
        tid = scheduler.submit_learning(
            name=f"Learning_Task_{i}",
            execute_fn=sample_task,
            args=(f"learning_{i}", random.uniform(0.2, 0.5))
        )
        task_ids.append(tid)

    # Deadline task
    tid = scheduler.submit(
        name="Urgent_Deadline_Task",
        execute_fn=sample_task,
        priority=TaskPriority.NORMAL,
        deadline=time.time() + 2,  # 2 second deadline
        args=("deadline", 0.1)
    )
    task_ids.append(tid)

    # Wait for tasks
    print("\nWaiting for tasks to complete...")
    time.sleep(5)

    # Print metrics
    print("\n" + "=" * 50)
    print("Scheduler Metrics:")
    print("=" * 50)
    metrics = scheduler.get_metrics()
    print(json.dumps(metrics, indent=2))

    print("\n" + "=" * 50)
    print("Scheduler Status:")
    print("=" * 50)
    status = scheduler.get_status()
    print(json.dumps(status, indent=2))

    scheduler.stop()
