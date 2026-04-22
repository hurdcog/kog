"""
CogSelf - Cognitive Self Framework

A framework for cognitive identity and persona integration, featuring
the NeuroCog Core Self unified cognitive architecture.
"""

__version__ = "0.1.0"
__author__ = "OpenCog Community"

from .neurocog_core_self import (
    NeuroCogCoreSelf,
    PersonalityTensor,
    CognitiveCharacteristic,
    EpisodicMemory,
    CognitiveGoal,
    CognitiveSubsystem
)

__all__ = [
    'NeuroCogCoreSelf',
    'PersonalityTensor',
    'CognitiveCharacteristic',
    'EpisodicMemory',
    'CognitiveGoal',
    'CognitiveSubsystem',
]
