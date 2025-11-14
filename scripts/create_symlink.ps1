# Script to create symbolic link for AINiagara plugin
# Usage: .\create_symlink.ps1 [ProjectPath]

param(
    [string]$ProjectPath = "G:\ToS\_Backups\NetworkSample"
)

# Source plugin path
$SourcePath = Join-Path $PSScriptRoot ".."
$SourcePath = Resolve-Path $SourcePath -ErrorAction Stop

# Target plugin path
$TargetPluginsDir = Join-Path $ProjectPath "Plugins"
$TargetPluginPath = Join-Path $TargetPluginsDir "AINiagara"

Write-Host "========================================"
Write-Host "Creating Symbolic Link for AINiagara Plugin"
Write-Host "========================================"
Write-Host ""
Write-Host "Source: $SourcePath"
Write-Host "Target: $TargetPluginPath"
Write-Host ""

# Check if source exists
if (-not (Test-Path $SourcePath)) {
    Write-Host "ERROR: Source plugin not found at: $SourcePath" -ForegroundColor Red
    exit 1
}

# Check if project directory exists
if (-not (Test-Path $ProjectPath)) {
    Write-Host "ERROR: Project directory not found at: $ProjectPath" -ForegroundColor Red
    exit 1
}

# Create Plugins directory if it doesn't exist
if (-not (Test-Path $TargetPluginsDir)) {
    Write-Host "Creating Plugins directory: $TargetPluginsDir"
    New-Item -ItemType Directory -Path $TargetPluginsDir -Force | Out-Null
}

# Check if target already exists
if (Test-Path $TargetPluginPath) {
    Write-Host "WARNING: Target already exists: $TargetPluginPath" -ForegroundColor Yellow
    
    # Check if it's already a symlink
    $item = Get-Item $TargetPluginPath -Force -ErrorAction SilentlyContinue
    if ($item.LinkType -eq "SymbolicLink") {
        Write-Host "Existing symbolic link found. Removing it..." -ForegroundColor Yellow
        Remove-Item $TargetPluginPath -Force
    } else {
        Write-Host "ERROR: Target exists and is not a symbolic link!" -ForegroundColor Red
        Write-Host "Please remove it manually: $TargetPluginPath" -ForegroundColor Red
        exit 1
    }
}

# Check if running as administrator
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)

# Try to create junction first (doesn't require admin)
# Junctions work well for directories and don't need admin privileges
Write-Host "Creating junction (works without admin privileges)..."
$junctionCmd = "cmd /c mklink /J `"$TargetPluginPath`" `"$SourcePath`""
$result = Invoke-Expression $junctionCmd 2>&1

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "========================================"
    Write-Host "Junction created successfully!" -ForegroundColor Green
    Write-Host "========================================"
    Write-Host ""
    Write-Host "Plugin is now linked at: $TargetPluginPath"
    Write-Host ""
    Write-Host "Next steps:"
    Write-Host "1. Open the project in Unreal Editor"
    Write-Host "2. Enable the AINiagara plugin if prompted"
    Write-Host "3. Restart the editor if needed"
    Write-Host ""
    exit 0
}

# If junction failed and we have admin, try symbolic link
if ($isAdmin) {
    Write-Host "Junction failed, trying symbolic link (requires admin)..." -ForegroundColor Yellow
} else {
    Write-Host ""
    Write-Host "ERROR: Failed to create junction" -ForegroundColor Red
    Write-Host "Output: $result" -ForegroundColor Red
    Write-Host ""
    Write-Host "If you need a symbolic link instead of a junction:"
    Write-Host "1. Run PowerShell as Administrator and execute this script again"
    Write-Host "2. Or enable Developer Mode in Windows Settings"
    Write-Host ""
    exit 1
}

# Create symbolic link (requires admin)
try {
    Write-Host "Creating symbolic link..."
    
    # PowerShell 5.1+ uses New-Item with -ItemType SymbolicLink
    # For directories, we need to use -ItemType Directory with -Target
    $null = New-Item -ItemType SymbolicLink -Path $TargetPluginPath -Target $SourcePath -Force
    
    Write-Host ""
    Write-Host "========================================"
    Write-Host "Symbolic link created successfully!" -ForegroundColor Green
    Write-Host "========================================"
    Write-Host ""
    Write-Host "Plugin is now linked at: $TargetPluginPath"
    Write-Host ""
    Write-Host "Next steps:"
    Write-Host "1. Open the project in Unreal Editor"
    Write-Host "2. Enable the AINiagara plugin if prompted"
    Write-Host "3. Restart the editor if needed"
    Write-Host ""
    
} catch {
    Write-Host ""
    Write-Host "ERROR: Failed to create symbolic link" -ForegroundColor Red
    Write-Host "Error: $($_.Exception.Message)" -ForegroundColor Red
    Write-Host ""
    Write-Host "Trying junction as fallback..."
    
    # Try junction as fallback
    $junctionCmd = "cmd /c mklink /J `"$TargetPluginPath`" `"$SourcePath`""
    $result = Invoke-Expression $junctionCmd 2>&1
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host ""
        Write-Host "Junction created successfully as fallback!" -ForegroundColor Green
        Write-Host "Plugin is now linked at: $TargetPluginPath"
        exit 0
    } else {
        Write-Host "Failed to create junction as well: $result" -ForegroundColor Red
        exit 1
    }
}

