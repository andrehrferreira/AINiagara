# Advanced Niagara and Cascade Features - Technical Design

## Overview

This document outlines the technical architecture and implementation approach for adding advanced VFX features to the AINiagara plugin. The design prioritizes backward compatibility, extensibility, and maintaining the existing DSL-based generation workflow.

## Architecture Principles

### 1. Backward Compatibility
- **All new DSL fields are optional**
- Existing DSL files continue to work without modification
- Default values for new fields match pre-Phase 20 behavior
- No breaking changes to existing APIs

### 2. Extensibility
- Modular design allows adding features incrementally
- Each advanced feature is independent
- Features can be combined without conflicts
- Future additions follow the same patterns

### 3. Niagara vs Cascade Parity
- Document which features are system-specific
- Provide fallbacks where possible
- Warn users about incompatibilities
- Maintain feature matrix documentation

## DSL Schema Extensions

### Collision System

```cpp
USTRUCT(BlueprintType)
struct FVFXDSLAdvancedCollision
{
    GENERATED_BODY()

    // Depth buffer collision
    UPROPERTY(BlueprintReadWrite)
    bool bUseDepthBuffer = false;

    // Custom collision planes
    UPROPERTY(BlueprintReadWrite)
    TArray<FVFXDSLCollisionPlane> CollisionPlanes;

    // Collision events
    UPROPERTY(BlueprintReadWrite)
    FVFXDSLCollisionEvent CollisionEvent;

    // Physics response
    UPROPERTY(BlueprintReadWrite)
    float Bounce = 0.5f; // 0-1

    UPROPERTY(BlueprintReadWrite)
    float Friction = 0.1f; // 0-1

    UPROPERTY(BlueprintReadWrite)
    bool bKillOnCollision = false;
};

USTRUCT(BlueprintType)
struct FVFXDSLCollisionPlane
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FVector Position;

    UPROPERTY(BlueprintReadWrite)
    FVector Normal;

    UPROPERTY(BlueprintReadWrite)
    float Bounce = 0.5f;

    UPROPERTY(BlueprintReadWrite)
    float Friction = 0.1f;
};

USTRUCT(BlueprintType)
struct FVFXDSLCollisionEvent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    bool bEnabled = false;

    UPROPERTY(BlueprintReadWrite)
    int32 SpawnCount = 1;

    UPROPERTY(BlueprintReadWrite)
    float VelocityInheritance = 0.5f; // 0-1

    UPROPERTY(BlueprintReadWrite)
    FString EmitterName; // Name of emitter to spawn
};
```

### GPU Simulation

```cpp
USTRUCT(BlueprintType)
struct FVFXDSLGPUSimulation
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    bool bEnabled = false;

    UPROPERTY(BlueprintReadWrite)
    int32 MaxParticles = 100000;

    UPROPERTY(BlueprintReadWrite)
    bool bFixedBounds = false;

    UPROPERTY(BlueprintReadWrite)
    FBox FixedBounds;
};
```

### Advanced Rendering

```cpp
UENUM(BlueprintType)
enum class EVFXRenderMode : uint8
{
    Sprite UMETA(DisplayName = "Sprite"),
    Ribbon UMETA(DisplayName = "Ribbon"),
    Mesh UMETA(DisplayName = "Mesh"),
    Light UMETA(DisplayName = "Light"),
    Decal UMETA(DisplayName = "Decal")
};

USTRUCT(BlueprintType)
struct FVFXDSLRibbonRender
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    float Width = 10.0f;

    UPROPERTY(BlueprintReadWrite)
    float WidthTaper = 1.0f; // End width multiplier

    UPROPERTY(BlueprintReadWrite)
    int32 MaxRibbonCount = 10;

    UPROPERTY(BlueprintReadWrite)
    float UVScale = 1.0f;

    UPROPERTY(BlueprintReadWrite)
    float UVTiling = 1.0f;
};

USTRUCT(BlueprintType)
struct FVFXDSLMeshRender
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FString MeshPath; // Asset path to static mesh

    UPROPERTY(BlueprintReadWrite)
    FVector MeshScale = FVector(1.0f);

    UPROPERTY(BlueprintReadWrite)
    bool bScaleByParticleSize = true;

    UPROPERTY(BlueprintReadWrite)
    FRotator MeshRotation = FRotator::ZeroRotator;

    UPROPERTY(BlueprintReadWrite)
    bool bRotateWithParticle = true;
};

USTRUCT(BlueprintType)
struct FVFXDSLLightRender
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    float LightRadius = 100.0f;

    UPROPERTY(BlueprintReadWrite)
    float LightIntensity = 1000.0f;

    UPROPERTY(BlueprintReadWrite)
    bool bUseParticleColor = true;

    UPROPERTY(BlueprintReadWrite)
    FLinearColor LightColor = FLinearColor::White;

    UPROPERTY(BlueprintReadWrite)
    bool bCastShadows = false;
};
```

