#!/bin/bash
# Script to run Unreal Engine automation tests
# Usage: ./run_tests.sh [TestFilter]

# Set Unreal Engine installation path (adjust as needed)
UE_PATH="${UE_PATH:-/opt/unreal-engine/Engine/Binaries/Linux/UnrealEditor}"

# Set project path (relative to script location)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_PATH="${SCRIPT_DIR}/../../ToS.uproject"

# Set test filter (optional)
TEST_FILTER="${1:-AINiagara}"

echo "Running automation tests with filter: ${TEST_FILTER}"
echo ""

# Check if UE path exists
if [ ! -f "$UE_PATH" ]; then
    echo "Error: Unreal Engine not found at $UE_PATH"
    echo "Please set UE_PATH environment variable or update the script"
    exit 1
fi

# Check if project file exists
if [ ! -f "$PROJECT_PATH" ]; then
    echo "Error: Project file not found at $PROJECT_PATH"
    exit 1
fi

# Run tests
"$UE_PATH" "$PROJECT_PATH" -ExecCmds="Automation RunTests ${TEST_FILTER};Quit" -Unattended -NoLogTimes -NullRHI -NoSplash -TestExit="Automation Test Queue Empty"

