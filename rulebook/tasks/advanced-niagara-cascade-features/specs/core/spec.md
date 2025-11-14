# Advanced Niagara and Cascade Features Specification

## Purpose

This specification defines advanced VFX features for the AINiagara plugin, extending beyond basic particle system generation to include professional-grade capabilities such as collision systems, GPU simulation, advanced rendering (ribbons, meshes, lights), dynamic parameters, event systems, and complex force interactions. These features enable production-quality effects generation through AI prompts.

## ADDED Requirements

### Requirement: Advanced Collision System
The system SHALL support advanced particle collision with scene geometry, custom collision planes, collision events, and physically accurate bounce/friction responses.

#### Scenario: Depth Buffer Collision
Given a particle system with depth buffer collision enabled
When particles move through the scene
Then particles SHALL collide with visible geometry
And particles SHALL respond with configured bounce and friction values

#### Scenario: Collision Planes
Given a particle system with custom collision planes defined
When particles intersect collision plane boundaries
Then particles SHALL collide with the planes
And particles SHALL respond according to plane normal direction

#### Scenario: Collision Events
Given a particle system with collision event spawning configured
When a particle collides with a surface
Then a new particle SHALL spawn at the collision point
And the spawned particle SHALL inherit velocity based on configuration

#### Scenario: Kill On Collision
Given a particle system with kill on collision enabled
When a particle collides with any surface
Then the particle SHALL be immediately destroyed

### Requirement: GPU Simulation
The system SHALL support GPU-accelerated particle simulation for Niagara systems, enabling massive particle counts (100,000+) with efficient collision and force calculations.

#### Scenario: GPU Particle Creation
Given a DSL spec with GPU simulation enabled
When generating a Niagara system
Then the system SHALL create a GPU emitter
And the emitter SHALL support at least 100,000 particles

#### Scenario: GPU Forces
Given a GPU-simulated particle system with forces defined
When forces are applied (gravity, vortex, etc.)
Then forces SHALL execute on GPU for all particles
And force calculations SHALL maintain 60+ FPS with 100k particles

#### Scenario: GPU Collision
Given a GPU-simulated system with collision enabled
When particles move through the scene
Then collision detection SHALL execute on GPU
And collision SHALL maintain performance with high particle counts

### Requirement: Ribbon Rendering
The system SHALL support ribbon/trail rendering for creating beams, trails, and connected particle effects.

#### Scenario: Basic Ribbon Creation
Given a DSL spec with ribbon rendering configured
When generating the particle system
Then the system SHALL create a ribbon renderer
And ribbons SHALL connect particle positions over time

#### Scenario: Ribbon Width Configuration
Given a ribbon renderer with width/taper parameters
When particles spawn and move
Then ribbon width SHALL follow configured values
And width SHALL taper based on particle lifetime

#### Scenario: Ribbon UV Configuration
Given a ribbon with UV scale and tiling configured
When the ribbon is rendered
Then UVs SHALL be correctly mapped along the ribbon length
And textures SHALL tile according to configuration

### Requirement: Mesh Rendering
The system SHALL support rendering 3D meshes at particle positions for creating debris, foliage, and complex visual elements.

#### Scenario: Mesh Particle Creation
Given a DSL spec with mesh rendering and mesh asset reference
When generating the particle system
Then the system SHALL create a mesh renderer
And each particle SHALL render the specified mesh

#### Scenario: Mesh Scale and Rotation
Given a mesh renderer with scale and rotation parameters
When particles are rendered
Then meshes SHALL scale according to particle size
And meshes SHALL rotate according to particle rotation attributes

### Requirement: Light Rendering
The system SHALL support dynamic lights at particle positions for creating glowing effects, sparks, and fireflies.

#### Scenario: Light Particle Creation
Given a DSL spec with light rendering configured
When generating the particle system
Then the system SHALL create a light renderer
And each particle SHALL emit dynamic light

#### Scenario: Light Properties
Given a light renderer with color, intensity, and radius
When particles are rendered
Then lights SHALL use configured color and intensity
And light radius SHALL match configured values

### Requirement: Advanced Forces - Vortex
The system SHALL support vortex force for creating swirling, tornado, and whirlpool effects.

#### Scenario: Vortex Force Application
Given a particle system with vortex force configured
When particles enter the vortex radius
Then particles SHALL spiral around the vortex axis
And spiral strength SHALL match configured values

#### Scenario: Vortex Parameters
Given a vortex force with axis, strength, and radius
When the force is applied
Then particles SHALL rotate around the specified axis
And force SHALL decrease with distance from axis

### Requirement: Advanced Forces - Curl Noise
The system SHALL support curl noise force for creating organic, turbulent motion.

#### Scenario: Curl Noise Application
Given a particle system with curl noise force configured
When particles move through the force field
Then particles SHALL follow curl noise vector field
And motion SHALL appear organic and turbulent

#### Scenario: Curl Noise Parameters
Given curl noise with frequency and strength
When the force is applied
Then noise frequency SHALL control turbulence scale
And strength SHALL control force magnitude

### Requirement: Dynamic Material Parameters
The system SHALL support dynamic material parameters for runtime control of particle materials.

#### Scenario: Material Parameter Binding
Given a DSL spec with dynamic material parameters
When generating the particle system
Then the system SHALL create parameter bindings
And parameters SHALL be accessible to materials

#### Scenario: Curve-Based Parameters
Given a dynamic parameter with time-based curve
When particles age
Then parameter values SHALL follow the curve
And values SHALL interpolate correctly between keyframes

### Requirement: Event System - Collision Events
The system SHALL support spawning particles in response to collision events.

