# Niagara Fluids Support - Technical Design

## Overview

This document outlines the technical architecture for adding Niagara Fluids support to the AINiagara plugin. Niagara Fluids is a GPU-accelerated, grid-based fluid simulation system available in Unreal Engine 5.0+, enabling realistic water, smoke, fire, and gas effects through voxel-based physics simulation.

## Architecture Principles

### 1. UE5+ Exclusive
- **Niagara Fluids requires UE 5.0+**
- No Cascade equivalent
- Must enforce version check
- Clear error messages for UE4 projects

### 2. GPU-First Approach
- Leverage GPU compute shaders
- Optimize for real-time performance
- Target 60+ FPS with 128³ grid
- Profile and optimize GPU usage

### 3. Grid-Based Simulation
- Voxel grid structure
- Configurable resolution (64³ to 256³)
- Memory-intensive (plan for 500MB to 3GB VRAM)
- Adaptive resolution for optimization

## DSL Schema Extensions

### New Effect Type

```cpp
UENUM(BlueprintType)
enum class EVFXEffectType : uint8
{
    Niagara UMETA(DisplayName = "Niagara"),
    Cascade UMETA(DisplayName = "Cascade"),
    NiagaraFluid UMETA(DisplayName = "Niagara Fluid") // NEW
};
```

### Fluid Simulation Core

```cpp
UENUM(BlueprintType)
enum class EVFXFluidType : uint8
{
    Liquid UMETA(DisplayName = "Liquid"),
    Gas UMETA(DisplayName = "Gas"),
    Smoke UMETA(DisplayName = "Smoke"),
    Fire UMETA(DisplayName = "Fire"),
    Steam UMETA(DisplayName = "Steam")
};

USTRUCT(BlueprintType)
struct FVFXDSLFluidSimulation
{
    GENERATED_BODY()

    // Fluid type
    UPROPERTY(BlueprintReadWrite)
    EVFXFluidType FluidType = EVFXFluidType::Liquid;

    // Grid configuration
    UPROPERTY(BlueprintReadWrite)
    FIntVector GridSize = FIntVector(128, 128, 128); // Voxel count

    UPROPERTY(BlueprintReadWrite)
    FVector GridWorldSize = FVector(1000, 1000, 1000); // World units

    UPROPERTY(BlueprintReadWrite)
    FVector GridCenter = FVector::ZeroVector;

    // Solver parameters
    UPROPERTY(BlueprintReadWrite)
    float TimeStep = 0.016f; // 60 FPS

    UPROPERTY(BlueprintReadWrite)
    int32 SolverIterations = 10;

    // Boundary conditions
    UPROPERTY(BlueprintReadWrite)
    bool bOpenBoundary = true; // vs closed

    UPROPERTY(BlueprintReadWrite)
    bool bPeriodicBoundary = false;

    // Physical properties
    UPROPERTY(BlueprintReadWrite)
    FVFXDSLFluidProperties Properties;

    // Simulation parameters
    UPROPERTY(BlueprintReadWrite)
    FVFXDSLFluidSimulationParams SimulationParams;
};
```

### Fluid Physical Properties

```cpp
USTRUCT(BlueprintType)
struct FVFXDSLFluidProperties
{
    GENERATED_BODY()

    // Density (kg/m³)
    UPROPERTY(BlueprintReadWrite)
    float Density = 1000.0f; // Water

    // Viscosity (Pa·s)
    UPROPERTY(BlueprintReadWrite)
    float Viscosity = 0.001f; // Water

    // Temperature (Kelvin)
    UPROPERTY(BlueprintReadWrite)
    float Temperature = 293.0f; // 20°C

    // Pressure (Pa)
    UPROPERTY(BlueprintReadWrite)
    float Pressure = 101325.0f; // 1 atm

    // Buoyancy factor
    UPROPERTY(BlueprintReadWrite)
    float Buoyancy = 0.0f; // 0 = neutral, >0 = rises, <0 = sinks

    // Surface tension (N/m) - for liquids
    UPROPERTY(BlueprintReadWrite)
    float SurfaceTension = 0.072f; // Water
};
```

### Simulation Parameters

```cpp
USTRUCT(BlueprintType)
struct FVFXDSLFluidSimulationParams
{
    GENERATED_BODY()

    // Vorticity confinement (turbulence)
    UPROPERTY(BlueprintReadWrite)
    float VorticityConfinement = 0.5f;

    // Dissipation rate (0-1)
    UPROPERTY(BlueprintReadWrite)
    float DissipationRate = 0.01f;

    // Advection quality (1-5)
    UPROPERTY(BlueprintReadWrite)
    int32 AdvectionQuality = 3;

    // Diffusion coefficient
    UPROPERTY(BlueprintReadWrite)
    float Diffusion = 0.1f;

    // Time scale (simulation speed multiplier)
    UPROPERTY(BlueprintReadWrite)
    float TimeScale = 1.0f;
};
```

