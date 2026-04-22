# Tools Directory

This directory contains essential build and installation tools mirrored locally for SSR (Server-Side Rendering) reliability and reproducible builds.

## GNU Guix Installer

### Version Information

- **File**: `guix-install.sh`
- **Source**: https://git.savannah.gnu.org/cgit/guix.git/plain/etc/guix-install.sh
- **Retrieved**: 2025-11-06
- **SHA256 Checksum**: `7200027a2d9bea7b61363ae2298cc988840be47326f168f8010002559fe3b134`
- **Lines**: 1063

### Purpose

The `guix-install.sh` script is the official GNU Guix installation script. It is mirrored locally to:

1. **Eliminate Network Dependencies**: Avoid CI failures due to remote download issues
2. **Ensure Reproducibility**: Pin a specific, vetted version of the installer
3. **Provide Transparency**: Enable security review and audit of the installer script
4. **Follow SSR Best Practices**: Server-side processes should not depend on external network resources

### SSR Rationale (RegimA Zone Principles)

This approach aligns with RegimA Zone principles:

- **Scientific Integrity**: Transparent provenance and version control of critical infrastructure
- **Holistic Integration**: Anti-network-inflammation/root-cause correction for reliable CI/CD
- **Professional Excellence**: Backend reliability through reproducible, deterministic builds

### Updating the Installer

When updating to a new version of the Guix installer:

1. Download the latest version:
   ```bash
   curl -fsSL "https://git.savannah.gnu.org/cgit/guix.git/plain/etc/guix-install.sh" -o tools/guix-install.sh
   ```

2. Verify the script by reviewing its contents and checking for any suspicious modifications

3. Calculate and record the new SHA256 checksum:
   ```bash
   sha256sum tools/guix-install.sh
   ```

4. Update this README with the new checksum, retrieval date, and line count

5. Test the updated script in a clean environment before committing

### License

The `guix-install.sh` script is part of GNU Guix and is licensed under the GNU General Public License v3 or later.
See the file header for full copyright and license information.

---

**Note**: This SSR solution ensures all builds run server-side, reproducibly, with transparent installer provenance.
