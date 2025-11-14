# Proposal: Niagara Fluids Support

## Why

Niagara Fluids is a powerful GPU-accelerated fluid simulation system introduced in Unreal Engine 5.x that enables realistic water, smoke, fire, and gas simulations directly in the Niagara VFX system. Currently, the AINiagara plugin supports standard particle systems but lacks support for fluid simulation, which is essential for creating modern, realistic VFX like waterfalls, smoke plumes, fire explosions, and atmospheric effects. Adding Niagara Fluids support will enable artists to generate production-quality fluid simulations through AI prompts, dramatically expanding the plugin's capabilities and positioning it as a comprehensive VFX solution for modern game development and real-time applications.

Niagara Fluids provides:
- **GPU-accelerated simulation**: Real-time fluid dynamics with millions of particles
- **Grid-based simulation**: Voxel-based approach for accurate fluid behavior
- **Multiple fluid types**: Liquid, gas, smoke, fire with different behaviors
- **Physical accuracy**: Pressure, viscosity, temperature, buoyancy simulation
- **Interaction**: Collision with scene geometry and dynamic forces
- **Rendering flexibility**: Volume rendering, particle rendering, mesh surface reconstruction

Without Niagara Fluids support, users cannot create:
- Realistic water effects (waterfalls, rivers, ocean waves)
- Volumetric smoke and fog
- Fire with proper fluid dynamics
- Steam and gas effects
- Explosions with pressure waves
- Weather effects (rain accumulation, snow)

## What Changes

This task implements comprehensive support for Niagara Fluids simulation:

### 1. Fluid Simulation Core
- **Grid-based simulation**: Voxel grid configuration and resolution
- **Fluid types**: Liquid, gas, smoke, fire with appropriate physics
- **Solver parameters**: Time step, iterations, stability settings
- **Boundary conditions**: Open, closed, periodic boundaries
- **Grid dimensions**: Size, resolution, scale configuration

### 2. Fluid Properties
- **Physical properties**:
  - Density: Mass per unit volume
  - Viscosity: Resistance to flow (water vs honey)
  - Temperature: Heat simulation for fire/steam
  - Pressure: Force per unit area
  - Buoyancy: Upward force based on density
  - Surface tension: Liquid cohesion
- **Simulation parameters**:
  - Vorticity confinement: Swirl/turbulence preservation
  - Dissipation: Energy loss over time
  - Advection quality: Fluid movement accuracy

### 3. Fluid Sources & Sinks
- **Emitters**: Inject fluid into simulation
  - Volume emitters (box, sphere, cone)
  - Surface emitters (mesh surfaces)
  - Point emitters
  - Velocity-based injection
- **Drains**: Remove fluid from simulation
  - Volume drains
  - Surface drains
  - Conditional removal (temperature, density)

### 4. Forces & Interactions
- **External forces**:
  - Gravity/directional acceleration
  - Wind/directional flow
  - Vortex forces
  - Turbulence/noise forces
- **Collision**:
  - Static geometry collision
  - Dynamic object interaction
  - Collision response (bounce, stick, slide)
- **Particle coupling**: Convert particles to/from fluid

### 5. Rendering Options
- **Volume rendering**: Direct voxel visualization
  - Density-based opacity
  - Temperature-based color
  - Lighting/shadowing
- **Particle rendering**: Fluid grid to particles
  - Particle spawning from high-density areas
  - Particle advection by fluid velocity
  - Hybrid fluid-particle rendering
- **Mesh reconstruction**: Surface mesh from fluid
  - Marching cubes algorithm
  - Mesh smoothing
  - Material application

### 6. Performance Optimization
- **GPU acceleration**: CUDA/compute shader based
- **LOD system**: Distance-based quality reduction
- **Culling**: Frustum and occlusion culling
- **Adaptive resolution**: Dynamic grid resolution
- **Multi-threading**: CPU pre/post processing

### 7. Fluid Presets
- **Water**: Low viscosity, surface tension, realistic liquid
- **Smoke**: Low density, high dissipation, buoyant gas
- **Fire**: Temperature-driven, combustion, emissive
- **Steam**: Medium density, temperature decay
- **Thick smoke**: High density, low dissipation
- **Viscous liquid**: High viscosity (honey, lava, oil)

## Impact

### Affected Specs
- `/rulebook/tasks/implement-ainiagara-plugin/specs/core/spec.md` - Core DSL schema
- `/docs/DSL_SCHEMA.md` - DSL documentation
- `/docs/FEATURES.md` - Feature documentation