### Fluid Sources & Sinks

```cpp
UENUM(BlueprintType)
enum class EVFXFluidSourceType : uint8
{
    Volume UMETA(DisplayName = "Volume"),
    Surface UMETA(DisplayName = "Surface"),
    Point UMETA(DisplayName = "Point")
};

UENUM(BlueprintType)
enum class EVFXFluidSourceShape : uint8
{
    Box UMETA(DisplayName = "Box"),
    Sphere UMETA(DisplayName = "Sphere"),
    Cone UMETA(DisplayName = "Cone"),
    Cylinder UMETA(DisplayName = "Cylinder")
};

USTRUCT(BlueprintType)
struct FVFXDSLFluidSource
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    EVFXFluidSourceType Type = EVFXFluidSourceType::Volume;

    UPROPERTY(BlueprintReadWrite)
    EVFXFluidSourceShape Shape = EVFXFluidSourceShape::Box;

    UPROPERTY(BlueprintReadWrite)
    FVector Position = FVector::ZeroVector;

    UPROPERTY(BlueprintReadWrite)
    FVector Size = FVector(100, 100, 100);

    UPROPERTY(BlueprintReadWrite)
    FRotator Rotation = FRotator::ZeroRotator;

    UPROPERTY(BlueprintReadWrite)
    FVector Velocity = FVector::ZeroVector;

    UPROPERTY(BlueprintReadWrite)
    float EmissionRate = 1000.0f; // Density per second

    UPROPERTY(BlueprintReadWrite)
    float Temperature = 293.0f; // Initial temperature
};

USTRUCT(BlueprintType)
struct FVFXDSLFluidSink
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FVector Position = FVector::ZeroVector;

    UPROPERTY(BlueprintReadWrite)
    FVector Size = FVector(100, 100, 100);

    UPROPERTY(BlueprintReadWrite)
    float DrainStrength = 1.0f;

    // Conditional removal
    UPROPERTY(BlueprintReadWrite)
    bool bRemoveByTemperature = false;

    UPROPERTY(BlueprintReadWrite)
    float MinTemperature = 0.0f;

    UPROPERTY(BlueprintReadWrite)
    float MaxTemperature = 1000.0f;

    UPROPERTY(BlueprintReadWrite)
    bool bRemoveByDensity = false;

    UPROPERTY(BlueprintReadWrite)
    float MinDensity = 0.0f;

    UPROPERTY(BlueprintReadWrite)
    float MaxDensity = 10000.0f;
};
```

### Rendering Options

```cpp
UENUM(BlueprintType)
enum class EVFXFluidRenderMode : uint8
{
    Volume UMETA(DisplayName = "Volume"),
    Particle UMETA(DisplayName = "Particle"),
    Mesh UMETA(DisplayName = "Mesh"),
    Hybrid UMETA(DisplayName = "Hybrid (Volume + Particle)")
};

USTRUCT(BlueprintType)
struct FVFXDSLFluidRendering
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    EVFXFluidRenderMode RenderMode = EVFXFluidRenderMode::Volume;

    // Volume rendering
    UPROPERTY(BlueprintReadWrite)
    FVFXDSLFluidVolumeRender VolumeRender;

    // Particle rendering
    UPROPERTY(BlueprintReadWrite)
    FVFXDSLFluidParticleRender ParticleRender;

    // Mesh reconstruction
    UPROPERTY(BlueprintReadWrite)
    FVFXDSLFluidMeshRender MeshRender;
};

USTRUCT(BlueprintType)
struct FVFXDSLFluidVolumeRender
{
    GENERATED_BODY()

    // Density to opacity curve
    UPROPERTY(BlueprintReadWrite)
    float DensityMin = 0.0f;

    UPROPERTY(BlueprintReadWrite)
    float DensityMax = 1000.0f;

    UPROPERTY(BlueprintReadWrite)
    float OpacityScale = 1.0f;

    // Temperature to color gradient
    UPROPERTY(BlueprintReadWrite)
    bool bUseTemperatureColor = false;

    UPROPERTY(BlueprintReadWrite)
    FLinearColor ColdColor = FLinearColor::Blue;

    UPROPERTY(BlueprintReadWrite)
    FLinearColor HotColor = FLinearColor(1.0f, 0.3f, 0.0f, 1.0f);

    // Lighting
    UPROPERTY(BlueprintReadWrite)
    bool bReceiveLighting = true;

    UPROPERTY(BlueprintReadWrite)
    bool bCastShadows = false;

    // Quality
    UPROPERTY(BlueprintReadWrite)
    int32 RenderQuality = 2; // 0=low, 1=medium, 2=high, 3=ultra
};

USTRUCT(BlueprintType)
struct FVFXDSLFluidParticleRender
{
    GENERATED_BODY()

    // Spawn threshold (density)
    UPROPERTY(BlueprintReadWrite)
    float SpawnThreshold = 0.5f;

    // Particle properties
    UPROPERTY(BlueprintReadWrite)
    float ParticleSizeMin = 2.0f;

    UPROPERTY(BlueprintReadWrite)
    float ParticleSizeMax = 5.0f;

    UPROPERTY(BlueprintReadWrite)
    float ParticleLifetime = 5.0f;

    // Color from temperature
    UPROPERTY(BlueprintReadWrite)
    bool bColorFromTemperature = true;
};

USTRUCT(BlueprintType)
struct FVFXDSLFluidMeshRender
{
    GENERATED_BODY()

    // Marching cubes
    UPROPERTY(BlueprintReadWrite)
    float IsoSurface = 0.5f; // Density threshold

    UPROPERTY(BlueprintReadWrite)
    int32 GridSampling = 2; // Sampling frequency

    // Smoothing
    UPROPERTY(BlueprintReadWrite)
    bool bEnableSmoothing = true;

    UPROPERTY(BlueprintReadWrite)
    int32 SmoothingIterations = 3;

    // Material
    UPROPERTY(BlueprintReadWrite)
    FString MaterialPath;
};
```

