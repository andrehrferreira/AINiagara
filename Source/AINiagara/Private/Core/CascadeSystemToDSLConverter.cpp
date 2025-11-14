// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CascadeSystemToDSLConverter.h"
#include "Core/VFXDSLParser.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleEmitter.h"
#include "Particles/ParticleModule.h"
#include "Particles/ParticleModuleRequired.h"
#include "Particles/ParticleModuleSpawn.h"
#include "Particles/ParticleModuleSize.h"
#include "Particles/ParticleModuleColor.h"
#include "Particles/ParticleModuleVelocity.h"
#include "Particles/ParticleModuleRotation.h"
#include "Particles/ParticleModuleVelocityOverLifetime.h"
#include "Materials/MaterialInterface.h"

bool UCascadeSystemToDSLConverter::ConvertSystemToDSL(
	UParticleSystem* ParticleSystem,
	FVFXDSL& OutDSL,
	FString& OutError)
{
	if (!ParticleSystem)
	{
		OutError = TEXT("ParticleSystem is null");
		return false;
	}

	// Initialize DSL with defaults
	OutDSL = FVFXDSL();
	OutDSL.Effect.Type = EVFXEffectType::Cascade;
	OutDSL.Effect.Duration = ParticleSystem->Duration;
	OutDSL.Effect.bLooping = false; // Default, as this is not directly stored in UParticleSystem

	// Get emitters from system
	int32 NumEmitters = ParticleSystem->Emitters.Num();
	
	if (NumEmitters == 0)
	{
		OutError = TEXT("ParticleSystem has no emitters");
		return false;
	}

	// Convert each emitter
	for (int32 i = 0; i < NumEmitters; ++i)
	{
		UParticleEmitter* Emitter = ParticleSystem->Emitters[i];
		
		if (!Emitter)
		{
			UE_LOG(LogTemp, Warning, TEXT("Emitter %d is null, skipping"), i);
			continue;
		}

		FVFXDSLEmitter EmitterDSL;
		FString EmitterError;
		
		// Convert emitter
		if (!ConvertEmitterToDSL(Emitter, EmitterDSL, EmitterError))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to convert emitter %d: %s"), i, *EmitterError);
			continue;
		}

		// Set emitter name
		EmitterDSL.Name = Emitter->EmitterName.ToString();
		
		// Add to DSL
		OutDSL.Emitters.Add(EmitterDSL);
	}

	if (OutDSL.Emitters.Num() == 0)
	{
		OutError = TEXT("No emitters could be converted");
		return false;
	}

	return true;
}

bool UCascadeSystemToDSLConverter::ConvertEmitterToDSL(
	UParticleEmitter* Emitter,
	FVFXDSLEmitter& OutEmitterDSL,
	FString& OutError)
{
	if (!Emitter)
	{
		OutError = TEXT("Emitter is null");
		return false;
	}

	// Initialize with defaults
	OutEmitterDSL = FVFXDSLEmitter();
	OutEmitterDSL.Name = Emitter->EmitterName.ToString();

	// Extract spawner configuration
	FString SpawnerError;
	if (!ExtractSpawnerConfig(Emitter, OutEmitterDSL.Spawners, SpawnerError))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to extract spawner config: %s"), *SpawnerError);
		// Don't fail - use defaults
	}

	// Extract initialization configuration
	FString InitError;
	if (!ExtractInitializationConfig(Emitter, OutEmitterDSL.Initialization, InitError))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to extract initialization config: %s"), *InitError);
		// Don't fail - use defaults
	}

	// Extract update configuration
	FString UpdateError;
	if (!ExtractUpdateConfig(Emitter, OutEmitterDSL.Update, UpdateError))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to extract update config: %s"), *UpdateError);
		// Don't fail - use defaults
	}

	// Extract render configuration
	FString RenderError;
	if (!ExtractRenderConfig(Emitter, OutEmitterDSL.Render, RenderError))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to extract render config: %s"), *RenderError);
		// Don't fail - use defaults
	}

	return true;
}

