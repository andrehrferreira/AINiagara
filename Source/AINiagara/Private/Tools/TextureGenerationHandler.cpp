// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tools/TextureGenerationHandler.h"
#include "Core/GeminiAPIClient.h"
#include "Engine/Texture2D.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Misc/Base64.h"
#include "Modules/ModuleManager.h"
#include "TextureResource.h"
#include "RenderUtils.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"

void UTextureGenerationHandler::GenerateTexture(
	const FTextureGenerationRequest& Request,
	FOnTextureGenerated OnComplete
)
{
	// Validate request
	FString ValidationError;
	if (!ValidateRequest(Request, ValidationError))
	{
		FTextureGenerationResult ErrorResult;
		ErrorResult.bSuccess = false;
		ErrorResult.ErrorMessage = ValidationError;
		OnComplete.ExecuteIfBound(ErrorResult);
		return;
	}

	// Generate single frame
	if (Request.Frames == 1)
	{
		GenerateSingleFrame(Request, 0, OnComplete);
	}
	// Generate flipbook
	else
	{
		GenerateFlipbook(Request, OnComplete);
	}
}

void UTextureGenerationHandler::GenerateFlipbook(
	const FTextureGenerationRequest& Request,
	FOnTextureGenerated OnComplete
)
{
	// Store frame textures
	struct FFlipbookGenerationState
	{
		TArray<UTexture2D*> FrameTextures;
		int32 TotalFrames = 0;
		int32 CompletedFrames = 0;
		FTextureGenerationRequest OriginalRequest;
		FOnTextureGenerated Callback;
		FString LastError;
	};

	// Create shared state
	TSharedPtr<FFlipbookGenerationState> State = MakeShared<FFlipbookGenerationState>();
	State->TotalFrames = Request.Frames;
	State->OriginalRequest = Request;
	State->Callback = OnComplete;
	State->FrameTextures.SetNum(Request.Frames);

	// Generate each frame
	for (int32 FrameIndex = 0; FrameIndex < Request.Frames; ++FrameIndex)
	{
		FTextureGenerationRequest FrameRequest = Request;
		FrameRequest.Frames = 1;  // Single frame per request
		FrameRequest.Prompt = FString::Printf(
			TEXT("%s (frame %d of %d animation sequence)"),
			*Request.Prompt,
			FrameIndex + 1,
			Request.Frames
		);

		// Capture frame index by value
		GenerateSingleFrame(FrameRequest, FrameIndex, 
			FOnTextureGenerated::CreateLambda([State, FrameIndex](const FTextureGenerationResult& Result)
			{
				// Store frame texture
				if (Result.bSuccess && Result.Texture)
				{
					State->FrameTextures[FrameIndex] = Result.Texture;
				}
				else
				{
					State->LastError = Result.ErrorMessage;
				}

				// Increment completed count
				State->CompletedFrames++;

				// Check if all frames are complete
				if (State->CompletedFrames == State->TotalFrames)
				{
					FTextureGenerationResult FinalResult;

					// Check if any frame failed
					bool bAllSuccess = true;
					for (UTexture2D* FrameTexture : State->FrameTextures)
					{
						if (!FrameTexture)
						{
							bAllSuccess = false;
							break;
						}
					}

					if (bAllSuccess)
					{
						// Calculate atlas dimensions
						int32 FramesX = FMath::CeilToInt(FMath::Sqrt(static_cast<float>(State->TotalFrames)));
						int32 FramesY = FMath::CeilToInt(static_cast<float>(State->TotalFrames) / FramesX);

						// Create flipbook atlas
						UTexture2D* AtlasTexture = nullptr;
						FString AtlasName = FString::Printf(TEXT("T_Flipbook_%s"), *State->OriginalRequest.TextureType);
						
						if (CreateFlipbookAtlas(State->FrameTextures, AtlasName, FramesX, FramesY, AtlasTexture))
						{
							FinalResult.bSuccess = true;
							FinalResult.Texture = AtlasTexture;
							FinalResult.FrameCount = State->TotalFrames;
						}
						else
						{
							FinalResult.bSuccess = false;
							FinalResult.ErrorMessage = TEXT("Failed to create flipbook atlas");
						}
					}
					else
					{
						FinalResult.bSuccess = false;
						FinalResult.ErrorMessage = FString::Printf(
							TEXT("Failed to generate one or more frames: %s"),
							*State->LastError
						);
					}

					// Call completion callback
					State->Callback.ExecuteIfBound(FinalResult);
				}
			})
		);
	}
}

