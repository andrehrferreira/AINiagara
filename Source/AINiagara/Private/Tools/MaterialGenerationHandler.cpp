// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tools/MaterialGenerationHandler.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionAdd.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Engine/Texture2D.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "UObject/Package.h"
#include "Misc/PackageName.h"

void UMaterialGenerationHandler::GenerateMaterial(
	const FMaterialGenerationRequest& Request,
	const FString& PackagePath,
	FOnMaterialGenerated OnComplete
)
{
	// Validate request
	FString ValidationError;
	if (!ValidateRequest(Request, ValidationError))
	{
		FMaterialGenerationResult ErrorResult;
		ErrorResult.bSuccess = false;
		ErrorResult.ErrorMessage = ValidationError;
		OnComplete.ExecuteIfBound(ErrorResult);
		return;
	}

	// Generate material name if not provided
	FString MaterialName = Request.MaterialName;
	if (MaterialName.IsEmpty())
	{
		MaterialName = TEXT("M_GeneratedMaterial_") + FGuid::NewGuid().ToString();
	}

	// Create base material
	UMaterial* BaseMaterial = nullptr;
	FString CreateError;
	if (!CreateBaseMaterial(Request, PackagePath, BaseMaterial, CreateError))
	{
		FMaterialGenerationResult ErrorResult;
		ErrorResult.bSuccess = false;
		ErrorResult.ErrorMessage = CreateError;
		OnComplete.ExecuteIfBound(ErrorResult);
		return;
	}

	// Create material instance if we have texture bindings or need customization
	UMaterialInterface* FinalMaterial = BaseMaterial;
	
	if (Request.TextureBindings.Num() > 0 || !Request.ShaderReference.IsEmpty())
	{
		UMaterialInstanceConstant* MaterialInstance = nullptr;
		FString InstanceError;
		
		if (CreateMaterialInstance(BaseMaterial, MaterialName + TEXT("_Inst"), PackagePath, MaterialInstance, InstanceError))
		{
			// Apply texture bindings
			if (Request.TextureBindings.Num() > 0)
			{
				FString BindingError;
				if (!ApplyTextureBindings(MaterialInstance, Request.TextureBindings, BindingError))
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to apply texture bindings: %s"), *BindingError);
				}
			}
			
			FinalMaterial = MaterialInstance;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create material instance, using base material: %s"), *InstanceError);
		}
	}

	// Set blend mode
	if (UMaterial* MaterialToModify = Cast<UMaterial>(FinalMaterial))
	{
		SetMaterialBlendMode(MaterialToModify, Request.BlendMode);
	}

	// Build result
	FMaterialGenerationResult Result;
	Result.bSuccess = true;
	Result.Material = FinalMaterial;
	Result.MaterialPath = FinalMaterial->GetPathName();

	// Call completion callback
	OnComplete.ExecuteIfBound(Result);
}

bool UMaterialGenerationHandler::CreateBaseMaterial(
	const FMaterialGenerationRequest& Request,
	const FString& PackagePath,
	UMaterial*& OutMaterial,
	FString& OutError
)
{
	// Generate material name
	FString MaterialName = Request.MaterialName;
	if (MaterialName.IsEmpty())
	{
		MaterialName = TEXT("M_GeneratedMaterial_") + FGuid::NewGuid().ToString();
	}

	// Create package
	FString FullPackagePath = PackagePath + TEXT("/") + MaterialName;
	UPackage* Package = CreatePackage(*FullPackagePath);
	if (!Package)
	{
		OutError = FString::Printf(TEXT("Failed to create package at path: %s"), *FullPackagePath);
		return false;
	}

	// Create material
	OutMaterial = NewObject<UMaterial>(Package, *MaterialName, RF_Public | RF_Standalone);
	if (!OutMaterial)
	{
		OutError = TEXT("Failed to create UMaterial object");
		return false;
	}

	// Set blend mode
	SetMaterialBlendMode(OutMaterial, Request.BlendMode);

	// Parse properties JSON if provided
	if (!Request.Properties.IsEmpty())
	{
		TSharedPtr<FJsonObject> PropertiesObj;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Request.Properties);

		if (FJsonSerializer::Deserialize(Reader, PropertiesObj) && PropertiesObj.IsValid())
		{
			// Set base color if specified
			if (PropertiesObj->HasTypedField<EJson::Object>(TEXT("BaseColor")))
			{
				TSharedPtr<FJsonObject> ColorObj = PropertiesObj->GetObjectField(TEXT("BaseColor"));
				float R = ColorObj->GetNumberField(TEXT("R"));
				float G = ColorObj->GetNumberField(TEXT("G"));
				float B = ColorObj->GetNumberField(TEXT("B"));
				float A = ColorObj->GetNumberField(TEXT("A"));

				// Create constant expression for base color
				UMaterialExpressionConstant3Vector* BaseColorExpr = NewObject<UMaterialExpressionConstant3Vector>(OutMaterial);
				BaseColorExpr->Constant = FLinearColor(R, G, B, A);
				OutMaterial->Expressions.Add(BaseColorExpr);
				OutMaterial->BaseColor.Expression = BaseColorExpr;
			}

			// Set emissive color if specified
			if (PropertiesObj->HasTypedField<EJson::Object>(TEXT("EmissiveColor")))
			{
				TSharedPtr<FJsonObject> ColorObj = PropertiesObj->GetObjectField(TEXT("EmissiveColor"));
				float R = ColorObj->GetNumberField(TEXT("R"));
				float G = ColorObj->GetNumberField(TEXT("G"));
				float B = ColorObj->GetNumberField(TEXT("B"));

				UMaterialExpressionConstant3Vector* EmissiveExpr = NewObject<UMaterialExpressionConstant3Vector>(OutMaterial);
				EmissiveExpr->Constant = FLinearColor(R, G, B, 1.0f);
				OutMaterial->Expressions.Add(EmissiveExpr);
				OutMaterial->EmissiveColor.Expression = EmissiveExpr;
			}

			// Set opacity if specified
			if (PropertiesObj->HasTypedField<EJson::Number>(TEXT("Opacity")))
			{
				float Opacity = PropertiesObj->GetNumberField(TEXT("Opacity"));
				UMaterialExpressionConstant* OpacityExpr = NewObject<UMaterialExpressionConstant>(OutMaterial);
				OpacityExpr->Value = Opacity;
				OutMaterial->Expressions.Add(OpacityExpr);
				OutMaterial->Opacity.Expression = OpacityExpr;
			}
		}
	}

	// Mark package as dirty
	Package->MarkPackageDirty();

	// Notify asset registry
	FAssetRegistryModule::AssetCreated(OutMaterial);

	// Compile material
	OutMaterial->PostEditChange();

	UE_LOG(LogTemp, Log, TEXT("Created base material '%s' at %s"), *MaterialName, *FullPackagePath);
	return true;
}

