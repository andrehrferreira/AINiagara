# Quick build script - compiles and shows errors in real-time
# Usage: .\quick-build.ps1

param(
    [string]$ProjectPath = "G:\ToS\_Backups\NetworkSample\NetworkSample.uproject",
    [string]$EnginePath = "W:\UE_5.3"
)

$ErrorActionPreference = "Continue"

Write-Host "Quick Build - AINiagara Plugin" -ForegroundColor Cyan
Write-Host ""

$BuildBat = Join-Path $EnginePath "Engine\Build\BatchFiles\Build.bat"

if ($ProjectPath -and (Test-Path $ProjectPath)) {
    $ProjectName = [System.IO.Path]::GetFileNameWithoutExtension($ProjectPath)
    $TargetName = "${ProjectName}Editor"
    
    Write-Host "Building: $TargetName" -ForegroundColor Yellow
    Write-Host "Project: $ProjectPath" -ForegroundColor Gray
    Write-Host ""
    
    & $BuildBat ${TargetName} Win64 Development -Project="$ProjectPath" -WaitMutex -FromMsBuild 2>&1 | ForEach-Object {
        $line = $_
        
        if ($line -match "(?i)error|failed|fatal") {
            Write-Host $line -ForegroundColor Red
        }
        elseif ($line -match "(?i)warning") {
            Write-Host $line -ForegroundColor Yellow
        }
        elseif ($line -match "(?i)(building|compiling|linking|succeeded|completed|total)") {
            Write-Host $line -ForegroundColor Cyan
        }
        else {
            Write-Host $line
        }
    }
} else {
    Write-Host "ERROR: Project not found: $ProjectPath" -ForegroundColor Red
    exit 1
}

