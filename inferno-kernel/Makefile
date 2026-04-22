# Makefile for Inferno Kernel AGI Modules

LIMBO = limbo
MODULES = atomspace attention reasoning learning memory cogserver ure pln

.PHONY: all clean install test

all: $(MODULES)

atomspace:
@echo "Building AtomSpace kernel module..."
cd atomspace && $(LIMBO) atomspace_kern.b

attention:
@echo "Building Attention kernel module..."
cd attention && $(LIMBO) attention_kern.b

cogserver:
@echo "Building CogServer Limbo module..."
cd limbo && $(LIMBO) cogserver.b

ure:
@echo "Building URE Limbo module..."
cd limbo && $(LIMBO) ure.b

pln:
@echo "Building PLN Limbo module..."
cd limbo && $(LIMBO) pln.b

reasoning:
@echo "Building Reasoning application..."
cd appl/reasoning && $(LIMBO) simple_inference.b

learning:
@echo "Building Learning application..."
cd appl/learning && $(LIMBO) learning.b

memory:
@echo "Building Perception and Action applications..."
cd appl/perception && $(LIMBO) perception.b
cd appl/action && $(LIMBO) action.b

clean:
@echo "Cleaning build artifacts..."
find . -name "*.dis" -delete
find . -name "*.sbl" -delete

install:
@echo "Installing kernel modules..."
mkdir -p /dis/inferno-kernel
find . -name "*.dis" -exec cp {} /dis/inferno-kernel/ \;
chmod 755 /dis/inferno-kernel/*.dis

test:
@echo "Running tests..."
@echo "TODO: Implement tests"

help:
@echo "Inferno Kernel AGI Build System"
@echo ""
@echo "Targets:"
@echo "  all       - Build all kernel modules"
@echo "  clean     - Remove build artifacts"
@echo "  install   - Install modules to /dis/inferno-kernel"
@echo "  test      - Run test suite"
@echo "  help      - Show this help message"
@echo ""
@echo "Modules:"
@echo "  atomspace - AtomSpace hypergraph kernel service"
@echo "  attention - ECAN attention allocation kernel service"
@echo "  cogserver - CogServer Limbo module"
@echo "  ure       - Unified Rule Engine Limbo module"
@echo "  pln       - Probabilistic Logic Networks Limbo module"
@echo "  reasoning - PLN/URE reasoning application"
@echo "  learning  - MOSES/pattern-learning application"
@echo "  memory    - Perception and Action applications"

