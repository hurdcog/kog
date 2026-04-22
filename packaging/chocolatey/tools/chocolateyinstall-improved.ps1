$ErrorActionPreference = 'Stop'

$packageName = 'opencog'
$toolsDir = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"

# For local installation from included files
$opencogDir = Join-Path $toolsDir 'opencog'

Write-Host "=== Installing OpenCog Framework ===" -ForegroundColor Cyan
Write-Host "Package: $packageName" -ForegroundColor White
Write-Host "Tools directory: $toolsDir" -ForegroundColor White
Write-Host "Installation directory: $opencogDir" -ForegroundColor White

# Check if opencog directory exists (for local package installation)
if (Test-Path $opencogDir) {
    Write-Host "✓ OpenCog binaries found in package" -ForegroundColor Green
    
    # Verify critical files exist
    $binDir = Join-Path $opencogDir 'bin'
    if (-not (Test-Path $binDir)) {
        Write-Error "OpenCog bin directory not found at: $binDir"
        throw "Invalid package structure"
    }
    
    Write-Host "✓ Binary directory verified" -ForegroundColor Green
    
    # Add to PATH
    Write-Host "Adding OpenCog to system PATH..." -ForegroundColor Yellow
    Install-ChocolateyPath -PathToInstall $binDir -PathType 'Machine'
    Write-Host "✓ PATH updated" -ForegroundColor Green
    
    # Set environment variables
    $env:OPENCOG_HOME = $opencogDir
    [Environment]::SetEnvironmentVariable('OPENCOG_HOME', $opencogDir, 'Machine')
    Write-Host "✓ OPENCOG_HOME set to: $opencogDir" -ForegroundColor Green
    
} else {
    Write-Warning "OpenCog binaries not found in package"
    Write-Warning "This may be a metadata-only package"
    Write-Host "Please ensure Windows builds have completed successfully" -ForegroundColor Yellow
}

# Check for Visual C++ Redistributable
Write-Host "Checking Visual C++ Redistributable..." -ForegroundColor Yellow
$vcRedist = Get-ItemProperty "HKLM:\SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" -ErrorAction SilentlyContinue

if ($vcRedist) {
    Write-Host "✓ Visual C++ Redistributable already installed" -ForegroundColor Green
} else {
    Write-Host "Visual C++ Redistributable not found" -ForegroundColor Yellow
    Write-Host "Installing via Chocolatey dependency..." -ForegroundColor Yellow
    # This will be handled by Chocolatey dependency system
}

# Create start menu shortcuts (optional)
$startMenuDir = Join-Path $env:ProgramData "Microsoft\Windows\Start Menu\Programs\OpenCog"
if (Test-Path $opencogDir) {
    try {
        New-Item -ItemType Directory -Force -Path $startMenuDir | Out-Null
        Write-Host "✓ Start menu directory created" -ForegroundColor Green
    } catch {
        Write-Warning "Could not create start menu directory: $_"
    }
}

Write-Host ""
Write-Host "=== OpenCog Installation Complete ===" -ForegroundColor Green
Write-Host ""
Write-Host "Installation Details:" -ForegroundColor Cyan
Write-Host "  Location: $opencogDir" -ForegroundColor White
Write-Host "  Binaries: $(Join-Path $opencogDir 'bin')" -ForegroundColor White
Write-Host "  Libraries: $(Join-Path $opencogDir 'lib')" -ForegroundColor White
Write-Host "  Headers: $(Join-Path $opencogDir 'include')" -ForegroundColor White
Write-Host ""
Write-Host "Getting Started:" -ForegroundColor Cyan
Write-Host "  1. Open a NEW command prompt or PowerShell window" -ForegroundColor White
Write-Host "  2. Verify installation: opencog --version" -ForegroundColor White
Write-Host "  3. For Python: import opencog" -ForegroundColor White
Write-Host "  4. Documentation: https://wiki.opencog.org/" -ForegroundColor White
Write-Host ""
Write-Host "Note: You may need to restart your terminal for PATH changes to take effect" -ForegroundColor Yellow
Write-Host ""
Write-Host "For support, visit: https://github.com/cogpy/occ/issues" -ForegroundColor White
