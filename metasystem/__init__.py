"""
Metasystem - Viable System Implementation for OCC

Based on Eric Schwarz's organizational systems theory depicting:
- Three planes of existence (Energy/Objects, Information/Relations, Totality/Whole)
- Six cycles defining viable natural systems
- Seven steps of self-production (Spiral and Triad)
- 11-stage evolutionary helix

This module implements the theoretical framework for self-organizing,
autopoietic cognitive systems with full metasystem capabilities.
"""

from .planes import (
    PlaneOfEnergy,
    PlaneOfInformation,
    PlaneOfTotality,
    ThreePlanes,
)

from .cycles import (
    Cycle,
    VortexCycle,
    RetroactionCycle,
    SelfReferentialCycle,
    MorphogenesisCycle,
    AutopoiesisCycle,
    AutogenesisCycle,
    SixCycles,
)

from .spiral_triad import (
    SpiralTriad,
    SpiralStep,
)

from .evolutionary_helix import (
    EvolutionaryHelix,
    HelixStage,
)

from .autopoiesis import (
    AutopoieticSystem,
    AutopoieticBoundary,
    AutopoieticProduction,
)

from .metasystem_core import (
    Metasystem,
    MetasystemState,
    MetasystemConfig,
)

from .enterprise_autogenesis import (
    EnterpriseAutogenesis,
    EnterpriseStage,
    ThinSlice,
    LearningLoop,
    AutonomousPod,
    VisionGoalBet,
    LoopType,
    ControlMode,
)

__all__ = [
    # Planes
    'PlaneOfEnergy',
    'PlaneOfInformation',
    'PlaneOfTotality',
    'ThreePlanes',
    # Cycles
    'Cycle',
    'VortexCycle',
    'RetroactionCycle',
    'SelfReferentialCycle',
    'MorphogenesisCycle',
    'AutopoiesisCycle',
    'AutogenesisCycle',
    'SixCycles',
    # Spiral
    'SpiralTriad',
    'SpiralStep',
    # Helix
    'EvolutionaryHelix',
    'HelixStage',
    # Autopoiesis
    'AutopoieticSystem',
    'AutopoieticBoundary',
    'AutopoieticProduction',
    # Core
    'Metasystem',
    'MetasystemState',
    'MetasystemConfig',
    # Enterprise Autogenesis
    'EnterpriseAutogenesis',
    'EnterpriseStage',
    'ThinSlice',
    'LearningLoop',
    'AutonomousPod',
    'VisionGoalBet',
    'LoopType',
    'ControlMode',
]

__version__ = '0.1.0'
