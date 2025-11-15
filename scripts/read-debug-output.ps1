# Script to read and display AINiagara debug output JSON
param(
    [switch]$Watch,
    [int]$Interval = 2
)

$DebugFile = Join-Path $PSScriptRoot "..\debug_output.json"

if (-not (Test-Path $DebugFile)) {
    Write-Host "Debug file not found: $DebugFile" -ForegroundColor Yellow
    Write-Host "Make sure the plugin is loaded and the log monitor is initialized." -ForegroundColor Yellow
    Write-Host "You can get the path by running: AINiagara.GetDebugPath in the Unreal console" -ForegroundColor Yellow
    exit 1
}

function Read-DebugOutput {
    if (Test-Path $DebugFile) {
        try {
            $json = Get-Content $DebugFile -Raw | ConvertFrom-Json
            
            Write-Host "`n=== AINiagara Debug Output ===" -ForegroundColor Cyan
            Write-Host "Last Update: $($json.lastUpdate)" -ForegroundColor Gray
            Write-Host "Monitoring: $($json.isMonitoring)" -ForegroundColor $(if ($json.isMonitoring) { "Green" } else { "Red" })
            Write-Host "Log File: $($json.logFilePath)" -ForegroundColor Gray
            Write-Host "Log File Size: $($json.logFileSize) bytes" -ForegroundColor Gray
            
            Write-Host "`n--- Live Coding Status ---" -ForegroundColor Yellow
            $lc = $json.liveCoding
            Write-Host "Status: $($lc.status)" -ForegroundColor $(switch ($lc.status) {
                "success" { "Green" }
                "failed" { "Red" }
                "compiling" { "Yellow" }
                default { "Gray" }
            })
            Write-Host "Last Event: $($lc.lastEvent)" -ForegroundColor Gray
            Write-Host "Last Event Time: $($lc.lastEventTime)" -ForegroundColor Gray
            
            Write-Host "`n--- Recent Messages ($($json.messageCount)) ---" -ForegroundColor Yellow
            if ($json.recentMessages.Count -gt 0) {
                foreach ($msg in $json.recentMessages) {
                    $color = switch ($msg.verbosity) {
                        "Error" { "Red" }
                        "Warning" { "Yellow" }
                        default { "White" }
                    }
                    Write-Host "[$($msg.timestamp)] [$($msg.verbosity)] $($msg.message)" -ForegroundColor $color
                }
            } else {
                Write-Host "No relevant messages found yet." -ForegroundColor Gray
            }
            
            Write-Host "`n================================`n" -ForegroundColor Cyan
        }
        catch {
            Write-Host "Error reading debug file: $_" -ForegroundColor Red
        }
    }
}

if ($Watch) {
    Write-Host "Watching debug output file (Press Ctrl+C to stop)..." -ForegroundColor Green
    while ($true) {
        Clear-Host
        Read-DebugOutput
        Start-Sleep -Seconds $Interval
    }
} else {
    Read-DebugOutput
}

