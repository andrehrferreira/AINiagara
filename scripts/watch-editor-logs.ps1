# Script to watch Unreal Editor logs for AINiagara messages
param(
    [int]$Lines = 50
)

$logPath = "G:\ToS\_Backups\NetworkSample\Saved\Logs"
if (-not (Test-Path $logPath)) {
    Write-Host "Log directory not found: $logPath" -ForegroundColor Red
    exit 1
}

# Find the most recent log file
$latestLog = Get-ChildItem $logPath -Filter "*.log" | Sort-Object LastWriteTime -Descending | Select-Object -First 1

if (-not $latestLog) {
    Write-Host "No log files found in: $logPath" -ForegroundColor Red
    exit 1
}

Write-Host "Monitoring: $($latestLog.FullName)" -ForegroundColor Cyan
Write-Host "Press Ctrl+C to stop" -ForegroundColor Yellow
Write-Host ""

# Get initial position
$lastPosition = $latestLog.Length

# Watch for new content
while ($true) {
    Start-Sleep -Seconds 1
    
    if ($latestLog.Exists) {
        $currentLength = $latestLog.Length
        
        if ($currentLength -gt $lastPosition) {
            # Read new content
            $stream = [System.IO.File]::OpenRead($latestLog.FullName)
            $stream.Position = $lastPosition
            $reader = New-Object System.IO.StreamReader($stream)
            $newContent = $reader.ReadToEnd()
            $reader.Close()
            $stream.Close()
            
            if ($newContent) {
                # Filter for AINiagara related messages
                $newContent -split "`n" | ForEach-Object {
                    $line = $_.Trim()
                    if ($line -match "AINiagara|ToolMenus|Menu|Toolbar|Successfully|Failed|Error|Warning") {
                        Write-Host $line
                    }
                }
            }
            
            $lastPosition = $currentLength
        }
    } else {
        # File was deleted/recreated, reset position
        $latestLog = Get-ChildItem $logPath -Filter "*.log" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
        if ($latestLog) {
            $lastPosition = $latestLog.Length
        }
    }
}

