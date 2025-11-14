# API Reference

This document provides comprehensive API documentation for the AINiagara plugin's public classes and functions.

## Table of Contents

- [Core Classes](#core-classes)
  - [FGeminiAPIClient](#fgeminiapiclient)
  - [UNiagaraSystemGenerator](#uniagarasystemgenerator)
  - [UCascadeSystemGenerator](#ucascadesystemgenerator)
  - [UVFXDSLParser](#uvfxdslparser)
  - [UVFXDSLValidator](#uvfxdslvalidator)
  - [UVFXPromptBuilder](#uvfxpromptbuilder)
  - [UConversationHistoryManager](#uconversationhistorymanager)
  - [UAINiagaraSettings](#uainiagarasettings)
- [Data Structures](#data-structures)
  - [FVFXDSL](#fvfxdsl)
  - [FConversationMessage](#fconversationmessage)
  - [FVFXToolFunction](#fvfx toolfunction)
- [UI Classes](#ui-classes)
  - [SAINiagaraChatWidget](#sainiagarachatwidget)
  - [SAINiagaraAPIKeyDialog](#sainiagaraapikeydialog)

---

## Core Classes

### FGeminiAPIClient

Client class for making requests to the Google Gemini API.

#### Methods

##### `FGeminiAPIClient()`
Constructor. Initializes the API client.

##### `~FGeminiAPIClient()`
Destructor. Cleans up HTTP requests.

##### `void SetAPIKey(const FString& InAPIKey)`
Sets the API key for authentication.

**Parameters:**
- `InAPIKey`: The Gemini API key string

##### `void LoadAPIKeyFromSettings()`
Loads the API key from project settings (UAINiagaraSettings).

##### `FString GetAPIKey() const`
Gets the current API key (masked for security).

**Returns:** Masked API key string (e.g., "AIza***")

##### `void TestAPIKey(const FString& APIKey, FOnGeminiResponse OnSuccess, FOnGeminiError OnError)`
Tests if an API key is valid by making a simple request.

**Parameters:**
- `APIKey`: The API key to test
- `OnSuccess`: Delegate called on successful test
- `OnError`: Delegate called on error (ErrorCode, ErrorMessage)

##### `void SendChatCompletion(const FString& UserMessage, const TArray<FConversationMessage>& ConversationHistory, const TArray<FVFXToolFunction>& AvailableTools, FOnGeminiResponse OnSuccess, FOnGeminiError OnError)`
Sends a chat completion request to the Gemini API.

**Parameters:**
- `UserMessage`: The user's message/prompt
- `ConversationHistory`: Array of previous conversation messages
- `AvailableTools`: Array of available tool functions for function calling
- `OnSuccess`: Delegate called with the response text
- `OnError`: Delegate called on error (ErrorCode, ErrorMessage)

**Example:**
```cpp
FGeminiAPIClient APIClient;
APIClient.LoadAPIKeyFromSettings();

TArray<FConversationMessage> History;
History.Add(FConversationMessage(TEXT("user"), TEXT("Create a fire effect")));

APIClient.SendChatCompletion(
    TEXT("Make it more intense"),
    History,
    TArray<FVFXToolFunction>(),
    FOnGeminiResponse::CreateLambda([](const FString& Response) {
        UE_LOG(LogTemp, Log, TEXT("Response: %s"), *Response);
    }),
    FOnGeminiError::CreateLambda([](int32 Code, const FString& Error) {
        UE_LOG(LogTemp, Error, TEXT("Error %d: %s"), Code, *Error);
    })
);
```

---

### UNiagaraSystemGenerator

Static utility class for generating Niagara particle systems from DSL specifications.

#### Static Methods

##### `static bool CreateSystemFromDSL(const FVFXDSL& DSL, const FString& PackagePath, const FString& SystemName, UNiagaraSystem*& OutSystem, FString& OutError)`
Creates a complete Niagara system from a DSL specification.

**Parameters:**
- `DSL`: The VFX DSL specification
- `PackagePath`: Package path where to create the system (e.g., "/Game/VFX")
- `SystemName`: Name for the system asset
- `OutSystem`: Output parameter - the created system
- `OutError`: Output parameter - error message if creation failed

**Returns:** `true` if system was created successfully, `false` otherwise

**Example:**
```cpp
FVFXDSL DSL;
// ... populate DSL ...

UNiagaraSystem* System = nullptr;
FString Error;
if (UNiagaraSystemGenerator::CreateSystemFromDSL(DSL, TEXT("/Game/VFX"), TEXT("MyFireSystem"), System, Error))
{
    UE_LOG(LogTemp, Log, TEXT("System created: %s"), *System->GetPathName());
}
else
{
    UE_LOG(LogTemp, Error, TEXT("Failed: %s"), *Error);
}
```

##### `static bool CreateEmitterFromDSL(const FVFXDSLEmitter& EmitterDSL, const FString& PackagePath, const FString& EmitterName, UNiagaraEmitter*& OutEmitter, FString& OutError)`
Creates a Niagara emitter from an emitter DSL specification.

**Parameters:**
- `EmitterDSL`: The emitter DSL specification
- `PackagePath`: Package path where to create the emitter
- `EmitterName`: Name for the emitter asset
- `OutEmitter`: Output parameter - the created emitter
- `OutError`: Output parameter - error message if creation failed

**Returns:** `true` if emitter was created successfully

##### `static bool ConfigureSpawnModule(UNiagaraEmitter* Emitter, const FVFXDSLSpawners& SpawnersDSL, FString& OutError)`
Configures the spawn module of an emitter from DSL spawners specification.

**Parameters:**
- `Emitter`: The emitter to configure
- `SpawnersDSL`: The spawners DSL specification
- `OutError`: Output parameter - error message if configuration failed

**Returns:** `true` if configuration was successful

##### `static bool ConfigureInitializeModule(UNiagaraEmitter* Emitter, const FVFXDSLInitialization& InitDSL, FString& OutError)`
Configures the initialize particle module from DSL initialization specification.

**Parameters:**
- `Emitter`: The emitter to configure
- `InitDSL`: The initialization DSL specification
- `OutError`: Output parameter - error message if configuration failed

**Returns:** `true` if configuration was successful

##### `static bool ConfigureUpdateModule(UNiagaraEmitter* Emitter, const FVFXDSLUpdate& UpdateDSL, FString& OutError)`
Configures the update module from DSL update specification.

**Parameters:**
- `Emitter`: The emitter to configure
- `UpdateDSL`: The update DSL specification
- `OutError`: Output parameter - error message if configuration failed

**Returns:** `true` if configuration was successful

##### `static bool ConfigureRenderModule(UNiagaraEmitter* Emitter, const FVFXDSLRender& RenderDSL, FString& OutError)`
Configures the render module from DSL render specification.

**Parameters:**
- `Emitter`: The emitter to configure
- `RenderDSL`: The render DSL specification
- `OutError`: Output parameter - error message if configuration failed

**Returns:** `true` if configuration was successful

---

### UCascadeSystemGenerator

Static utility class for generating Cascade particle systems from DSL specifications.

#### Static Methods

##### `static bool CreateSystemFromDSL(const FVFXDSL& DSL, const FString& PackagePath, const FString& SystemName, UParticleSystem*& OutSystem, FString& OutError)`
Creates a complete Cascade particle system from a DSL specification.

**Parameters:**
- `DSL`: The VFX DSL specification
- `PackagePath`: Package path where to create the system
- `SystemName`: Name for the system asset
- `OutSystem`: Output parameter - the created Cascade system
- `OutError`: Output parameter - error message if creation failed

**Returns:** `true` if system was created successfully

**Note:** Similar methods exist for configuring Cascade modules (spawn, initialization, update, render) as in UNiagaraSystemGenerator.

---

### UVFXDSLParser

Static utility class for parsing DSL from JSON strings.

#### Static Methods

##### `static bool ParseFromJSON(const FString& JSONString, FVFXDSL& OutDSL, FString& OutError)`
Parses a DSL specification from a JSON string.

**Parameters:**
- `JSONString`: The JSON string containing the DSL
- `OutDSL`: Output parameter - the parsed DSL structure
- `OutError`: Output parameter - error message if parsing failed

**Returns:** `true` if parsing was successful

**Example:**
```cpp
FString JSON = TEXT(R"({
    "effect": {
        "type": "Niagara",
        "duration": 5.0,
        "looping": true
    },
    "emitters": [...]
})");

FVFXDSL DSL;
FString Error;
if (UVFXDSLParser::ParseFromJSON(JSON, DSL, Error))
{
    UE_LOG(LogTemp, Log, TEXT("DSL parsed successfully"));
}
else
{
    UE_LOG(LogTemp, Error, TEXT("Parse error: %s"), *Error);
}
```

##### `static FString SerializeToJSON(const FVFXDSL& DSL)`
Serializes a DSL specification to a JSON string.

**Parameters:**
- `DSL`: The DSL structure to serialize

**Returns:** JSON string representation of the DSL

---

### UVFXDSLValidator

Static utility class for validating DSL specifications.

#### Static Methods

##### `static FVFXDSLValidationResult Validate(const FVFXDSL& DSL)`
Validates a DSL specification against the schema.

**Parameters:**
- `DSL`: The DSL structure to validate

**Returns:** `FVFXDSLValidationResult` containing validation status and errors

**Example:**
```cpp
FVFXDSL DSL;
// ... populate DSL ...

FVFXDSLValidationResult Result = UVFXDSLValidator::Validate(DSL);
if (Result.bIsValid)
{
    UE_LOG(LogTemp, Log, TEXT("DSL is valid"));
}
else
{
    for (const FString& Error : Result.Errors)
    {
        UE_LOG(LogTemp, Error, TEXT("Validation error: %s"), *Error);
    }
}
```

---

### UVFXPromptBuilder

Static utility class for building prompts for the LLM.

#### Static Methods

##### `static FString BuildSystemPrompt()`
Builds the system prompt that defines the VFX expert persona.

**Returns:** The system prompt string

##### `static FString BuildUserPrompt(const FString& UserInput, const TArray<FConversationMessage>& ConversationHistory)`
Builds a user prompt with conversation context.

**Parameters:**
- `UserInput`: The user's current input
- `ConversationHistory`: Previous conversation messages

**Returns:** The formatted user prompt

##### `static FString GetDSLFormatSpecification()`
Gets the DSL format specification documentation.

**Returns:** String describing the DSL format

##### `static TArray<FVFXToolFunction> GetAvailableTools()`
Gets the list of available tool functions for function calling.

**Returns:** Array of tool function definitions

##### `static FString GetToolFunctionDocumentation()`
Gets documentation for all available tool functions.

**Returns:** String describing available tools

---

### UConversationHistoryManager

Singleton manager for conversation history per asset.

#### Static Methods

##### `static UConversationHistoryManager* Get()`
Gets the singleton instance of the conversation history manager.

**Returns:** Pointer to the manager instance

#### Instance Methods

##### `void AddMessage(const FString& AssetPath, const FString& Role, const FString& Content)`
Adds a message to the conversation history for a specific asset.

**Parameters:**
- `AssetPath`: Path to the asset (e.g., "/Game/VFX/MySystem")
- `Role`: Message role ("user", "assistant", or "system")
- `Content`: Message content

##### `TArray<FConversationMessage> GetHistory(const FString& AssetPath) const`
Gets the conversation history for a specific asset.

**Parameters:**
- `AssetPath`: Path to the asset

**Returns:** Array of conversation messages

##### `int32 GetHistoryCount(const FString& AssetPath) const`
Gets the number of messages in the history for an asset.

**Parameters:**
- `AssetPath`: Path to the asset

**Returns:** Number of messages

##### `bool HasHistory(const FString& AssetPath) const`
Checks if an asset has conversation history.

**Parameters:**
- `AssetPath`: Path to the asset

**Returns:** `true` if history exists

##### `void ClearHistory(const FString& AssetPath)`
Clears conversation history for an asset.

**Parameters:**
- `AssetPath`: Path to the asset

##### `void SaveHistory(const FString& AssetPath)`
Saves conversation history to persistent storage.

**Parameters:**
- `AssetPath`: Path to the asset

##### `void LoadHistory(const FString& AssetPath)`
Loads conversation history from persistent storage.

**Parameters:**
- `AssetPath`: Path to the asset

---

### UAINiagaraSettings

Project settings class for storing API key and configuration.

#### Static Methods

##### `static UAINiagaraSettings* Get()`
Gets the project settings instance.

**Returns:** Pointer to the settings instance

#### Instance Methods

##### `void SetGeminiAPIKey(const FString& InAPIKey, bool bSaveToConfig)`
Sets the Gemini API key.

**Parameters:**
- `InAPIKey`: The API key to store
- `bSaveToConfig`: Whether to save to config file

##### `FString GetGeminiAPIKey() const`
Gets the stored API key.

**Returns:** The API key string

##### `bool IsAPIKeyConfigured() const`
Checks if an API key is configured.

**Returns:** `true` if API key is set

##### `FString GetMaskedAPIKey() const`
Gets a masked version of the API key for display.

**Returns:** Masked API key (e.g., "AIza***")

---

## Data Structures

### FVFXDSL

Main DSL structure containing effect specification.

**Properties:**
- `FVFXDSLEffect Effect`: Effect configuration (type, duration, looping)
- `TArray<FVFXDSLEmitter> Emitters`: Array of emitter specifications

### FVFXDSLEmitter

Emitter specification structure.

**Properties:**
- `FString Name`: Emitter name
- `FVFXDSLSpawners Spawners`: Spawn configuration
- `FVFXDSLInitialization Initialization`: Initialization configuration
- `FVFXDSLUpdate Update`: Update configuration
- `FVFXDSLRender Render`: Render configuration

### FConversationMessage

Message structure for conversation history.

**Properties:**
- `FString Role`: Message role ("user", "assistant", "system")
- `FString Content`: Message content
- `FDateTime Timestamp`: Message timestamp

### FVFXToolFunction

Tool function definition for LLM function calling.

**Properties:**
- `FString Name`: Tool function name (e.g., "tool:texture")
- `FString Description`: Tool function description
- `TMap<FString, FString> Parameters`: Parameter schema

### FVFXDSLValidationResult

Validation result structure.

**Properties:**
- `bool bIsValid`: Whether the DSL is valid
- `TArray<FString> Errors`: Array of validation error messages

---

## UI Classes

### SAINiagaraChatWidget

Slate widget for the AI chat interface.

#### Construction

```cpp
TSharedRef<SAINiagaraChatWidget> Widget = SNew(SAINiagaraChatWidget)
    .AssetPath(TEXT("/Game/VFX/MySystem"));
```

**SLATE Arguments:**
- `AssetPath`: Optional asset path for conversation history

#### Features

- Message history display
- Input field for user messages
- Loading indicators
- Export/Import DSL buttons
- Regenerate from DSL button

### SAINiagaraAPIKeyDialog

Slate widget for API key configuration dialog.

#### Construction

```cpp
TSharedRef<SAINiagaraAPIKeyDialog> Dialog = SNew(SAINiagaraAPIKeyDialog)
    .OnAPIKeyConfigured(FSimpleDelegate::CreateLambda([]() {
        // Handle API key configured
    }));
```

**SLATE Arguments:**
- `OnAPIKeyConfigured`: Delegate called when API key is successfully configured

#### Features

- API key input field (masked)
- Test API key button
- Save/Cancel buttons
- Status message display

---

## Delegates

### FOnGeminiResponse

Delegate for successful Gemini API responses.

```cpp
DECLARE_DELEGATE_OneParam(FOnGeminiResponse, const FString& ResponseText);
```

**Parameters:**
- `ResponseText`: The response text from the API

### FOnGeminiError

Delegate for Gemini API errors.

```cpp
DECLARE_DELEGATE_TwoParams(FOnGeminiError, int32 ErrorCode, const FString& ErrorMessage);
```

**Parameters:**
- `ErrorCode`: HTTP error code
- `ErrorMessage`: Error message string

---

## Enumerations

### EVFXEffectType

Effect type enumeration.

- `Niagara`: Niagara particle system
- `Cascade`: Cascade particle system

---

## Notes

- All classes are in the `AINiagara` namespace/module
- Most generator classes use static methods for stateless operations
- Error handling is done through output parameters (`OutError`) and return values
- Delegate-based async operations are used for API calls
- All DSL structures are USTRUCTs and can be serialized/deserialized

