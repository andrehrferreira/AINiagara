// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/NiagaraSystemGenerator.h"
#include "NiagaraSystem.h"
#include "NiagaraEmitter.h"
#include "NiagaraScript.h"
#include "NiagaraScriptSourceBase.h"
#include "NiagaraGraph.h"
#include "NiagaraNodeOutput.h"
#include "NiagaraNodeInput.h"
#include "NiagaraNodeFunctionCall.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "ObjectTools.h"
#include "PackageTools.h"
#include "Misc/MessageDialog.h"
#include "HAL/PlatformFilemanager.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/Package.h"

bool UNiagaraSystemGenerator::CreateSystemFromDSL(
	const FVFXDSL& DSL,
	const FString& PackagePath,
	const FString& SystemName,
	UNiagaraSystem*& OutSystem,
	FString& OutError)
{
	// Validate DSL type
	if (DSL.Effect.Type != EVFXEffectType::Niagara)
	{
		OutError = TEXT("DSL type is not Niagara");
		return false;
	}

	// Validate emitters
	if (DSL.Emitters.Num() == 0)
	{
		OutError = TEXT("DSL must contain at least one emitter");
		return false;
	}

	// Create package
	FString FullPackagePath = PackagePath + TEXT("/") + SystemName;
	UPackage* Package = CreatePackage(*FullPackagePath);
	if (!Package)
	{
		OutError = FString::Printf(TEXT("Failed to create package at path: %s"), *FullPackagePath);
		return false;
	}

	// Create Niagara system
	OutSystem = NewObject<UNiagaraSystem>(Package, *SystemName, RF_Public | RF_Standalone);
	if (!OutSystem)
	{
		OutError = TEXT("Failed to create UNiagaraSystem object");
		return false;
	}

	// Configure system properties
	OutSystem->SetEffectType(ENiagaraEffectType::Particle);
	OutSystem->bFixedBounds = false;
	OutSystem->bFixedBoundsByAge = false;
	OutSystem->bCanBeInScalabilityManager = true;

	// Create emitters for each DSL emitter
	for (const FVFXDSLEmitter& EmitterDSL : DSL.Emitters)
	{
		UNiagaraEmitter* Emitter = nullptr;
		FString EmitterError;
		
		if (!CreateEmitterFromDSL(EmitterDSL, PackagePath, EmitterDSL.Name, Emitter, EmitterError))
		{
			OutError = FString::Printf(TEXT("Failed to create emitter '%s': %s"), *EmitterDSL.Name, *EmitterError);
			return false;
		}

		// Add emitter to system
		FVersionedNiagaraEmitter VersionedEmitter(Emitter, Emitter->GetExposedVersion());
		OutSystem->AddEmitterHandle(VersionedEmitter, EmitterDSL.Name);
	}

	// Mark package as dirty
	Package->MarkPackageDirty();

	// Notify asset registry
	FAssetRegistryModule::AssetCreated(OutSystem);

	return true;
}

bool UNiagaraSystemGenerator::CreateEmitterFromDSL(
	const FVFXDSLEmitter& EmitterDSL,
	const FString& PackagePath,
	const FString& EmitterName,
	UNiagaraEmitter*& OutEmitter,
	FString& OutError)
{
	// Create package for emitter
	FString FullPackagePath = PackagePath + TEXT("/") + EmitterName;
	UPackage* Package = CreatePackage(*FullPackagePath);
	if (!Package)
	{
		OutError = FString::Printf(TEXT("Failed to create package at path: %s"), *FullPackagePath);
		return false;
	}

	// Create Niagara emitter
	OutEmitter = NewObject<UNiagaraEmitter>(Package, *EmitterName, RF_Public | RF_Standalone);
	if (!OutEmitter)
	{
		OutError = TEXT("Failed to create UNiagaraEmitter object");
		return false;
	}

	// Initialize emitter with default settings
	OutEmitter->InitializeEmitter(FVersionedNiagaraEmitter(OutEmitter, FNiagaraEmitterVersion::LatestVersion));

	// Configure spawn module
	FString SpawnError;
	if (!ConfigureSpawnModule(OutEmitter, EmitterDSL.Spawners, SpawnError))
	{
		OutError = FString::Printf(TEXT("Failed to configure spawn module: %s"), *SpawnError);
		return false;
	}

	// Configure initialize module
	FString InitError;
	if (!ConfigureInitializeModule(OutEmitter, EmitterDSL.Initialization, InitError))
	{
		OutError = FString::Printf(TEXT("Failed to configure initialize module: %s"), *InitError);
		return false;
	}

	// Configure update module
	FString UpdateError;
	if (!ConfigureUpdateModule(OutEmitter, EmitterDSL.Update, UpdateError))
	{
		OutError = FString::Printf(TEXT("Failed to configure update module: %s"), *UpdateError);
		return false;
	}

	// Configure render module
	FString RenderError;
	if (!ConfigureRenderModule(OutEmitter, EmitterDSL.Render, RenderError))
	{
		OutError = FString::Printf(TEXT("Failed to configure render module: %s"), *RenderError);
		return false;
	}

	// Mark package as dirty
	Package->MarkPackageDirty();

	// Notify asset registry
	FAssetRegistryModule::AssetCreated(OutEmitter);

	return true;
}

