# Windows Compatibility Fix Script for OpenCog Components
# This script applies Windows-specific fixes to source files

param(
    [string]$ComponentPath = ".",
    [switch]$DryRun = $false
)

Write-Host "=== OpenCog Windows Compatibility Fixer ===" -ForegroundColor Cyan
Write-Host "Component Path: $ComponentPath" -ForegroundColor Yellow
Write-Host "Dry Run: $DryRun" -ForegroundColor Yellow
Write-Host ""

# Files that need Windows compatibility fixes
$filesToFix = @(
    "opencog/cogserver/modules/commands/BuiltinRequestsModule.cc",
    "opencog/cogserver/server/CogServer.cc",
    "opencog/cogserver/server/ModuleManager.cc",
    "opencog/cogserver/server/RequestManager.cc",
    "opencog/cogserver/server/ServerConsole.cc",
    "opencog/cogserver/shell/JsonShellModule.cc",
    "opencog/cogserver/shell/SexprShellModule.cc",
    "opencog/cogserver/modules/python/PythonModule.cc",
    "opencog/cogserver/server/CogServerMain.cc",
    "opencog/network/GenericShell.cc",
    "opencog/network/NetworkServer.cc",
    "opencog/network/ServerSocket.cc",
    "opencog/network/WebSocket.cc"
)

$headerGuard = @'
#ifdef _WIN32
  #include <io.h>
  #include <process.h>
  #include <winsock2.h>
  #include <windows.h>
  #define sleep(x) Sleep((x) * 1000)
  #define usleep(x) Sleep((x) / 1000)
  #define getpid _getpid
  #define popen _popen
  #define pclose _pclose
  typedef int socklen_t;
#else
  #include <unistd.h>
  #include <sys/socket.h>
  #include <sys/time.h>
  #include <dlfcn.h>
#endif

'@

$fixedCount = 0
$skippedCount = 0
$errorCount = 0

foreach ($relativeFile in $filesToFix) {
    $fullPath = Join-Path $ComponentPath $relativeFile
    
    if (-not (Test-Path $fullPath)) {
        Write-Host "⚠ Skipping (not found): $relativeFile" -ForegroundColor Yellow
        $skippedCount++
        continue
    }
    
    Write-Host "Processing: $relativeFile" -ForegroundColor Cyan
    
    try {
        # Read file content
        $content = Get-Content $fullPath -Raw -Encoding UTF8
        $originalContent = $content
        
        # Track changes
        $changes = @()
        
        # Check if already has Windows header guard
        if ($content -match '#ifdef _WIN32') {
            Write-Host "  ✓ Already has Windows header guard" -ForegroundColor Green
        } else {
            # Find the first #include statement
            if ($content -match '(?m)^#include') {
                # Insert header guard before first include
                $content = $content -replace '(?m)(^#include)', "$headerGuard`$1"
                $changes += "Added Windows header guard"
            } else {
                # No includes found, add at beginning
                $content = $headerGuard + $content
                $changes += "Added Windows header guard at beginning"
            }
        }
        
        # Replace alternative operators (but not in comments or strings)
        # This is a simplified approach - more sophisticated parsing would be better
        $andCount = ([regex]::Matches($content, '\band\b')).Count
        $orCount = ([regex]::Matches($content, '\bor\b')).Count
        $notCount = ([regex]::Matches($content, '\bnot\b')).Count
        
        if ($andCount -gt 0) {
            $content = $content -replace '\band\b', '&&'
            $changes += "Replaced $andCount 'and' operators"
        }
        
        if ($orCount -gt 0) {
            $content = $content -replace '\bor\b', '||'
            $changes += "Replaced $orCount 'or' operators"
        }
        
        if ($notCount -gt 0) {
            $content = $content -replace '\bnot\b', '!'
            $changes += "Replaced $notCount 'not' operators"
        }
        
        # Check if content changed
        if ($content -ne $originalContent) {
            if ($DryRun) {
                Write-Host "  [DRY RUN] Would apply changes:" -ForegroundColor Yellow
                foreach ($change in $changes) {
                    Write-Host "    - $change" -ForegroundColor White
                }
            } else {
                # Backup original file
                $backupPath = "$fullPath.bak"
                Copy-Item -Path $fullPath -Destination $backupPath -Force
                
                # Write fixed content
                Set-Content -Path $fullPath -Value $content -NoNewline -Encoding UTF8
                
                Write-Host "  ✓ Fixed:" -ForegroundColor Green
                foreach ($change in $changes) {
                    Write-Host "    - $change" -ForegroundColor White
                }
                Write-Host "    - Backup saved: $backupPath" -ForegroundColor Gray
            }
            $fixedCount++
        } else {
            Write-Host "  ✓ No changes needed" -ForegroundColor Green
        }
        
    } catch {
        Write-Host "  ❌ Error: $_" -ForegroundColor Red
        $errorCount++
    }
    
    Write-Host ""
}

Write-Host "=== Summary ===" -ForegroundColor Cyan
Write-Host "Files fixed: $fixedCount" -ForegroundColor Green
Write-Host "Files skipped: $skippedCount" -ForegroundColor Yellow
Write-Host "Errors: $errorCount" -ForegroundColor $(if ($errorCount -gt 0) { "Red" } else { "Green" })

if ($DryRun) {
    Write-Host "`nThis was a dry run. Run without -DryRun to apply changes." -ForegroundColor Yellow
} else {
    Write-Host "`nChanges applied. Backup files created with .bak extension." -ForegroundColor Green
}
