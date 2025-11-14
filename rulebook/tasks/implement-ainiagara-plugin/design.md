# Technical Design Document

## Architecture Overview

The AINiagara plugin follows a modular architecture with clear separation of concerns:

```
┌─────────────────────────────────────────────────────────────┐
│                    Unreal Editor Layer                      │
│  ┌──────────────┐  ┌──────────────┐                        │
│  │ Niagara      │  │ Cascade      │                        │
│  │ Editor       │  │ Editor       │                        │
│  └──────┬───────┘  └──────┬───────┘                        │
│         │                  │                                │
│         └────────┬─────────┘                                │
│                  │                                          │
│         ┌────────▼──────────┐                              │
│         │ Toolbar Button    │                              │
│         │ Integration       │                              │
│         └────────┬──────────┘                              │
└──────────────────┼──────────────────────────────────────────┘
                   │
┌──────────────────▼──────────────────────────────────────────┐
│                    UI Layer                                  │
│  ┌─────────────────────────────────────────────┐           │
│  │ Chat Interface Widget (Slate)               │           │
│  │  - Message Display                          │           │
│  │  - Input Field                              │           │
│  │  - Loading Indicators                       │           │
│  │  - Error/Success Notifications              │           │
│  └─────────────────────────────────────────────┘           │
│                                                             │
│  ┌─────────────────────────────────────────────┐           │
│  │ API Configuration Dialog (Slate)            │           │
│  │  - API Key Input                            │           │
│  │  - Validation Feedback                      │           │
│  └─────────────────────────────────────────────┘           │
└──────────────────┬──────────────────────────────────────────┘
                   │
┌──────────────────▼──────────────────────────────────────────┐
│              Core Service Layer                             │
│  ┌──────────────────┐  ┌──────────────────┐               │
│  │ Gemini API       │  │ DSL Parser       │               │
│  │ Client           │  │ & Validator      │               │
│  └────────┬─────────┘  └────────┬─────────┘               │
│           │                      │                          │
│  ┌────────▼──────────────────────▼─────────┐              │
│  │ Prompt Builder                          │              │
│  │  - VFX Expert Persona                   │              │
│  │  - DSL Instructions                     │              │
│  │  - Tool Function Docs                   │              │
│  └────────┬────────────────────────────────┘              │
│           │                                                 │
│  ┌────────▼────────────────────────────────┐              │
│  │ Conversation History Manager            │              │
│  │  - Per-asset storage                    │              │
│  │  - Context preservation                 │              │
│  └─────────────────────────────────────────┘              │
└──────────────────┬──────────────────────────────────────────┘
                   │
┌──────────────────▼──────────────────────────────────────────┐
│           System Generation Layer                           │
│  ┌──────────────────┐  ┌──────────────────┐               │
│  │ Niagara System   │  │ Cascade System   │               │
│  │ Generator        │  │ Generator        │               │
│  │  - UNiagaraSystem│  │  - UParticleSys  │               │
│  │  - UNiagaraEmit  │  │  - Modules       │               │
│  └──────────────────┘  └──────────────────┘               │
│                                                             │
│  ┌──────────────────┐  ┌──────────────────┐               │
│  │ Texture Tool     │  │ Shader Tool      │               │
│  │ Generator        │  │ Generator        │               │
│  └──────────────────┘  └──────────────────┘               │
└─────────────────────────────────────────────────────────────┘
```

## Component Design

### 1. Gemini API Client

**Class**: `FGeminiAPIClient`

**Responsibilities**:
- HTTP communication with Gemini API
- Request/response handling
- Error handling and retry logic
- API key management

**Dependencies**:
- Unreal Engine `Http` module for HTTP requests
- Unreal Engine `Json` module for JSON parsing
- Unreal Engine `JsonUtilities` for JSON utilities

**Key Methods**:
```cpp
class FGeminiAPIClient
{
public:
    // Send chat completion request
    void SendChatCompletion(
        const FString& Prompt,
        const TArray<FMessage>& ConversationHistory,
        const TArray<FToolFunction>& AvailableTools,
        FOnResponseDelegate OnResponse,
        FOnErrorDelegate OnError
    );
    
    // Test API key validity
    bool TestAPIKey(const FString& APIKey);
    
private:
    FString APIKey;
    FString BaseURL = TEXT("https://generativelanguage.googleapis.com/v1beta");
};
```

