# Script to build AINiagara plugin
# Usage: .\build_plugin.ps1 [ProjectPath] [Configuration]

param(
    [string]$ProjectPath = "G:\ToS\_Backups\NetworkSample\NetworkSample.uproject",
    [string]$Configuration = "Development",
    [string]$UEPath = "W:\UE_5.3"
)

# Plugin source path
$PluginPath = Join-Path $PSScriptRoot ".."
$PluginPath = Resolve-Path $PluginPath -ErrorAction Stop

# Unreal Build Tool path
$UBTPath = Join-Path $UEPath "Engine\Build\BatchFiles\Build.bat"

Write-Host "========================================"
Write-Host "Building AINiagara Plugin"
Write-Host "========================================"
Write-Host ""
Write-Host "Plugin Path: $PluginPath"
Write-Host "Project: $ProjectPath"
Write-Host "Configuration: $Configuration"
Write-Host "Unreal Engine: $UEPath"
Write-Host ""

# Validate paths
if (-not (Test-Path $UEPath)) {
    Write-Host "ERROR: Unreal Engine not found at: $UEPath" -ForegroundColor Red
    exit 1
}

if (-not (Test-Path $UBTPath)) {
    Write-Host "ERROR: Unreal Build Tool not found at: $UBTPath" -ForegroundColor Red
    exit 1
}

if (-not (Test-Path $ProjectPath)) {
    Write-Host "ERROR: Project file not found at: $ProjectPath" -ForegroundColor Red
    exit 1
}

if (-not (Test-Path $PluginPath)) {
    Write-Host "ERROR: Plugin path not found at: $PluginPath" -ForegroundColor Red
    exit 1
}

# Get project name from .uproject file
$projectName = [System.IO.Path]::GetFileNameWithoutExtension($ProjectPath)

Write-Host "Building plugin for project: $projectName"
Write-Host "This may take several minutes..."
Write-Host ""

# Build command for plugin
# Format: Build.bat <TargetName>Editor <Platform> <Configuration> -Project="<ProjectFile>" -Plugin="<PluginPath>"
$buildArgs = @(
    "${projectName}Editor",
    "Win64",
    $Configuration,
    "-Project=`"$ProjectPath`"",
    "-Plugin=`"$PluginPath`""
)

Write-Host "Build command:"
Write-Host "  $UBTPath $($buildArgs -join ' ')"
Write-Host ""

try {
    # Execute build using cmd to properly handle the batch file
    $processInfo = New-Object System.Diagnostics.ProcessStartInfo
    $processInfo.FileName = "cmd.exe"
    $processInfo.Arguments = "/c `"$UBTPath`" $($buildArgs -join ' ')"
    $processInfo.UseShellExecute = $false
    $processInfo.RedirectStandardOutput = $true
    $processInfo.RedirectStandardError = $true
    $processInfo.CreateNoWindow = $true
    
    $process = New-Object System.Diagnostics.Process
    $process.StartInfo = $processInfo
    
    # Add event handlers for real-time output
    $outputBuilder = New-Object System.Text.StringBuilder
    $errorBuilder = New-Object System.Text.StringBuilder
    
    $script:outputData = ""
    $script:errorData = ""
    
    $process.add_OutputDataReceived({
        if ($EventArgs.Data -ne $null) {
            $outputBuilder.AppendLine($EventArgs.Data) | Out-Null
            Write-Host $EventArgs.Data
        }
    })
    
    $process.add_ErrorDataReceived({
        if ($EventArgs.Data -ne $null) {
            $errorBuilder.AppendLine($EventArgs.Data) | Out-Null
            Write-Host $EventArgs.Data -ForegroundColor Red
        }
    })
    
    Write-Host "Starting build process..."
    Write-Host ""
    
    $process.Start() | Out-Null
    $process.BeginOutputReadLine()
    $process.BeginErrorReadLine()
    $process.WaitForExit()
    
    $result = $outputBuilder.ToString()
    $errorOutput = $errorBuilder.ToString()
    $exitCode = $process.ExitCode
    
    # Check exit code
    if ($exitCode -eq 0) {
        Write-Host ""
        Write-Host "========================================"
        Write-Host "Build completed successfully!" -ForegroundColor Green
        Write-Host "========================================"
        Write-Host ""
        
        # Check for built files
        $binariesPath = Join-Path $PluginPath "Binaries\Win64"
        if (Test-Path $binariesPath) {
            $dllFiles = Get-ChildItem -Path $binariesPath -Filter "*.dll" -ErrorAction SilentlyContinue
            if ($dllFiles) {
                Write-Host "Built files:" -ForegroundColor Green
                foreach ($dll in $dllFiles) {
                    Write-Host "  - $($dll.Name)" -ForegroundColor Green
                }
            }
        }
        
        Write-Host ""
        Write-Host "Plugin is ready to use!"
        Write-Host ""
        exit 0
    } else {
        Write-Host ""
        Write-Host "========================================"
        Write-Host "Build failed with exit code: $exitCode" -ForegroundColor Red
        Write-Host "========================================"
        Write-Host ""
        if ($errorOutput) {
            Write-Host "Error output:" -ForegroundColor Red
            Write-Host $errorOutput
        }
        exit $exitCode
    }
} catch {
    Write-Host ""
    Write-Host "========================================"
    Write-Host "ERROR: Build execution failed" -ForegroundColor Red
    Write-Host "========================================"
    Write-Host ""
    Write-Host "Error: $($_.Exception.Message)" -ForegroundColor Red
    Write-Host ""
    exit 1
}

