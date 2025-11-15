// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "UObject/NoExportTypes.h"
#include "TextureGenerationHandler.generated.h"

/**
 * Texture generation request parameters
 */
USTRUCT()
struct FTextureGenerationRequest
{
	GENERATED_BODY()

	/** Texture description prompt */
	UPROPERTY()
	FString Prompt;

	/** Texture type (noise, fire, smoke, sparks, distortion, etc.) */
	UPROPERTY()
	FString TextureType;

	/** Resolution (width and height, must be power of 2) */
	UPROPERTY()
	int32 Resolution = 512;

	/** Color scheme/palette */
	UPROPERTY()
	FString ColorScheme;

	/** Number of frames for flipbook (1 for single texture) */
	UPROPERTY()
	int32 Frames = 1;

	/** Target emitter to apply texture to */
	UPROPERTY()
	FString TargetEmitterName;
};

/**
 * Result of texture generation
 */
USTRUCT()
struct FTextureGenerationResult
{
	GENERATED_BODY()

	/** Generated texture asset */
	UPROPERTY()
	TObjectPtr<UTexture2D> Texture = nullptr;

	/** Base64-encoded PNG data (for debugging/export) */
	UPROPERTY()
	FString Base64Data;

	/** Success status */
	UPROPERTY()
	bool bSuccess = false;

	/** Error message if failed */
	UPROPERTY()
	FString ErrorMessage;

	/** Number of frames in flipbook (1 for single texture) */
	UPROPERTY()
	int32 FrameCount = 1;
};

/**
 * Delegate called when texture generation completes
 */
DECLARE_DELEGATE_OneParam(FOnTextureGenerated, const FTextureGenerationResult&);

/**
 * Handler for texture generation using Gemini Imagen 3 API
 * Processes tool:texture function calls from LLM
 */
UCLASS()
class AINIAGARA_API UTextureGenerationHandler : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Generate a single texture from Imagen 3 API
	 * @param Request Texture generation parameters
	 * @param OnComplete Callback when generation completes
	 */
	static void GenerateTexture(
		const FTextureGenerationRequest& Request,
		FOnTextureGenerated OnComplete
	);

	/**
	 * Generate a flipbook texture sequence from Imagen 3 API
	 * @param Request Texture generation parameters (with Frames > 1)
	 * @param OnComplete Callback when generation completes
	 */
	static void GenerateFlipbook(
		const FTextureGenerationRequest& Request,
		FOnTextureGenerated OnComplete
	);

	/**
	 * Create UTexture2D from base64-encoded PNG data
	 * @param Base64Data Base64-encoded PNG image
	 * @param TextureName Name for the texture asset
	 * @param OutTexture Created texture
	 * @return True if texture was created successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Texture")
	static bool CreateTextureFromBase64(
		const FString& Base64Data,
		const FString& TextureName,
		UTexture2D*& OutTexture
	);

	/**
	 * Create UTexture2D from raw PNG byte array
	 * @param PNGData Raw PNG bytes
	 * @param TextureName Name for the texture asset
	 * @param OutTexture Created texture
	 * @return True if texture was created successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Texture")
	static bool CreateTextureFromPNG(
		const TArray<uint8>& PNGData,
		const FString& TextureName,
		UTexture2D*& OutTexture
	);

	/**
	 * Create a flipbook atlas texture from multiple frames
	 * @param FrameTextures Array of frame textures
	 * @param AtlasName Name for the atlas texture
	 * @param FramesX Number of frames in X direction
	 * @param FramesY Number of frames in Y direction
	 * @param OutAtlas Created atlas texture
	 * @return True if atlas was created successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Texture")
	static bool CreateFlipbookAtlas(
		const TArray<UTexture2D*>& FrameTextures,
		const FString& AtlasName,
		int32 FramesX,
		int32 FramesY,
		UTexture2D*& OutAtlas
	);

	/**
	 * Decode base64 string to byte array
	 * @param Base64String Base64-encoded string
	 * @param OutBytes Decoded bytes
	 * @return True if decoding succeeded
	 */
	static bool DecodeBase64(const FString& Base64String, TArray<uint8>& OutBytes);

	/**
	 * Validate texture generation request
	 * @param Request Request to validate
	 * @param OutError Error message if invalid
	 * @return True if request is valid
	 * Note: Not exposed to Blueprint because FTextureGenerationRequest is not supported by Blueprint
	 */
	static bool ValidateRequest(const FTextureGenerationRequest& Request, FString& OutError);

private:
	/**
	 * Generate a single texture frame
	 * @param Request Generation parameters
	 * @param FrameIndex Frame index (for flipbooks)
	 * @param OnComplete Callback when complete
	 */
	static void GenerateSingleFrame(
		const FTextureGenerationRequest& Request,
		int32 FrameIndex,
		FOnTextureGenerated OnComplete
	);
};