### Advanced Forces

```cpp
USTRUCT(BlueprintType)
struct FVFXDSLVortexForce
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FVector Axis = FVector(0, 0, 1);

    UPROPERTY(BlueprintReadWrite)
    float Strength = 1000.0f;

    UPROPERTY(BlueprintReadWrite)
    float Radius = 500.0f;

    UPROPERTY(BlueprintReadWrite)
    float CoreRadius = 50.0f;
};

USTRUCT(BlueprintType)
struct FVFXDSLCurlNoiseForce
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    float Frequency = 1.0f;

    UPROPERTY(BlueprintReadWrite)
    float Strength = 100.0f;

    UPROPERTY(BlueprintReadWrite)
    int32 NoiseQuality = 3; // 1-5
};

USTRUCT(BlueprintType)
struct FVFXDSLTurbulence
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    float NoiseScale = 100.0f;

    UPROPERTY(BlueprintReadWrite)
    float Strength = 50.0f;

    UPROPERTY(BlueprintReadWrite)
    float Roughness = 1.0f;

    UPROPERTY(BlueprintReadWrite)
    int32 Levels = 3;
};
```

### Updated FVFXDSLRender

```cpp
USTRUCT(BlueprintType)
struct FVFXDSLRender
{
    GENERATED_BODY()

    // Existing fields
    UPROPERTY(BlueprintReadWrite)
    FString Material;

    UPROPERTY(BlueprintReadWrite)
    FString Texture;

    UPROPERTY(BlueprintReadWrite)
    FString BlendMode = "Translucent";

    UPROPERTY(BlueprintReadWrite)
    FString Sort = "ViewDepth";

    // NEW: Render mode
    UPROPERTY(BlueprintReadWrite)
    EVFXRenderMode RenderMode = EVFXRenderMode::Sprite;

    // NEW: Advanced rendering options
    UPROPERTY(BlueprintReadWrite)
    FVFXDSLRibbonRender RibbonRender;

    UPROPERTY(BlueprintReadWrite)
    FVFXDSLMeshRender MeshRender;

    UPROPERTY(BlueprintReadWrite)
    FVFXDSLLightRender LightRender;

    // NEW: Sub-UV animation
    UPROPERTY(BlueprintReadWrite)
    FVFXDSLSubUV SubUV;
};
```

## Module Generation Strategy

### Niagara Module Mapping

| Feature | Niagara Module |
|---------|----------------|
| Depth buffer collision | `UNiagaraCollisionEventHandlerProperties` |
| Collision planes | `UNiagaraPlaneCollisionEventHandlerProperties` |
| Ribbon rendering | `UNiagaraRibbonRendererProperties` |
| Mesh rendering | `UNiagaraMeshRendererProperties` |
| Light rendering | `UNiagaraLightRendererProperties` |
| Vortex force | `UNiagaraVortexForce` |
| Curl noise | `UNiagaraCurlNoiseForce` |
| GPU simulation | `UNiagaraEmitterProperties::SimTarget = GPUComputeSim` |

### Cascade Module Mapping

| Feature | Cascade Module |
|---------|----------------|
| Collision | `UParticleModuleCollision` |
| Collision planes | `UParticleModuleCollisionPlanes` |
| Ribbon rendering | `UParticleModuleTypeDataRibbon` |
| Mesh rendering | `UParticleModuleTypeDataMesh` |
| Light rendering | `UParticleModuleLight` |
| Vortex force | `UParticleModuleAttractorParticle` (approximation) |
| Turbulence | `UParticleModuleTurbulence` |

## Implementation Phases

### Phase 1: Foundation (20.1-20.3)
**Priority: HIGH**
- Advanced collision system
- Advanced rendering (ribbon, mesh, light)
- Advanced forces (vortex, curl noise, turbulence)

**Rationale**: These are the most requested features and provide the biggest visual impact.

### Phase 2: Parameters & Events (20.4-20.5)
**Priority: MEDIUM**
- Dynamic parameters
- Event system (collision, death, location)

**Rationale**: Enable complex behaviors and material control.

