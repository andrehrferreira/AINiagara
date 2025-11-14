// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tools/TextureMaterialHelper.h"
#include "NiagaraSystem.h"
#include "NiagaraEmitter.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleEmitter.h"
#include "Particles/ParticleModuleRequired.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Engine/Texture2D.h"

bool UTextureMaterialHelper::ApplyTextureToNiagaraEmitter(
	UNiagaraEmitter* Emitter,
	UTexture2D* Texture,
	const FName& ParameterName,
	FString& OutError
)
{
	if (!Emitter)
	{
		OutError = TEXT("Emitter is null");
		return false;
	}

	if (!Texture)
	{
		OutError = TEXT("Texture is null");
		return false;
	}

	// Note: Niagara texture parameters are complex and require graph manipulation
	// This is a simplified placeholder that would need full implementation
	// For now, we log a warning and return success to avoid breaking the workflow
	
	UE_LOG(LogTemp, Warning, TEXT("ApplyTextureToNiagaraEmitter: Niagara texture application requires graph manipulation (not yet implemented). Texture: %s"), *Texture->GetName());
	
	// TODO: Implement Niagara graph manipulation to set texture parameters
	// This requires:
	// 1. Access to the emitter's render module
	// 2. Finding or creating a sprite renderer
	// 3. Setting the sprite texture parameter
	// 4. Compiling the emitter graph
	
	OutError = TEXT("Niagara texture application not yet fully implemented (requires graph API)");
	return false;
}

bool UTextureMaterialHelper::ApplyTextureToCascadeEmitter(
	UParticleEmitter* Emitter,
	UTexture2D* Texture,
	const FName& ParameterName,
	FString& OutError
)
{
	if (!Emitter)
	{
		OutError = TEXT("Emitter is null");
		return false;
	}

	if (!Texture)
	{
		OutError = TEXT("Texture is null");
		return false;
	}

	// Find the Required module (contains material)
	UParticleModuleRequired* RequiredModule = nullptr;
	for (UParticleModule* Module : Emitter->RequiredModules)
	{
		if (UParticleModuleRequired* Required = Cast<UParticleModuleRequired>(Module))
		{
			RequiredModule = Required;
			break;
		}
	}

	if (!RequiredModule)
	{
		OutError = TEXT("Could not find Required module in Cascade emitter");
		return false;
	}

	// Get the current material
	UMaterialInterface* BaseMaterial = RequiredModule->Material;
	if (!BaseMaterial)
	{
		// Create a default particle material if none exists
		BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
		if (!BaseMaterial)
		{
			OutError = TEXT("No material assigned to emitter and failed to load default material");
			return false;
		}
	}

	// Create dynamic material instance
	UMaterialInstanceDynamic* DynamicMaterial = nullptr;
	FString MaterialError;
	if (!CreateDynamicMaterialInstance(BaseMaterial, DynamicMaterial, MaterialError))
	{
		OutError = FString::Printf(TEXT("Failed to create dynamic material: %s"), *MaterialError);
		return false;
	}

	// Set texture parameter
	if (!SetMaterialTextureParameter(DynamicMaterial, ParameterName, Texture))
	{
		OutError = FString::Printf(TEXT("Failed to set texture parameter '%s' on material"), *ParameterName.ToString());
		return false;
	}

	// Apply dynamic material to emitter
	RequiredModule->Material = DynamicMaterial;
	Emitter->MarkPackageDirty();

	UE_LOG(LogTemp, Log, TEXT("Applied texture '%s' to Cascade emitter with parameter '%s'"), 
		*Texture->GetName(), *ParameterName.ToString());

	return true;
}

