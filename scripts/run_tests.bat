@echo off
REM Script to run Unreal Engine automation tests
REM Usage: run_tests.bat [TestFilter] [ProjectPath]

setlocal

REM Set Unreal Engine installation path
set UE_PATH=W:\UE_5.3\Engine\Binaries\Win64\UnrealEditor.exe

REM Set project path (can be overridden by second parameter)
if not "%~2"=="" (
    set PROJECT_PATH=%~2
) else (
    set PROJECT_PATH=%~dp0..\..\..\ToS.uproject
)

REM Set test filter (optional)
set TEST_FILTER=AINiagara
if not "%~1"=="" set TEST_FILTER=%~1

echo ========================================
echo Running AINiagara Automation Tests
echo ========================================
echo.
echo Unreal Engine: %UE_PATH%
echo Project: %PROJECT_PATH%
echo Test Filter: %TEST_FILTER%
echo.

REM Check if Unreal Engine exists
if not exist "%UE_PATH%" (
    echo ERROR: Unreal Engine not found at: %UE_PATH%
    echo Please update UE_PATH in the script.
    pause
    exit /b 1
)

REM Check if project file exists
if not exist "%PROJECT_PATH%" (
    echo ERROR: Project file not found at: %PROJECT_PATH%
    echo.
    echo Usage: run_tests.bat [TestFilter] [ProjectPath]
    echo Example: run_tests.bat AINiagara "G:\ToS\YourProject.uproject"
    echo.
    pause
    exit /b 1
)

echo Starting Unreal Editor to run tests...
echo This may take a few moments...
echo.

REM Run tests with log output
echo Running tests... This may take several minutes.
echo.
"%UE_PATH%" "%PROJECT_PATH%" -ExecCmds="Automation RunTests %TEST_FILTER%;Quit" -Unattended -NoLogTimes -NullRHI -NoSplash -TestExit="Automation Test Queue Empty" -Log="TestResults.log"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo Tests completed successfully!
    echo ========================================
    echo.
    echo Check TestResults.log for detailed results.
) else (
    echo.
    echo ========================================
    echo Tests completed with errors (Exit Code: %ERRORLEVEL%)
    echo ========================================
    echo.
    echo Check TestResults.log for detailed error information.
    echo Log file location: %CD%\TestResults.log
    echo.
)

endlocal

