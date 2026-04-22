# New Developer Guide

Welcome to GNU Mach development! This guide will help you get started contributing to the microkernel project.

## Getting Started

### Prerequisites

- Linux or WSL environment (Windows users)
- Basic understanding of C programming and operating systems
- Familiarity with git and build systems

### Development Environment Setup

1. **Install dependencies** (Ubuntu/WSL):
   ```bash
   sudo apt update && sudo apt install -y build-essential gcc-multilib binutils binutils-multiarch \
     autoconf automake libtool pkg-config gawk bison flex nasm \
     xorriso grub-pc-bin mtools qemu-system-x86 \
     git python3 cppcheck clang-tools texinfo
   ```

2. **Set up MIG (Mach Interface Generator)**:
   ```bash
   # Set up headers for MIG build
   sudo mkdir -p /usr/include/mach
   sudo cp -r include/mach/* /usr/include/mach/
   sudo ln -sf $(pwd)/i386/include/mach/i386 /usr/include/mach/machine
   
   # Build and install MIG
   cd mig && autoreconf --install && ./configure CPPFLAGS="-I/usr/include"
   make -j$(nproc) && sudo make install && cd ..
   ```

3. **Build the kernel**:
   ```bash
   autoreconf --install
   ./configure --host=i686-gnu CC='gcc -m32' LD='ld -melf_i386'
   make -j$(nproc)
   ```

4. **Run tests**:
   ```bash
   make check                  # Full test suite
   make run-hello             # Basic functionality test
   make run-vm                # Virtual memory test
   ```

## Development Phases

GNU Mach development is organized into four phases. **New developers should start with Phase 1** issues.

### Phase 1: Foundation & Quick Wins
- **Scope**: Housekeeping, well-known fixes
- **Skills**: Basic C programming, understanding of build systems
- **Typical tasks**: Fix compiler warnings, add header guards, clean up code
- **Definition of done**: Zero new warnings, tests pass, CI green

### Phase 2: Improvements to Existing Subsystems
- **Scope**: Targeted performance/robustness improvements
- **Skills**: Understanding of specific subsystems (VM, IPC, etc.)
- **Typical tasks**: Optimize algorithms, improve error handling, add validation

### Phase 3: New Features & Architecture Changes
- **Scope**: New subsystem features
- **Skills**: Deep kernel knowledge, architectural design
- **Typical tasks**: Implement new APIs, add major features

### Phase 4: Experimental R&D
- **Scope**: Research prototypes
- **Skills**: Advanced kernel development, research methodology
- **Typical tasks**: Prototype new concepts, publish research results

## Your First Contribution

### Step 1: Choose an Issue
- Browse issues labeled `good first issue` and `phase:1`
- Look for documentation improvements or simple bug fixes
- Comment on the issue to claim it

### Step 2: Understand the Code
- Read the relevant source files
- Understand the context and current implementation
- Ask questions in the issue comments if needed

### Step 3: Make Your Changes
- Follow the coding standards (see CONTRIBUTING.md)
- Make minimal, focused changes
- Test your changes thoroughly

### Step 4: Test and Validate
- Run the build: `make -j$(nproc)`
- Run relevant tests: `make run-hello` or specific tests
- Run static analysis: `./scripts/run-static-analysis.sh`

### Step 5: Submit a Pull Request
- Create a descriptive commit message
- Open a pull request with clear description
- Address review feedback promptly

## Code Organization

```
cognu-mach/
├── kern/           # Core kernel (scheduler, IPC, memory management)
├── vm/             # Virtual memory subsystem
├── ipc/            # Inter-process communication
├── i386/           # x86 32-bit architecture support
├── x86_64/         # x86 64-bit architecture support
├── device/         # Device drivers
├── linux/          # Linux device driver compatibility
├── tests/          # Test programs
├── scripts/        # Development and build scripts
└── docs/           # Documentation
```

## Development Best Practices

### Coding Standards
- Match existing formatting and style
- Use descriptive variable and function names
- Add comments for complex logic
- Handle errors early with guard clauses
- Avoid deep nesting

### Safety and Robustness
- Use the safety macros in `include/mach/mach_safety.h`
- Validate inputs and check for edge cases
- Use bounds checking and overflow protection
- Follow the defensive programming principles

### Testing
- Always test your changes before submitting
- Add tests for new functionality when appropriate
- Ensure existing tests continue to pass
- Use QEMU for kernel testing

## Common Development Scenarios

### Adding a Simple Fix
1. Identify the issue in the code
2. Make minimal changes to fix it
3. Test that the fix works
4. Verify no regressions

### Adding a New Test
1. Look at existing tests in `tests/` directory
2. Follow the same pattern for new tests
3. Add the test to the appropriate Makefile
4. Verify the test runs with `make run-<testname>`

### Debugging Issues
1. Use the debug build: `make debug-hello`
2. Attach GDB: `gdb gnumach -ex 'target remote :1234'`
3. Use kernel debugging features (KDB if enabled)
4. Check system logs and kernel output

## Getting Help

### Communication Channels
- GitHub issues for specific problems
- Mailing lists: `bug-hurd@gnu.org`, `help-hurd@gnu.org`
- IRC: #hurd on libera.chat

### Mentorship Program
- Experienced developers are available to mentor newcomers
- Request mentorship in your first issue comment
- Mentors will provide guidance on technical approach and code review

### Learning Resources
- Read the GNU Hurd documentation: https://www.gnu.org/software/hurd/
- Study the existing codebase and documentation
- Look at similar changes in the git history

## Common Mistakes to Avoid

1. **Making changes too broad**: Keep PRs small and focused
2. **Not testing thoroughly**: Always run tests before submitting
3. **Ignoring existing code style**: Match the surrounding code formatting
4. **Not understanding the context**: Read related code and documentation first
5. **Submitting without validation**: Run static analysis and builds

## Progressive Learning Path

### Week 1-2: Environment Setup
- Set up development environment
- Build and run tests successfully
- Explore the codebase structure
- Claim a simple Phase 1 issue

### Week 3-4: First Contribution
- Fix a simple bug or warning
- Learn the PR process
- Get your first change merged

### Month 2-3: Understanding Subsystems
- Deep dive into a specific subsystem
- Take on more complex Phase 1 issues
- Start reading Phase 2 issues

### Month 4-6: Advanced Contributions
- Contribute to Phase 2 improvements
- Understand cross-subsystem interactions
- Consider becoming a mentor

## Success Metrics

### Personal Goals
- First merged PR within 2 weeks
- Understanding of build and test process
- Ability to navigate the codebase confidently
- Regular contributions over time

### Community Goals
- Helping other new developers
- Improving documentation
- Contributing to code reviews
- Participating in design discussions

---

Remember: everyone was a beginner once. The GNU Mach community welcomes new contributors and values diverse perspectives. Don't hesitate to ask questions and seek help when needed!