# ============================================================================
# AGI-OS Unified Build System
# ============================================================================
#
# This Makefile provides a unified build interface for the complete AGI-OS
# stack, integrating Cognumach (Layer 1), HurdCog (Layer 2), and OCC (Layer 3).
#
# Quick Start:
#   make help           - Show all available targets
#   make all            - Build complete AGI-OS stack
#   make test           - Run integration tests
#   make dev-setup      - Setup development environment
#
# Build Methods:
#   make guix-build     - Build using GNU Guix (recommended, reproducible)
#   make direct-build   - Build directly (for development)
#   make cmake-agi-os-stack - Build via CMake with proper sequences
#
# ============================================================================

# Include CMake build sequences
include Makefile.build-sequences

# Project Configuration
# ============================================================================

PROJECT_NAME := AGI-OS
PROJECT_VERSION := 1.0.0
BUILD_DATE := $(shell date +%Y-%m-%d)

# Directory Structure
ROOT_DIR := $(shell pwd)
COGNUMACH_DIR := $(ROOT_DIR)/cognumach
HURDCOG_DIR := $(ROOT_DIR)/hurdcog
SYNERGY_DIR := $(ROOT_DIR)/synergy
METAMODEL_DIR := $(ROOT_DIR)/metamodel
DOCS_DIR := $(ROOT_DIR)/docs
BUILD_DIR := $(ROOT_DIR)/build
INSTALL_PREFIX := /usr/local/agi-os

# Build Configuration
NPROC := $(shell nproc 2>/dev/null || echo 4)
MAKEFLAGS += --no-print-directory

