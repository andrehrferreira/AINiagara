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
	// Note: SetEffectType may not be available in UE 5.3
	// System properties are configured automatically
	OutSystem->bFixedBounds = false;

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
		// AddEmitterHandle requires: UNiagaraEmitter&, FName, FGuid
		FGuid EmitterVersion = FGuid::NewGuid();
		OutSystem->AddEmitterHandle(*Emitter, FName(*EmitterDSL.Name), EmitterVersion);
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

	// Emitter is initialized automatically when created
	// No need to call InitializeEmitter explicitly

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

	// TODO: Configure spawn module
	// Note: In UE 5.3, accessing scripts requires using the Niagara Graph API
	// This is a placeholder implementation that will need to be expanded
	// to properly configure spawn rate and burst parameters through the graph
	
	// For now, we'll skip script configuration as it requires graph manipulation
	// which is complex and version-specific

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

	// TODO: Configure initialize module
	// Note: In UE 5.3, accessing scripts requires using the Niagara Graph API
	// This is a placeholder implementation that will need to be expanded
	// to properly configure initialization parameters through the graph

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

	// TODO: Configure update module
	// Note: In UE 5.3, accessing scripts requires using the Niagara Graph API
	// This is a placeholder implementation that will need to be expanded
	// to properly configure update parameters through the graph

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

	// TODO: Configure render module
	// Note: In UE 5.3, configuring render parameters requires using the Niagara Graph API
	// This is a placeholder implementation that will need to be expanded
	// to properly configure render parameters through the graph

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

