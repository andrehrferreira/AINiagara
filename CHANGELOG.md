# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Planned
- Texture generation with Gemini Imagen 3 (Phase 10)
- Cascade system generation from DSL (Phase 9)
- Shader/Material generation tool (Phase 11)
- 3D Model integration (Phase 12)
- Cascade to Niagara conversion (Phase 13)
- Reverse engineering - System to DSL export (Phase 14)

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

[Unreleased]: https://github.com/yourusername/ainiagara/compare/v0.9.0...HEAD
[0.9.0]: https://github.com/yourusername/ainiagara/releases/tag/v0.9.0
[0.1.0]: https://github.com/yourusername/ainiagara/releases/tag/v0.1.0
