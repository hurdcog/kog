$ErrorActionPreference = 'Stop'

$packageName = 'opencog'
$toolsDir = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"
$url64 = 'https://github.com/cogpy/occ/releases/download/__VERSION__/opencog-__VERSION__-win64.zip'

$packageArgs = @{
  packageName   = $packageName
  unzipLocation = $toolsDir
  url64bit      = $url64
  checksum64    = '__CHECKSUM64__'
  checksumType64= 'sha256'
}

Install-ChocolateyZipPackage @packageArgs

# Add to PATH
$installPath = Join-Path $toolsDir 'opencog'
Install-ChocolateyPath -PathToInstall "$installPath\bin" -PathType 'Machine'

# Install Visual C++ Redistributable if not present
$vcRedistInstalled = Get-ItemProperty "HKLM:\SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" -ErrorAction SilentlyContinue
if (-not $vcRedistInstalled) {
    Write-Host "Installing Visual C++ Redistributable..."
    Install-ChocolateyPackage -PackageName 'vcredist2019'
}

Write-Host "OpenCog has been installed successfully!"
Write-Host "Installation directory: $installPath"
Write-Host ""
Write-Host "To get started:"
Write-Host "  1. Open a new command prompt or PowerShell window"
Write-Host "  2. Run 'guile' to start the Scheme interpreter"
Write-Host "  3. Load OpenCog: (use-modules (opencog))"
Write-Host ""
Write-Host "For more information, visit: https://wiki.opencog.org/"
