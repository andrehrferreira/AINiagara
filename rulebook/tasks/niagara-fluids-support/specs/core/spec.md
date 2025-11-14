# Niagara Fluids Support Specification

## Purpose

This specification defines support for Niagara Fluids GPU-accelerated fluid simulation system within the AINiagara plugin, enabling realistic water, smoke, fire, and gas effects through AI-generated prompts. Niagara Fluids provides grid-based voxel simulation with physical properties (density, viscosity, temperature, pressure) and multiple rendering modes (volume, particle, mesh), requiring Unreal Engine 5.0+ and dedicated GPU compute capabilities for real-time performance.

## ADDED Requirements

### Requirement: Fluid Simulation Core
The system SHALL support Niagara Fluids grid-based simulation with configurable grid dimensions, solver parameters, and boundary conditions for realistic fluid dynamics.

#### Scenario: Grid Configuration
Given a DSL specification with Niagara Fluid simulation
When generating the fluid system
Then the system SHALL create a voxel grid with specified dimensions
And the grid resolution SHALL be configurable (64³ to 256³ voxels)
And boundary conditions SHALL be configured (open, closed, or periodic)

#### Scenario: Solver Parameters
Given fluid simulation with solver parameters defined
When the fluid simulation executes
Then the solver SHALL use configured timestep and iteration count
And the simulation SHALL maintain stability with configured parameters

#### Scenario: Niagara-Only Enforcement
Given a DSL with Niagara Fluid simulation
When the effect type is set to Cascade
Then the parser SHALL reject the DSL
And an error message SHALL indicate Niagara Fluids requires Niagara

### Requirement: Fluid Types
The system SHALL support multiple fluid types (Liquid, Gas, Smoke, Fire) with appropriate physical behaviors and rendering characteristics.

#### Scenario: Liquid Fluid Type
Given a fluid simulation configured as Liquid type
When the fluid is simulated
Then the fluid SHALL exhibit low viscosity behavior
And surface tension SHALL be applied
And density SHALL match water-like properties

#### Scenario: Smoke Fluid Type
Given a fluid simulation configured as Smoke type
When the fluid is simulated
Then the fluid SHALL exhibit low density gas behavior
And high dissipation SHALL reduce density over time
And buoyancy SHALL cause upward movement

#### Scenario: Fire Fluid Type
Given a fluid simulation configured as Fire type
When the fluid is simulated
Then temperature SHALL drive the simulation
And high-temperature regions SHALL emit light
And buoyancy SHALL be temperature-dependent

### Requirement: Fluid Physical Properties
The system SHALL support configuration of physical properties including density, viscosity, temperature, pressure, buoyancy, and surface tension.

#### Scenario: Density Configuration
Given a fluid with specified density
When the fluid is simulated
Then particle mass SHALL be based on density
And collision response SHALL account for density
And buoyancy SHALL be density-dependent

#### Scenario: Viscosity Configuration
Given a fluid with specified viscosity
When the fluid flows
Then resistance to flow SHALL match viscosity value
And low viscosity SHALL flow freely (water)
And high viscosity SHALL flow slowly (honey, lava)

#### Scenario: Temperature Simulation
Given a fluid with temperature enabled
When the fluid is simulated
Then temperature SHALL affect buoyancy (hot rises)
And temperature SHALL affect color (fire/emissive)
And temperature SHALL dissipate over time

### Requirement: Fluid Simulation Parameters
The system SHALL support simulation parameters including vorticity confinement, dissipation rate, advection quality, and time scale.

#### Scenario: Vorticity Confinement
Given a fluid with vorticity confinement enabled
When the fluid flows
Then small-scale turbulence SHALL be preserved
And swirling motion SHALL be enhanced
And vorticity strength SHALL match configured value

#### Scenario: Dissipation Rate
Given a fluid with dissipation rate configured
When the fluid is simulated
Then energy SHALL decrease over time
And dissipation rate SHALL control decay speed
And high dissipation SHALL cause rapid fading (smoke)

#### Scenario: Advection Quality
Given a fluid with advection quality setting
When fluid particles move through the grid
Then movement accuracy SHALL match quality level
And higher quality SHALL reduce numerical diffusion
And lower quality SHALL improve performance

### Requirement: Fluid Presets
The system SHALL provide fluid presets for common types (water, smoke, fire, steam, thick smoke, viscous liquid) with appropriate default properties.

#### Scenario: Water Preset
Given a user requests water simulation
When the water preset is applied
Then density SHALL be set to 1000 kg/m³
And viscosity SHALL be set to 0.001 Pa·s
And surface tension SHALL be enabled
And buoyancy SHALL be neutral

#### Scenario: Smoke Preset
Given a user requests smoke simulation
When the smoke preset is applied
Then density SHALL be low (100 kg/m³)
And dissipation SHALL be high
And buoyancy SHALL be positive (rises)
And temperature SHALL be neutral

