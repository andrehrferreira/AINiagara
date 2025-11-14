// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Core/VFXDSL.h"
#include "CascadeSystemGenerator.generated.h"

class UParticleSystem;
class UParticleEmitter;
class UParticleModule;

/**
 * Generator for Cascade particle systems from DSL specifications
 */
UCLASS(BlueprintType)
class AINIAGARA_API UCascadeSystemGenerator : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Create a Cascade particle system from DSL specification
	 * @param DSL The DSL specification to convert
	 * @param PackagePath The package path where to create the system (e.g., "/Game/VFX/MySystem")
	 * @param SystemName The name for the system asset
	 * @param OutSystem The created system (output)
	 * @param OutError Error message if creation failed (output)
	 * @return True if system was created successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Cascade")
	static bool CreateSystemFromDSL(
		const FVFXDSL& DSL,
		const FString& PackagePath,
		const FString& SystemName,
		UParticleSystem*& OutSystem,
		FString& OutError
	);

	/**
	 * Create a Cascade emitter from DSL emitter specification
	 * @param EmitterDSL The emitter DSL specification
	 * @param PackagePath The package path where to create the emitter
	 * @param EmitterName The name for the emitter asset
	 * @param OutEmitter The created emitter (output)
	 * @param OutError Error message if creation failed (output)
	 * @return True if emitter was created successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Cascade")
	static bool CreateEmitterFromDSL(
		const FVFXDSLEmitter& EmitterDSL,
		const FString& PackagePath,
		const FString& EmitterName,
		UParticleEmitter*& OutEmitter,
		FString& OutError
	);

	/**
	 * Configure spawn module from DSL spawners
	 * @param Emitter The emitter to configure
	 * @param SpawnersDSL The spawners DSL specification
	 * @param OutError Error message if configuration failed (output)
	 * @return True if configuration was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Cascade")
	static bool ConfigureSpawnModule(
		UParticleEmitter* Emitter,
		const FVFXDSLSpawners& SpawnersDSL,
		FString& OutError
	);

	/**
	 * Configure initialization module from DSL initialization
	 * @param Emitter The emitter to configure
	 * @param InitializationDSL The initialization DSL specification
	 * @param OutError Error message if configuration failed (output)
	 * @return True if configuration was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Cascade")
	static bool ConfigureInitializeModule(
		UParticleEmitter* Emitter,
		const FVFXDSLInitialization& InitializationDSL,
		FString& OutError
	);

	/**
	 * Configure update module from DSL update section
	 * @param Emitter The emitter to configure
	 * @param UpdateDSL The update DSL specification
	 * @param OutError Error message if configuration failed (output)
	 * @return True if configuration was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Cascade")
	static bool ConfigureUpdateModule(
		UParticleEmitter* Emitter,
		const FVFXDSLUpdate& UpdateDSL,
		FString& OutError
	);

	/**
	 * Configure render module from DSL render section
	 * @param Emitter The emitter to configure
	 * @param RenderDSL The render DSL specification
	 * @param OutError Error message if configuration failed (output)
	 * @return True if configuration was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Cascade")
	static bool ConfigureRenderModule(
		UParticleEmitter* Emitter,
		const FVFXDSLRender& RenderDSL,
		FString& OutError
	);
};