void UTextureGenerationHandler::GenerateSingleFrame(
	const FTextureGenerationRequest& Request,
	int32 FrameIndex,
	FOnTextureGenerated OnComplete
)
{
	// Create API client
	FGeminiAPIClient* APIClient = new FGeminiAPIClient();

	// Build prompt with color scheme if provided
	FString FullPrompt = Request.Prompt;
	if (!Request.ColorScheme.IsEmpty())
	{
		FullPrompt += FString::Printf(TEXT(" Color scheme: %s."), *Request.ColorScheme);
	}

	// Add texture-specific guidance
	FullPrompt += TEXT(" Generate a seamless, tileable texture suitable for particle effects.");

	// Request texture generation from Imagen 3
	APIClient->GenerateTexture(
		FullPrompt,
		Request.TextureType,
		Request.Resolution,
		FOnGeminiResponse::CreateLambda([APIClient, Request, OnComplete](const FString& ResponseText)
		{
			FTextureGenerationResult Result;

			// Parse response JSON to extract base64 image
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseText);

			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				// Try to extract image data from response
				// Imagen 3 returns: { "predictions": [{ "bytesBase64Encoded": "..." }] }
				// or { "candidates": [{ "content": { "parts": [{ "inlineData": { "data": "..." } }] } }] }
				
				FString Base64Data;
				
				// Try predictions format
				if (JsonObject->HasTypedField<EJson::Array>(TEXT("predictions")))
				{
					const TArray<TSharedPtr<FJsonValue>>& Predictions = JsonObject->GetArrayField(TEXT("predictions"));
					if (Predictions.Num() > 0)
					{
						TSharedPtr<FJsonObject> PredictionObj = Predictions[0]->AsObject();
						if (PredictionObj.IsValid() && PredictionObj->HasTypedField<EJson::String>(TEXT("bytesBase64Encoded")))
						{
							Base64Data = PredictionObj->GetStringField(TEXT("bytesBase64Encoded"));
						}
					}
				}
				// Try candidates format
				else if (JsonObject->HasTypedField<EJson::Array>(TEXT("candidates")))
				{
					const TArray<TSharedPtr<FJsonValue>>& Candidates = JsonObject->GetArrayField(TEXT("candidates"));
					if (Candidates.Num() > 0)
					{
						TSharedPtr<FJsonObject> CandidateObj = Candidates[0]->AsObject();
						if (CandidateObj.IsValid())
						{
							TSharedPtr<FJsonObject> ContentObj = CandidateObj->GetObjectField(TEXT("content"));
							if (ContentObj.IsValid())
							{
								const TArray<TSharedPtr<FJsonValue>>& Parts = ContentObj->GetArrayField(TEXT("parts"));
								if (Parts.Num() > 0)
								{
									TSharedPtr<FJsonObject> PartObj = Parts[0]->AsObject();
									if (PartObj.IsValid())
									{
										TSharedPtr<FJsonObject> InlineDataObj = PartObj->GetObjectField(TEXT("inlineData"));
										if (InlineDataObj.IsValid() && InlineDataObj->HasTypedField<EJson::String>(TEXT("data")))
										{
											Base64Data = InlineDataObj->GetStringField(TEXT("data"));
										}
									}
								}
							}
						}
					}
				}

				if (!Base64Data.IsEmpty())
				{
					// Create texture from base64 data
					FString TextureName = FString::Printf(TEXT("T_%s_%d"), *Request.TextureType, FMath::Rand());
					UTexture2D* GeneratedTexture = nullptr;

					if (CreateTextureFromBase64(Base64Data, TextureName, GeneratedTexture))
					{
						Result.bSuccess = true;
						Result.Texture = GeneratedTexture;
						Result.Base64Data = Base64Data;
						Result.FrameCount = 1;
					}
					else
					{
						Result.bSuccess = false;
						Result.ErrorMessage = TEXT("Failed to create texture from base64 data");
					}
				}
				else
				{
					Result.bSuccess = false;
					Result.ErrorMessage = TEXT("No image data found in Imagen 3 response");
				}
			}
			else
			{
				Result.bSuccess = false;
				Result.ErrorMessage = TEXT("Failed to parse Imagen 3 response JSON");
			}

			// Cleanup API client
			delete APIClient;

			// Call completion callback
			OnComplete.ExecuteIfBound(Result);
		}),
		FOnGeminiError::CreateLambda([APIClient, OnComplete](int32 ErrorCode, const FString& ErrorMessage)
		{
			FTextureGenerationResult Result;
			Result.bSuccess = false;
			Result.ErrorMessage = FString::Printf(
				TEXT("Imagen 3 API error %d: %s"),
				ErrorCode,
				*ErrorMessage
			);

			// Cleanup API client
			delete APIClient;

			// Call completion callback
			OnComplete.ExecuteIfBound(Result);
		})
	);
}

