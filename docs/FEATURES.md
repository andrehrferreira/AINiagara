# AINiagara Plugin - Features Specification

## Overview

AINiagara is an Unreal Engine 5 plugin that leverages AI (Gemini API) to generate Niagara and Cascade particle systems through natural language prompts. The plugin provides an intuitive chat interface integrated directly into the Niagara and Cascade editors, enabling artists and developers to create complex VFX without manual system configuration.

## ADDED Requirements

### Requirement: Editor Integration
The plugin MUST integrate exclusively with Niagara and Cascade editors, providing a button in the editor toolbar that opens the AI chat interface when clicked.

#### Scenario: Plugin button appears in Niagara editor
Given the plugin is installed and enabled
When the user opens a Niagara system in the editor
Then a button with the AINiagara icon MUST be visible in the editor toolbar

#### Scenario: Plugin button appears in Cascade editor
Given the plugin is installed and enabled
When the user opens a Cascade particle system in the editor
Then a button with the AINiagara icon MUST be visible in the editor toolbar

#### Scenario: Chat window opens on button click
Given the user is viewing a Niagara or Cascade system in the editor
When the user clicks the AINiagara toolbar button
Then the AI chat interface window MUST open

### Requirement: API Configuration
The plugin MUST prompt the user to configure the Gemini API key if it is not already configured, and MUST store the API key securely for future sessions.

#### Scenario: API key prompt on first use
Given the Gemini API key is not configured
When the user opens the chat interface for the first time
Then the plugin MUST display a prompt requesting the Gemini API key

#### Scenario: API key storage
Given the user provides a valid Gemini API key
When the API key is submitted
Then the plugin MUST store the key securely for future sessions

#### Scenario: Chat interface after configuration
Given the Gemini API key is configured
When the user opens the chat interface
Then the interface MUST display the Cursor-style chat UI styled to match Unreal Engine 5

### Requirement: Chat Interface
The plugin MUST provide a chat interface that accepts natural language prompts and preserves conversation history per asset.

#### Scenario: Natural language input
Given the chat interface is open
When the user enters a description of a desired VFX effect in plain English
Then the plugin MUST accept the input and process it

#### Scenario: Per-asset history preservation
Given the user has a conversation about a specific Niagara system
When the user closes and reopens the chat for that same system
Then the previous conversation history MUST be restored

#### Scenario: Independent history per asset
Given the user has conversations about multiple Niagara systems
When the user switches between different systems
Then each system MUST maintain its own independent chat history

### Requirement: AI Prompt Processing
The plugin MUST send structured prompts to the Gemini API with instructions for generating VFX in DSL format.

#### Scenario: Prompt structure sent to LLM
Given the user submits a VFX generation request
When the plugin processes the request
Then it MUST send a structured prompt to Gemini containing:
- Instructions to act as a VFX expert for Unreal Engine 5
- Instructions to generate DSL format output
- Specification of required DSL components (effect, emitters, spawners, initialization, update, render)
- Instructions for tool usage (texture, shader, material)
- Instructions for handling 3D model requirements

#### Scenario: DSL component specification
Given the prompt is sent to the LLM
When the LLM processes the request
Then the prompt MUST specify that the DSL MUST include:
- Effect type (Niagara or Cascade), duration, and looping status
- List of emitters
- Spawners with burst and rate parameters
- Initialization with color, size, and velocity
- Update with forces, drag, and collision
- Render with material, texture, blend mode, and sort

### Requirement: DSL Format Generation
The LLM MUST generate a valid DSL specification in the defined format containing all required components.

#### Scenario: Valid DSL structure
Given the LLM receives a valid VFX generation request
When the LLM generates a response
Then the DSL MUST contain a valid JSON structure with:
- Effect object with type, duration, and looping fields
- Emitters array with at least one emitter
- Each emitter containing spawners, initialization, update, and render sections

