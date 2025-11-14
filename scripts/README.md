# AINiagara Scripts

PowerShell scripts to manage the AINiagara plugin.

## Available Scripts

### 1. `create_symlink.ps1`
Creates a symbolic link (junction) from the plugin to the project.

**Usage:**
```powershell
.\create_symlink.ps1 [ProjectPath]
```

**Example:**
```powershell
.\create_symlink.ps1 "G:\ToS\_Backups\NetworkSample"
```

**Description:**
- Creates a junction from the plugin to `ProjectPath\Plugins\AINiagara`
- Does not require administrator privileges
- Removes existing link before creating a new one

### 2. `build_plugin.ps1`
Builds the AINiagara plugin using Unreal Build Tool.

**Usage:**
```powershell
.\build_plugin.ps1 [ProjectPath] [Configuration] [UEPath]
```

**Parameters:**
- `ProjectPath` - Path to the .uproject file (default: `G:\ToS\_Backups\NetworkSample\NetworkSample.uproject`)
- `Configuration` - Build configuration: Development, Debug, Shipping (default: `Development`)
- `UEPath` - Unreal Engine path (default: `W:\UE_5.3`)

**Example:**
```powershell
.\build_plugin.ps1 "G:\ToS\_Backups\NetworkSample\NetworkSample.uproject" "Development"
```

**Description:**
- Builds the plugin for the specified project
- Shows real-time progress
- Verifies generated files after build

### 3. `build_and_test.ps1`
Builds the plugin and runs automated tests.

**Usage:**
```powershell
.\build_and_test.ps1 [ProjectPath] [Configuration]
```

**Example:**
```powershell
.\build_and_test.ps1 "G:\ToS\_Backups\NetworkSample\NetworkSample.uproject"
```

**Description:**
- Runs `build_plugin.ps1` first
- If build succeeds, runs `run_tests.bat`
- Stops on first failure

### 4. `run_tests.bat`
Runs automated tests for the plugin.

**Usage:**
```batch
run_tests.bat [TestFilter] [ProjectPath]
```

**Example:**
```batch
run_tests.bat AINiagara "G:\ToS\_Backups\NetworkSample\NetworkSample.uproject"
```

## Complete Workflow

### First time (setup):
```powershell
# 1. Create symbolic link
.\create_symlink.ps1 "G:\ToS\_Backups\NetworkSample"

# 2. Build plugin
.\build_plugin.ps1 "G:\ToS\_Backups\NetworkSample\NetworkSample.uproject"

# 3. Run tests
.\run_tests.bat AINiagara "G:\ToS\_Backups\NetworkSample\NetworkSample.uproject"
```

### Development (after changes):
```powershell
# Build and test in one command
.\build_and_test.ps1 "G:\ToS\_Backups\NetworkSample\NetworkSample.uproject"
```

## Configuration

Scripts use the following default paths:
- **Unreal Engine**: `W:\UE_5.3`
- **Project**: `G:\ToS\_Backups\NetworkSample\NetworkSample.uproject`

To change, pass parameters or edit default values in the scripts.

## Requirements

- PowerShell 5.1 or higher
- Unreal Engine 5.3 installed
- Visual Studio or Build Tools installed
- Unreal Engine project configured

## Troubleshooting

### Error: "Unreal Build Tool not found"
- Verify Unreal Engine is installed at `W:\UE_5.3`
- Or pass the correct path: `.\build_plugin.ps1 -UEPath "C:\UnrealEngine\5.3"`

### Error: "Project file not found"
- Verify the project path
- Use complete absolute path

### Error: "Build failed"
- Verify Visual Studio is installed
- Check for compilation errors in the code
- Try opening the project in Unreal Editor first to generate necessary files

### Symbolic link doesn't work
- Script uses junction (doesn't require admin)
- If you need a real symlink, run PowerShell as Administrator
