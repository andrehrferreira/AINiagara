// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialInterface.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "UObject/NoExportTypes.h"
#include "MaterialGenerationHandler.generated.h"

/**
 * Material generation request parameters
 */
USTRUCT()
struct FMaterialGenerationRequest
{
	GENERATED_BODY()

	/** Material properties (JSON string or description) */
	UPROPERTY()
	FString Properties;

	/** Shader reference (path to shader or HLSL code) */
	UPROPERTY()
	FString ShaderReference;

	/** Texture bindings (map of parameter name to texture path) */
	UPROPERTY()
	TMap<FString, FString> TextureBindings;

	/** Material name */
	UPROPERTY()
	FString MaterialName;

	/** Blend mode (Opaque, Translucent, Additive, Modulate) */
	UPROPERTY()
	FString BlendMode = TEXT("Translucent");

	/** Shading model (DefaultLit, Unlit, etc.) */
	UPROPERTY()
	FString ShadingModel = TEXT("DefaultLit");
};

/**
 * Result of material generation
 */
USTRUCT()
struct FMaterialGenerationResult
{
	GENERATED_BODY()

	/** Generated material asset */
	UPROPERTY()
	TObjectPtr<UMaterialInterface> Material = nullptr;

	/** Success status */
	UPROPERTY()
	bool bSuccess = false;

	/** Error message if failed */
	UPROPERTY()
	FString ErrorMessage;

	/** Material asset path */
	UPROPERTY()
	FString MaterialPath;
};

/**
 * Delegate called when material generation completes
 */
DECLARE_DELEGATE_OneParam(FOnMaterialGenerated, const FMaterialGenerationResult&);

/**
 * Handler for material generation from specifications
 * Processes tool:material function calls from LLM
 */
UCLASS()
class AINIAGARA_API UMaterialGenerationHandler : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Generate a material from specifications
	 * @param Request Material generation parameters
	 * @param PackagePath Package path for the material asset
	 * @param OnComplete Callback when generation completes
	 */
	static void GenerateMaterial(
		const FMaterialGenerationRequest& Request,
		const FString& PackagePath,
		FOnMaterialGenerated OnComplete
	);

	/**
	 * Create a material instance from a base material
	 * @param BaseMaterial Base material to create instance from
	 * @param MaterialName Name for the instance
	 * @param PackagePath Package path
	 * @param OutMaterial Created material instance
	 * @param OutError Error message if failed
	 * @return True if material instance was created successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Material")
	static bool CreateMaterialInstance(
		UMaterialInterface* BaseMaterial,
		const FString& MaterialName,
		const FString& PackagePath,
		UMaterialInstanceConstant*& OutMaterial,
		FString& OutError
	);

	/**
	 * Apply texture bindings to a material
	 * @param Material Material to modify
	 * @param TextureBindings Map of parameter names to texture paths
	 * @param OutError Error message if failed
	 * @return True if textures were applied successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Material")
	static bool ApplyTextureBindings(
		UMaterialInstanceConstant* Material,
		const TMap<FString, FString>& TextureBindings,
		FString& OutError
	);

	/**
	 * Set material blend mode
	 * @param Material Material to modify
	 * @param BlendMode Blend mode string (Opaque, Translucent, Additive, Modulate)
	 * @return True if blend mode was set successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Material")
	static bool SetMaterialBlendMode(UMaterial* Material, const FString& BlendMode);

	/**
	 * Validate material generation request
	 * @param Request Request to validate
	 * @param OutError Error message if invalid
	 * @return True if request is valid
	 */
	static bool ValidateRequest(const FMaterialGenerationRequest& Request, FString& OutError);

private:
	/**
	 * Create a base material from specifications
	 * @param Request Generation parameters
	 * @param PackagePath Package path
	 * @param OutMaterial Created material
	 * @param OutError Error message if failed
	 * @return True if material was created successfully
	 */
	static bool CreateBaseMaterial(
		const FMaterialGenerationRequest& Request,
		const FString& PackagePath,
		UMaterial*& OutMaterial,
		FString& OutError
	);
};