bool UCascadeSystemToDSLConverter::ExtractSpawnerConfig(
	UParticleEmitter* Emitter,
	FVFXDSLSpawners& OutSpawners,
	FString& OutError)
{
	if (!Emitter)
	{
		OutError = TEXT("Emitter is null");
		return false;
	}

	// Find spawn module
	UParticleModuleSpawn* SpawnModule = nullptr;
	for (UParticleModule* Module : Emitter->SpawnModules)
	{
		if (UParticleModuleSpawn* Spawn = Cast<UParticleModuleSpawn>(Module))
		{
			SpawnModule = Spawn;
			break;
		}
	}

	if (SpawnModule)
	{
		// Extract spawn rate
		if (SpawnModule->Rate.GetValue() > 0.0f)
		{
			OutSpawners.Rate.SpawnRate = SpawnModule->Rate.GetValue();
		}
		else
		{
			OutSpawners.Rate.SpawnRate = 10.0f; // Default
		}

		// Extract bursts
		if (SpawnModule->BurstList.Num() > 0)
		{
			const FParticleBurst& FirstBurst = SpawnModule->BurstList[0];
			OutSpawners.Burst.Count = FirstBurst.Count;
			OutSpawners.Burst.Time = FirstBurst.Time;

			// Extract additional bursts
			for (int32 i = 1; i < SpawnModule->BurstList.Num(); ++i)
			{
				OutSpawners.Burst.Intervals.Add(SpawnModule->BurstList[i].Time);
			}
		}
		else
		{
			OutSpawners.Burst.Count = 0;
			OutSpawners.Burst.Time = 0.0f;
		}
	}
	else
	{
		// Use defaults if no spawn module found
		OutSpawners.Rate.SpawnRate = 10.0f;
		OutSpawners.Rate.ScaleOverTime = 1.0f;
		OutSpawners.Burst.Count = 0;
		OutSpawners.Burst.Time = 0.0f;
	}

	return true;
}

bool UCascadeSystemToDSLConverter::ExtractInitializationConfig(
	UParticleEmitter* Emitter,
	FVFXDSLInitialization& OutInitialization,
	FString& OutError)
{
	if (!Emitter)
	{
		OutError = TEXT("Emitter is null");
		return false;
	}

	// Extract size
	UParticleModuleSize* SizeModule = nullptr;
	for (UParticleModule* Module : Emitter->SpawnModules)
	{
		if (UParticleModuleSize* Size = Cast<UParticleModuleSize>(Module))
		{
			SizeModule = Size;
			break;
		}
	}

	if (SizeModule)
	{
		FVector2D StartSize = SizeModule->StartSize.GetValue();
		OutInitialization.Size.Min = StartSize.X;
		OutInitialization.Size.Max = StartSize.Y;
	}
	else
	{
		OutInitialization.Size.Min = 10.0f;
		OutInitialization.Size.Max = 20.0f;
	}

	// Extract color
	UParticleModuleColor* ColorModule = nullptr;
	for (UParticleModule* Module : Emitter->SpawnModules)
	{
		if (UParticleModuleColor* Color = Cast<UParticleModuleColor>(Module))
		{
			ColorModule = Color;
			break;
		}
	}

	if (ColorModule)
	{
		FVector4 StartColor = ColorModule->StartColor.GetValue();
		OutInitialization.Color.R = StartColor.X;
		OutInitialization.Color.G = StartColor.Y;
		OutInitialization.Color.B = StartColor.Z;
		OutInitialization.Color.A = StartColor.W;
	}
	else
	{
		OutInitialization.Color.R = 1.0f;
		OutInitialization.Color.G = 1.0f;
		OutInitialization.Color.B = 1.0f;
		OutInitialization.Color.A = 1.0f;
	}

	// Extract velocity
	UParticleModuleVelocity* VelocityModule = nullptr;
	for (UParticleModule* Module : Emitter->SpawnModules)
	{
		if (UParticleModuleVelocity* Velocity = Cast<UParticleModuleVelocity>(Module))
		{
			VelocityModule = Velocity;
			break;
		}
	}

	if (VelocityModule)
	{
		FVector StartVelocity = VelocityModule->StartVelocity.GetValue();
		OutInitialization.Velocity.X = StartVelocity.X;
		OutInitialization.Velocity.Y = StartVelocity.Y;
		OutInitialization.Velocity.Z = StartVelocity.Z;
	}
	else
	{
		OutInitialization.Velocity.X = 0.0f;
		OutInitialization.Velocity.Y = 0.0f;
		OutInitialization.Velocity.Z = 0.0f;
	}

	// Extract rotation
	UParticleModuleRotation* RotationModule = nullptr;
	for (UParticleModule* Module : Emitter->SpawnModules)
	{
		if (UParticleModuleRotation* Rotation = Cast<UParticleModuleRotation>(Module))
		{
			RotationModule = Rotation;
			break;
		}
	}

	if (RotationModule)
	{
		FVector2D StartRotation = RotationModule->StartRotation.GetValue();
		OutInitialization.Rotation.Min = StartRotation.X;
		OutInitialization.Rotation.Max = StartRotation.Y;
	}
	else
	{
		OutInitialization.Rotation.Min = 0.0f;
		OutInitialization.Rotation.Max = 360.0f;
	}

	return true;
}