**Implementation Notes**:
- Use `FHttpModule` for HTTP requests
- Use async delegates for non-blocking operations
- Implement exponential backoff for retries
- Store API key securely using Unreal's settings system (`GEditorPerProjectIni`)
- Use `FHttpRequest` with POST method for Gemini API
- Include API key in request headers: `x-goog-api-key: <API_KEY>`
- Request body in JSON format with `FJsonObject` serialization
- Response handling with `FHttpResponsePtr` and JSON parsing

### 2. DSL Parser and Validator

**Class**: `FVFXDSLParser`, `FVFXDSLValidator`

**Responsibilities**:
- Parse JSON DSL from LLM response
- Validate DSL structure and values
- Generate detailed error messages
- Convert DSL to internal data structures

**DSL Structure**:
```cpp
struct FVFXDSLEffect
{
    FString Type;  // "Niagara" or "Cascade"
    float Duration;
    bool bLooping;
};

struct FVFXDSLEmitter
{
    FString Name;
    FDSLSpawners Spawners;
    FDSLInitialization Initialization;
    FDSLUpdate Update;
    FDSLRender Render;
};

struct FVFXDSL
{
    FVFXDSLEffect Effect;
    TArray<FVFXDSLEmitter> Emitters;
};
```

**Validation Rules**:
- Effect type must be "Niagara" or "Cascade"
- At least one emitter required
- All required emitter sections must be present
- Value ranges validated (e.g., color 0-1, size > 0)

### 3. Chat Interface Widget

**Class**: `SAINiagaraChatWidget` (Slate widget)

**Responsibilities**:
- Display message history
- Handle user input
- Show loading states
- Display errors and success messages

**Slate Widget Structure**:
```
SAINiagaraChatWidget
├── SVerticalBox (Main Container)
│   ├── SScrollBox (Message History)
│   │   └── SMessageList (Messages)
│   ├── SHorizontalBox (Input Area)
│   │   ├── SEditableTextBox (Input Field)
│   │   └── SButton (Send Button)
│   └── SProgressBar (Loading Indicator)
```

### 4. Conversation History Manager

**Class**: `FAINiagaraConversationHistoryManager`

**Responsibilities**:
- Store conversation history per asset
- Persist history across sessions
- Load history on asset open
- Maintain context for LLM prompts

**Storage Strategy**:
- Store history in `.uasset` metadata or separate JSON files
- Use asset path as key for history lookup
- Store messages with timestamps and role (user/assistant)

**Data Structure**:
```cpp
struct FConversationMessage
{
    FString Role;        // "user" or "assistant"
    FString Content;
    FDateTime Timestamp;
    TMap<FString, FString> Metadata;  // Tool calls, etc.
};

struct FConversationHistory
{
    FString AssetPath;
    TArray<FConversationMessage> Messages;
};
```

### 5. Prompt Builder

**Class**: `FVFXPromptBuilder`

**Responsibilities**:
- Construct structured prompts for Gemini API
- Include VFX expert persona instructions
- Add DSL format specifications
- Include tool function documentation
- Append conversation history context

**Prompt Template Structure**:
```
System Instructions:
- You are an expert VFX artist for Unreal Engine 5
- Generate DSL format output
- Use tool functions when needed

DSL Format Specification:
[Detailed DSL schema documentation]

Available Tools:
[Tool function documentation]

Conversation History:
[Previous messages]

User Request:
[Current user input]
```

### 6. Niagara System Generator

**Class**: `FNiagaraSystemGenerator`

**Responsibilities**:
- Create `UNiagaraSystem` from DSL
- Create `UNiagaraEmitter` instances
- Configure modules (Spawn, Initialize, Update, Render)
- Set module parameters from DSL values

**Module Mapping**:
- DSL Spawners → Spawn Module (Burst, Rate)
- DSL Initialization → Initialize Particle Module (Color, Size, Velocity)
- DSL Update → Update Module (Forces, Drag, Collision)
- DSL Render → Render Module (Material, Blend Mode, Sort)

### 7. Cascade System Generator

**Class**: `FCascadeSystemGenerator`