### Updated FVFXDSL Structure

```cpp
USTRUCT(BlueprintType)
struct FVFXDSL
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FVFXDSLEffect Effect;

    UPROPERTY(BlueprintReadWrite)
    TArray<FVFXDSLEmitter> Emitters;

    // NEW: Fluid simulation (only for NiagaraFluid type)
    UPROPERTY(BlueprintReadWrite)
    FVFXDSLFluidSimulation FluidSimulation;

    UPROPERTY(BlueprintReadWrite)
    TArray<FVFXDSLFluidSource> FluidSources;

    UPROPERTY(BlueprintReadWrite)
    TArray<FVFXDSLFluidSink> FluidSinks;

    UPROPERTY(BlueprintReadWrite)
    FVFXDSLFluidForces FluidForces;

    UPROPERTY(BlueprintReadWrite)
    FVFXDSLFluidRendering FluidRendering;
};
```

## Niagara Fluids API Integration

### UNiagaraFluidSystemGenerator

```cpp
UCLASS()
class AINIAGARA_API UNiagaraFluidSystemGenerator : public UObject
{
    GENERATED_BODY()

public:
    /**
     * Create a Niagara Fluid system from DSL
     */
    static bool CreateFluidSystemFromDSL(
        const FVFXDSL& DSL,
        const FString& PackagePath,
        const FString& SystemName,
        UNiagaraSystem*& OutSystem,
        FString& OutError
    );

private:
    static bool ConfigureFluidGrid(UNiagaraSystem* System, const FVFXDSLFluidSimulation& FluidSim, FString& OutError);
    static bool ConfigureFluidProperties(UNiagaraSystem* System, const FVFXDSLFluidProperties& Properties, FString& OutError);
    static bool ConfigureFluidSources(UNiagaraSystem* System, const TArray<FVFXDSLFluidSource>& Sources, FString& OutError);
    static bool ConfigureFluidForces(UNiagaraSystem* System, const FVFXDSLFluidForces& Forces, FString& OutError);
    static bool ConfigureFluidRendering(UNiagaraSystem* System, const FVFXDSLFluidRendering& Rendering, FString& OutError);
};
```

## Fluid Presets

### Preset System

```cpp
struct FFluidPreset
{
    EVFXFluidType Type;
    FVFXDSLFluidProperties Properties;
    FVFXDSLFluidSimulationParams SimParams;
    EVFXFluidRenderMode RenderMode;
};

// Water preset
static const FFluidPreset WaterPreset = {
    EVFXFluidType::Liquid,
    {1000.0f, 0.001f, 293.0f, 101325.0f, 0.0f, 0.072f}, // Properties
    {0.3f, 0.01f, 3, 0.1f, 1.0f}, // Sim params
    EVFXFluidRenderMode::Hybrid
};

// Smoke preset
static const FFluidPreset SmokePreset = {
    EVFXFluidType::Smoke,
    {100.0f, 0.00001f, 293.0f, 101325.0f, 0.5f, 0.0f},
    {0.7f, 0.05f, 2, 0.2f, 1.0f},
    EVFXFluidRenderMode::Volume
};

// Fire preset
static const FFluidPreset FirePreset = {
    EVFXFluidType::Fire,
    {50.0f, 0.00001f, 1000.0f, 101325.0f, 1.0f, 0.0f},
    {0.5f, 0.02f, 3, 0.15f, 1.0f},
    EVFXFluidRenderMode::Volume
};
```

## Performance Considerations

### Grid Resolution Impact

