# External Repositories

This directory contains external GNU and Hurd repositories that have been integrated into the main repository.

## Status

The git submodule references (mode 160000) have been removed. To complete the repository integration:

1. **Clone repositories**: Run `./clone-repos.sh` from the root directory when network access to git.savannah.gnu.org is available
2. **Integrate repositories**: The script will clone the repositories with their .git directories
3. **Remove .git directories**: Run `find external/ -name ".git" -type d -exec rm -rf {} +` to convert them to regular directories
4. **Add to repository**: Run `git add external/` to track the integrated content
5. **Commit**: Run `git commit -m "Integrate external repositories"`

## Repositories Included

### GNU Repositories
- **bash**: GNU Bash shell

### Hurd Repositories  
- **glibc**: GNU C Library
- **gnumach**: GNU Mach microkernel
- **hurd**: GNU Hurd core
- **hurd-meta**: Hurd metadata
- **incubator**: Hurd incubator projects
- **libpthread**: POSIX threads library
- **mig**: Mach Interface Generator
- **procfs**: Process filesystem
- **unionfs**: Union filesystem
- **viengoos**: Viengoos microkernel
- **web**: Hurd web resources

## GitHub Actions

The repository includes a GitHub Actions workflow (`.github/workflows/clone-repos.yml`) that automatically runs the clone script when:
- The workflow is manually triggered
- Changes are made to `clone-repos.sh` or the workflow file

The workflow has proper error handling, timeout protection, and artifact uploads for debugging.