// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Core/VFXDSL.h"
#include "NiagaraSystemGenerator.generated.h"

class UNiagaraSystem;
class UNiagaraEmitter;
class UNiagaraScript;

/**
 * Generator for Niagara systems from DSL specifications
 */
UCLASS(BlueprintType)
class AINIAGARA_API UNiagaraSystemGenerator : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Create a Niagara system from DSL specification
	 * @param DSL The DSL specification to convert
	 * @param PackagePath The package path where to create the system (e.g., "/Game/VFX/MySystem")
	 * @param SystemName The name for the system asset
	 * @param OutSystem The created system (output)
	 * @param OutError Error message if creation failed (output)
	 * @return True if system was created successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Niagara")
	static bool CreateSystemFromDSL(
		const FVFXDSL& DSL,
		const FString& PackagePath,
		const FString& SystemName,
		UNiagaraSystem*& OutSystem,
		FString& OutError
	);

	/**
	 * Create a Niagara emitter from DSL emitter specification
	 * @param EmitterDSL The emitter DSL specification
	 * @param PackagePath The package path where to create the emitter
	 * @param EmitterName The name for the emitter asset
	 * @param OutEmitter The created emitter (output)
	 * @param OutError Error message if creation failed (output)
	 * @return True if emitter was created successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Niagara")
	static bool CreateEmitterFromDSL(
		const FVFXDSLEmitter& EmitterDSL,
		const FString& PackagePath,
		const FString& EmitterName,
		UNiagaraEmitter*& OutEmitter,
		FString& OutError
	);

	/**
	 * Configure spawn module from DSL spawners
	 * @param Emitter The emitter to configure
	 * @param SpawnersDSL The spawners DSL specification
	 * @param OutError Error message if configuration failed (output)
	 * @return True if configuration was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Niagara")
	static bool ConfigureSpawnModule(
		UNiagaraEmitter* Emitter,
		const FVFXDSLSpawners& SpawnersDSL,
		FString& OutError
	);

	/**
	 * Configure initialize particle module from DSL initialization
	 * @param Emitter The emitter to configure
	 * @param InitDSL The initialization DSL specification
	 * @param OutError Error message if configuration failed (output)
	 * @return True if configuration was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Niagara")
	static bool ConfigureInitializeModule(
		UNiagaraEmitter* Emitter,
		const FVFXDSLInitialization& InitDSL,
		FString& OutError
	);

	/**
	 * Configure update module from DSL update section
	 * @param Emitter The emitter to configure
	 * @param UpdateDSL The update DSL specification
	 * @param OutError Error message if configuration failed (output)
	 * @return True if configuration was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Niagara")
	static bool ConfigureUpdateModule(
		UNiagaraEmitter* Emitter,
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
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Niagara")
	static bool ConfigureRenderModule(
		UNiagaraEmitter* Emitter,
		const FVFXDSLRender& RenderDSL,
		FString& OutError
	);

private:
	/**
	 * Set a float parameter value in a Niagara script
	 */
	static bool SetFloatParameter(UNiagaraScript* Script, const FName& ParameterName, float Value, FString& OutError);

	/**
	 * Set a vector parameter value in a Niagara script
	 */
	static bool SetVectorParameter(UNiagaraScript* Script, const FName& ParameterName, const FVector& Value, FString& OutError);

	/**
	 * Set a color parameter value in a Niagara script
	 */
	static bool SetColorParameter(UNiagaraScript* Script, const FName& ParameterName, const FLinearColor& Value, FString& OutError);

	/**
	 * Set an integer parameter value in a Niagara script
	 */
	static bool SetIntParameter(UNiagaraScript* Script, const FName& ParameterName, int32 Value, FString& OutError);

	/**
	 * Set a boolean parameter value in a Niagara script
	 */
	static bool SetBoolParameter(UNiagaraScript* Script, const FName& ParameterName, bool Value, FString& OutError);
};

