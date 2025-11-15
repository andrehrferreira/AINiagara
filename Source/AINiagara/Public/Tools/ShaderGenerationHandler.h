// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ShaderGenerationHandler.generated.h"

/**
 * Shader generation request parameters
 */
USTRUCT()
struct FShaderGenerationRequest
{
	GENERATED_BODY()

	/** Shader description/specifications */
	UPROPERTY()
	FString Specifications;

	/** Shader functionality (e.g., "particle color", "distortion", "custom lighting") */
	UPROPERTY()
	FString Functionality;

	/** Shader type (PixelShader, VertexShader, ComputeShader) */
	UPROPERTY()
	FString ShaderType = TEXT("PixelShader");

	/** Additional parameters */
	UPROPERTY()
	TMap<FString, FString> Parameters;
};

/**
 * Result of shader generation
 */
USTRUCT()
struct FShaderGenerationResult
{
	GENERATED_BODY()

	/** Generated HLSL code */
	UPROPERTY()
	FString HLSLCode;

	/** Success status */
	UPROPERTY()
	bool bSuccess = false;

	/** Error message if failed */
	UPROPERTY()
	FString ErrorMessage;

	/** Shader entry point name */
	UPROPERTY()
	FString EntryPoint = TEXT("MainPS");
};

/**
 * Delegate called when shader generation completes
 */
DECLARE_DELEGATE_OneParam(FOnShaderGenerated, const FShaderGenerationResult&);

/**
 * Handler for shader code generation using AI
 * Processes tool:shader function calls from LLM
 */
UCLASS()
class AINIAGARA_API UShaderGenerationHandler : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Generate HLSL shader code from specifications
	 * @param Request Shader generation parameters
	 * @param OnComplete Callback when generation completes
	 */
	static void GenerateShader(
		const FShaderGenerationRequest& Request,
		FOnShaderGenerated OnComplete
	);

	/**
	 * Validate shader generation request
	 * @param Request Request to validate
	 * @param OutError Error message if invalid
	 * @return True if request is valid
	 */
	static bool ValidateRequest(const FShaderGenerationRequest& Request, FString& OutError);

	/**
	 * Generate HLSL code from AI response
	 * @param AIResponse AI-generated shader code/text
	 * @param OutHLSLCode Extracted HLSL code
	 * @return True if HLSL code was extracted successfully
	 */
	static bool ExtractHLSLFromAIResponse(const FString& AIResponse, FString& OutHLSLCode);

private:
	/**
	 * Request shader generation from Gemini API
	 * @param Request Generation parameters
	 * @param OnComplete Callback when complete
	 */
	static void RequestShaderFromAI(
		const FShaderGenerationRequest& Request,
		FOnShaderGenerated OnComplete
	);
};