#### Scenario: Fire Preset
Given a user requests fire simulation
When the fire preset is applied
Then temperature SHALL be high (1000+ K)
And emissive rendering SHALL be enabled
And buoyancy SHALL be temperature-driven
And combustion SHALL be simulated

### Requirement: Volume Rendering
The system SHALL support volume rendering with density-based opacity, temperature-based color, lighting, and quality settings.

#### Scenario: Density to Opacity Mapping
Given volume rendering with density mapping
When the fluid is rendered
Then high-density regions SHALL be more opaque
And low-density regions SHALL be transparent
And opacity SHALL scale linearly with density

#### Scenario: Temperature to Color Mapping
Given volume rendering with temperature mapping
When the fluid is rendered
Then high-temperature regions SHALL render as hot colors (red, orange)
And low-temperature regions SHALL render as cool colors (blue, black)
And color gradient SHALL be configurable

#### Scenario: Render Quality Settings
Given volume rendering with quality level configured
When the fluid is rendered
Then sampling density SHALL match quality level
And higher quality SHALL use more samples
And lower quality SHALL improve performance

### Requirement: Fluid Sources (Emitters)
The system SHALL support fluid sources for injecting fluid into the simulation with configurable shape, position, velocity, and emission rate.

#### Scenario: Volume Source
Given a fluid source configured as volume emitter
When the fluid is emitted
Then fluid SHALL spawn within the volume bounds
And emission rate SHALL control fluid density
And initial velocity SHALL be applied to spawned fluid

#### Scenario: Source Shapes
Given a fluid source with specified shape
When the fluid is emitted
Then box shape SHALL emit from rectangular volume
And sphere shape SHALL emit from spherical volume
And cone shape SHALL emit in cone direction
And cylinder shape SHALL emit from cylindrical volume

#### Scenario: Velocity Injection
Given a fluid source with velocity configured
When the fluid is emitted
Then spawned fluid SHALL have initial velocity
And velocity direction SHALL match source orientation
And velocity magnitude SHALL match configured value

### Requirement: Fluid Sinks (Drains)
The system SHALL support fluid sinks for removing fluid from simulation based on volume, surface, or conditional criteria.

#### Scenario: Volume Drain
Given a fluid sink configured as volume drain
When fluid enters the drain volume
Then fluid density SHALL decrease
And drain strength SHALL control removal rate
And fluid SHALL be removed from simulation

#### Scenario: Conditional Removal
Given a fluid sink with conditional removal
When fluid meets removal criteria (temperature, density)
Then matching fluid SHALL be removed
And other fluid SHALL remain
And removal rate SHALL be configurable

### Requirement: External Forces
The system SHALL support external forces including gravity, wind, vortex, and turbulence to affect fluid motion.

#### Scenario: Gravity Force
Given a fluid simulation with gravity configured
When the fluid is simulated
Then fluid SHALL accelerate in gravity direction
And gravity strength SHALL match configured value
And buoyancy SHALL oppose gravity

#### Scenario: Wind Force
Given a fluid with directional wind force
When the fluid is simulated
Then fluid SHALL be pushed in wind direction
And wind strength SHALL control force magnitude
And wind SHALL create directional flow

#### Scenario: Vortex Force
Given a fluid with vortex force configured
When the fluid is near vortex center
Then fluid SHALL spiral around vortex axis
And vortex strength SHALL control rotation speed
And vortex SHALL create swirling motion

#### Scenario: Turbulence Force
Given a fluid with turbulence noise
When the fluid is simulated
Then random turbulent motion SHALL be added
And turbulence frequency SHALL control noise scale
And turbulence strength SHALL control force magnitude

### Requirement: Fluid Collision
The system SHALL support collision with static and dynamic geometry with configurable collision response.

#### Scenario: Static Geometry Collision
Given a fluid simulation with collision enabled
When fluid encounters static geometry
Then fluid SHALL not penetrate geometry
And fluid SHALL respond at collision boundary
And collision normal SHALL affect fluid direction

#### Scenario: Collision Response
Given fluid collision with response parameters
When fluid collides with surface
Then bounce parameter SHALL control reflection
And friction SHALL reduce tangential velocity
And stick behavior SHALL prevent sliding

#### Scenario: Dynamic Object Collision
Given a fluid with dynamic object collision
When moving objects interact with fluid
Then fluid SHALL react to object motion
And objects SHALL push/displace fluid
And two-way coupling SHALL be supported

### Requirement: Particle Rendering Mode
The system SHALL support particle rendering mode where high-density fluid regions spawn particles for visualization.

#### Scenario: Density-Based Particle Spawning
Given particle rendering with spawn threshold
When fluid density exceeds threshold
Then particles SHALL spawn in high-density regions
And spawn rate SHALL be proportional to density
And particle count SHALL be managed

#### Scenario: Fluid Advection of Particles
Given particles spawned from fluid
When the fluid flows
Then particles SHALL be advected by fluid velocity
And advection strength SHALL be configurable
And particles SHALL follow fluid motion

