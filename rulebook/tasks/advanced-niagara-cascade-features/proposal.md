# Proposal: Advanced Niagara and Cascade Features

## Why

This change is needed to extend the AINiagara plugin with advanced VFX capabilities that go beyond basic particle system generation. Current implementation covers fundamental features (spawn, initialization, update, render), but professional VFX work requires advanced modules like particle collision, GPU simulation, dynamic parameters, ribbon/mesh rendering, and complex force systems. Supporting these advanced features will enable artists to create production-quality effects entirely through AI prompts, eliminating the need for manual configuration of complex Niagara and Cascade modules.

Additionally, advanced features enable:
- **Professional-grade VFX**: Industry-standard effects with complex behaviors
- **Competitive advantage**: Differentiation from basic particle generators
- **Workflow acceleration**: Complex setups generated in seconds vs. hours
- **Creative exploration**: AI-powered experimentation with advanced techniques
- **Legacy support**: Complete Cascade feature parity for migration workflows

## What Changes

This task implements a comprehensive suite of advanced Niagara and Cascade features:

### 1. Advanced Collision System
- **Collision depth buffer**: Particles collide with scene geometry
- **Collision planes**: Custom collision surfaces
- **Collision events**: Spawn particles on collision
- **Bounce/friction**: Realistic physics response
- **Kill particles on collision**: Lifetime control

### 2. GPU Simulation (Niagara Only)
- **GPU particle simulation**: Massive particle counts (100k+)
- **GPU forces**: Gravity, wind, vortex on GPU
- **GPU collision**: Efficient collision detection
- **GPU spawning**: High-frequency spawn rates

### 3. Advanced Rendering
- **Ribbon rendering**: Trails and beams
- **Mesh rendering**: 3D mesh particles
- **Light rendering**: Dynamic lights from particles
- **Decal rendering**: Surface projection
- **Custom materials**: Advanced material parameters
- **Sub-UV animation**: Sprite sheet animation

### 4. Dynamic Parameters
- **Material dynamic parameters**: Runtime material control
- **Curve-based parameters**: Time-based curves
- **Random distribution**: Various distribution types
- **Parameter inheritance**: Emitter-to-particle data flow

### 5. Advanced Forces
- **Vortex force**: Swirling/tornado effects
- **Curl noise force**: Organic turbulence
- **Drag force**: Speed-dependent resistance
- **Attraction/repulsion**: Point/line attractors
- **Turbulence**: Perlin noise-based forces
- **Vector field**: 3D velocity field forces

### 6. Event System
- **Collision events**: Spawn on collision
- **Death events**: Spawn on particle death
- **Location events**: Spawn at specific locations
- **Timer events**: Periodic spawning
- **Distance events**: Proximity-based spawning

### 7. Particle Attributes
- **Custom attributes**: User-defined data
- **Sprite size/rotation**: Per-particle control
- **Camera offset**: Depth sorting/layering
- **Pivot offset**: Rotation point control
- **UV scale/offset**: Texture manipulation

### 8. Advanced Spawning
- **Skeletal mesh spawning**: Emit from bones/sockets
- **Static mesh spawning**: Emit from mesh surfaces
- **Spline spawning**: Emit along splines
- **Volume spawning**: Emit in 3D volumes
- **Burst lists**: Multiple timed bursts

## Impact

### Affected Specs
- `/rulebook/tasks/implement-ainiagara-plugin/specs/core/spec.md` - Core DSL schema
- `/docs/DSL_SCHEMA.md` - DSL documentation
- `/docs/FEATURES.md` - Feature documentation

### Affected Code
- `Source/AINiagara/Public/Core/VFXDSL.h` - DSL structures
- `Source/AINiagara/Private/Core/VFXDSLParser.cpp` - DSL parsing
- `Source/AINiagara/Private/Core/NiagaraSystemGenerator.cpp` - Niagara generation
- `Source/AINiagara/Private/Core/CascadeSystemGenerator.cpp` - Cascade generation
- `Source/AINiagara/Private/Core/VFXPromptBuilder.cpp` - Prompt engineering
- `Source/AINiagara/Tests/*` - Test coverage

### Breaking Change
**NO** - This is an additive change. All existing DSL structures remain compatible.

New fields are optional and will default to disabled/null if not specified.

### User Benefit
- **Professional VFX**: Create industry-standard effects with AI
- **Faster workflow**: Complex setups in seconds instead of hours
- **Creative freedom**: Experiment with advanced techniques effortlessly
- **Production ready**: Generate effects for shipping games
- **Learning tool**: AI explains and demonstrates advanced features
- **Complete feature set**: No need to manually configure advanced modules

## Implementation Priority

### High Priority (Phase 20.1-20.3)
1. **Advanced Collision** - Essential for realistic effects
2. **Advanced Rendering** - Ribbons, meshes, essential for variety
3. **Advanced Forces** - Vortex, turbulence, curl noise

### Medium Priority (Phase 20.4-20.6)
4. **Dynamic Parameters** - Material control, curves
5. **Event System** - Spawn on collision/death
6. **Particle Attributes** - Custom data, sprite control

### Lower Priority (Phase 20.7-20.9)
7. **GPU Simulation** - Niagara-only, performance optimization
8. **Advanced Spawning** - Mesh/skeletal/spline spawning
9. **Sub-UV Animation** - Sprite sheet animation

## Technical Considerations

### DSL Extensibility
- Use optional fields for backward compatibility
- Group related features in sub-structures
- Follow existing DSL patterns

### Niagara vs Cascade
- Some features are Niagara-only (GPU simulation)
- Some features are Cascade-only (legacy modules)
- Document feature availability per system

### Performance
- GPU features for high particle counts
- CPU features for complex logic
- Provide performance guidelines in prompts

### Testing Strategy
- Unit tests for each advanced feature
- Integration tests for feature combinations
- Performance tests for GPU simulation
- Round-trip DSL conversion validation

## Migration Path

No migration required - this is an additive change.

Existing DSL files and systems remain fully functional.

New features are opt-in through prompt requests or DSL modification.