#### Scenario: DSL effect type validation
Given the LLM generates a DSL specification
When the DSL is parsed
Then the effect type MUST be either "Niagara" or "Cascade"

#### Scenario: DSL emitter completeness
Given the LLM generates a DSL specification
When the DSL is parsed
Then each emitter MUST contain all required sections: spawners, initialization, update, and render

### Requirement: DSL Validation
The plugin MUST validate the DSL format returned by the LLM and request corrections if validation fails.

#### Scenario: Successful DSL validation
Given the LLM returns a DSL specification
When the plugin validates the DSL
And the DSL matches the required format
Then the plugin MUST proceed with system creation

#### Scenario: Failed DSL validation
Given the LLM returns a DSL specification
When the plugin validates the DSL
And the DSL does not match the required format
Then the plugin MUST send the validation error back to the LLM with correction instructions

#### Scenario: LLM correction retry
Given the LLM receives a validation error
When the LLM generates a corrected DSL
Then the plugin MUST re-validate the corrected DSL

### Requirement: Niagara System Creation
The plugin MUST create UNiagaraSystem and UNiagaraEmitter instances from validated DSL specifications.

#### Scenario: System creation from DSL
Given a validated DSL specification with type "Niagara"
When the plugin processes the DSL
Then it MUST create a UNiagaraSystem instance

#### Scenario: Emitter creation from DSL
Given a validated DSL specification with multiple emitters
When the plugin processes the DSL
Then it MUST create a UNiagaraEmitter instance for each emitter in the DSL

#### Scenario: Module configuration from DSL
Given a validated DSL specification
When the plugin creates the Niagara system
Then it MUST configure standard modules (Spawn, Initialize Particle, Update, Render) with values from the DSL

### Requirement: Cascade System Creation
The plugin MUST create Cascade particle systems from validated DSL specifications when the DSL specifies Cascade as the effect type.

#### Scenario: Cascade system creation
Given a validated DSL specification with type "Cascade"
When the plugin processes the DSL
Then it MUST create a Cascade particle system instance

#### Scenario: Cascade emitter configuration
Given a validated DSL specification with Cascade type
When the plugin creates the Cascade system
Then it MUST configure Cascade emitters and modules with values from the DSL

### Requirement: Texture Generation Tool
The plugin MUST provide a texture generation tool that can be called by the LLM using tool:texture function.

#### Scenario: LLM requests texture generation
Given the LLM determines a custom texture is needed
When the LLM calls tool:texture with specifications
Then the plugin MUST call Gemini Imagen 3 (Nano Banana) API with detailed texture specifications

#### Scenario: Texture type determination
Given the LLM calls tool:texture
When the plugin processes the request
Then the AI MUST determine the texture type (noise, fire, smoke, sparks, distortion)

#### Scenario: Single texture generation
Given the LLM requests a single texture
When the texture generation completes
Then the plugin MUST create either a diffuse, opacity, or emissive texture

#### Scenario: Flipbook generation
Given the LLM requests a flipbook texture
When the texture generation completes
Then the plugin MUST generate an N-frame sequence and convert it to a flipbook atlas

#### Scenario: Flipbook atlas conversion
Given a PNG sequence is generated
When the plugin processes the sequence
Then a C++/Python utility MUST convert the sequence to a flipbook atlas
And create UTexture2D or UPaperSprite/NiagaraSprite
And update the generated emitter material

### Requirement: Shader Generation Tool
The plugin MUST provide a shader generation tool that can be called by the LLM using tool:shader and tool:material functions.

#### Scenario: LLM requests shader generation
Given the LLM determines a custom shader is needed
When the LLM calls tool:shader with specifications
Then the plugin MUST generate custom shader code based on the specifications

#### Scenario: Material creation from shader
Given a shader is generated
When the LLM calls tool:material
Then the plugin MUST create and configure a material using the generated shader

#### Scenario: Material integration
Given a material is created
When the particle system is generated
Then the plugin MUST integrate the material into the emitter render module