#### Scenario: Particle Appearance
Given particles rendering fluid
When particles are visible
Then particle size SHALL be configurable
And particle color SHALL derive from fluid temperature
And particle lifetime SHALL be configurable

### Requirement: Mesh Reconstruction
The system SHALL support mesh reconstruction from fluid simulation using marching cubes algorithm with smoothing and material configuration.

#### Scenario: Marching Cubes Generation
Given mesh reconstruction enabled
When the fluid is simulated
Then a surface mesh SHALL be generated
And iso-surface threshold SHALL determine surface boundary
And mesh SHALL update in real-time (or per-frame)

#### Scenario: Mesh Smoothing
Given mesh reconstruction with smoothing enabled
When the mesh is generated
Then Laplacian smoothing SHALL be applied
And smoothing iterations SHALL control quality
And normals SHALL be recalculated after smoothing

#### Scenario: Mesh Material
Given mesh reconstruction with material configured
When the mesh is rendered
Then the specified material SHALL be applied
And UVs SHALL be generated (if applicable)
And lighting SHALL affect the mesh

### Requirement: Performance Optimization
The system SHALL support performance optimization through LOD, culling, adaptive resolution, and GPU acceleration.

#### Scenario: Distance-Based LOD
Given a fluid with LOD system enabled
When the camera distance changes
Then grid resolution SHALL reduce with distance
And rendering quality SHALL reduce with distance
And LOD transitions SHALL be smooth

#### Scenario: Frustum Culling
Given a fluid with culling enabled
When the fluid is outside camera view
Then simulation SHALL be paused or reduced
And rendering SHALL be skipped
And resources SHALL be conserved

#### Scenario: Adaptive Resolution
Given a fluid with adaptive resolution enabled
When fluid density varies
Then high-density regions SHALL use higher resolution
And low-density regions SHALL use lower resolution
And resolution SHALL adapt dynamically

### Requirement: Prompt Engineering for Fluids
The system SHALL extend prompt engineering to recognize fluid-related requests and generate appropriate Niagara Fluids systems.

#### Scenario: Fluid Keyword Detection
Given a user prompt containing fluid keywords
When the AI processes the prompt
Then fluid simulation SHALL be suggested
And appropriate fluid type SHALL be selected
And rendering mode SHALL be recommended

#### Scenario: Water Effect Prompts
Given a user prompt requesting water effect
When the AI generates DSL
Then Liquid fluid type SHALL be selected
And water preset SHALL be applied
And appropriate rendering mode SHALL be chosen

#### Scenario: Smoke Effect Prompts
Given a user prompt requesting smoke effect
When the AI generates DSL
Then Smoke fluid type SHALL be selected
And smoke preset SHALL be applied
And volume rendering SHALL be selected

#### Scenario: Fire Effect Prompts
Given a user prompt requesting fire effect
When the AI generates DSL
Then Fire fluid type SHALL be selected
And fire preset SHALL be applied
And temperature-based rendering SHALL be enabled
And emissive properties SHALL be configured

#### Scenario: Performance Guidance
Given a user prompt with high quality requirements
When the AI responds
Then appropriate grid resolution SHALL be suggested
And performance warnings SHALL be provided
And optimization suggestions SHALL be included

### Requirement: Engine Version Enforcement
The system SHALL enforce Unreal Engine 5.0+ requirement for Niagara Fluids and provide clear error messages for incompatible versions.

#### Scenario: UE5 Requirement Check
Given a project using Unreal Engine 4.x
When attempting to generate Niagara Fluids system
Then generation SHALL fail with clear error message
And error SHALL indicate UE5+ requirement
And alternative particle system SHALL be suggested

#### Scenario: Niagara Fluids Plugin Check
Given a project without Niagara Fluids plugin enabled
When attempting to generate fluid system
Then generation SHALL fail with warning
And warning SHALL indicate plugin requirement
And instructions to enable plugin SHALL be provided

## MODIFIED Requirements

### Requirement: Effect Type Enum
The EVFXEffectType enum SHALL be extended to include NiagaraFluid type while maintaining existing Niagara and Cascade types.

#### Scenario: NiagaraFluid Type
Given a DSL with effect type set to NiagaraFluid
When parsing the DSL
Then the type SHALL be recognized as valid
And fluid-specific generation SHALL be triggered
And Niagara Fluids system SHALL be created

### Requirement: System Generation
The system generation SHALL be extended to support Niagara Fluids while maintaining existing Niagara particle and Cascade generation capabilities.

#### Scenario: Fluid System Generation
Given a DSL with NiagaraFluid effect type
When generating the system
Then UNiagaraFluidSystemGenerator SHALL be invoked
And a Niagara Fluids system SHALL be created
And existing particle system generation SHALL remain unchanged

## REMOVED Requirements

None. This is an additive change with no removals.

## RENAMED Requirements

None. No existing requirements are being renamed.