### Affected Code
- `Source/AINiagara/Public/Core/VFXDSL.h` - Add fluid simulation structures
- `Source/AINiagara/Private/Core/VFXDSLParser.cpp` - Parse fluid DSL
- `Source/AINiagara/Private/Core/NiagaraSystemGenerator.cpp` - Generate fluid systems
- `Source/AINiagara/Private/Core/VFXPromptBuilder.cpp` - Fluid prompts
- `Source/AINiagara/Tests/*` - Comprehensive fluid tests

### Breaking Change
**NO** - This is an additive change. All existing DSL structures remain compatible.

Fluid simulation is a new, optional system type. Existing particle systems are unaffected.

### User Benefit
- **Realistic fluids**: Create water, smoke, fire with proper physics
- **Production quality**: Industry-standard fluid simulation
- **AI-powered**: Generate complex fluid setups through prompts
- **Time savings**: Hours of manual setup reduced to seconds
- **Experimentation**: Try different fluid configurations instantly
- **Modern VFX**: Access to cutting-edge Niagara Fluids technology
- **Complete solution**: Particle + fluid systems in one plugin

## Implementation Priority

### High Priority (Phase 21.1-21.3)
1. **Fluid Simulation Core** - Grid setup, solver, basic physics
2. **Fluid Properties** - Density, viscosity, temperature, pressure
3. **Basic Rendering** - Volume rendering for visualization

### Medium Priority (Phase 21.4-21.6)
4. **Fluid Sources** - Emitters for injecting fluid
5. **Forces & Collision** - External forces and scene interaction
6. **Particle Rendering** - Hybrid fluid-particle rendering

### Lower Priority (Phase 21.7-21.9)
7. **Mesh Reconstruction** - Surface mesh generation
8. **Advanced Features** - LOD, optimization, presets
9. **Prompt Engineering** - Fluid-specific prompts and suggestions

## Technical Considerations

### Engine Requirements
- **Unreal Engine 5.0+**: Niagara Fluids introduced in UE5
- **GPU support**: Compute shaders (DirectX 12, Vulkan)
- **Memory**: Fluid grids can be memory-intensive (64³ to 256³ voxels)

### Niagara-Only Feature
- Fluids are **Niagara-exclusive** (no Cascade equivalent)
- DSL should clearly indicate Niagara requirement
- Parser should validate and reject for Cascade

### Performance Considerations
- **Grid resolution**: Higher = more accurate but slower (64³, 128³, 256³)
- **GPU memory**: Large grids require substantial VRAM
- **Simulation cost**: Real-time at 60 FPS requires optimization
- **Rendering cost**: Volume rendering can be expensive

### Testing Strategy
- Unit tests for DSL parsing and validation
- Integration tests for fluid system generation
- Visual regression tests for rendering
- Performance benchmarks (grid sizes, FPS)
- Memory usage profiling

## Migration Path

No migration required - this is an additive change.

Existing particle systems remain fully functional.

New fluid systems are created through:
1. Explicit user prompts ("create water simulation")
2. DSL with fluid simulation parameters
3. Separate effect type: `EVFXEffectType::NiagaraFluid`

## Example Usage

### User Prompt
```
"Create a realistic waterfall with splashing at the bottom"
```

### AI Response
```json
{
  "effect": {
    "type": "NiagaraFluid",
    "duration": 0,
    "looping": true
  },
  "fluidSimulation": {
    "fluidType": "Liquid",
    "gridSize": [128, 256, 128],
    "gridResolution": 128,
    "density": 1000.0,
    "viscosity": 0.01,
    "gravity": [0, 0, -980]
  },
  "fluidSources": [
    {
      "type": "Volume",
      "shape": "Box",
      "position": [0, 0, 200],
      "size": [50, 50, 10],
      "velocity": [0, 0, -500],
      "emissionRate": 1000
    }
  ],
  "rendering": {
    "type": "Particle",
    "particleSpawnThreshold": 0.5,
    "particleSize": [2, 5]
  }
}
```

## Success Criteria

1. ✅ Users can create fluid simulations through AI prompts
2. ✅ All fluid types supported (water, smoke, fire, steam)
3. ✅ Grid-based simulation working correctly
4. ✅ Fluid sources and forces functional
5. ✅ Volume and particle rendering options available
6. ✅ Performance meets targets (60 FPS with 128³ grid)
7. ✅ Comprehensive test coverage (95%+)
8. ✅ Documentation complete with examples
9. ✅ Backward compatibility maintained
10. ✅ Task validated and archived

