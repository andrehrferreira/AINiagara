// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/VFXDSL.h"
#include "VFXDSLParser.generated.h"

/**
 * Parser for VFX DSL JSON format
 */
UCLASS(BlueprintType)
class AINIAGARA_API UVFXDSLParser : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Parse DSL from JSON string
	 * @param JsonString JSON string to parse
	 * @param OutDSL Output DSL structure
	 * @param OutError Error message if parsing fails
	 * @return True if parsing succeeded
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|DSL")
	static bool ParseFromJSON(const FString& JsonString, FVFXDSL& OutDSL, FString& OutError);

	/**
	 * Convert DSL to JSON string
	 * @param DSL DSL structure to convert
	 * @param OutJsonString Output JSON string
	 * @return True if conversion succeeded
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|DSL")
	static bool ToJSON(const FVFXDSL& DSL, FString& OutJsonString);

private:
	/**
	 * Parse effect from JSON object
	 */
	static bool ParseEffect(const TSharedPtr<FJsonObject>& JsonObject, FVFXDSLEffect& OutEffect);

	/**
	 * Parse emitter from JSON object
	 */
	static bool ParseEmitter(const TSharedPtr<FJsonObject>& JsonObject, FVFXDSLEmitter& OutEmitter);

	/**
	 * Parse spawners from JSON object
	 */
	static bool ParseSpawners(const TSharedPtr<FJsonObject>& JsonObject, FVFXDSLSpawners& OutSpawners);

	/**
	 * Parse initialization from JSON object
	 */
	static bool ParseInitialization(const TSharedPtr<FJsonObject>& JsonObject, FVFXDSLInitialization& OutInit);

	/**
	 * Parse update from JSON object
	 */
	static bool ParseUpdate(const TSharedPtr<FJsonObject>& JsonObject, FVFXDSLUpdate& OutUpdate);

	/**
	 * Parse render from JSON object
	 */
	static bool ParseRender(const TSharedPtr<FJsonObject>& JsonObject, FVFXDSLRender& OutRender);

	/**
	 * Parse color from JSON object
	 */
	static bool ParseColor(const TSharedPtr<FJsonObject>& JsonObject, FVFXDSLColor& OutColor);

	/**
	 * Parse velocity from JSON object
	 */
	static bool ParseVelocity(const TSharedPtr<FJsonObject>& JsonObject, FVFXDSLVelocity& OutVelocity);

	/**
	 * Convert effect type string to enum
	 */
	static EVFXEffectType ParseEffectType(const FString& TypeString);
};

/**
 * Validator for VFX DSL structure
 */
UCLASS(BlueprintType)
class AINIAGARA_API UVFXDSLValidator : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Validate DSL structure
	 * @param DSL DSL structure to validate
	 * @return Validation result with errors if any
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|DSL")
	static FVFXDSLValidationResult Validate(const FVFXDSL& DSL);

	/**
	 * Validate effect structure
	 */
	static void ValidateEffect(const FVFXDSLEffect& Effect, FVFXDSLValidationResult& Result);

	/**
	 * Validate emitter structure
	 */
	static void ValidateEmitter(const FVFXDSLEmitter& Emitter, int32 EmitterIndex, FVFXDSLValidationResult& Result);

	/**
	 * Validate color values (0-1 range)
	 */
	static void ValidateColor(const FVFXDSLColor& Color, const FString& Context, FVFXDSLValidationResult& Result);

	/**
	 * Validate size values (must be positive)
	 */
	static void ValidateSize(const FVFXDSLSize& Size, const FString& Context, FVFXDSLValidationResult& Result);
};