bool UTextureGenerationHandler::CreateTextureFromBase64(
	const FString& Base64Data,
	const FString& TextureName,
	UTexture2D*& OutTexture
)
{
	// Decode base64 to bytes
	TArray<uint8> ImageBytes;
	if (!DecodeBase64(Base64Data, ImageBytes))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to decode base64 texture data"));
		return false;
	}

	// Create texture from PNG bytes
	return CreateTextureFromPNG(ImageBytes, TextureName, OutTexture);
}

bool UTextureGenerationHandler::CreateTextureFromPNG(
	const TArray<uint8>& PNGData,
	const FString& TextureName,
	UTexture2D*& OutTexture
)
{
	// Get image wrapper module
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	if (!ImageWrapper.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create PNG image wrapper"));
		return false;
	}

	// Set compressed data
	if (!ImageWrapper->SetCompressed(PNGData.GetData(), PNGData.Num()))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to set compressed PNG data"));
		return false;
	}

	// Get raw image data
	TArray<uint8> RawData;
	if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to decompress PNG data"));
		return false;
	}

	// Get image dimensions
	const int32 Width = ImageWrapper->GetWidth();
	const int32 Height = ImageWrapper->GetHeight();

	// Create texture
	OutTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
	if (!OutTexture)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create transient texture"));
		return false;
	}

	// Copy data to texture
	void* TextureData = OutTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, RawData.GetData(), RawData.Num());
	OutTexture->GetPlatformData()->Mips[0].BulkData.Unlock();

	// Update texture
	OutTexture->UpdateResource();

	UE_LOG(LogTemp, Log, TEXT("Created texture '%s' (%dx%d)"), *TextureName, Width, Height);
	return true;
}