bool UTextureMaterialHelper::ApplyTextureToNiagaraSystem(
	UNiagaraSystem* System,
	UTexture2D* Texture,
	const FString& EmitterName,
	const FName& ParameterName,
	FString& OutError
)
{
	if (!System)
	{
		OutError = TEXT("System is null");
		return false;
	}

	if (!Texture)
	{
		OutError = TEXT("Texture is null");
		return false;
	}

	// Note: This requires Niagara graph API access
	// For now, return a helpful error message
	
	OutError = TEXT("Niagara system texture application not yet fully implemented. Use Cascade systems or apply textures manually.");
	UE_LOG(LogTemp, Warning, TEXT("ApplyTextureToNiagaraSystem: Not yet implemented (requires Niagara graph API)"));
	
	return false;
}

bool UTextureMaterialHelper::ApplyTextureToCascadeSystem(
	UParticleSystem* System,
	UTexture2D* Texture,
	const FString& EmitterName,
	const FName& ParameterName,
	FString& OutError
)
{
	if (!System)
	{
		OutError = TEXT("System is null");
		return false;
	}

	if (!Texture)
	{
		OutError = TEXT("Texture is null");
		return false;
	}

	bool bAppliedToAny = false;
	FString LastError;

	// Apply to all emitters (or specific emitter if name provided)
	for (UParticleEmitter* Emitter : System->Emitters)
	{
		if (!Emitter)
		{
			continue;
		}

		// Check if we should apply to this emitter
		bool bShouldApply = EmitterName.IsEmpty() || Emitter->GetEmitterName().ToString().Equals(EmitterName);
		
		if (bShouldApply)
		{
			FString EmitterError;
			if (ApplyTextureToCascadeEmitter(Emitter, Texture, ParameterName, EmitterError))
			{
				bAppliedToAny = true;
			}
			else
			{
				LastError = EmitterError;
			}
		}
	}

	if (!bAppliedToAny)
	{
		if (EmitterName.IsEmpty())
		{
			OutError = FString::Printf(TEXT("Failed to apply texture to any emitter: %s"), *LastError);
		}
		else
		{
			OutError = FString::Printf(TEXT("Failed to find or apply texture to emitter '%s': %s"), *EmitterName, *LastError);
		}
		return false;
	}

	System->MarkPackageDirty();
	return true;
}

bool UTextureMaterialHelper::CreateDynamicMaterialInstance(
	UMaterialInterface* BaseMaterial,
	UMaterialInstanceDynamic*& OutMaterial,
	FString& OutError
)
{
	if (!BaseMaterial)
	{
		OutError = TEXT("Base material is null");
		return false;
	}

	// Create dynamic material instance
	OutMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, nullptr);
	if (!OutMaterial)
	{
		OutError = TEXT("Failed to create UMaterialInstanceDynamic");
		return false;
	}

	return true;
}

bool UTextureMaterialHelper::SetMaterialTextureParameter(
	UMaterialInstanceDynamic* Material,
	const FName& ParameterName,
	UTexture2D* Texture
)
{
	if (!Material)
	{
		UE_LOG(LogTemp, Error, TEXT("Material is null"));
		return false;
	}

	if (!Texture)
	{
		UE_LOG(LogTemp, Error, TEXT("Texture is null"));
		return false;
	}

	// Set texture parameter
	Material->SetTextureParameterValue(ParameterName, Texture);

	// Also try common variations of the parameter name
	TArray<FName> CommonNames = GetCommonTextureParameterNames();
	for (const FName& CommonName : CommonNames)
	{
		if (CommonName != ParameterName)
		{
			Material->SetTextureParameterValue(CommonName, Texture);
		}
	}

	return true;
}

TArray<FName> UTextureMaterialHelper::GetCommonTextureParameterNames()
{
	TArray<FName> Names;
	Names.Add(FName(TEXT("Texture")));
	Names.Add(FName(TEXT("BaseTexture")));
	Names.Add(FName(TEXT("DiffuseTexture")));
	Names.Add(FName(TEXT("SpriteTexture")));
	Names.Add(FName(TEXT("ParticleTexture")));
	Names.Add(FName(TEXT("MainTexture")));
	Names.Add(FName(TEXT("Albedo")));
	Names.Add(FName(TEXT("BaseColor")));
	return Names;
}

