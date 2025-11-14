# VFX DSL Schema Documentation

## Overview

The VFX DSL (Domain-Specific Language) is a JSON-based specification format for describing particle effects in both Niagara and Cascade systems. This schema enables AI-powered generation of particle systems from natural language prompts.

## Schema Version

**Current Version**: 1.0.0  
**Compatible With**: Niagara, Cascade  
**Unreal Engine**: 5.0+

## Root Structure

```json
{
  "effect": { ... },
  "emitters": [ ... ]
}
```

## Effect Configuration

The `effect` object defines global system properties.

### Schema

```json
{
  "type": "Niagara" | "Cascade",
  "duration": <float>,
  "looping": <boolean>
}
```

### Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `type` | enum | `"Niagara"` | System type: `"Niagara"` or `"Cascade"` |
| `duration` | float | `5.0` | Effect duration in seconds |
| `looping` | boolean | `false` | Whether the effect loops continuously |

### Example

```json
{
  "effect": {
    "type": "Niagara",
    "duration": 10.0,
    "looping": true
  }
}
```

## Emitters Array

The `emitters` array contains one or more emitter configurations.

```json
{
  "emitters": [
    {
      "name": "EmitterName",
      "spawners": { ... },
      "initialization": { ... },
      "update": { ... },
      "render": { ... }
    }
  ]
}
```

## Emitter Configuration

### Basic Structure

| Property | Type | Required | Description |
|----------|------|----------|-------------|
| `name` | string | Yes | Unique emitter identifier |
| `spawners` | object | Yes | Particle spawn configuration |
| `initialization` | object | Yes | Initial particle properties |
| `update` | object | Yes | Per-frame particle updates |
| `render` | object | Yes | Visual rendering settings |

## Spawners Configuration

Controls how particles are emitted.

### Schema

```json
{
  "burst": {
    "count": <int>,
    "time": <float>,
    "intervals": [<float>, ...]
  },
  "rate": {
    "spawnRate": <float>,
    "scaleOverTime": <float>
  }
}
```

### Properties

#### Burst Spawn

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `count` | int | `10` | Number of particles to spawn in burst |
| `time` | float | `0.0` | Time (seconds) when burst occurs |
| `intervals` | float[] | `[]` | Additional burst times (for multiple bursts) |

#### Rate Spawn

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `spawnRate` | float | `10.0` | Particles spawned per second |
| `scaleOverTime` | float | `1.0` | Spawn rate multiplier over lifetime |

### Example

```json
{
  "spawners": {
    "burst": {
      "count": 50,
      "time": 0.0,
      "intervals": [0.5, 1.0, 1.5]
    },
    "rate": {
      "spawnRate": 20.0,
      "scaleOverTime": 0.8
    }
  }
}
```

## Initialization Configuration

Sets initial particle properties at spawn time.

### Schema

```json
{
  "color": {
    "r": <float>,
    "g": <float>,
    "b": <float>,
    "a": <float>
  },
  "size": {
    "min": <float>,
    "max": <float>
  },
  "velocity": {
    "x": <float>,
    "y": <float>,
    "z": <float>
  },
  "rotation": {
    "min": <float>,
    "max": <float>
  }
}
```

### Properties

#### Color (RGBA)

| Property | Type | Range | Default | Description |
|----------|------|-------|---------|-------------|
| `r` | float | 0.0-1.0 | `1.0` | Red channel |
| `g` | float | 0.0-1.0 | `1.0` | Green channel |
| `b` | float | 0.0-1.0 | `1.0` | Blue channel |
| `a` | float | 0.0-1.0 | `1.0` | Alpha (opacity) |

#### Size

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `min` | float | `1.0` | Minimum particle size (Unreal units) |
| `max` | float | `2.0` | Maximum particle size (Unreal units) |