bool UMaterialGenerationHandler::CreateMaterialInstance(
	UMaterialInterface* BaseMaterial,
	const FString& MaterialName,
	const FString& PackagePath,
	UMaterialInstanceConstant*& OutMaterial,
	FString& OutError
)
{
	if (!BaseMaterial)
	{
		OutError = TEXT("Base material is null");
		return false;
	}

	// Create package
	FString FullPackagePath = PackagePath + TEXT("/") + MaterialName;
	UPackage* Package = CreatePackage(*FullPackagePath);
	if (!Package)
	{
		OutError = FString::Printf(TEXT("Failed to create package at path: %s"), *FullPackagePath);
		return false;
	}

	// Create material instance
	OutMaterial = NewObject<UMaterialInstanceConstant>(Package, *MaterialName, RF_Public | RF_Standalone);
	if (!OutMaterial)
	{
		OutError = TEXT("Failed to create UMaterialInstanceConstant object");
		return false;
	}

	// Set parent material
	OutMaterial->SetParentEditorOnly(BaseMaterial);

	// Mark package as dirty
	Package->MarkPackageDirty();

	// Notify asset registry
	FAssetRegistryModule::AssetCreated(OutMaterial);

	// Compile material instance
	OutMaterial->PostEditChange();

	UE_LOG(LogTemp, Log, TEXT("Created material instance '%s' at %s"), *MaterialName, *FullPackagePath);
	return true;
}

bool UMaterialGenerationHandler::ApplyTextureBindings(
	UMaterialInstanceConstant* Material,
	const TMap<FString, FString>& TextureBindings,
	FString& OutError
)
{
	if (!Material)
	{
		OutError = TEXT("Material is null");
		return false;
	}

	// Apply each texture binding
	for (const auto& Binding : TextureBindings)
	{
		FName ParameterName = FName(*Binding.Key);
		FString TexturePath = Binding.Value;

		// Load texture
		UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, *TexturePath);
		if (!Texture)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load texture at path: %s"), *TexturePath);
			continue;
		}

		// Set texture parameter
		Material->SetTextureParameterValueEditorOnly(ParameterName, Texture);
	}

	// Compile material instance
	Material->PostEditChange();

	return true;
}

bool UMaterialGenerationHandler::SetMaterialBlendMode(UMaterial* Material, const FString& BlendMode)
{
	if (!Material)
	{
		return false;
	}

	FString LowerBlendMode = BlendMode.ToLower();
	
	if (LowerBlendMode.Contains(TEXT("opaque")))
	{
		Material->BlendMode = BLEND_Opaque;
	}
	else if (LowerBlendMode.Contains(TEXT("translucent")))
	{
		Material->BlendMode = BLEND_Translucent;
	}
	else if (LowerBlendMode.Contains(TEXT("additive")))
	{
		Material->BlendMode = BLEND_Additive;
	}
	else if (LowerBlendMode.Contains(TEXT("modulate")))
	{
		Material->BlendMode = BLEND_Modulate;
	}
	else
	{
		Material->BlendMode = BLEND_Translucent; // Default
	}

	Material->PostEditChange();
	return true;
}

bool UMaterialGenerationHandler::ValidateRequest(const FMaterialGenerationRequest& Request, FString& OutError)
{
	// Check material name or properties
	if (Request.MaterialName.IsEmpty() && Request.Properties.IsEmpty())
	{
		OutError = TEXT("Material name or properties must be provided");
		return false;
	}

	// Validate blend mode
	FString LowerBlendMode = Request.BlendMode.ToLower();
	if (!LowerBlendMode.Contains(TEXT("opaque")) &&
	    !LowerBlendMode.Contains(TEXT("translucent")) &&
	    !LowerBlendMode.Contains(TEXT("additive")) &&
	    !LowerBlendMode.Contains(TEXT("modulate")))
	{
		OutError = FString::Printf(
			TEXT("Invalid blend mode '%s'. Must be Opaque, Translucent, Additive, or Modulate"),
			*Request.BlendMode
		);
		return false;
	}

	return true;
}

