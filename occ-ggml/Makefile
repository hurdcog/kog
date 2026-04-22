# OCC-GGML Makefile
# Oscillator Cellular Cognition Tensor Library

CC = gcc
CFLAGS = -O3 -Wall -Wextra -std=c11 -I./include
LDFLAGS = -lm

# Architecture detection
UNAME_M := $(shell uname -m)
ifeq ($(UNAME_M),x86_64)
    CFLAGS += -mavx2 -mfma -DOCC_USE_AVX2=1
endif
ifeq ($(UNAME_M),aarch64)
    CFLAGS += -march=armv8-a -DOCC_USE_NEON=1
endif

# Debug build
ifdef DEBUG
    CFLAGS += -g -O0 -DDEBUG
endif

# Source files
SRCS = src/occ-ggml.c \
       src/occ-ops-avx2.c \
       src/occ-atomspace.c \
       src/occ-propagation.c

OBJS = $(SRCS:.c=.o)

# Test files
TEST_SRCS = tests/test-atomspace.c \
            tests/test-propagation.c \
            tests/test-benchmark.c

TEST_BINS = $(TEST_SRCS:.c=)

# Example files
EXAMPLE_SRCS = examples/simple-propagation.c
EXAMPLE_BINS = $(EXAMPLE_SRCS:.c=)

# Library
LIB = libocc-ggml.a

# Default target
all: $(LIB) examples

# Build library
$(LIB): $(OBJS)
	ar rcs $@ $^

# Build object files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Build examples
examples: $(LIB) $(EXAMPLE_BINS)

examples/%: examples/%.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $< -L. -locc-ggml $(LDFLAGS)

# Build tests
tests: $(LIB) $(TEST_BINS)

tests/%: tests/%.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $< -L. -locc-ggml $(LDFLAGS)

# Run tests
test: tests
	@echo "Running AtomSpace tests..."
	./tests/test-atomspace
	@echo "Running Propagation tests..."
	./tests/test-propagation
	@echo "Running Benchmarks..."
	./tests/test-benchmark

# Clean
clean:
	rm -f $(OBJS) $(LIB) $(TEST_BINS) $(EXAMPLE_BINS)

# Install
PREFIX ?= /usr/local
install: $(LIB)
	install -d $(PREFIX)/lib
	install -d $(PREFIX)/include/occ-ggml
	install -m 644 $(LIB) $(PREFIX)/lib/
	install -m 644 include/*.h $(PREFIX)/include/occ-ggml/

# Uninstall
uninstall:
	rm -f $(PREFIX)/lib/$(LIB)
	rm -rf $(PREFIX)/include/occ-ggml

# Help
help:
	@echo "OCC-GGML Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build library and examples (default)"
	@echo "  tests     - Build test programs"
	@echo "  test      - Run all tests"
	@echo "  clean     - Remove build artifacts"
	@echo "  install   - Install library and headers"
	@echo "  uninstall - Remove installed files"
	@echo ""
	@echo "Options:"
	@echo "  DEBUG=1   - Build with debug symbols"
	@echo "  PREFIX=   - Installation prefix (default: /usr/local)"

.PHONY: all examples tests test clean install uninstall help
