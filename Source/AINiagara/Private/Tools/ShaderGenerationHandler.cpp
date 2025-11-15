// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tools/ShaderGenerationHandler.h"
#include "Core/GeminiAPIClient.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"

void UShaderGenerationHandler::GenerateShader(
	const FShaderGenerationRequest& Request,
	FOnShaderGenerated OnComplete
)
{
	// Validate request
	FString ValidationError;
	if (!ValidateRequest(Request, ValidationError))
	{
		FShaderGenerationResult ErrorResult;
		ErrorResult.bSuccess = false;
		ErrorResult.ErrorMessage = ValidationError;
		OnComplete.ExecuteIfBound(ErrorResult);
		return;
	}

	// Request shader generation from AI
	RequestShaderFromAI(Request, OnComplete);
}

void UShaderGenerationHandler::RequestShaderFromAI(
	const FShaderGenerationRequest& Request,
	FOnShaderGenerated OnComplete
)
{
	// Create API client
	FGeminiAPIClient* APIClient = new FGeminiAPIClient();

	// Build prompt for shader generation
	FString Prompt = FString::Printf(
		TEXT("Generate HLSL shader code for Unreal Engine particle effects.\n\n")
		TEXT("Specifications: %s\n")
		TEXT("Functionality: %s\n")
		TEXT("Shader Type: %s\n\n")
		TEXT("Requirements:\n")
		TEXT("- Output ONLY valid HLSL code\n")
		TEXT("- Use standard Unreal Engine shader conventions\n")
		TEXT("- Include proper input/output structures\n")
		TEXT("- Use MainPS as entry point for pixel shaders\n")
		TEXT("- Use MainVS as entry point for vertex shaders\n")
		TEXT("- Include proper semantic annotations\n")
		TEXT("- Support particle color, alpha, and UV coordinates\n\n")
		TEXT("Generate the complete shader code:"),
		*Request.Specifications,
		*Request.Functionality,
		*Request.ShaderType
	);

	// Add additional parameters if provided
	if (Request.Parameters.Num() > 0)
	{
		Prompt += TEXT("\n\nAdditional Parameters:\n");
		for (const auto& Param : Request.Parameters)
		{
			Prompt += FString::Printf(TEXT("- %s: %s\n"), *Param.Key, *Param.Value);
		}
	}

	// Send request to Gemini API
	TArray<FConversationMessage> EmptyHistory;
	TArray<FVFXToolFunction> EmptyTools;

	APIClient->SendChatCompletion(
		Prompt,
		EmptyHistory,
		EmptyTools,
		FOnGeminiResponse::CreateLambda([APIClient, Request, OnComplete](const FString& ResponseText)
		{
			FShaderGenerationResult Result;

			// Extract HLSL code from AI response
			FString HLSLCode;
			if (ExtractHLSLFromAIResponse(ResponseText, HLSLCode))
			{
				Result.bSuccess = true;
				Result.HLSLCode = HLSLCode;
				Result.EntryPoint = Request.ShaderType.Contains(TEXT("Vertex")) ? TEXT("MainVS") : TEXT("MainPS");
			}
			else
			{
				// Try to use response as-is if extraction failed
				Result.bSuccess = true;
				Result.HLSLCode = ResponseText;
				Result.EntryPoint = Request.ShaderType.Contains(TEXT("Vertex")) ? TEXT("MainVS") : TEXT("MainPS");
				
				UE_LOG(LogTemp, Warning, TEXT("Could not extract HLSL code from AI response, using response as-is"));
			}

			// Cleanup API client
			delete APIClient;

			// Call completion callback
			OnComplete.ExecuteIfBound(Result);
		}),
		FOnGeminiError::CreateLambda([APIClient, OnComplete](int32 ErrorCode, const FString& ErrorMessage)
		{
			FShaderGenerationResult Result;
			Result.bSuccess = false;
			Result.ErrorMessage = FString::Printf(
				TEXT("Gemini API error %d: %s"),
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

bool UShaderGenerationHandler::ExtractHLSLFromAIResponse(const FString& AIResponse, FString& OutHLSLCode)
{
	// Try to find HLSL code block (```hlsl ... ```)
	int32 StartIndex = AIResponse.Find(TEXT("```hlsl"), ESearchCase::IgnoreCase);
	if (StartIndex == INDEX_NONE)
	{
		// Try generic code block
		StartIndex = AIResponse.Find(TEXT("```"), ESearchCase::IgnoreCase);
	}

	if (StartIndex != INDEX_NONE)
	{
		// Find end of code block
		int32 CodeStart = AIResponse.Find(TEXT("\n"), ESearchCase::CaseSensitive, ESearchDir::FromStart, StartIndex);
		if (CodeStart == INDEX_NONE)
		{
			CodeStart = StartIndex + 3; // Skip ```
		}
		else
		{
			CodeStart++; // Skip newline
		}

		int32 EndIndex = AIResponse.Find(TEXT("```"), ESearchCase::IgnoreCase, ESearchDir::FromStart, CodeStart);
		if (EndIndex != INDEX_NONE)
		{
			OutHLSLCode = AIResponse.Mid(CodeStart, EndIndex - CodeStart);
			OutHLSLCode.TrimStartAndEndInline();
			return true;
		}
	}

	// Try to find HLSL code between common markers
	FString LowerResponse = AIResponse.ToLower();
	if (LowerResponse.Contains(TEXT("hlsl")) || LowerResponse.Contains(TEXT("shader")))
	{
		// Look for code-like content (contains common HLSL keywords)
		TArray<FString> HLSLKeywords = {
			TEXT("float"), TEXT("float2"), TEXT("float3"), TEXT("float4"),
			TEXT("sampler"), TEXT("texture"), TEXT("return"), TEXT("struct"),
			TEXT("void"), TEXT("MainPS"), TEXT("MainVS"), TEXT("SV_Position")
		};

		bool bContainsHLSL = false;
		for (const FString& Keyword : HLSLKeywords)
		{
			if (AIResponse.Contains(Keyword, ESearchCase::IgnoreCase))
			{
				bContainsHLSL = true;
				break;
			}
		}

		if (bContainsHLSL)
		{
			// Use response as HLSL code (might need manual cleanup)
			OutHLSLCode = AIResponse;
			OutHLSLCode.TrimStartAndEndInline();
			return true;
		}
	}

	return false;
}

bool UShaderGenerationHandler::ValidateRequest(const FShaderGenerationRequest& Request, FString& OutError)
{
	// Check specifications
	if (Request.Specifications.IsEmpty())
	{
		OutError = TEXT("Shader specifications cannot be empty");
		return false;
	}

	// Check functionality
	if (Request.Functionality.IsEmpty())
	{
		OutError = TEXT("Shader functionality cannot be empty");
		return false;
	}

	// Check shader type
	if (Request.ShaderType.IsEmpty())
	{
		OutError = TEXT("Shader type cannot be empty");
		return false;
	}

	// Validate shader type
	FString LowerType = Request.ShaderType.ToLower();
	if (!LowerType.Contains(TEXT("pixel")) && 
	    !LowerType.Contains(TEXT("vertex")) && 
	    !LowerType.Contains(TEXT("compute")))
	{
		OutError = FString::Printf(
			TEXT("Invalid shader type '%s'. Must be PixelShader, VertexShader, or ComputeShader"),
			*Request.ShaderType
		);
		return false;
	}

	return true;
}