bool UTextureGenerationHandler::CreateFlipbookAtlas(
	const TArray<UTexture2D*>& FrameTextures,
	const FString& AtlasName,
	int32 FramesX,
	int32 FramesY,
	UTexture2D*& OutAtlas
)
{
	if (FrameTextures.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No frame textures provided for flipbook atlas"));
		return false;
	}

	// Validate all frames have the same size
	const int32 FrameWidth = FrameTextures[0]->GetSizeX();
	const int32 FrameHeight = FrameTextures[0]->GetSizeY();

	for (UTexture2D* Frame : FrameTextures)
	{
		if (!Frame || Frame->GetSizeX() != FrameWidth || Frame->GetSizeY() != FrameHeight)
		{
			UE_LOG(LogTemp, Error, TEXT("All frame textures must have the same dimensions"));
			return false;
		}
	}

	// Calculate atlas dimensions
	const int32 AtlasWidth = FrameWidth * FramesX;
	const int32 AtlasHeight = FrameHeight * FramesY;

	// Create atlas texture
	OutAtlas = UTexture2D::CreateTransient(AtlasWidth, AtlasHeight, PF_B8G8R8A8);
	if (!OutAtlas)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create flipbook atlas texture"));
		return false;
	}

	// Get atlas texture data
	FTexture2DMipMap& AtlasMip = OutAtlas->GetPlatformData()->Mips[0];
	uint8* AtlasData = static_cast<uint8*>(AtlasMip.BulkData.Lock(LOCK_READ_WRITE));

	// Clear atlas to transparent
	FMemory::Memzero(AtlasData, AtlasWidth * AtlasHeight * 4);

	// Copy each frame to atlas
	for (int32 FrameIndex = 0; FrameIndex < FrameTextures.Num(); ++FrameIndex)
	{
		UTexture2D* FrameTexture = FrameTextures[FrameIndex];
		if (!FrameTexture)
		{
			continue;
		}

		// Calculate frame position in atlas
		const int32 FrameX = FrameIndex % FramesX;
		const int32 FrameY = FrameIndex / FramesX;
		const int32 OffsetX = FrameX * FrameWidth;
		const int32 OffsetY = FrameY * FrameHeight;

		// Get frame texture data
		FTexture2DMipMap& FrameMip = FrameTexture->GetPlatformData()->Mips[0];
		const uint8* FrameData = static_cast<const uint8*>(FrameMip.BulkData.Lock(LOCK_READ_ONLY));

		// Copy frame data to atlas
		for (int32 Y = 0; Y < FrameHeight; ++Y)
		{
			for (int32 X = 0; X < FrameWidth; ++X)
			{
				const int32 FramePixelIndex = (Y * FrameWidth + X) * 4;
				const int32 AtlasPixelIndex = ((OffsetY + Y) * AtlasWidth + (OffsetX + X)) * 4;

				AtlasData[AtlasPixelIndex + 0] = FrameData[FramePixelIndex + 0];  // B
				AtlasData[AtlasPixelIndex + 1] = FrameData[FramePixelIndex + 1];  // G
				AtlasData[AtlasPixelIndex + 2] = FrameData[FramePixelIndex + 2];  // R
				AtlasData[AtlasPixelIndex + 3] = FrameData[FramePixelIndex + 3];  // A
			}
		}

		FrameMip.BulkData.Unlock();
	}

	AtlasMip.BulkData.Unlock();

	// Update atlas texture
	OutAtlas->UpdateResource();

	UE_LOG(LogTemp, Log, TEXT("Created flipbook atlas '%s' (%dx%d, %d frames)"), 
		*AtlasName, AtlasWidth, AtlasHeight, FrameTextures.Num());
	return true;
}

bool UTextureGenerationHandler::DecodeBase64(const FString& Base64String, TArray<uint8>& OutBytes)
{
	return FBase64::Decode(Base64String, OutBytes);
}

bool UTextureGenerationHandler::ValidateRequest(const FTextureGenerationRequest& Request, FString& OutError)
{
	// Check prompt
	if (Request.Prompt.IsEmpty())
	{
		OutError = TEXT("Texture prompt cannot be empty");
		return false;
	}

	// Check texture type
	if (Request.TextureType.IsEmpty())
	{
		OutError = TEXT("Texture type cannot be empty");
		return false;
	}

	// Check resolution is power of 2
	if (Request.Resolution <= 0 || !FMath::IsPowerOfTwo(Request.Resolution))
	{
		OutError = FString::Printf(TEXT("Resolution must be a power of 2 (got %d)"), Request.Resolution);
		return false;
	}

	// Check resolution is within valid range (64-2048)
	if (Request.Resolution < 64 || Request.Resolution > 2048)
	{
		OutError = FString::Printf(TEXT("Resolution must be between 64 and 2048 (got %d)"), Request.Resolution);
		return false;
	}

	// Check frame count
	if (Request.Frames < 1 || Request.Frames > 64)
	{
		OutError = FString::Printf(TEXT("Frame count must be between 1 and 64 (got %d)"), Request.Frames);
		return false;
	}

	return true;
}

