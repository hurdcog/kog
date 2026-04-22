"""
AGI-OS Core Module

This module provides the core kernel components for the AGI-OS:
- AGI_Boot: Unified boot orchestrator
- AGI_Scheduler: Priority-aware task scheduler
- AGI_EventBus: Event/interrupt controller

These components address critical gaps identified in the AGI-Kern evaluation:
- Boot/Init: 65% -> target 90%
- Scheduling: 40% -> target 75%
- Interrupts: 20% -> target 60%
"""

from .agi_boot import (
    AGIBootOrchestrator,
    BootConfig,
    BootPhase,
    BootComponent,
    ComponentStatus,
    get_boot_orchestrator,
    boot_agi_os
)

from .agi_scheduler import (
    AGIScheduler,
    CognitiveTask,
    TaskPriority,
    TaskState,
    TaskType,
    get_scheduler
)

from .agi_event_bus import (
    AGIEventBus,
    CognitiveEvent,
    EventPriority,
    EventCategory,
    EventHandler,
    EventTypes,
    get_event_bus
)

__all__ = [
    # Boot
    'AGIBootOrchestrator',
    'BootConfig',
    'BootPhase',
    'BootComponent',
    'ComponentStatus',
    'get_boot_orchestrator',
    'boot_agi_os',

    # Scheduler
    'AGIScheduler',
    'CognitiveTask',
    'TaskPriority',
    'TaskState',
    'TaskType',
    'get_scheduler',

    # Event Bus
    'AGIEventBus',
    'CognitiveEvent',
    'EventPriority',
    'EventCategory',
    'EventHandler',
    'EventTypes',
    'get_event_bus'
]

__version__ = '0.1.0'