### Requirement: 3D Model Integration
The plugin MUST handle 3D model requirements by requesting user import and using simple meshes for most VFX.

#### Scenario: 3D model requirement detection
Given the LLM determines a 3D model is required
When processing the request
Then the plugin MUST request the user to import a static mesh and provide the location

#### Scenario: Simple mesh usage
Given a VFX request that can use simple meshes
When the plugin processes the request
Then the LLM MUST select from available simple meshes (billboards, cones, spheres) with scale and rotation

#### Scenario: Custom FBX handling
Given a VFX requires a custom FBX model
When the user provides the static mesh location
Then the plugin MUST use the mesh with LLM-determined scale and rotation
And the mesh MUST not be generated automatically

### Requirement: Cascade to Niagara Conversion
The plugin MUST use the official "Cascade To Niagara" plugin to convert Cascade systems to Niagara when requested by the user.

#### Scenario: Conversion request
Given the user requests conversion from Cascade to Niagara
When the conversion is initiated
Then the plugin MUST use the "Cascade To Niagara" plugin to perform the conversion

#### Scenario: System type detection
Given a particle system is open in the editor
When the user requests conversion
Then the plugin MUST detect the system type (Cascade or Niagara)
And handle the conversion workflow appropriately

### Requirement: Reverse Engineering DSL Export
The plugin MUST provide functionality to read existing Niagara/Cascade systems and generate DSL from them.

#### Scenario: Niagara system to DSL
Given an existing Niagara system is open
When the user requests DSL export
Then the plugin MUST read the system and generate a DSL specification

#### Scenario: Cascade system to DSL
Given an existing Cascade system is open
When the user requests DSL export
Then the plugin MUST read the system and generate a DSL specification

#### Scenario: DSL modification workflow
Given a DSL is exported from an existing system
When the user modifies the effect through the AI chat
Then the plugin MUST allow regenerating the system from the modified DSL

### Requirement: Real-time Preview System
The plugin MUST provide real-time preview of Niagara/Cascade systems in the editor viewport as the DSL is updated through the chat interface.

#### Scenario: Preview updates on DSL change
Given the preview is enabled in the chat interface
When the AI generates or updates a DSL specification
Then the preview system in the editor viewport MUST update automatically to reflect the new DSL

#### Scenario: Preview toggle control
Given the chat interface is open
When the user toggles preview on/off
Then the preview MUST be enabled or disabled accordingly
And the preview system MUST be cleaned up when disabled

#### Scenario: Preview system cleanup
Given a preview system is active
When the chat interface is closed or the user switches assets
Then the preview system MUST be cleaned up and removed from memory

#### Scenario: Preview performance
Given rapid DSL updates occur
When the preview system processes updates
Then updates MUST be throttled/debounced to prevent editor lag
And the preview MUST remain responsive

### Requirement: Chat History Management
The plugin MUST preserve chat history individually for each Niagara/Cascade asset across editor sessions.

#### Scenario: History persistence per asset
Given a conversation exists for a specific Niagara system
When the editor session ends and restarts
Then the conversation history for that system MUST be preserved and restored

#### Scenario: Independent history tracking
Given multiple Niagara/Cascade systems have conversations
When the user switches between systems
Then each system MUST maintain its own independent conversation history

#### Scenario: Full context preservation
Given a conversation includes multiple messages and tool calls
When the user returns to that asset
Then the complete conversation context MUST be available for continued interaction

### Requirement: Unreal Engine Compatibility
The plugin MUST be compatible with any Unreal Engine 5.x version without limitations.

#### Scenario: Version compatibility
Given the plugin is installed
When used with any Unreal Engine 5.x version
Then the plugin MUST function correctly without version-specific limitations

#### Scenario: Editor mode operation
Given the plugin is installed
When used in the Unreal Engine editor
Then the plugin MUST operate exclusively in editor mode
And MUST not require runtime components