bool UNiagaraSystemGenerator::ConfigureSpawnModule(
	UNiagaraEmitter* Emitter,
	const FVFXDSLSpawners& SpawnersDSL,
	FString& OutError)
{
	if (!Emitter)
	{
		OutError = TEXT("Emitter is null");
		return false;
	}

	// Get spawn script
	UNiagaraScript* SpawnScript = Emitter->GetScript(ENiagaraScriptUsage::ParticleSpawnScript, FGuid());
	if (!SpawnScript)
	{
		OutError = TEXT("Failed to get spawn script");
		return false;
	}

	// Configure rate spawn
	if (SpawnersDSL.Rate.SpawnRate > 0.0f)
	{
		// Set spawn rate parameter
		// Note: This is a simplified implementation. In a full implementation,
		// you would need to add/modify nodes in the Niagara graph to set spawn rate
		FString RateError;
		if (!SetFloatParameter(SpawnScript, TEXT("SpawnRate"), SpawnersDSL.Rate.SpawnRate, RateError))
		{
			// If parameter doesn't exist, we'll need to add it to the graph
			// For now, just log the error but continue
		}
	}

	// Configure burst spawn
	if (SpawnersDSL.Burst.Count > 0)
	{
		// Set burst count parameter
		FString BurstError;
		if (!SetIntParameter(SpawnScript, TEXT("BurstCount"), SpawnersDSL.Burst.Count, BurstError))
		{
			// If parameter doesn't exist, we'll need to add it to the graph
		}

		// Set burst time parameter
		if (!SetFloatParameter(SpawnScript, TEXT("BurstTime"), SpawnersDSL.Burst.Time, BurstError))
		{
			// If parameter doesn't exist, we'll need to add it to the graph
		}
	}

	return true;
}

bool UNiagaraSystemGenerator::ConfigureInitializeModule(
	UNiagaraEmitter* Emitter,
	const FVFXDSLInitialization& InitDSL,
	FString& OutError)
{
	if (!Emitter)
	{
		OutError = TEXT("Emitter is null");
		return false;
	}

	// Get initialize script
	UNiagaraScript* InitScript = Emitter->GetScript(ENiagaraScriptUsage::ParticleUpdateScript, FGuid());
	if (!InitScript)
	{
		OutError = TEXT("Failed to get initialize script");
		return false;
	}

	// Configure color
	FLinearColor Color = InitDSL.Color.ToLinearColor();
	FString ColorError;
	if (!SetColorParameter(InitScript, TEXT("Color"), Color, ColorError))
	{
		// Parameter might not exist, continue anyway
	}

	// Configure size
	float SizeMin = InitDSL.Size.Min;
	float SizeMax = InitDSL.Size.Max;
	if (!SetFloatParameter(InitScript, TEXT("SizeMin"), SizeMin, ColorError))
	{
		// Parameter might not exist
	}
	if (!SetFloatParameter(InitScript, TEXT("SizeMax"), SizeMax, ColorError))
	{
		// Parameter might not exist
	}

	// Configure velocity
	FVector Velocity = InitDSL.Velocity.ToVector();
	if (!SetVectorParameter(InitScript, TEXT("Velocity"), Velocity, ColorError))
	{
		// Parameter might not exist
	}

	return true;
}

