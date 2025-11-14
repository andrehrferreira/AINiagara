// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Core/VFXDSL.h"
#include "NiagaraSystemToDSLConverter.generated.h"

class UNiagaraSystem;
class UNiagaraEmitter;

/**
 * Converter for Niagara systems to DSL specifications (Reverse Engineering)
 */
UCLASS(BlueprintType)
class AINIAGARA_API UNiagaraSystemToDSLConverter : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Convert a Niagara system to DSL specification
	 * @param NiagaraSystem The Niagara system to convert
	 * @param OutDSL The resulting DSL specification (output)
	 * @param OutError Error message if conversion failed (output)
	 * @return True if conversion was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|ReverseEngineering")
	static bool ConvertSystemToDSL(
		UNiagaraSystem* NiagaraSystem,
		FVFXDSL& OutDSL,
		FString& OutError
	);

	/**
	 * Convert a Niagara emitter to DSL emitter specification
	 * @param Emitter The Niagara emitter to convert
	 * @param OutEmitterDSL The resulting emitter DSL (output)
	 * @param OutError Error message if conversion failed (output)
	 * @return True if conversion was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|ReverseEngineering")
	static bool ConvertEmitterToDSL(
		UNiagaraEmitter* Emitter,
		FVFXDSLEmitter& OutEmitterDSL,
		FString& OutError
	);

	/**
	 * Export DSL to JSON string
	 * @param DSL The DSL to export
	 * @param OutJson The resulting JSON string (output)
	 * @param bPrettyPrint Whether to pretty-print the JSON
	 * @return True if export was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|ReverseEngineering")
	static bool ExportDSLToJSON(
		const FVFXDSL& DSL,
		FString& OutJson,
		bool bPrettyPrint = true
	);

	/**
	 * Export DSL to file
	 * @param DSL The DSL to export
	 * @param FilePath The file path to save to
	 * @return True if export was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|ReverseEngineering")
	static bool ExportDSLToFile(
		const FVFXDSL& DSL,
		const FString& FilePath
	);

private:
	/**
	 * Extract spawner configuration from emitter
	 */
	static bool ExtractSpawnerConfig(
		UNiagaraEmitter* Emitter,
		FVFXDSLSpawners& OutSpawners,
		FString& OutError
	);

	/**
	 * Extract initialization configuration from emitter
	 */
	static bool ExtractInitializationConfig(
		UNiagaraEmitter* Emitter,
		FVFXDSLInitialization& OutInitialization,
		FString& OutError
	);

	/**
	 * Extract update configuration from emitter
	 */
	static bool ExtractUpdateConfig(
		UNiagaraEmitter* Emitter,
		FVFXDSLUpdate& OutUpdate,
		FString& OutError
	);

	/**
	 * Extract render configuration from emitter
	 */
	static bool ExtractRenderConfig(
		UNiagaraEmitter* Emitter,
		FVFXDSLRender& OutRender,
		FString& OutError
	);
};

