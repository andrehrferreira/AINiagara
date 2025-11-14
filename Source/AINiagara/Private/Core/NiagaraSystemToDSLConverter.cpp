// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/NiagaraSystemToDSLConverter.h"
#include "Core/VFXDSLParser.h"
#include "NiagaraSystem.h"
#include "NiagaraEmitter.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

bool UNiagaraSystemToDSLConverter::ConvertSystemToDSL(
	UNiagaraSystem* NiagaraSystem,
	FVFXDSL& OutDSL,
	FString& OutError)
{
	if (!NiagaraSystem)
	{
		OutError = TEXT("NiagaraSystem is null");
		return false;
	}

	// Initialize DSL with defaults
	OutDSL = FVFXDSL();
	OutDSL.Effect.Type = EVFXEffectType::Niagara;
	OutDSL.Effect.Duration = 5.0f; // Default, as this is not stored in Niagara system
	OutDSL.Effect.bLooping = false; // Default

	// Get emitters from system
	int32 NumEmitters = NiagaraSystem->GetNumEmitters();
	
	if (NumEmitters == 0)
	{
		OutError = TEXT("NiagaraSystem has no emitters");
		return false;
	}

	// Convert each emitter
	for (int32 i = 0; i < NumEmitters; ++i)
	{
		FVFXDSLEmitter EmitterDSL;
		FString EmitterError;
		
		// Get emitter handle
		const FNiagaraEmitterHandle& EmitterHandle = NiagaraSystem->GetEmitterHandle(i);
		
		// Get emitter instance
		FVersionedNiagaraEmitter VersionedEmitter = EmitterHandle.GetInstance();
		UNiagaraEmitter* Emitter = VersionedEmitter.Emitter;
		
		if (!Emitter)
		{
			UE_LOG(LogTemp, Warning, TEXT("Emitter %d is null, skipping"), i);
			continue;
		}

		// Convert emitter
		if (!ConvertEmitterToDSL(Emitter, EmitterDSL, EmitterError))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to convert emitter %d: %s"), i, *EmitterError);
			continue;
		}

		// Set emitter name from handle
		EmitterDSL.Name = EmitterHandle.GetName().ToString();
		
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

bool UNiagaraSystemToDSLConverter::ConvertEmitterToDSL(
	UNiagaraEmitter* Emitter,
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
	OutEmitterDSL.Name = Emitter->GetName();

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

bool UNiagaraSystemToDSLConverter::ExportDSLToJSON(
	const FVFXDSL& DSL,
	FString& OutJson,
	bool bPrettyPrint)
{
	// Use the parser's ToJSON method
	return UVFXDSLParser::ToJSON(DSL, OutJson);
}

bool UNiagaraSystemToDSLConverter::ExportDSLToFile(
	const FVFXDSL& DSL,
	const FString& FilePath)
{
	FString JsonString;
	if (!ExportDSLToJSON(DSL, JsonString, true))
	{
		return false;
	}

	// Ensure directory exists
	FString Directory = FPaths::GetPath(FilePath);
	if (!Directory.IsEmpty())
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		if (!PlatformFile.DirectoryExists(*Directory))
		{
			if (!PlatformFile.CreateDirectoryTree(*Directory))
			{
				return false;
			}
		}
	}

	// Write to file
	return FFileHelper::SaveStringToFile(JsonString, *FilePath);
}

bool UNiagaraSystemToDSLConverter::ExtractSpawnerConfig(
	UNiagaraEmitter* Emitter,
	FVFXDSLSpawners& OutSpawners,
	FString& OutError)
{
	// TODO: Extract actual spawn rate from Niagara emitter
	// This requires accessing the Niagara graph and reading module parameters
	// For now, use reasonable defaults
	
	OutSpawners.Rate.SpawnRate = 10.0f;
	OutSpawners.Rate.ScaleOverTime = 1.0f;
	
	OutSpawners.Burst.Count = 0;
	OutSpawners.Burst.Time = 0.0f;
	OutSpawners.Burst.Intervals.Empty();

	UE_LOG(LogTemp, Log, TEXT("Spawner extraction not fully implemented, using defaults"));
	return true;
}

bool UNiagaraSystemToDSLConverter::ExtractInitializationConfig(
	UNiagaraEmitter* Emitter,
	FVFXDSLInitialization& OutInitialization,
	FString& OutError)
{
	// TODO: Extract actual initialization values from Niagara emitter
	// This requires accessing the Niagara graph and reading module parameters
	// For now, use reasonable defaults
	
	OutInitialization.Color.R = 1.0f;
	OutInitialization.Color.G = 1.0f;
	OutInitialization.Color.B = 1.0f;
	OutInitialization.Color.A = 1.0f;
	
	OutInitialization.Size.Min = 10.0f;
	OutInitialization.Size.Max = 20.0f;
	
	OutInitialization.Velocity.X = 0.0f;
	OutInitialization.Velocity.Y = 0.0f;
	OutInitialization.Velocity.Z = 0.0f;
	
	OutInitialization.Rotation.Min = 0.0f;
	OutInitialization.Rotation.Max = 360.0f;

	UE_LOG(LogTemp, Log, TEXT("Initialization extraction not fully implemented, using defaults"));
	return true;
}

bool UNiagaraSystemToDSLConverter::ExtractUpdateConfig(
	UNiagaraEmitter* Emitter,
	FVFXDSLUpdate& OutUpdate,
	FString& OutError)
{
	// TODO: Extract actual update configuration from Niagara emitter
	// This requires accessing the Niagara graph and reading module parameters
	// For now, use reasonable defaults
	
	OutUpdate.Forces.Gravity = -980.0f;
	OutUpdate.Forces.Wind.X = 0.0f;
	OutUpdate.Forces.Wind.Y = 0.0f;
	OutUpdate.Forces.Wind.Z = 0.0f;
	
	OutUpdate.Drag = 0.0f;
	
	OutUpdate.Collision.bEnabled = false;
	OutUpdate.Collision.Bounce = 0.5f;

	UE_LOG(LogTemp, Log, TEXT("Update extraction not fully implemented, using defaults"));
	return true;
}

bool UNiagaraSystemToDSLConverter::ExtractRenderConfig(
	UNiagaraEmitter* Emitter,
	FVFXDSLRender& OutRender,
	FString& OutError)
{
	// TODO: Extract actual render configuration from Niagara emitter
	// This requires accessing renderer properties
	// For now, use reasonable defaults
	
	OutRender.Material = TEXT("");
	OutRender.Texture = TEXT("");
	OutRender.BlendMode = TEXT("Translucent");
	OutRender.Sort = TEXT("ViewDepth");

	UE_LOG(LogTemp, Log, TEXT("Render extraction not fully implemented, using defaults"));
	return true;
}