### Phase 3: Attributes & GPU (20.6-20.7)
**Priority: MEDIUM-LOW**
- Particle attributes
- GPU simulation

**Rationale**: GPU simulation is Niagara-only and requires optimization testing.

### Phase 4: Advanced Spawning (20.8)
**Priority: LOW**
- Skeletal mesh spawning
- Static mesh spawning
- Spline spawning
- Volume spawning

**Rationale**: Nice-to-have features for specialized use cases.

## Error Handling Strategy

### Invalid Configurations
```cpp
// Example: Ribbon with invalid width
if (RibbonRender.Width <= 0.0f)
{
    OutError = "Ribbon width must be positive";
    RibbonRender.Width = 10.0f; // Use default
    // Continue generation with warning
}
```

### Feature Compatibility
```cpp
// Example: GPU simulation requested for Cascade
if (DSL.Effect.Type == EVFXEffectType::Cascade && DSL.GPUSimulation.bEnabled)
{
    OutError = "GPU simulation is Niagara-only. Cascade will use CPU simulation.";
    // Disable GPU simulation for Cascade
    // Continue generation
}
```

### Fallback Implementations
```cpp
// Example: Curl noise for Cascade (no direct equivalent)
if (DSL.Effect.Type == EVFXEffectType::Cascade && HasCurlNoiseForce)
{
    // Fallback to turbulence with similar parameters
    CascadeTurbulence.NoiseScale = CurlNoise.Frequency * 100.0f;
    CascadeTurbulence.Strength = CurlNoise.Strength;
    // Log warning about approximation
}
```

## Testing Strategy

### Unit Tests
- DSL parsing for each new structure
- Validation of parameter ranges
- Serialization/deserialization
- Default value verification

### Integration Tests
- Niagara generation for each feature
- Cascade generation for each feature
- Feature combinations
- Round-trip DSL conversion

### Performance Tests
- GPU simulation with 100k+ particles
- Collision with complex geometry
- System generation time with advanced features
- Memory usage profiling

## Prompt Engineering

### Feature Detection
```cpp
// Detect keywords in user prompts
if (Prompt.Contains("trail") || Prompt.Contains("beam"))
{
    // Suggest ribbon rendering
    SuggestedFeatures.Add("Ribbon rendering for trail effect");
}

if (Prompt.Contains("debris") || Prompt.Contains("chunks"))
{
    // Suggest mesh rendering
    SuggestedFeatures.Add("Mesh rendering for debris particles");
}

if (Prompt.Contains("glow") || Prompt.Contains("firefly"))
{
    // Suggest light rendering
    SuggestedFeatures.Add("Light rendering for glowing effect");
}
```

### Performance Guidance
```cpp
if (RequestedParticleCount > 50000)
{
    // Suggest GPU simulation
    PerformanceTips.Add("Use GPU simulation for high particle counts (100k+)");
    PerformanceTips.Add("Limit collision complexity for GPU particles");
}
```

## Migration & Compatibility

### Version Detection
```cpp
// Detect DSL version (if versioning is added)
if (DSL.Version < 2.0f)
{
    // Use legacy parsing
    // No advanced features
}
else
{
    // Use new parsing with advanced features
}
```

### Backward Compatibility Testing
- Load and parse all existing test DSL files
- Verify generation produces identical results
- Ensure no regressions in existing functionality

## Documentation Requirements

### DSL Schema
- Document all new structures
- Provide examples for each feature
- Create Niagara vs Cascade feature matrix

### User Guide
- Step-by-step guides for each advanced feature
- Common use cases and examples
- Troubleshooting section

### API Reference
- Document all new classes and methods
- Include code examples
- Document feature compatibility

## Risks & Mitigation

### Risk: Complexity Explosion
- **Mitigation**: Modular design, optional features, clear defaults

### Risk: Performance Issues
- **Mitigation**: GPU simulation support, performance testing, guidelines

### Risk: Niagara/Cascade Divergence
- **Mitigation**: Feature matrix, compatibility warnings, fallbacks

### Risk: Backward Compatibility Break
- **Mitigation**: Optional fields, default values, extensive testing

## Success Criteria

1. ✅ All Phase 20 tasks completed
2. ✅ 95%+ test coverage maintained
3. ✅ No regressions in existing functionality
4. ✅ All builds pass (Windows, Linux, Mac)
5. ✅ Documentation complete and up-to-date
6. ✅ Performance benchmarks meet targets
7. ✅ User guide examples validated
8. ✅ Task validated and archived

