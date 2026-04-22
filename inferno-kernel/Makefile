# Makefile for Inferno Kernel AGI Modules

LIMBO = limbo
MODULES = atomspace attention reasoning learning memory

.PHONY: all clean install test

all: $(MODULES)

atomspace:
@echo "Building AtomSpace kernel module..."
cd atomspace && $(LIMBO) atomspace_kern.b

attention:
@echo "Building Attention kernel module..."
cd attention && $(LIMBO) attention_kern.b

reasoning:
@echo "Building Reasoning kernel module..."
@echo "TODO: Implement reasoning module"

learning:
@echo "Building Learning kernel module..."
@echo "TODO: Implement learning module"

memory:
@echo "Building Memory kernel module..."
@echo "TODO: Implement memory module"

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
@echo "  reasoning - PLN/URE reasoning kernel service"
@echo "  learning  - MOSES learning kernel service"
@echo "  memory    - Memory management kernel service"