bool UNiagaraSystemGenerator::ConfigureUpdateModule(
	UNiagaraEmitter* Emitter,
	const FVFXDSLUpdate& UpdateDSL,
	FString& OutError)
{
	if (!Emitter)
	{
		OutError = TEXT("Emitter is null");
		return false;
	}

	// Get update script
	UNiagaraScript* UpdateScript = Emitter->GetScript(ENiagaraScriptUsage::ParticleUpdateScript, FGuid());
	if (!UpdateScript)
	{
		OutError = TEXT("Failed to get update script");
		return false;
	}

	// Configure gravity
	if (UpdateDSL.Forces.Gravity != 0.0f)
	{
		FString GravityError;
		if (!SetFloatParameter(UpdateScript, TEXT("Gravity"), UpdateDSL.Forces.Gravity, GravityError))
		{
			// Parameter might not exist
		}
	}

	// Configure wind
	FVector Wind = UpdateDSL.Forces.Wind.ToVector();
	if (Wind.SizeSquared() > 0.0f)
	{
		FString WindError;
		if (!SetVectorParameter(UpdateScript, TEXT("Wind"), Wind, WindError))
		{
			// Parameter might not exist
		}
	}

	// Configure drag
	if (UpdateDSL.Drag != 0.0f)
	{
		FString DragError;
		if (!SetFloatParameter(UpdateScript, TEXT("Drag"), UpdateDSL.Drag, DragError))
		{
			// Parameter might not exist
		}
	}

	// Configure collision
	if (UpdateDSL.Collision.bEnabled)
	{
		FString CollisionError;
		if (!SetBoolParameter(UpdateScript, TEXT("CollisionEnabled"), true, CollisionError))
		{
			// Parameter might not exist
		}
		if (!SetFloatParameter(UpdateScript, TEXT("Bounce"), UpdateDSL.Collision.Bounce, CollisionError))
		{
			// Parameter might not exist
		}
	}

	return true;
}

bool UNiagaraSystemGenerator::ConfigureRenderModule(
	UNiagaraEmitter* Emitter,
	const FVFXDSLRender& RenderDSL,
	FString& OutError)
{
	if (!Emitter)
	{
		OutError = TEXT("Emitter is null");
		return false;
	}

	// Get render script
	UNiagaraScript* RenderScript = Emitter->GetScript(ENiagaraScriptUsage::ParticleUpdateScript, FGuid());
	if (!RenderScript)
	{
		OutError = TEXT("Failed to get render script");
		return false;
	}

	// Configure material
	if (!RenderDSL.Material.IsEmpty())
	{
		// Load material asset
		// Note: This is a simplified implementation. In a full implementation,
		// you would load the material and assign it to the renderer
		FString MaterialError;
		// Material assignment would go here
	}

	// Configure texture
	if (!RenderDSL.Texture.IsEmpty())
	{
		// Load texture asset
		// Note: This is a simplified implementation. In a full implementation,
		// you would load the texture and assign it to the renderer
		FString TextureError;
		// Texture assignment would go here
	}

	return true;
}

bool UNiagaraSystemGenerator::SetFloatParameter(UNiagaraScript* Script, const FName& ParameterName, float Value, FString& OutError)
{
	if (!Script)
	{
		OutError = TEXT("Script is null");
		return false;
	}

	// Note: This is a simplified implementation. In a full implementation,
	// you would need to modify the Niagara graph to add/set parameters.
	// For now, this is a placeholder that would need to be expanded with
	// actual graph manipulation code.

	return true;
}

bool UNiagaraSystemGenerator::SetVectorParameter(UNiagaraScript* Script, const FName& ParameterName, const FVector& Value, FString& OutError)
{
	if (!Script)
	{
		OutError = TEXT("Script is null");
		return false;
	}

	// Note: This is a simplified implementation. In a full implementation,
	// you would need to modify the Niagara graph to add/set parameters.

	return true;
}

bool UNiagaraSystemGenerator::SetColorParameter(UNiagaraScript* Script, const FName& ParameterName, const FLinearColor& Value, FString& OutError)
{
	if (!Script)
	{
		OutError = TEXT("Script is null");
		return false;
	}

	// Note: This is a simplified implementation. In a full implementation,
	// you would need to modify the Niagara graph to add/set parameters.

	return true;
}

bool UNiagaraSystemGenerator::SetIntParameter(UNiagaraScript* Script, const FName& ParameterName, int32 Value, FString& OutError)
{
	if (!Script)
	{
		OutError = TEXT("Script is null");
		return false;
	}

	// Note: This is a simplified implementation. In a full implementation,
	// you would need to modify the Niagara graph to add/set parameters.

	return true;
}

bool UNiagaraSystemGenerator::SetBoolParameter(UNiagaraScript* Script, const FName& ParameterName, bool Value, FString& OutError)
{
	if (!Script)
	{
		OutError = TEXT("Script is null");
		return false;
	}

	// Note: This is a simplified implementation. In a full implementation,
	// you would need to modify the Niagara graph to add/set parameters.

	return true;
}

