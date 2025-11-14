# Testing Guide

This document describes how to run automated tests for the AINiagara plugin.

## Test Framework

The plugin uses Unreal Engine's **Automation Testing Framework** for automated tests. This framework provides:

- Unit tests for core functionality
- Integration tests for system generation
- UI tests for interface components

## Running Tests

### Method 1: Using Unreal Editor

1. Open the Unreal Editor
2. Go to **Window** → **Developer Tools** → **Session Frontend**
3. Click on the **Automation** tab
4. Expand **Tests** → **AINiagara**
5. Select the tests you want to run
6. Click **Start Tests**

### Method 2: Command Line (Windows)

```batch
cd scripts
run_tests.bat
```

Or with a specific test filter:

```batch
run_tests.bat AINiagara.VFXDSLParser
```

### Method 3: Command Line (Linux/Mac)

```bash
chmod +x scripts/run_tests.sh
./scripts/run_tests.sh
```

Or with a specific test filter:

```bash
./scripts/run_tests.sh AINiagara.VFXDSLParser
```

### Method 4: Direct Unreal Engine Command

```bash
UnrealEditor.exe YourProject.uproject -ExecCmds="Automation RunTests AINiagara;Quit" -Unattended -NoLogTimes -NullRHI -NoSplash
```

## Test Structure

Tests are located in `Source/AINiagara/Tests/` directory:

- `VFXDSLParserTest.cpp` - Tests for DSL JSON parsing
- `VFXDSLValidatorTest.cpp` - Tests for DSL validation
- `ConversationHistoryManagerTest.cpp` - Tests for conversation history management
- `SAINiagaraChatWidgetTest.cpp` - UI tests for chat interface
- `SAINiagaraAPIKeyDialogTest.cpp` - UI tests for API configuration dialog

See [FEATURES_VALIDATION.md](./FEATURES_VALIDATION.md) for complete scenario coverage mapping.

## Writing New Tests

To add a new test, create a new file in `Source/AINiagara/Tests/`:

```cpp
#include "Misc/AutomationTest.h"
#include "YourClass.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FYourClassTest,
	"AINiagara.YourClass.YourTestName",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FYourClassTest::RunTest(const FString& Parameters)
{
	// Your test code here
	TestTrue(TEXT("Test description"), true);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
```

## Test Assertions

Available test assertions:

- `TestTrue(Message, Condition)` - Assert condition is true
- `TestFalse(Message, Condition)` - Assert condition is false
- `TestEqual(Message, Actual, Expected)` - Assert values are equal
- `TestNotEqual(Message, Actual, Expected)` - Assert values are not equal
- `TestNull(Message, Pointer)` - Assert pointer is null
- `TestNotNull(Message, Pointer)` - Assert pointer is not null

## Test Coverage

Current test coverage targets:

- **Unit Tests**: 95%+ coverage for core modules
- **Integration Tests**: All major workflows
- **UI Tests**: Critical user interactions

## Continuous Integration

Tests can be integrated into CI/CD pipelines:

```yaml
# Example GitHub Actions workflow
- name: Run Tests
  run: |
    UnrealEditor.exe Project.uproject -ExecCmds="Automation RunTests AINiagara;Quit" -Unattended -NoLogTimes -NullRHI -NoSplash
```

## Troubleshooting

### Tests not appearing in Automation tab

- Ensure `WITH_DEV_AUTOMATION_TESTS` is defined
- Rebuild the plugin
- Check that test files are included in the build

### Tests failing

- Check the Output Log for detailed error messages
- Verify test data and setup
- Ensure all dependencies are available

### Performance issues

- Use `EAutomationTestFlags::SmokeFilter` for quick tests
- Use `EAutomationTestFlags::EngineFilter` for full tests
- Consider using `-NullRHI` flag for headless test runs

