# Script to find the AINiagara debug output file
# Checks multiple possible locations

$searchPaths = @(
    "G:\ToS\Plugins\AINiagara\debug_output.json",
    "G:\ToS\_Backups\NetworkSample\Plugins\AINiagara\debug_output.json",
    "G:\ToS\_Backups\NetworkSample\Saved\AINiagara\debug_output.json",
    "$env:USERPROFILE\Documents\Unreal Projects\*\Plugins\AINiagara\debug_output.json",
    "$env:USERPROFILE\Documents\Unreal Projects\*\Saved\AINiagara\debug_output.json"
)

Write-Host "Searching for AINiagara debug_output.json..." -ForegroundColor Cyan

$found = $false
foreach ($pattern in $searchPaths) {
    if ($pattern -like "*`**") {
        # Wildcard pattern - search recursively
        $files = Get-ChildItem -Path (Split-Path $pattern -Parent) -Filter "debug_output.json" -Recurse -ErrorAction SilentlyContinue
        foreach ($file in $files) {
            if ($file.FullName -like $pattern) {
                Write-Host "Found: $($file.FullName)" -ForegroundColor Green
                $found = $true
                break
            }
        }
    } else {
        if (Test-Path $pattern) {
            Write-Host "Found: $pattern" -ForegroundColor Green
            $found = $true
            break
        }
    }
}

if (-not $found) {
    Write-Host "`nDebug file not found. Possible reasons:" -ForegroundColor Yellow
    Write-Host "1. Plugin not loaded in Unreal Editor" -ForegroundColor Gray
    Write-Host "2. Plugin initialization not complete" -ForegroundColor Gray
    Write-Host "3. File in different location" -ForegroundColor Gray
    Write-Host "`nTo find the correct path, run in Unreal console: AINiagara.GetDebugPath" -ForegroundColor Cyan
}

