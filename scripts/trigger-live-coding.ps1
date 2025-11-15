# Script to trigger Live Coding in Unreal Editor
Add-Type -AssemblyName System.Windows.Forms

# Find Unreal Editor window
$unrealProcess = Get-Process | Where-Object { $_.MainWindowTitle -like "*Unreal Editor*" -or $_.ProcessName -like "*UnrealEditor*" }

if ($unrealProcess) {
    Write-Host "Found Unreal Editor process: $($unrealProcess.ProcessName)"
    
    # Activate the window
    [System.Windows.Forms.SendKeys]::SendWait("^%{F11}")
    Write-Host "Sent Ctrl+Alt+F11 to trigger Live Coding"
} else {
    Write-Host "Unreal Editor not found. Please make sure the editor is running."
    exit 1
}

