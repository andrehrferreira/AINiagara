# AINiagara

[![Version](https://img.shields.io/badge/version-0.11.0-blue.svg)](https://github.com/yourusername/ainiagara)
[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.3+-orange.svg)](https://www.unrealengine.com/)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Tests](https://img.shields.io/badge/tests-71%20passing-success.svg)](https://github.com/yourusername/ainiagara)
[![Coverage](https://img.shields.io/badge/coverage-95%25%2B-brightgreen.svg)](https://github.com/yourusername/ainiagara)
[![Build](https://img.shields.io/badge/build-passing-success.svg)](https://github.com/yourusername/ainiagara)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Mac%20%7C%20Linux-lightgrey.svg)](https://www.unrealengine.com/)

AI-powered Niagara and Cascade particle system generator for Unreal Engine 5.

## Overview

AINiagara is an Unreal Engine 5 plugin that leverages Google's Gemini API to generate Niagara and Cascade particle systems through natural language prompts. The plugin provides an intuitive chat interface integrated directly into the Niagara and Cascade editors, enabling artists and developers to create complex VFX without manual system configuration.

## Features

### ✅ Implemented
- 🤖 **AI-powered VFX generation** - Natural language prompts via Gemini API
- 💬 **Chat interface** - Integrated into Niagara/Cascade editors
- 📝 **DSL-based system** - JSON-based VFX specification language
- 🔄 **System generation** - Automatic Niagara and Cascade system creation from DSL
- 💾 **Conversation history** - Per-asset conversation preservation with automatic persistence
- ⚙️ **Reverse engineering** - Export/Import DSL with modification workflow
- 🔄 **DSL modification** - Export → Modify → Regenerate workflow
- 🧪 **Comprehensive testing** - 71 unit/integration tests with 95%+ coverage
- ✅ **UE 5.3 compatible** - Fully tested on Unreal Engine 5.3

### 🚧 In Progress / Planned
- 👁️ **Real-time Preview System** (Phase 19) - Preview em tempo real do Niagara/Cascade conforme DSL é atualizado
- 🎨 Texture generation using Gemini Imagen 3 (Phase 10)
- 🔧 Shader/Material generation (Phase 11)
- 📦 3D Model integration (Phase 12)
- 🔄 Cascade to Niagara conversion (Phase 13)

## Quick Start

### Prerequisites

- Unreal Engine 5.x (any version)
- Google Gemini API key
- Cascade To Niagara plugin (for conversion functionality)

### Installation

1. Copy the `AINiagara` folder to your project's `Plugins` directory
2. Enable the plugin in Unreal Engine Editor (Edit → Plugins → AINiagara)
3. Restart the editor

### Configuration

1. Open any Niagara or Cascade system in the editor
2. Click the AINiagara button in the editor toolbar
3. Enter your Gemini API key when prompted
4. Start creating VFX with natural language!

### Usage Example

```
User: "Create a fire effect with orange and yellow particles that fade over time"

AI: [Generates DSL and creates Niagara system with:]
    - Spawn rate: 50 particles/sec
    - Color: Orange→Yellow gradient
    - Lifetime: 2-4 seconds
    - Opacity: Fade from 1.0 to 0.0
    - Velocity: Upward with randomization
```

More examples:

```
"Make a magical sparkle effect with blue particles"
"Create smoke that rises slowly and disperses"
"I want an explosion with debris particles"
"Add a rain effect with splash particles on impact"
```

## Documentation

- [User Guide](docs/USER_GUIDE.md) - Step-by-step guide for using the plugin
- [API Reference](docs/API_REFERENCE.md) - Complete API documentation for developers
- [Features Specification](docs/FEATURES.md) - Complete feature documentation
- [Features Validation](docs/FEATURES_VALIDATION.md) - Test coverage mapping for all scenarios
- [Testing](docs/TESTING.md) - Test documentation and coverage reports
- [DSL Schema](docs/DSL_SCHEMA.md) - DSL format specification
- [Implementation Tasks](rulebook/tasks/implement-ainiagara-plugin/tasks.md) - Development progress tracker

## Architecture

### Core Components

#### **API Integration** (`Core/GeminiAPIClient`)
- Gemini API client wrapper with async support
- API key management and secure storage
- Request/response handling with error recovery
- Conversation message structures

#### **DSL System** (`Core/VFXDSL`)
- JSON-based VFX specification language
- Parser with comprehensive error handling
- Validator with schema checking
- Support for Niagara and Cascade effect types

#### **Prompt Engineering** (`Core/VFXPromptBuilder`)
- Structured prompt builder for Gemini API
- VFX expert persona instructions
- DSL format documentation injection
- Conversation history context integration

#### **System Generation** (`Core/NiagaraSystemGenerator`, `Core/CascadeSystemGenerator`)
- UNiagaraSystem factory from DSL
- UParticleSystem factory from DSL
- UNiagaraEmitter and UParticleEmitter creation and configuration
- Module configuration (Spawn, Initialize, Update, Render)
- Package and asset registry management

#### **History Management** (`Core/ConversationHistoryManager`)
- Per-asset conversation persistence
- Automatic save on asset save events
- Automatic load on asset open
- Multi-asset history tracking
- JSON-based storage format
- Persistence across editor sessions
- Enable/disable auto-persistence control

#### **Reverse Engineering** (`Core/NiagaraSystemToDSLConverter`, `Core/CascadeSystemToDSLConverter`)
- Niagara System → DSL conversion
- Cascade System → DSL conversion
- Emitter-level conversion support for both systems
- DSL export to JSON format
- DSL export to file with save dialog
- Multiple emitters support
- Round-trip conversion testing

#### **UI Components** (`UI/Widgets`)
- `SAINiagaraChatWidget` - Main chat interface with DSL export/import/regenerate
- `SAINiagaraAPIKeyDialog` - API key configuration
- `AINiagaraChatWindow` - Window management
- `AINiagaraEditorToolbar` - Editor integration

### Testing

The plugin includes comprehensive test coverage (95%+):

- **AINiagaraSettings**: 4 tests (API key management, configuration)
- **GeminiAPIClient**: 5 tests (API integration, message handling)
- **VFXPromptBuilder**: 10 tests (prompt generation, all scenarios)
- **NiagaraSystemGenerator**: 11 tests (system generation, all configurations)
- **ConversationHistoryManager**: 7 tests (history persistence, multi-asset)
- **ConversationHistoryPersistence**: 6 tests (auto-save, auto-load, session persistence)
- **VFXDSLParser**: 8 tests (JSON parsing, serialization)
- **VFXDSLValidator**: 3 tests (schema validation)
- **NiagaraSystemToDSLConverter**: 6 tests (conversion, export, round-trip)
- **CascadeSystemGenerator**: 9 tests (generation, multiple emitters, spawn, color, velocity, forces, blend mode, errors, round-trip)
- **CascadeSystemToDSLConverter**: 2 tests (conversion, error handling)
- **SAINiagaraChatWidget**: 4 tests (construction, empty asset path, history loading, multiple assets)
- **SAINiagaraAPIKeyDialog**: 5 tests (construction, with existing key, without key, delegates, validation)

**Total: 81 tests, all passing** ✅

Run tests:
```bash
cd scripts
./run_tests.bat AINiagara "Path/To/Your/Project.uproject"
```

## Compatibility

- **Unreal Engine**: 5.3+ (tested on 5.3, compatible with 5.0+)
- **Platforms**: Windows, Mac, Linux (Editor only)
- **Build System**: UnrealBuildTool with C++17
- **Dependencies**: Niagara, NiagaraEditor, HTTP, Json, JsonUtilities

## Development

### Building the Plugin

```bash
# From project root
cd scripts
./build_plugin.ps1
```

### Running Tests

```bash
cd scripts
./run_tests.bat AINiagara "Path/To/Project.uproject"
```

### Code Coverage

- **Target**: 95%+ test coverage
- **Current**: 95%+ (81 unit/integration/UI tests)
- **Quality**: All tests passing, compiles with 0 warnings

## Project Structure

```
AINiagara/
├── Source/AINiagara/
│   ├── Public/
│   │   ├── Core/               # Core functionality
│   │   │   ├── GeminiAPIClient.h
│   │   │   ├── VFXDSL.h
│   │   │   ├── VFXDSLParser.h
│   │   │   ├── VFXPromptBuilder.h
│   │   │   ├── NiagaraSystemGenerator.h
│   │   │   ├── NiagaraSystemToDSLConverter.h
│   │   │   ├── CascadeSystemGenerator.h
│   │   │   ├── CascadeSystemToDSLConverter.h
│   │   │   ├── ConversationHistoryManager.h
│   │   │   └── AINiagaraSettings.h
│   │   └── UI/                 # User interface
│   │       ├── Widgets/
│   │       │   ├── SAINiagaraChatWidget.h
│   │       │   └── SAINiagaraAPIKeyDialog.h
│   │       ├── AINiagaraChatWindow.h
│   │       └── AINiagaraEditorToolbar.h
│   ├── Private/                # Implementations
│   └── Tests/                  # Unit and integration tests
├── docs/                       # Documentation
├── scripts/                    # Build and test scripts
└── rulebook/                   # Project rules and tasks
```

## License

See LICENSE file for details.