| Grid Size | Voxels | Memory | Performance |
|-----------|--------|--------|-------------|
| 64³       | 262K   | ~100 MB | 120+ FPS    |
| 128³      | 2M     | ~500 MB | 60+ FPS     |
| 192³      | 7M     | ~1.5 GB | 30+ FPS     |
| 256³      | 16M    | ~3 GB   | 15+ FPS     |

### Optimization Strategies

1. **LOD System**
   - Reduce grid resolution with distance
   - Reduce rendering quality with distance
   - Pause simulation when off-screen

2. **Culling**
   - Frustum culling
   - Occlusion culling
   - Distance-based simulation disable

3. **Adaptive Resolution**
   - Higher resolution in high-density regions
   - Lower resolution in low-density regions
   - Dynamic grid refinement

## Prompt Engineering

### Keyword Detection

```cpp
// Water-related keywords
const TArray<FString> WaterKeywords = {
    TEXT("water"), TEXT("waterfall"), TEXT("river"), TEXT("ocean"),
    TEXT("wave"), TEXT("splash"), TEXT("liquid"), TEXT("fluid")
};

// Smoke-related keywords
const TArray<FString> SmokeKeywords = {
    TEXT("smoke"), TEXT("fog"), TEXT("mist"), TEXT("vapor"),
    TEXT("steam"), TEXT("gas"), TEXT("cloud")
};

// Fire-related keywords
const TArray<FString> FireKeywords = {
    TEXT("fire"), TEXT("flame"), TEXT("combustion"), TEXT("burn"),
    TEXT("explosion"), TEXT("blast"), TEXT("inferno")
};
```

### Grid Resolution Recommendation

```cpp
FIntVector RecommendGridResolution(const FString& Prompt)
{
    // High quality keywords
    if (Prompt.Contains(TEXT("realistic")) || 
        Prompt.Contains(TEXT("detailed")) ||
        Prompt.Contains(TEXT("high quality")))
    {
        return FIntVector(192, 192, 192); // High quality
    }
    // Performance keywords
    else if (Prompt.Contains(TEXT("fast")) || 
             Prompt.Contains(TEXT("performant")) ||
             Prompt.Contains(TEXT("mobile")))
    {
        return FIntVector(64, 64, 64); // Performance
    }
    // Default
    else
    {
        return FIntVector(128, 128, 128); // Balanced
    }
}
```

## Error Handling

### Version Check

```cpp
bool CheckUE5Requirement(FString& OutError)
{
    #if ENGINE_MAJOR_VERSION < 5
        OutError = TEXT("Niagara Fluids requires Unreal Engine 5.0 or higher. Your version: ") +
                   FString::Printf(TEXT("%d.%d"), ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION) +
                   TEXT(". Please upgrade to UE5+ or use standard Niagara particle systems.");
        return false;
    #endif
    return true;
}
```

### Plugin Check

```cpp
bool CheckNiagaraFluidsPlugin(FString& OutError)
{
    const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("NiagaraFluids"));
    if (!Plugin.IsValid() || !Plugin->IsEnabled())
    {
        OutError = TEXT("Niagara Fluids plugin is not enabled. Please enable it in: Edit → Plugins → Niagara → Niagara Fluids");
        return false;
    }
    return true;
}
```

## Testing Strategy

### Unit Tests
- DSL parsing for all fluid structures
- Preset validation
- Parameter range validation
- JSON serialization

### Integration Tests
- Fluid system generation
- Grid configuration
- Source/sink creation
- Force application
- Rendering mode configuration

### Performance Tests
- FPS benchmarks for different grid sizes
- Memory usage profiling
- GPU utilization monitoring
- Scalability tests

### Visual Tests
- Screenshot regression tests
- Render quality validation
- Preset appearance validation

## Migration & Compatibility

### Backward Compatibility
- All fluid fields are optional in FVFXDSL
- Existing particle systems unaffected
- Parse errors provide clear guidance
- Fallback to particles if UE4

### Version Detection
```cpp
if (DSL.Effect.Type == EVFXEffectType::NiagaraFluid)
{
    if (!CheckUE5Requirement(OutError))
    {
        // Suggest fallback
        OutError += TEXT("\n\nConsider using standard Niagara particle system instead.");
        return false;
    }
}
```

## Success Criteria

1. ✅ Fluid systems generate correctly from DSL
2. ✅ All fluid types working (water, smoke, fire)
3. ✅ Performance targets met (60 FPS @ 128³)
4. ✅ Memory usage within targets (<1GB @ 128³)
5. ✅ Rendering modes functional (volume, particle, mesh)
6. ✅ UE5+ requirement enforced
7. ✅ 95%+ test coverage
8. ✅ Documentation complete
9. ✅ Backward compatibility maintained
10. ✅ Task validated and archived