#### Scenario: Collision Event Spawning
Given a particle system with collision event configured
When a particle collides with a surface
Then new particles SHALL spawn at the collision point
And spawn count SHALL match configured value

#### Scenario: Velocity Inheritance
Given a collision event with velocity inheritance enabled
When particles spawn on collision
Then spawned particles SHALL inherit velocity from parent
And velocity SHALL be modified by configured factor

### Requirement: Event System - Death Events
The system SHALL support spawning particles when particles die.

#### Scenario: Death Event Spawning
Given a particle system with death event configured
When a particle reaches end of lifetime
Then new particles SHALL spawn at the death location
And spawn count SHALL match configured value

### Requirement: Particle Attributes - Custom Attributes
The system SHALL support custom user-defined particle attributes for advanced logic and material control.

#### Scenario: Custom Attribute Creation
Given a DSL spec with custom attributes defined
When generating a Niagara system
Then the system SHALL create custom attributes
And attributes SHALL have configured names and types

#### Scenario: Attribute Access in Materials
Given a particle system with custom attributes
When particles are rendered
Then materials SHALL have access to attribute values
And values SHALL update per-particle

### Requirement: Skeletal Mesh Spawning
The system SHALL support spawning particles from skeletal mesh bones and sockets.

#### Scenario: Bone Spawning
Given a DSL spec with skeletal mesh spawn from bones
When generating the particle system
Then particles SHALL spawn at bone locations
And bone selection SHALL match configured bones

#### Scenario: Socket Spawning
Given a skeletal mesh spawn from sockets
When particles spawn
Then particles SHALL spawn at socket locations
And socket selection SHALL match configuration

### Requirement: Static Mesh Surface Spawning
The system SHALL support spawning particles from static mesh surfaces.

#### Scenario: Surface Spawning
Given a DSL spec with static mesh surface spawning
When generating the particle system
Then particles SHALL spawn on mesh surface
And spawn distribution SHALL be uniform across surface

### Requirement: Spline Spawning
The system SHALL support spawning particles along spline paths.

#### Scenario: Spline Distribution
Given a DSL spec with spline spawning
When generating the particle system
Then particles SHALL spawn along the spline path
And distribution SHALL follow configured spacing

### Requirement: Volume Spawning
The system SHALL support spawning particles within 3D volumes (box, sphere, cylinder, cone).

#### Scenario: Box Volume
Given a volume spawn with box shape
When particles spawn
Then particles SHALL spawn within box bounds
And distribution SHALL be uniform within volume

#### Scenario: Sphere Volume
Given a volume spawn with sphere shape
When particles spawn
Then particles SHALL spawn within sphere radius
And distribution SHALL be uniform or shell-based

#### Scenario: Cylinder Volume
Given a volume spawn with cylinder shape
When particles spawn
Then particles SHALL spawn within cylinder volume
And height and radius SHALL match configuration

### Requirement: Sub-UV Animation
The system SHALL support sprite sheet animation via sub-UV frame selection.

#### Scenario: Frame Progression
Given a particle system with sub-UV animation
When particles age
Then frame index SHALL progress based on frame rate
And frames SHALL loop if configured

#### Scenario: Random Start Frame
Given sub-UV animation with random start frame enabled
When particles spawn
Then each particle SHALL start at random frame
And progression SHALL continue from that frame

### Requirement: Prompt Engineering for Advanced Features
The system SHALL extend prompt engineering to include examples and documentation for all advanced features.

#### Scenario: Advanced Feature Suggestions
Given a user prompt describing a complex effect
When the AI processes the prompt
Then the AI SHALL suggest appropriate advanced features
And the AI SHALL explain why each feature is recommended

#### Scenario: Performance Guidance
Given a user prompt requesting high particle counts
When the AI generates a response
Then the AI SHALL recommend GPU simulation
And the AI SHALL provide performance optimization tips

#### Scenario: Feature Compatibility
Given a user prompt mixing Niagara-only and Cascade features
When the AI generates a response
Then the AI SHALL warn about feature compatibility
And the AI SHALL suggest alternatives for Cascade

### Requirement: Backward Compatibility
The system SHALL maintain backward compatibility with existing DSL specifications.

#### Scenario: Legacy DSL Parsing
Given a DSL specification from v0.12.0 or earlier
When parsing the DSL
Then all existing fields SHALL parse correctly
And new advanced fields SHALL default to disabled/null

#### Scenario: Optional Advanced Fields
Given a DSL specification without advanced features
When generating a particle system
Then the system SHALL generate successfully
And behavior SHALL match pre-Phase 20 generation

## MODIFIED Requirements

### Requirement: DSL Schema
The DSL schema SHALL be extended to include advanced feature structures while maintaining backward compatibility with existing specifications.

#### Scenario: Extended Collision Structure
Given the existing FVFXDSLCollision structure
When adding advanced collision features
Then new fields SHALL be added as optional
And existing collision fields SHALL remain unchanged

#### Scenario: Extended Forces Structure
Given the existing FVFXDSLForces structure
When adding advanced force types
Then new force types SHALL be added to existing enum
And existing force types SHALL remain unchanged

### Requirement: System Generation
The system generation SHALL be extended to support advanced features while maintaining existing generation capabilities.

#### Scenario: Advanced Module Creation
Given a DSL with advanced features enabled
When generating a Niagara/Cascade system
Then advanced modules SHALL be created
And existing module creation SHALL remain functional

#### Scenario: Feature Fallback
Given a DSL with Niagara-only features for Cascade generation
When generating a Cascade system
Then the system SHALL use fallback implementations
Or the system SHALL log warnings about unsupported features

## REMOVED Requirements

None. This is an additive change with no removals.

## RENAMED Requirements

None. No existing requirements are being renamed.

