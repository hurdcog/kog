$ErrorActionPreference = 'Stop'

$packageName = 'opencog'
$toolsDir = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"
$installPath = Join-Path $toolsDir 'opencog'

# Remove from PATH
$pathToRemove = "$installPath\bin"
$envPath = [Environment]::GetEnvironmentVariable('Path', 'Machine')
if ($envPath -like "*$pathToRemove*") {
    $newPath = ($envPath.Split(';') | Where-Object { $_ -ne $pathToRemove }) -join ';'
    [Environment]::SetEnvironmentVariable('Path', $newPath, 'Machine')
    Write-Host "Removed $pathToRemove from system PATH"
}

# Remove installation directory
if (Test-Path $installPath) {
    Remove-Item -Path $installPath -Recurse -Force
    Write-Host "Removed installation directory: $installPath"
}

Write-Host "OpenCog has been uninstalled successfully!"