#### Velocity (3D Vector)

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `x` | float | `0.0` | Velocity on X axis (Unreal units/s) |
| `y` | float | `0.0` | Velocity on Y axis (Unreal units/s) |
| `z` | float | `0.0` | Velocity on Z axis (Unreal units/s) |

#### Rotation

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `min` | float | `0.0` | Minimum initial rotation (degrees) |
| `max` | float | `360.0` | Maximum initial rotation (degrees) |

### Example

```json
{
  "initialization": {
    "color": {
      "r": 1.0,
      "g": 0.5,
      "b": 0.0,
      "a": 1.0
    },
    "size": {
      "min": 10.0,
      "max": 20.0
    },
    "velocity": {
      "x": 0.0,
      "y": 0.0,
      "z": 100.0
    },
    "rotation": {
      "min": 0.0,
      "max": 360.0
    }
  }
}
```

## Update Configuration

Defines how particles change over their lifetime.

### Schema

```json
{
  "forces": {
    "gravity": <float>,
    "wind": {
      "x": <float>,
      "y": <float>,
      "z": <float>
    }
  },
  "drag": <float>,
  "collision": {
    "enabled": <boolean>,
    "bounce": <float>
  }
}
```

### Properties

#### Forces

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `gravity` | float | `-980.0` | Gravity force (Unreal units/s²) |
| `wind.x` | float | `0.0` | Wind force on X axis |
| `wind.y` | float | `0.0` | Wind force on Y axis |
| `wind.z` | float | `0.0` | Wind force on Z axis |

#### Drag

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `drag` | float | `0.0` | Air resistance coefficient (0-1) |

#### Collision

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `enabled` | boolean | `false` | Enable world collision |
| `bounce` | float | `0.5` | Bounce coefficient (0-1) |

### Example

```json
{
  "update": {
    "forces": {
      "gravity": -980.0,
      "wind": {
        "x": 50.0,
        "y": 0.0,
        "z": 0.0
      }
    },
    "drag": 0.1,
    "collision": {
      "enabled": true,
      "bounce": 0.7
    }
  }
}
```

## Render Configuration

Controls visual appearance and rendering.

### Schema

```json
{
  "material": <string>,
  "texture": <string>,
  "blendMode": <string>,
  "sort": <string>
}
```

### Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `material` | string | `""` | Material asset path |
| `texture` | string | `""` | Texture asset path |
| `blendMode` | enum | `"Translucent"` | Blend mode: `"Opaque"`, `"Translucent"`, `"Additive"`, `"Modulate"` |
| `sort` | enum | `"ViewDepth"` | Sort mode: `"ViewDepth"`, `"Distance"`, `"None"` |

### Example

```json
{
  "render": {
    "material": "/Game/Materials/M_Particle",
    "texture": "/Game/Textures/T_Smoke",
    "blendMode": "Additive",
    "sort": "Distance"
  }
}
```

## Complete Example

Fire effect with smoke trails:

