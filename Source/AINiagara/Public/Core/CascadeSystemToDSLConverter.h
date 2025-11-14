// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Core/VFXDSL.h"
#include "CascadeSystemToDSLConverter.generated.h"

class UParticleSystem;
class UParticleEmitter;

/**
 * Converter for Cascade particle systems to DSL specifications (Reverse Engineering)
 */
UCLASS(BlueprintType)
class AINIAGARA_API UCascadeSystemToDSLConverter : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Convert a Cascade particle system to DSL specification
	 * @param ParticleSystem The Cascade particle system to convert
	 * @param OutDSL The resulting DSL specification (output)
	 * @param OutError Error message if conversion failed (output)
	 * @return True if conversion was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|ReverseEngineering")
	static bool ConvertSystemToDSL(
		UParticleSystem* ParticleSystem,
		FVFXDSL& OutDSL,
		FString& OutError
	);

	/**
	 * Convert a Cascade emitter to DSL emitter specification
	 * @param Emitter The Cascade emitter to convert
	 * @param OutEmitterDSL The resulting emitter DSL (output)
	 * @param OutError Error message if conversion failed (output)
	 * @return True if conversion was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|ReverseEngineering")
	static bool ConvertEmitterToDSL(
		UParticleEmitter* Emitter,
		FVFXDSLEmitter& OutEmitterDSL,
		FString& OutError
	);

private:
	/**
	 * Extract spawner configuration from Cascade emitter
	 */
	static bool ExtractSpawnerConfig(
		UParticleEmitter* Emitter,
		FVFXDSLSpawners& OutSpawners,
		FString& OutError
	);

	/**
	 * Extract initialization configuration from Cascade emitter
	 */
	static bool ExtractInitializationConfig(
		UParticleEmitter* Emitter,
		FVFXDSLInitialization& OutInitialization,
		FString& OutError
	);

	/**
	 * Extract update configuration from Cascade emitter
	 */
	static bool ExtractUpdateConfig(
		UParticleEmitter* Emitter,
		FVFXDSLUpdate& OutUpdate,
		FString& OutError
	);

	/**
	 * Extract render configuration from Cascade emitter
	 */
	static bool ExtractRenderConfig(
		UParticleEmitter* Emitter,
		FVFXDSLRender& OutRender,
		FString& OutError
	);
};