### Requirement: Dependency Management
The plugin MUST require and work with the official "Cascade To Niagara" plugin for conversion functionality.

#### Scenario: Dependency check
Given the plugin is installed
When conversion functionality is accessed
Then the plugin MUST verify that "Cascade To Niagara" plugin is available
And display an error if the dependency is missing

## Technical Specifications

### DSL Schema

The DSL MUST follow this JSON structure:

```json
{
  "effect": {
    "type": "Niagara" | "Cascade",
    "duration": number,
    "looping": boolean
  },
  "emitters": [
    {
      "name": "string",
      "spawners": {
        "burst": {
          "count": number,
          "time": number,
          "intervals": number[]
        },
        "rate": {
          "spawnRate": number,
          "scaleOverTime": number
        }
      },
      "initialization": {
        "color": {
          "r": number,
          "g": number,
          "b": number,
          "a": number
        },
        "size": {
          "min": number,
          "max": number
        },
        "velocity": {
          "x": number,
          "y": number,
          "z": number
        }
      },
      "update": {
        "forces": {
          "gravity": number,
          "wind": { "x": number, "y": number, "z": number }
        },
        "drag": number,
        "collision": {
          "enabled": boolean,
          "bounce": number
        }
      },
      "render": {
        "material": "string",
        "texture": "string",
        "blendMode": "string",
        "sort": "string"
      }
    }
  ]
}
```

### Tool Function Specifications

#### tool:texture

**Input Requirements:**
- Texture type MUST be one of: noise, fire, smoke, sparks, distortion
- Resolution specifications MUST be provided
- Color scheme specifications MUST be provided
- Animation frame count MUST be provided for flipbook textures

**Output Requirements:**
- Generated texture asset path MUST be returned
- Material parameter binding MUST be created
- For flipbooks: PNG sequence MUST be converted to flipbook atlas
- UTexture2D or UPaperSprite/NiagaraSprite MUST be created

#### tool:shader

**Input Requirements:**
- Shader specifications MUST be provided
- Required functionality MUST be specified

**Output Requirements:**
- Shader asset path MUST be returned
- Shader MUST be integrated with material system

#### tool:material

**Input Requirements:**
- Material properties MUST be specified
- Shader references MUST be provided
- Texture bindings MUST be specified

**Output Requirements:**
- Material asset path MUST be returned
- Material MUST be integrated into the particle system render module

### User Interface Requirements

#### Chat Window Design

The chat interface MUST:
- Display message history in a scrollable view
- Provide an input field for user prompts
- Show loading indicators during AI processing
- Display error messages clearly
- Show success notifications when operations complete
- Use Cursor-style design language styled for Unreal Engine 5

#### Editor Integration

The plugin toolbar button MUST:
- Display the AINiagara icon
- Be visible only in Niagara and Cascade editors
- Not interfere with standard editor workflow
- Provide visual feedback when clicked

### Error Handling Requirements

#### Validation Error Handling

When DSL validation fails:
- Error details MUST be clearly communicated to the user
- Error information MUST be sent back to the LLM with correction instructions
- The plugin MUST allow retry after correction

#### API Error Handling

When API errors occur:
- Connection errors MUST display user-friendly messages
- API key validation errors MUST provide clear feedback
- Rate limiting MUST be handled gracefully with appropriate user notification
- The plugin MUST allow retry after resolving errors

### Limitations

#### Current Scope

The plugin:
- Operates ONLY in editor mode (no runtime functionality)
- REQUIRES internet connection for API calls
- DEPENDS on Gemini API availability
- REQUIRES manual import for custom FBX models
- IS LIMITED to particle system generation (no general VFX beyond particles)

#### Known Constraints

- Texture generation limited to specified types (noise, fire, smoke, sparks, distortion)
- Shader generation follows predefined patterns
- Complex mesh integration requires manual import steps
- 80% of gameplay VFX uses sprites/flipbooks and simple meshes (per design decision)