```json
{
  "effect": {
    "type": "Niagara",
    "duration": 5.0,
    "looping": true
  },
  "emitters": [
    {
      "name": "Fire",
      "spawners": {
        "burst": {
          "count": 10,
          "time": 0.0,
          "intervals": []
        },
        "rate": {
          "spawnRate": 50.0,
          "scaleOverTime": 1.0
        }
      },
      "initialization": {
        "color": {
          "r": 1.0,
          "g": 0.5,
          "b": 0.0,
          "a": 1.0
        },
        "size": {
          "min": 20.0,
          "max": 40.0
        },
        "velocity": {
          "x": 0.0,
          "y": 0.0,
          "z": 200.0
        },
        "rotation": {
          "min": 0.0,
          "max": 360.0
        }
      },
      "update": {
        "forces": {
          "gravity": -100.0,
          "wind": {
            "x": 10.0,
            "y": 10.0,
            "z": 0.0
          }
        },
        "drag": 0.1,
        "collision": {
          "enabled": false,
          "bounce": 0.0
        }
      },
      "render": {
        "material": "/Game/Materials/M_Fire",
        "texture": "/Game/Textures/T_Flame",
        "blendMode": "Additive",
        "sort": "Distance"
      }
    },
    {
      "name": "Smoke",
      "spawners": {
        "burst": {
          "count": 0,
          "time": 0.0,
          "intervals": []
        },
        "rate": {
          "spawnRate": 20.0,
          "scaleOverTime": 0.8
        }
      },
      "initialization": {
        "color": {
          "r": 0.3,
          "g": 0.3,
          "b": 0.3,
          "a": 0.5
        },
        "size": {
          "min": 50.0,
          "max": 80.0
        },
        "velocity": {
          "x": 0.0,
          "y": 0.0,
          "z": 150.0
        },
        "rotation": {
          "min": 0.0,
          "max": 360.0
        }
      },
      "update": {
        "forces": {
          "gravity": -50.0,
          "wind": {
            "x": 20.0,
            "y": 20.0,
            "z": 0.0
          }
        },
        "drag": 0.3,
        "collision": {
          "enabled": false,
          "bounce": 0.0
        }
      },
      "render": {
        "material": "/Game/Materials/M_Smoke",
        "texture": "/Game/Textures/T_Smoke",
        "blendMode": "Translucent",
        "sort": "ViewDepth"
      }
    }
  ]
}
```

## Validation Rules

### Required Fields
- `effect.type` must be specified
- `emitters` array must contain at least one emitter
- Each emitter must have a unique `name`
- All emitter sections (`spawners`, `initialization`, `update`, `render`) are required

### Value Constraints
- Color values (RGBA) must be in range `[0.0, 1.0]`
- Size values must be positive
- Duration must be positive
- Spawn rates must be non-negative
- Drag and bounce coefficients should be in range `[0.0, 1.0]`

### Best Practices
- Use descriptive emitter names
- Keep spawn rates reasonable (< 1000 particles/sec)
- Adjust gravity for desired effect scale
- Use Additive blend mode for glowing effects
- Use Translucent for smoke/fog effects

## API Usage

### Parsing DSL

```cpp
#include "Core/VFXDSLParser.h"

FString JsonString = TEXT("{ ... }");
FVFXDSL DSL;
FString Error;

if (UVFXDSLParser::ParseFromJSON(JsonString, DSL, Error))
{
    // Success - use DSL
}
else
{
    // Handle error
    UE_LOG(LogTemp, Error, TEXT("Parse error: %s"), *Error);
}
```

### Generating System

```cpp
#include "Core/NiagaraSystemGenerator.h"

FVFXDSL DSL = /* ... */;
UNiagaraSystem* System = nullptr;
FString Error;

if (UNiagaraSystemGenerator::CreateSystemFromDSL(
    DSL, 
    TEXT("/Game/VFX"), 
    TEXT("MySystem"), 
    System, 
    Error))
{
    // Success - System is created
}
else
{
    // Handle error
    UE_LOG(LogTemp, Error, TEXT("Generation error: %s"), *Error);
}
```

### Serializing DSL

```cpp
FVFXDSL DSL = /* ... */;
FString JsonOutput;

if (UVFXDSLParser::ToJSON(DSL, JsonOutput))
{
    // Success - JsonOutput contains serialized DSL
}
```

## Future Extensions

### Planned Features
- `lifetime` property in initialization (min/max particle lifespan)
- `friction` property in collision
- Color gradient over lifetime
- Size curve over lifetime
- Velocity curves
- Sub-emitters
- Events and triggers
- Custom parameters

### Version History
- **1.0.0** (2025-01-14): Initial schema release

## See Also

- [Features Specification](FEATURES.md)
- [Testing Documentation](TESTING.md)
- [README](../README.md)
- [Implementation Tasks](../rulebook/tasks/implement-ainiagara-plugin/tasks.md)