**Responsibilities**:
- Create Cascade particle system from DSL
- Configure Cascade modules
- Map DSL values to Cascade parameters

**Note**: Uses UParticleSystem and UParticleEmitter classes

### 8. Texture Generation Tool

**Class**: `FVFXTextureGenerator`

**Responsibilities**:
- Handle tool:texture function calls from LLM
- Call Gemini Imagen 3 API for texture generation
- Convert generated images to Unreal textures
- Create flipbook atlases from sequences

**Integration**:
- Use Gemini Imagen 3 (Nano Banana) API
- Generate textures in PNG format
- Convert to UTexture2D or UPaperSprite
- Update emitter materials with generated textures

### 9. Shader Generation Tool

**Class**: `FVFXShaderGenerator`

**Responsibilities**:
- Handle tool:shader function calls
- Generate HLSL shader code from specifications
- Create shader assets
- Handle tool:material calls to create materials

## Data Flow

### VFX Generation Flow

```
User Input
    ↓
Chat Widget
    ↓
Prompt Builder (adds context, instructions)
    ↓
Gemini API Client (HTTP request)
    ↓
LLM Response (DSL JSON)
    ↓
DSL Parser
    ↓
DSL Validator
    ├─ Valid → System Generator
    └─ Invalid → Error → Retry with correction
    ↓
Niagara/Cascade System Generator
    ├─ Create System
    ├─ Create Emitters
    ├─ Configure Modules
    └─ Apply Parameters
    ↓
Chat Widget (display success)
```

### Tool Function Flow

```
LLM Response (tool:texture call)
    ↓
Tool Function Handler
    ↓
Gemini Imagen 3 API
    ↓
Image Generation
    ↓
Texture Conversion
    ↓
Material Update
    ↓
Response to LLM (asset path)
```

## Dependencies

### Unreal Engine Modules

**Public Dependencies**:
- `Core`
- `CoreUObject`
- `Engine`

**Private Dependencies**:
- `Slate`
- `SlateCore`
- `EditorFramework`
- `EditorStyle`
- `UnrealEd`
- `LevelEditor`
- `InteractiveToolsFramework`
- `EditorInteractiveToolsFramework`
- `Http` - For API calls
- `Json` - For JSON parsing
- `JsonUtilities` - For JSON utilities
- `Niagara` - For Niagara system generation
- `NiagaraEditor` - For Niagara editor integration
- `CascadeToNiagaraConverter` - For conversion (if available)

### External APIs

- **Google Gemini API**: For AI chat completion
  - Endpoint: `https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent`
  
- **Google Imagen 3 (Nano Banana)**: For texture generation
  - Endpoint: `https://generativelanguage.googleapis.com/v1beta/models/imagen-3-generate-001:generateContent`

## Security Considerations

1. **API Key Storage**: Store API keys securely using Unreal's `GEditorPerProjectIni` or encrypted storage
2. **HTTPS Only**: Enforce HTTPS for all API communications
3. **Input Validation**: Validate all user inputs and API responses
4. **Error Messages**: Don't expose sensitive information in error messages

## Performance Considerations

1. **Async Operations**: All API calls must be async to avoid blocking the editor
2. **Caching**: Cache conversation history in memory for fast access
3. **Lazy Loading**: Load conversation history only when needed
4. **Texture Compression**: Compress generated textures appropriately

## Error Handling Strategy

1. **API Errors**: 
   - Network errors: Retry with exponential backoff
   - Rate limiting: Wait and retry
   - Invalid API key: Prompt user to reconfigure
   
2. **DSL Validation Errors**:
   - Parse error: Send to LLM with correction instructions
   - Validation error: Send specific error details to LLM
   - Retry limit: Show user-friendly error after max retries

3. **System Generation Errors**:
   - Module configuration errors: Log and show error message
   - Asset creation errors: Rollback and notify user

## Testing Strategy

1. **Unit Tests**: Test DSL parser, validator, and generators in isolation
2. **Integration Tests**: Test API client with mock responses
3. **UI Tests**: Test widget interactions and state management
4. **End-to-End Tests**: Test complete workflow from input to system generation

## Future Enhancements

1. Support for additional AI providers
2. Batch processing of multiple requests
3. Template system for common effects
4. Learning from user corrections
5. Enhanced texture generation options
6. Better 3D model integration