bool UCascadeSystemToDSLConverter::ExtractUpdateConfig(
	UParticleEmitter* Emitter,
	FVFXDSLUpdate& OutUpdate,
	FString& OutError)
{
	if (!Emitter)
	{
		OutError = TEXT("Emitter is null");
		return false;
	}

	// Extract velocity over lifetime (forces)
	UParticleModuleVelocityOverLifetime* VelocityOverLifetimeModule = nullptr;
	for (UParticleModule* Module : Emitter->UpdateModules)
	{
		if (UParticleModuleVelocityOverLifetime* VelocityOverLifetime = Cast<UParticleModuleVelocityOverLifetime>(Module))
		{
			VelocityOverLifetimeModule = VelocityOverLifetime;
			break;
		}
	}

	if (VelocityOverLifetimeModule)
	{
		FVector VelOverLife = VelocityOverLifetimeModule->VelOverLife.GetValue();
		OutUpdate.Forces.Wind.X = VelOverLife.X;
		OutUpdate.Forces.Wind.Y = VelOverLife.Y;
		// Gravity is typically in Z axis
		OutUpdate.Forces.Gravity = VelOverLife.Z;
	}
	else
	{
		OutUpdate.Forces.Gravity = -980.0f;
		OutUpdate.Forces.Wind.X = 0.0f;
		OutUpdate.Forces.Wind.Y = 0.0f;
		OutUpdate.Forces.Wind.Z = 0.0f;
	}

	OutUpdate.Drag = 0.0f;
	OutUpdate.Collision.bEnabled = false;
	OutUpdate.Collision.Bounce = 0.5f;

	return true;
}

bool UCascadeSystemToDSLConverter::ExtractRenderConfig(
	UParticleEmitter* Emitter,
	FVFXDSLRender& OutRender,
	FString& OutError)
{
	if (!Emitter)
	{
		OutError = TEXT("Emitter is null");
		return false;
	}

	// Find required module for material and blend mode
	UParticleModuleRequired* RequiredModule = nullptr;
	for (UParticleModule* Module : Emitter->RequiredModules)
	{
		if (UParticleModuleRequired* Required = Cast<UParticleModuleRequired>(Module))
		{
			RequiredModule = Required;
			break;
		}
	}

	if (RequiredModule)
	{
		// Extract material path
		if (RequiredModule->Material)
		{
			OutRender.Material = RequiredModule->Material->GetPathName();
		}
		else
		{
			OutRender.Material = TEXT("");
		}

		// Extract blend mode
		switch (RequiredModule->BlendMode)
		{
		case BLEND_Opaque:
			OutRender.BlendMode = TEXT("Opaque");
			break;
		case BLEND_Translucent:
			OutRender.BlendMode = TEXT("Translucent");
			break;
		case BLEND_Additive:
			OutRender.BlendMode = TEXT("Additive");
			break;
		case BLEND_Modulate:
			OutRender.BlendMode = TEXT("Modulate");
			break;
		default:
			OutRender.BlendMode = TEXT("Translucent");
			break;
		}
	}
	else
	{
		OutRender.Material = TEXT("");
		OutRender.Texture = TEXT("");
		OutRender.BlendMode = TEXT("Translucent");
	}

	OutRender.Sort = TEXT("ViewDepth");

	return true;
}