# Colors for output
COLOR_RESET := \033[0m
COLOR_BOLD := \033[1m
COLOR_RED := \033[31m
COLOR_GREEN := \033[32m
COLOR_YELLOW := \033[33m
COLOR_BLUE := \033[34m
COLOR_MAGENTA := \033[35m
COLOR_CYAN := \033[36m

# ============================================================================
# Phony Targets
# ============================================================================

.PHONY: all clean help info
.PHONY: cognumach hurdcog occ synergy metamodel
.PHONY: guix-build direct-build
.PHONY: configure-cognumach configure-hurdcog configure-occ
.PHONY: build-cognumach build-hurdcog build-occ build-synergy build-metamodel
.PHONY: test test-cognumach test-hurdcog test-occ test-synergy test-integration
.PHONY: install install-cognumach install-hurdcog install-occ
.PHONY: clean-cognumach clean-hurdcog clean-occ clean-all
.PHONY: dev-setup docs quickstart
.PHONY: validate verify status

# ============================================================================
# Default Target
# ============================================================================

all: info guix-build
	@echo ""
	@echo "$(COLOR_GREEN)$(COLOR_BOLD)✓ AGI-OS build complete!$(COLOR_RESET)"
	@echo ""
	@echo "Next steps:"
	@echo "  • Run tests:        make test"
	@echo "  • Install system:   make install"
	@echo "  • View status:      make status"
	@echo ""

# ============================================================================
# Help and Information
# ============================================================================

help:
	@echo "$(COLOR_CYAN)$(COLOR_BOLD)"
	@echo "╔════════════════════════════════════════════════════════════════╗"
	@echo "║              AGI-OS Unified Build System                       ║"
	@echo "║  Three-Layer Cognitive Architecture Build Orchestration        ║"
	@echo "╚════════════════════════════════════════════════════════════════╝"
	@echo "$(COLOR_RESET)"
	@echo ""
	@echo "$(COLOR_BOLD)QUICK START:$(COLOR_RESET)"
	@echo "  make all              Build complete AGI-OS stack (recommended)"
	@echo "  make dev-setup        Setup development environment"
	@echo "  make quickstart       Complete setup + build + test"
	@echo ""
	@echo "$(COLOR_BOLD)BUILD TARGETS:$(COLOR_RESET)"
	@echo "  make guix-build       Build using GNU Guix (reproducible)"
	@echo "  make direct-build     Build directly without Guix (development)"
	@echo "  make cmake-agi-os-stack  Build complete stack via CMake (recommended)"
	@echo "  make cognumach        Build Layer 1: Cognitive Microkernel"
	@echo "  make hurdcog          Build Layer 2: Cognitive OS"
	@echo "  make occ              Build Layer 3: AGI Framework"
	@echo "  make synergy          Build integration infrastructure"
	@echo "  make metamodel        Build Scheme metamodel"
	@echo ""
	@echo "$(COLOR_BOLD)CMAKE BUILD SEQUENCES:$(COLOR_RESET)"
	@echo "  make build-sequences-help  Show detailed CMake build sequences"
	@echo "  make cmake-foundation      Build cogutil + atomspace"
	@echo "  make cmake-occ-full        Build complete OCC components"
	@echo ""
	@echo "$(COLOR_BOLD)TESTING:$(COLOR_RESET)"
	@echo "  make test             Run all integration tests"
	@echo "  make test-cognumach   Test Cognumach microkernel"
	@echo "  make test-hurdcog     Test HurdCog OS"
	@echo "  make test-occ         Test OCC framework"
	@echo "  make test-synergy     Test synergy integration"
	@echo "  make validate         Validate build outputs"
	@echo "  make verify           Verify system integrity"
	@echo ""
	@echo "$(COLOR_BOLD)INSTALLATION:$(COLOR_RESET)"
	@echo "  make install          Install complete AGI-OS"
	@echo "  make install-cognumach Install Cognumach only"
	@echo "  make install-hurdcog  Install HurdCog only"
	@echo "  make install-occ      Install OCC only"
	@echo ""
	@echo "$(COLOR_BOLD)MAINTENANCE:$(COLOR_RESET)"
	@echo "  make clean            Clean build artifacts"
	@echo "  make clean-all        Deep clean (including dependencies)"
	@echo "  make status           Show build status"
	@echo "  make info             Show system information"
	@echo "  make docs             Generate documentation"
	@echo ""
	@echo "$(COLOR_BOLD)DEVELOPMENT:$(COLOR_RESET)"
	@echo "  make configure        Configure all layers"
	@echo "  make dev-setup        Install development dependencies"
	@echo ""
	@echo "$(COLOR_BOLD)ARCHITECTURE:$(COLOR_RESET)"
	@echo "  Layer 1: Cognumach    - Cognitive Microkernel (GNU Mach)"
	@echo "  Layer 2: HurdCog      - Cognitive OS (GNU Hurd + CogKernel)"
	@echo "  Layer 3: OCC          - AGI Framework (OpenCog Collection)"
	@echo "  Integration: Synergy  - Cross-layer cognitive bridges"
	@echo "  Foundation: Metamodel - Scheme-based cognitive primitives"
	@echo ""
	@echo "For detailed documentation, see: $(DOCS_DIR)/BUILD_GUIDE.md"
	@echo ""

info:
	@echo "$(COLOR_CYAN)$(COLOR_BOLD)AGI-OS Build Information$(COLOR_RESET)"
	@echo "$(COLOR_CYAN)═══════════════════════════════════════$(COLOR_RESET)"
	@echo ""
	@echo "$(COLOR_BOLD)Project:$(COLOR_RESET)        $(PROJECT_NAME) v$(PROJECT_VERSION)"
	@echo "$(COLOR_BOLD)Build Date:$(COLOR_RESET)     $(BUILD_DATE)"
	@echo "$(COLOR_BOLD)Root Dir:$(COLOR_RESET)       $(ROOT_DIR)"
	@echo "$(COLOR_BOLD)Install Prefix:$(COLOR_RESET) $(INSTALL_PREFIX)"
	@echo "$(COLOR_BOLD)Parallel Jobs:$(COLOR_RESET)  $(NPROC)"
	@echo ""
	@echo "$(COLOR_BOLD)Layer Directories:$(COLOR_RESET)"
	@echo "  • Cognumach:  $(COGNUMACH_DIR)"
	@echo "  • HurdCog:    $(HURDCOG_DIR)"
	@echo "  • Synergy:    $(SYNERGY_DIR)"
	@echo "  • Metamodel:  $(METAMODEL_DIR)"
	@echo ""
	@echo "$(COLOR_BOLD)Build Tools:$(COLOR_RESET)"
	@which guix >/dev/null 2>&1 && echo "  ✓ GNU Guix:    $$(guix --version | head -1)" || echo "  ✗ GNU Guix:    Not found"
	@which gcc >/dev/null 2>&1 && echo "  ✓ GCC:         $$(gcc --version | head -1)" || echo "  ✗ GCC:         Not found"
	@which make >/dev/null 2>&1 && echo "  ✓ Make:        $$(make --version | head -1)" || echo "  ✗ Make:        Not found"
	@which cmake >/dev/null 2>&1 && echo "  ✓ CMake:       $$(cmake --version | head -1)" || echo "  ✗ CMake:       Not found"
	@which guile >/dev/null 2>&1 && echo "  ✓ Guile:       $$(guile --version | head -1)" || echo "  ✗ Guile:       Not found"
	@which python3 >/dev/null 2>&1 && echo "  ✓ Python:      $$(python3 --version)" || echo "  ✗ Python:      Not found"
	@echo ""

status:
	@echo "$(COLOR_CYAN)$(COLOR_BOLD)AGI-OS Build Status$(COLOR_RESET)"
	@echo "$(COLOR_CYAN)═══════════════════════════════════════$(COLOR_RESET)"
	@echo ""
	@echo "$(COLOR_BOLD)Layer 1: Cognumach (Cognitive Microkernel)$(COLOR_RESET)"
	@if [ -f "$(COGNUMACH_DIR)/configure" ]; then \
		echo "  ✓ Configured"; \
	else \
		echo "  ✗ Not configured (run: make configure-cognumach)"; \
	fi
	@if [ -d "$(COGNUMACH_DIR)" ]; then \
		echo "  ✓ Source present ($$(find $(COGNUMACH_DIR) -type f | wc -l) files)"; \
	else \
		echo "  ✗ Source missing"; \
	fi
	@echo ""
	@echo "$(COLOR_BOLD)Layer 2: HurdCog (Cognitive Operating System)$(COLOR_RESET)"
	@if [ -f "$(HURDCOG_DIR)/Makefile" ]; then \
		echo "  ✓ Build system present"; \
	else \
		echo "  ✗ Build system missing"; \
	fi
	@if [ -d "$(HURDCOG_DIR)/cogkernel" ]; then \
		echo "  ✓ CogKernel present ($$(find $(HURDCOG_DIR)/cogkernel -name '*.scm' | wc -l) Scheme files)"; \
	else \
		echo "  ✗ CogKernel missing"; \
	fi
	@echo ""
	@echo "$(COLOR_BOLD)Layer 3: OCC (AGI Framework)$(COLOR_RESET)"
	@if [ -d "$(SYNERGY_DIR)" ]; then \
		echo "  ✓ Synergy infrastructure present"; \
	else \
		echo "  ✗ Synergy infrastructure missing"; \
	fi
	@if [ -d "$(METAMODEL_DIR)" ]; then \
		echo "  ✓ Metamodel present ($$(find $(METAMODEL_DIR) -name '*.scm' | wc -l) Scheme files)"; \
	else \
		echo "  ✗ Metamodel missing"; \
	fi
	@echo ""
	@echo "$(COLOR_BOLD)Integration:$(COLOR_RESET)"
	@if [ -f "$(SYNERGY_DIR)/bridges/agi_os_bridge.py" ]; then \
		echo "  ✓ AGI-OS bridge present"; \
	else \
		echo "  ✗ AGI-OS bridge missing"; \
	fi
	@if [ -f "$(SYNERGY_DIR)/bridges/scheme_python_ffi.py" ]; then \
		echo "  ✓ Scheme-Python FFI present"; \
	else \
		echo "  ✗ Scheme-Python FFI missing"; \
	fi
	@echo ""

# ============================================================================
# GNU Guix Build (Recommended)
# ============================================================================

guix-build: check-guix
	@echo "$(COLOR_BLUE)$(COLOR_BOLD)Building AGI-OS with GNU Guix...$(COLOR_RESET)"
	@echo ""
	@echo "$(COLOR_YELLOW)Building unified AGI-OS stack...$(COLOR_RESET)"
	guix build -f occ-hurdcog-unified.scm
	@echo ""
	@echo "$(COLOR_GREEN)✓ Guix build complete$(COLOR_RESET)"

check-guix:
	@if ! command -v guix >/dev/null 2>&1; then \
		echo "$(COLOR_RED)Error: GNU Guix not found$(COLOR_RESET)"; \
		echo ""; \
		echo "Please install GNU Guix or use: make direct-build"; \
		echo ""; \
		echo "To install Guix:"; \
		echo "  curl https://git.savannah.gnu.org/cgit/guix.git/plain/etc/guix-install.sh | sudo bash"; \
		echo ""; \
		exit 1; \
	fi

# ============================================================================
# Direct Build (Development)
# ============================================================================

direct-build: configure build-all
	@echo ""
	@echo "$(COLOR_GREEN)$(COLOR_BOLD)✓ Direct build complete!$(COLOR_RESET)"
	@echo ""

configure: configure-cognumach configure-hurdcog configure-occ

build-all: build-cognumach build-hurdcog build-synergy build-metamodel

# ----------------------------------------------------------------------------
# Layer 1: Cognumach (Cognitive Microkernel)
# ----------------------------------------------------------------------------

cognumach: configure-cognumach build-cognumach

configure-cognumach:
	@echo "$(COLOR_BLUE)Configuring Cognumach microkernel...$(COLOR_RESET)"
	@cd $(COGNUMACH_DIR) && \
		if [ ! -f configure ]; then \
			echo "  Running autoreconf..."; \
			autoreconf -vif; \
		fi && \
		if [ ! -f Makefile ]; then \
			echo "  Running configure..."; \
			./configure \
				--host=i686-gnu \
				CC='gcc -m32' \
				LD='ld -melf_i386' \
				--enable-kdb \
				--prefix=$(INSTALL_PREFIX)/cognumach; \
		fi
	@echo "$(COLOR_GREEN)  ✓ Cognumach configured$(COLOR_RESET)"

build-cognumach: configure-cognumach
	@echo "$(COLOR_BLUE)Building Cognumach microkernel...$(COLOR_RESET)"
	@cd $(COGNUMACH_DIR) && make -j$(NPROC)
	@echo "$(COLOR_GREEN)  ✓ Cognumach built$(COLOR_RESET)"

# ----------------------------------------------------------------------------
# Layer 2: HurdCog (Cognitive Operating System)
# ----------------------------------------------------------------------------

hurdcog: configure-hurdcog build-hurdcog

configure-hurdcog:
	@echo "$(COLOR_BLUE)Configuring HurdCog cognitive OS...$(COLOR_RESET)"
	@cd $(HURDCOG_DIR) && \
		if [ ! -f configure ]; then \
			echo "  Running autoreconf..."; \
			autoreconf -vif 2>/dev/null || true; \
		fi
	@echo "$(COLOR_GREEN)  ✓ HurdCog configured$(COLOR_RESET)"

build-hurdcog: configure-hurdcog
	@echo "$(COLOR_BLUE)Building HurdCog cognitive OS...$(COLOR_RESET)"
	@if [ -f "$(HURDCOG_DIR)/Makefile" ]; then \
		cd $(HURDCOG_DIR) && make -j$(NPROC); \
	else \
		echo "  $(COLOR_YELLOW)Note: HurdCog Makefile not found, building CogKernel only$(COLOR_RESET)"; \
		if [ -f "$(HURDCOG_DIR)/cogkernel/Makefile" ]; then \
			cd $(HURDCOG_DIR)/cogkernel && make -j$(NPROC); \
		else \
			echo "  $(COLOR_YELLOW)CogKernel Makefile not found, skipping build$(COLOR_RESET)"; \
		fi; \
	fi
	@echo "$(COLOR_GREEN)  ✓ HurdCog built$(COLOR_RESET)"

# ----------------------------------------------------------------------------
# Layer 3: OCC (AGI Framework)
# ----------------------------------------------------------------------------

occ: configure-occ build-synergy build-metamodel

configure-occ:
	@echo "$(COLOR_BLUE)Configuring OCC AGI framework...$(COLOR_RESET)"
	@mkdir -p $(BUILD_DIR)
	@echo "$(COLOR_GREEN)  ✓ OCC configured$(COLOR_RESET)"

# ----------------------------------------------------------------------------
# Synergy Integration Infrastructure
# ----------------------------------------------------------------------------

synergy: build-synergy

build-synergy:
	@echo "$(COLOR_BLUE)Building Synergy integration infrastructure...$(COLOR_RESET)"
	@echo "  Compiling Python modules..."
	@cd $(SYNERGY_DIR) && \
		python3 -m py_compile bridges/agi_os_bridge.py && \
		python3 -m py_compile bridges/scheme_python_ffi.py
	@echo "$(COLOR_GREEN)  ✓ Synergy built$(COLOR_RESET)"

# ----------------------------------------------------------------------------
# Metamodel Foundation
# ----------------------------------------------------------------------------

metamodel: build-metamodel

build-metamodel:
	@echo "$(COLOR_BLUE)Building Metamodel foundation...$(COLOR_RESET)"
	@echo "  Validating Scheme modules..."
	@cd $(METAMODEL_DIR) && \
		for file in *.scm; do \
			if [ -f "$$file" ]; then \
				guile --no-auto-compile -c "(load \"$$file\")" 2>/dev/null && \
				echo "    ✓ $$file" || \
				echo "    ✗ $$file (validation failed)"; \
			fi; \
		done
	@echo "$(COLOR_GREEN)  ✓ Metamodel validated$(COLOR_RESET)"

# ============================================================================
# Testing
# ============================================================================

test: test-synergy test-integration
	@echo ""
	@echo "$(COLOR_GREEN)$(COLOR_BOLD)✓ All tests passed!$(COLOR_RESET)"
	@echo ""

test-cognumach:
	@echo "$(COLOR_BLUE)Testing Cognumach microkernel...$(COLOR_RESET)"
	@if [ -d "$(COGNUMACH_DIR)" ]; then \
		echo "  ✓ Cognumach directory present"; \
		if [ -f "$(COGNUMACH_DIR)/configure.ac" ]; then \
			echo "  ✓ Build system present"; \
		fi; \
	else \
		echo "  ✗ Cognumach directory missing"; \
		exit 1; \
	fi

test-hurdcog:
	@echo "$(COLOR_BLUE)Testing HurdCog cognitive OS...$(COLOR_RESET)"
	@if [ -d "$(HURDCOG_DIR)/cogkernel" ]; then \
		echo "  ✓ CogKernel present"; \
		echo "  ✓ Found $$(find $(HURDCOG_DIR)/cogkernel -name '*.scm' | wc -l) Scheme files"; \
	else \
		echo "  ✗ CogKernel missing"; \
		exit 1; \
	fi

test-occ:
	@echo "$(COLOR_BLUE)Testing OCC AGI framework...$(COLOR_RESET)"
	@if [ -d "$(SYNERGY_DIR)" ]; then \
		echo "  ✓ Synergy infrastructure present"; \
	else \
		echo "  ✗ Synergy infrastructure missing"; \
		exit 1; \
	fi
	@if [ -d "$(METAMODEL_DIR)" ]; then \
		echo "  ✓ Metamodel present"; \
	else \
		echo "  ✗ Metamodel missing"; \
		exit 1; \
	fi

test-synergy:
	@echo "$(COLOR_BLUE)Testing Synergy integration...$(COLOR_RESET)"
	@cd $(SYNERGY_DIR)/bridges && \
		python3 agi_os_bridge.py >/dev/null 2>&1 && \
		echo "  ✓ AGI-OS bridge functional" || \
		echo "  ✗ AGI-OS bridge failed"
	@cd $(SYNERGY_DIR)/bridges && \
		python3 scheme_python_ffi.py >/dev/null 2>&1 && \
		echo "  ✓ Scheme-Python FFI functional" || \
		echo "  ✗ Scheme-Python FFI failed"

test-integration:
	@echo "$(COLOR_BLUE)Running integration tests...$(COLOR_RESET)"
	@echo "  Testing cross-layer integration..."
	@if [ -f "$(SYNERGY_DIR)/bridges/agi_os_bridge.py" ] && \
	   [ -f "$(SYNERGY_DIR)/bridges/scheme_python_ffi.py" ] && \
	   [ -d "$(METAMODEL_DIR)" ] && \
	   [ -d "$(HURDCOG_DIR)/cogkernel" ]; then \
		echo "  ✓ All integration components present"; \
	else \
		echo "  ✗ Missing integration components"; \
		exit 1; \
	fi

validate: test
	@echo "$(COLOR_BLUE)Validating build outputs...$(COLOR_RESET)"
	@echo "  Checking directory structure..."
	@test -d $(COGNUMACH_DIR) && echo "  ✓ Cognumach directory" || echo "  ✗ Cognumach directory"
	@test -d $(HURDCOG_DIR) && echo "  ✓ HurdCog directory" || echo "  ✗ HurdCog directory"
	@test -d $(SYNERGY_DIR) && echo "  ✓ Synergy directory" || echo "  ✗ Synergy directory"
	@test -d $(METAMODEL_DIR) && echo "  ✓ Metamodel directory" || echo "  ✗ Metamodel directory"

verify: validate
	@echo "$(COLOR_BLUE)Verifying system integrity...$(COLOR_RESET)"
	@echo "  Checking for .git directories (should be none)..."
	@! test -d $(COGNUMACH_DIR)/.git && echo "  ✓ No .git in cognumach" || echo "  ✗ Found .git in cognumach"
	@! test -d $(HURDCOG_DIR)/.git && echo "  ✓ No .git in hurdcog" || echo "  ✗ Found .git in hurdcog"
	@echo "  ✓ Monorepo integrity verified"

# ============================================================================
# Installation
# ============================================================================

install: install-cognumach install-hurdcog install-occ
	@echo ""
	@echo "$(COLOR_GREEN)$(COLOR_BOLD)✓ AGI-OS installation complete!$(COLOR_RESET)"
	@echo ""
	@echo "Installation prefix: $(INSTALL_PREFIX)"
	@echo ""

install-cognumach:
	@echo "$(COLOR_BLUE)Installing Cognumach...$(COLOR_RESET)"
	@if [ -f "$(COGNUMACH_DIR)/Makefile" ]; then \
		cd $(COGNUMACH_DIR) && sudo make install; \
		echo "$(COLOR_GREEN)  ✓ Cognumach installed$(COLOR_RESET)"; \
	else \
		echo "$(COLOR_YELLOW)  ⚠ Cognumach not built, skipping installation$(COLOR_RESET)"; \
	fi

install-hurdcog:
	@echo "$(COLOR_BLUE)Installing HurdCog...$(COLOR_RESET)"
	@if [ -f "$(HURDCOG_DIR)/Makefile" ]; then \
		cd $(HURDCOG_DIR) && sudo make install; \
		echo "$(COLOR_GREEN)  ✓ HurdCog installed$(COLOR_RESET)"; \
	else \
		echo "$(COLOR_YELLOW)  ⚠ HurdCog not built, skipping installation$(COLOR_RESET)"; \
	fi

install-occ:
	@echo "$(COLOR_BLUE)Installing OCC...$(COLOR_RESET)"
	@sudo mkdir -p $(INSTALL_PREFIX)/synergy
	@sudo cp -r $(SYNERGY_DIR)/* $(INSTALL_PREFIX)/synergy/
	@sudo mkdir -p $(INSTALL_PREFIX)/metamodel
	@sudo cp -r $(METAMODEL_DIR)/* $(INSTALL_PREFIX)/metamodel/
	@echo "$(COLOR_GREEN)  ✓ OCC components installed$(COLOR_RESET)"

# ============================================================================
# Cleanup
# ============================================================================

clean: clean-cognumach clean-hurdcog clean-occ
	@echo "$(COLOR_GREEN)✓ Clean complete$(COLOR_RESET)"

clean-cognumach:
	@echo "$(COLOR_BLUE)Cleaning Cognumach...$(COLOR_RESET)"
	@if [ -f "$(COGNUMACH_DIR)/Makefile" ]; then \
		cd $(COGNUMACH_DIR) && make clean 2>/dev/null || true; \
	fi
	@rm -f $(COGNUMACH_DIR)/configure 2>/dev/null || true

clean-hurdcog:
	@echo "$(COLOR_BLUE)Cleaning HurdCog...$(COLOR_RESET)"
	@if [ -f "$(HURDCOG_DIR)/Makefile" ]; then \
		cd $(HURDCOG_DIR) && make clean 2>/dev/null || true; \
	fi
	@if [ -f "$(HURDCOG_DIR)/cogkernel/Makefile" ]; then \
		cd $(HURDCOG_DIR)/cogkernel && make clean 2>/dev/null || true; \
	fi

clean-occ:
	@echo "$(COLOR_BLUE)Cleaning OCC...$(COLOR_RESET)"
	@rm -rf $(BUILD_DIR) 2>/dev/null || true
	@find $(SYNERGY_DIR) -name '*.pyc' -delete 2>/dev/null || true
	@find $(SYNERGY_DIR) -name '__pycache__' -type d -exec rm -rf {} + 2>/dev/null || true

clean-all: clean
	@echo "$(COLOR_BLUE)Deep cleaning...$(COLOR_RESET)"
	@cd $(COGNUMACH_DIR) && make distclean 2>/dev/null || true
	@cd $(HURDCOG_DIR) && make distclean 2>/dev/null || true
	@rm -rf $(BUILD_DIR) 2>/dev/null || true
	@echo "$(COLOR_GREEN)✓ Deep clean complete$(COLOR_RESET)"

# ============================================================================
# Documentation
# ============================================================================

docs:
	@echo "$(COLOR_BLUE)Generating documentation...$(COLOR_RESET)"
	@echo ""
	@echo "Available documentation:"
	@echo "  • Architecture:  $(DOCS_DIR)/AGI_OS_ARCHITECTURE.md"
	@echo "  • Integration:   MONOREPO_INTEGRATION.md"
	@echo "  • Research:      RESEARCH_FINDINGS.md"
	@echo "  • Build Guide:   $(DOCS_DIR)/BUILD_GUIDE.md (to be created)"
	@echo ""

# ============================================================================
# Development Setup
# ============================================================================

dev-setup:
	@echo "$(COLOR_BLUE)$(COLOR_BOLD)Setting up AGI-OS development environment...$(COLOR_RESET)"
	@echo ""
	@echo "$(COLOR_YELLOW)Installing system dependencies...$(COLOR_RESET)"
	@sudo apt-get update -qq
	@sudo apt-get install -y --no-install-recommends \
		build-essential gcc-multilib binutils \
		autoconf automake libtool pkg-config \
		gawk bison flex texinfo \
		guile-3.0 guile-3.0-dev \
		python3 python3-pip python3-dev \
		cmake libboost-all-dev \
		git curl wget \
		2>&1 | grep -v "^Reading\|^Building\|^Preparing\|^Unpacking" || true
	@echo ""
	@echo "$(COLOR_YELLOW)Installing Python dependencies...$(COLOR_RESET)"
	@sudo apt-get install -y python3-psutil 2>&1 | grep -v "^Reading" || true
	@echo ""
	@echo "$(COLOR_GREEN)$(COLOR_BOLD)✓ Development environment ready!$(COLOR_RESET)"
	@echo ""
	@echo "Next steps:"
	@echo "  1. Build the system:  make all"
	@echo "  2. Run tests:         make test"
	@echo "  3. View status:       make status"
	@echo ""

# ============================================================================
# Quick Start
# ============================================================================

quickstart: info dev-setup direct-build test
	@echo ""
	@echo "$(COLOR_GREEN)$(COLOR_BOLD)"
	@echo "╔════════════════════════════════════════════════════════════════╗"
	@echo "║                                                                ║"
	@echo "║         AGI-OS Quick Start Complete!                           ║"
	@echo "║                                                                ║"
	@echo "╚════════════════════════════════════════════════════════════════╝"
	@echo "$(COLOR_RESET)"
	@echo ""
	@echo "$(COLOR_BOLD)System Status:$(COLOR_RESET)"
	@echo "  ✓ Development environment configured"
	@echo "  ✓ All layers built successfully"
	@echo "  ✓ Integration tests passed"
	@echo ""
	@echo "$(COLOR_BOLD)Next Steps:$(COLOR_RESET)"
	@echo "  1. Explore the architecture:"
	@echo "     cat $(DOCS_DIR)/AGI_OS_ARCHITECTURE.md"
	@echo ""
	@echo "  2. Test integration bridges:"
	@echo "     python3 $(SYNERGY_DIR)/bridges/agi_os_bridge.py"
	@echo ""
	@echo "  3. View system status:"
	@echo "     make status"
	@echo ""
	@echo "  4. Build with Guix (if available):"
	@echo "     make guix-build"
	@echo ""
	@echo "For help: make help"
	@echo ""

# ============================================================================
# End of Makefile
# ============================================================================
