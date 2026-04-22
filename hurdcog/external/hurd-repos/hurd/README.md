# The GNU Hurd

**Repository**: https://git.savannah.gnu.org/git/hurd/hurd.git

## Description
The main Hurd repository containing the collection of servers and libraries that implement the GNU Hurd operating system on top of GNU Mach.

## Role in Hurd Ecosystem
- Core Hurd servers
- System services implementation
- Translator framework
- User-space system components

## Key Components
- Auth server (authentication)
- Proc server (process management)
- Exec server (program execution)
- File system translators (ext2fs, fatfs, etc.)
- Network translators (pfinet, pflocal)
- Device translators
- Core libraries (libports, libnetfs, etc.)

## Dependencies
- GNU Mach (gnumach)
- GNU C Library (glibc)
- MIG (interface generator)

## Integration Points
- Built on Mach IPC
- Translator protocol implementation
- System service coordination