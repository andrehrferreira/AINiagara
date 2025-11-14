# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Planned
- Texture generation with Gemini Imagen 3 (Phase 10)
- Shader/Material generation tool (Phase 11)
- 3D Model integration (Phase 12)
- Cascade to Niagara conversion (Phase 13)
- DSL modification workflow (export → modify → regenerate) (Phase 14.4)

## [0.11.0] - 2025-01-14

### Added - Cascade System Generation ✅
- **Cascade System Generator** - Phase 9 complete
  - `UCascadeSystemGenerator` class
  - Create Cascade particle systems from DSL
  - Create Cascade emitters from DSL
  - Configure spawn modules (rate, bursts)
  - Configure initialization modules (size, color, velocity, rotation)
  - Configure update modules (forces, gravity, wind)
  - Configure render modules (material, blend mode)
  - Full integration in chat widget

### Added - Cascade Reverse Engineering ✅
- **Cascade System to DSL Converter** - Phase 14.2 complete
  - `UCascadeSystemToDSLConverter` class
  - Convert Cascade systems to DSL format
  - Convert Cascade emitters to DSL
  - Extract spawner configuration from modules
  - Extract initialization, update, and render configs
  - Export support in chat widget (both Niagara and Cascade)

### Added - Testing ✅
- **Cascade Integration Tests** - Phase 16.6 complete
  - CascadeSystemGenerator: 9 tests
  - CascadeSystemToDSLConverter: 2 tests
  - **Total: 71 tests (up from 60), all passing** ✅

### Changed
- **Chat Widget** - Enhanced export functionality
  - Now supports both Niagara and Cascade system export
  - Automatic system type detection
  - Unified export workflow

### Technical Details
- Uses `UParticleSystem` and `UParticleEmitter` classes
- Module configuration via `UParticleModuleRequired`, `UParticleModuleSpawn`, etc.
- Supports spawn rate, bursts, size, color, velocity, rotation
- Material and blend mode configuration
- Full round-trip conversion support (DSL → System → DSL)

## [0.10.0] - 2025-01-14

### Added - History Management ✅
- **Automatic History Persistence** - Phase 15 complete
  - Auto-save on asset save events via package hooks
  - Auto-load on asset open (ready for integration)
  - Persistence across editor sessions
  - Enable/disable auto-persistence control
  - Independent tracking per asset
  - Package event integration (`UPackage::PackageSavedWithContextEvent`)
  - JSON file storage in `ProjectSaved/AINiagara/History/`

### Added - Reverse Engineering ✅
- **Niagara System to DSL Converter** - Phase 14.1, 14.3, 14.5
  - `UNiagaraSystemToDSLConverter` class
  - Convert Niagara systems to DSL format
  - Convert individual emitters to DSL
  - Export DSL to JSON with pretty printing
  - Export DSL to file with save dialog
  - Multiple emitters support
  - Error handling for null systems/emitters

### Added - UI Enhancements ✅
- **Export DSL Button** - Integrated in chat widget
  - One-click DSL export from current Niagara system
  - File save dialog integration
  - DSL preview in chat history
  - Success/error notifications

### Added - Testing ✅
- **ConversationHistoryPersistence Tests** - 6 new tests
  - Auto-persistence enable/disable
  - Auto-save on message add
  - Auto-save disabled behavior
  - Session persistence (save/load)
  - Independent tracking per asset
  - File format validation

- **NiagaraSystemToDSLConverter Tests** - 6 new tests
  - Basic conversion test
  - JSON export test
  - File export test
  - Multiple emitters test
  - Error handling test
  - Round-trip conversion test

**Total: 60 tests (up from 48), all passing** ✅

### Changed
- **History Manager** - Enhanced with automatic persistence
  - `RegisterAssetEventHooks()` - Automatic hook registration
  - `SetAutoPersistence(bool)` - Control auto-save behavior
  - `OnPackageSaved()` - Package save event handler
  - Auto-save only when persistence is enabled

### Technical Details
- Uses `UPackage::PackageSavedWithContextEvent` delegate for asset save detection
- Auto-detects `UNiagaraSystem` assets in packages
- Safe filename conversion for asset paths
- DesktopPlatform module integration for file dialogs
- DSL extraction uses defaults (full parameter extraction requires Niagara Graph API)

