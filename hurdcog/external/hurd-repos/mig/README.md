# MIG (Mach Interface Generator)

**Repository**: https://git.savannah.gnu.org/git/hurd/mig.git

## Description
The Mach Interface Generator (MIG) is a tool for generating C code from Mach Interface Definition Language (MIDL) specifications.

## Role in Hurd Ecosystem
- Interface code generation
- IPC stub generation
- Type checking for interfaces
- Protocol definition compilation

## Key Components
- MIDL parser
- C code generator
- Type system
- RPC stub generator
- Header file generation

## Dependencies
- Mach headers
- C compiler
- Build system tools

## Integration Points
- Used by all Hurd components
- Generates glibc interfaces
- Creates server/client stubs