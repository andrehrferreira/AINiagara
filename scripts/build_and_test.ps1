# Script to build plugin and run tests
# Usage: .\build_and_test.ps1 [ProjectPath] [Configuration]

param(
    [string]$ProjectPath = "G:\ToS\_Backups\NetworkSample\NetworkSample.uproject",
    [string]$Configuration = "Development"
)

$ErrorActionPreference = "Stop"

Write-Host "========================================"
Write-Host "Build and Test AINiagara Plugin"
Write-Host "========================================"
Write-Host ""

# Step 1: Build plugin
Write-Host "Step 1: Building plugin..." -ForegroundColor Cyan
Write-Host ""
$buildScript = Join-Path $PSScriptRoot "build_plugin.ps1"
& $buildScript -ProjectPath $ProjectPath -Configuration $Configuration

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "Build failed. Aborting test execution." -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host ""
Write-Host "Step 2: Running tests..." -ForegroundColor Cyan
Write-Host ""

# Step 2: Run tests
$testScript = Join-Path $PSScriptRoot "run_tests.bat"
& $testScript "AINiagara" $ProjectPath

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "Tests completed with errors (Exit Code: $LASTEXITCODE)" -ForegroundColor Yellow
    exit $LASTEXITCODE
}

Write-Host ""
Write-Host "========================================"
Write-Host "Build and Test completed successfully!" -ForegroundColor Green
Write-Host "========================================"
Write-Host ""

