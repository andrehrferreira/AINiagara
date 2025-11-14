# AINiagara

AI-powered Niagara and Cascade particle system generator for Unreal Engine 5.

## Overview

AINiagara is an Unreal Engine 5 plugin that leverages Google's Gemini API to generate Niagara and Cascade particle systems through natural language prompts. The plugin provides an intuitive chat interface integrated directly into the Niagara and Cascade editors, enabling artists and developers to create complex VFX without manual system configuration.

## Features

- 🤖 AI-powered VFX generation using natural language prompts
- 💬 Cursor-style chat interface integrated into Niagara/Cascade editors
- 🎨 Automatic texture generation using Gemini Imagen 3 (Nano Banana)
- 🔄 Cascade to Niagara conversion support
- 📝 DSL-based system specification and reverse engineering
- 💾 Per-asset conversation history preservation

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

AI: [Generates DSL and creates Niagara system with fire particles]
```

## Documentation

- [Features Specification](docs/FEATURES.md) - Complete feature documentation with requirements and scenarios

## Compatibility

- **Unreal Engine**: 5.0+ (all versions)
- **Platforms**: Windows, Mac, Linux (Editor only)

## License

See LICENSE file for details.