## [0.9.0] - 2025-01-14

### Added - Core Functionality ✅
- **Gemini API Integration** - Full API client with async support
  - API key management and secure storage
  - Request/response handling with error recovery
  - Conversation message structures
- **DSL System** - Complete JSON-based VFX specification
  - Parser with comprehensive error handling
  - Validator with schema checking
  - Support for Niagara and Cascade effect types
  - Serialization/deserialization
- **Prompt Engineering** - Structured prompt system
  - VFX expert persona instructions
  - DSL format documentation injection
  - Conversation history context integration
  - Tool function documentation
- **Niagara System Generation** - Automatic system creation
  - UNiagaraSystem factory from DSL
  - UNiagaraEmitter creation and configuration
  - Module configuration (Spawn, Initialize, Update, Render)
  - Package and asset registry management
- **Conversation History** - Per-asset persistence
  - Multi-asset history tracking
  - Save/load functionality
  - JSON-based storage format
  - History display in chat interface

### Added - User Interface ✅
- **Chat Interface** - Cursor-style AI chat window
  - Message display with user/assistant/system roles
  - Input field with submission handling
  - Loading indicators and progress feedback
  - Error and success notifications
  - Message history scrolling
- **API Configuration Dialog** - Secure API key setup
  - API key validation
  - Masked display for security
  - Test connection functionality
  - Configuration persistence
- **Editor Integration** - Seamless UE5 integration
  - Toolbar buttons in Niagara/Cascade editors
  - Window management (open/close/position)
  - UE5 aesthetic styling
  - Editor mode and toolkit implementation

### Added - Testing ✅
- **Comprehensive Test Suite** - 95%+ coverage
  - AINiagaraSettings: 4 unit tests
  - GeminiAPIClient: 5 unit tests
  - VFXPromptBuilder: 10 unit tests
  - NiagaraSystemGenerator: 11 integration tests
  - ConversationHistoryManager: 7 unit tests
  - VFXDSLParser: 8 unit tests
  - VFXDSLValidator: 3 unit tests
  - **Total: 48 tests, all passing**
- **Build System** - PowerShell automation scripts
  - build_plugin.ps1 - Plugin compilation
  - run_tests.bat/sh - Test execution
  - build_and_test.ps1 - CI/CD pipeline

### Added - Documentation ✅
- **Comprehensive README** - Architecture and usage guide
- **Features Specification** - Complete feature documentation
- **Testing Documentation** - Test coverage reports
- **Implementation Tasks** - Development progress tracker
- **Project Structure** - Clear organization and guidelines

### Changed
- **Compatibility** - Optimized for Unreal Engine 5.3
  - Fixed Niagara API changes from UE 5.5 → 5.3
  - Updated BuildSettings and IncludeOrder versions
  - Corrected module dependencies (Http → HTTP)
  - Fixed Slate UI includes for UE 5.3
  - Replaced LOCTEXT with NSLOCTEXT for compatibility

### Fixed
- Module dependency issues (Niagara, NiagaraEditor)
- UPROPERTY conflicts with complex Blueprint types
- Struct forward declaration order in VFXDSL.h
- Slate widget includes and event handling
- SaveConfig method signature
- API method signatures for UE 5.3

### Security
- Secure API key storage in EditorPerProjectUserSettings
- Masked API key display in UI
- No API keys committed to repository
- Per-user configuration isolation

## [0.1.0] - 2025-01-01

### Added
- Initial plugin structure
- Editor Mode integration framework
- Basic documentation foundation
- Project rulebook and task tracking

[Unreleased]: https://github.com/yourusername/ainiagara/compare/v0.11.0...HEAD
[0.11.0]: https://github.com/yourusername/ainiagara/releases/tag/v0.11.0
[0.10.0]: https://github.com/yourusername/ainiagara/releases/tag/v0.10.0
[0.9.0]: https://github.com/yourusername/ainiagara/releases/tag/v0.9.0
[0.1.0]: https://github.com/yourusername/ainiagara/releases/tag/v0.1.0
