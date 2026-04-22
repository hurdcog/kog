# Build Fix Notes

## x-gha Binary Caching Removed
- The `x-gha` vcpkg binary caching backend has been removed from vcpkg
- Replacement: Use NuGet-based binary caching with GitHub Packages
- VCPKG_BINARY_SOURCES should use: `clear;nuget,https://nuget.pkg.github.com/<OWNER>/index.json,readwrite`
- Or use filesystem-based caching: `clear;files,<path>,readwrite`
- Requires `permissions: packages: write` in workflow

## Key Changes Needed
1. Replace `VCPKG_BINARY_SOURCES: "clear;x-gha,readwrite"` with filesystem caching
2. Remove the `Export GitHub Actions Cache Variables` step (no longer needed for x-gha)
3. Use `actions/cache` for the vcpkg binary cache directory instead

## Simpler Alternative: Use actions/cache with filesystem binary sources
- VCPKG_BINARY_SOURCES: "clear;files,${{ env.VCPKG_DEFAULT_BINARY_CACHE }},readwrite"
- Use actions/cache to persist the binary cache directory
