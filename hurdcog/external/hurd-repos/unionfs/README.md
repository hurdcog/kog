# UnionFS (Union File System)

**Repository**: https://git.savannah.gnu.org/git/hurd/unionfs.git

## Description
A union filesystem translator for the Hurd that allows multiple filesystems to be overlaid and presented as a single unified view.

## Role in Hurd Ecosystem
- Filesystem layering
- Read-only overlay support
- Package management support
- Live system development

## Key Components
- Union translator
- Layer management
- File priority resolution
- Write redirection
- Directory merging

## Dependencies
- Hurd translator framework
- libnetfs
- File system translators

## Integration Points
- Translator protocol implementation
- Filesystem stacking
- Package manager integration