# Build script for Unreal Engine with AINiagara plugin
# Usage: .\build-engine.ps1 [ProjectPath]
# Example: .\build-engine.ps1 "G:\ToS\_Backups\NetworkSample\NetworkSample.uproject"

param(
    [string]$ProjectPath = "",
    [string]$EnginePath = "W:\UE_5.3",
    [string]$PluginPath = "G:\ToS\Plugins\AINiagara",
    [string]$Configuration = "Development",
    [string]$Platform = "Win64"
)

$ErrorActionPreference = "Continue"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "   AINiagara Plugin Build Script" -ForegroundColor Cyan
Write-Host "   UE 5.3 Compatibility Build" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Verify paths
if (-not (Test-Path $EnginePath)) {
    Write-Host "ERROR: Engine path not found: $EnginePath" -ForegroundColor Red
    exit 1
}

if (-not (Test-Path $PluginPath)) {
    Write-Host "ERROR: Plugin path not found: $PluginPath" -ForegroundColor Red
    exit 1
}

$BuildBat = Join-Path $EnginePath "Engine\Build\BatchFiles\Build.bat"
if (-not (Test-Path $BuildBat)) {
    Write-Host "ERROR: Build.bat not found at: $BuildBat" -ForegroundColor Red
    exit 1
}

Write-Host "Configuration:" -ForegroundColor Yellow
Write-Host "  Engine: $EnginePath" -ForegroundColor Gray
Write-Host "  Plugin: $PluginPath" -ForegroundColor Gray
Write-Host "  Platform: $Platform" -ForegroundColor Gray
Write-Host "  Configuration: $Configuration" -ForegroundColor Gray
Write-Host ""

# Determine target
if ($ProjectPath -and (Test-Path $ProjectPath)) {
    $ProjectName = [System.IO.Path]::GetFileNameWithoutExtension($ProjectPath)
    $TargetName = "${ProjectName}Editor"
    Write-Host "Building project: $ProjectName" -ForegroundColor Green
    Write-Host "  Project: $ProjectPath" -ForegroundColor Gray
    
    $BuildCommand = "& `"$BuildBat`" ${TargetName} ${Platform} ${Configuration} -Project=`"${ProjectPath}`" -WaitMutex -FromMsBuild"
} else {
    Write-Host "Building UnrealEditor with plugin..." -ForegroundColor Green
    $BuildCommand = "& `"$BuildBat`" UnrealEditor ${Platform} ${Configuration} `"${EnginePath}`" -WaitMutex"
}

Write-Host ""
Write-Host "Starting build..." -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Create logs directory
$LogDir = Join-Path $PluginPath "logs"
if (-not (Test-Path $LogDir)) {
    New-Item -ItemType Directory -Path $LogDir | Out-Null
}

$LogFile = Join-Path $LogDir "build-$(Get-Date -Format 'yyyyMMdd-HHmmss').log"
$ErrorLogFile = Join-Path $LogDir "errors-$(Get-Date -Format 'yyyyMMdd-HHmmss').log"

Write-Host "Log files:" -ForegroundColor Gray
Write-Host "  Full log: $LogFile" -ForegroundColor Gray
Write-Host "  Errors: $ErrorLogFile" -ForegroundColor Gray
Write-Host ""

# Track errors
$ErrorCount = 0
$WarningCount = 0
$ErrorPattern = "(?i)(error|failed|fatal)"
$WarningPattern = "(?i)(warning|deprecated)"

# Build function with real-time output
function Build-WithOutput {
    param([string]$Command)
    
    $process = Start-Process -FilePath "powershell.exe" `
        -ArgumentList "-NoProfile", "-Command", $Command `
        -NoNewWindow `
        -RedirectStandardOutput $LogFile `
        -RedirectStandardError (Join-Path $LogDir "stderr.log") `
        -PassThru `
        -Wait
    
    # Monitor log file in real-time
    if (Test-Path $LogFile) {
        $reader = [System.IO.StreamReader]::new($LogFile)
        $lastPosition = 0
        
        while (-not $process.HasExited -or $reader.BaseStream.Position -lt $reader.BaseStream.Length) {
            Start-Sleep -Milliseconds 500
            
            if ($reader.BaseStream.Length -gt $lastPosition) {
                $reader.BaseStream.Position = $lastPosition
                $newContent = $reader.ReadToEnd()
                $lastPosition = $reader.BaseStream.Position
                
                if ($newContent) {
                    $lines = $newContent -split "`n"
                    foreach ($line in $lines) {
                        $line = $line.Trim()
                        if (-not $line) { continue }
                        
                        # Check for errors
                        if ($line -match $ErrorPattern) {
                            Write-Host $line -ForegroundColor Red
                            $script:ErrorCount++
                            Add-Content -Path $ErrorLogFile -Value $line
                        }
                        # Check for warnings
                        elseif ($line -match $WarningPattern) {
                            Write-Host $line -ForegroundColor Yellow
                            $script:WarningCount++
                        }
                        # Normal output (show important lines)
                        elseif ($line -match "(?i)(building|compiling|linking|writing|error|warning|failed|succeeded|completed)") {
                            Write-Host $line -ForegroundColor Cyan
                        }
                        # Show all output for detailed debugging
                        else {
                            Write-Host $line -ForegroundColor Gray
                        }
                    }
                }
            }
            
            if (-not $process.HasExited) {
                Start-Sleep -Milliseconds 100
            }
        }
        
        $reader.Close()
    }
    
    return $process.ExitCode
}

# Alternative: Simple build with output streaming
Write-Host "Executing build command..." -ForegroundColor Yellow
Write-Host ""

try {
    # Execute build and capture output
    $output = Invoke-Expression $BuildCommand 2>&1 | Tee-Object -Variable fullOutput
    
    # Write all output
    $fullOutput | ForEach-Object {
        $line = $_.ToString()
        
        if ($line -match $ErrorPattern) {
            Write-Host $line -ForegroundColor Red
            $ErrorCount++
            Add-Content -Path $ErrorLogFile -Value $line -ErrorAction SilentlyContinue
        }
        elseif ($line -match $WarningPattern) {
            Write-Host $line -ForegroundColor Yellow
            $WarningCount++
        }
        elseif ($line -match "(?i)(building|compiling|linking|writing|succeeded|completed|total)") {
            Write-Host $line -ForegroundColor Cyan
        }
        else {
            Write-Host $line -ForegroundColor Gray
        }
    }
    
    # Save full output to log
    $fullOutput | Out-File -FilePath $LogFile -Encoding UTF8
    
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "Build Summary:" -ForegroundColor Yellow
    Write-Host "  Errors: $ErrorCount" -ForegroundColor $(if ($ErrorCount -gt 0) { "Red" } else { "Green" })
    Write-Host "  Warnings: $WarningCount" -ForegroundColor $(if ($WarningCount -gt 0) { "Yellow" } else { "Green" })
    Write-Host ""
    
    if ($ErrorCount -gt 0) {
        Write-Host "Build FAILED with $ErrorCount error(s)" -ForegroundColor Red
        Write-Host "Check error log: $ErrorLogFile" -ForegroundColor Yellow
        exit 1
    } else {
        Write-Host "Build SUCCEEDED!" -ForegroundColor Green
        exit 0
    }
}
catch {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Red
    Write-Host "Build Exception:" -ForegroundColor Red
    Write-Host $_.Exception.Message -ForegroundColor Red
    Write-Host $_.ScriptStackTrace -ForegroundColor Red
    exit 1
}

