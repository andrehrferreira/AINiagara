// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraSystem.h"
#include "NiagaraEmitter.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleEmitter.h"
#include "TextureMaterialHelper.generated.h"

/**
 * Helper class for applying generated textures to emitter materials
 */
UCLASS()
class AINIAGARA_API UTextureMaterialHelper : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Apply a texture to a Niagara emitter's material
	 * @param Emitter Niagara emitter to modify
	 * @param Texture Texture to apply
	 * @param ParameterName Material parameter name (default: "Texture")
	 * @param OutError Error message if failed
	 * @return True if texture was applied successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Texture")
	static bool ApplyTextureToNiagaraEmitter(
		UNiagaraEmitter* Emitter,
		UTexture2D* Texture,
		const FName& ParameterName,
		FString& OutError
	);

	/**
	 * Apply a texture to a Cascade emitter's material
	 * @param Emitter Cascade emitter to modify
	 * @param Texture Texture to apply
	 * @param ParameterName Material parameter name (default: "Texture")
	 * @param OutError Error message if failed
	 * @return True if texture was applied successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Texture")
	static bool ApplyTextureToCascadeEmitter(
		UParticleEmitter* Emitter,
		UTexture2D* Texture,
		const FName& ParameterName,
		FString& OutError
	);

	/**
	 * Apply a texture to a Niagara system (all emitters)
	 * @param System Niagara system to modify
	 * @param Texture Texture to apply
	 * @param EmitterName Optional: specific emitter name (empty = all emitters)
	 * @param ParameterName Material parameter name (default: "Texture")
	 * @param OutError Error message if failed
	 * @return True if texture was applied successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Texture")
	static bool ApplyTextureToNiagaraSystem(
		UNiagaraSystem* System,
		UTexture2D* Texture,
		const FString& EmitterName,
		const FName& ParameterName,
		FString& OutError
	);

	/**
	 * Apply a texture to a Cascade system (all emitters)
	 * @param System Cascade system to modify
	 * @param Texture Texture to apply
	 * @param EmitterName Optional: specific emitter name (empty = all emitters)
	 * @param ParameterName Material parameter name (default: "Texture")
	 * @param OutError Error message if failed
	 * @return True if texture was applied successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Texture")
	static bool ApplyTextureToCascadeSystem(
		UParticleSystem* System,
		UTexture2D* Texture,
		const FString& EmitterName,
		const FName& ParameterName,
		FString& OutError
	);

	/**
	 * Create a dynamic material instance from an emitter's material
	 * @param BaseMaterial Base material to create instance from
	 * @param OutMaterial Created dynamic material instance
	 * @param OutError Error message if failed
	 * @return True if material instance was created successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Texture")
	static bool CreateDynamicMaterialInstance(
		UMaterialInterface* BaseMaterial,
		UMaterialInstanceDynamic*& OutMaterial,
		FString& OutError
	);

	/**
	 * Set a texture parameter on a material instance
	 * @param Material Material instance to modify
	 * @param ParameterName Parameter name
	 * @param Texture Texture to set
	 * @return True if parameter was set successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Texture")
	static bool SetMaterialTextureParameter(
		UMaterialInstanceDynamic* Material,
		const FName& ParameterName,
		UTexture2D* Texture
	);

	/**
	 * Get common texture parameter names used in particle materials
	 * @return Array of common parameter names
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Texture")
	static TArray<FName> GetCommonTextureParameterNames();
};

