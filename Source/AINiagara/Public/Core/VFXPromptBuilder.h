// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/GeminiAPIClient.h"
#include "Core/VFXDSL.h"
#include "VFXPromptBuilder.generated.h"

/**
 * Builder for structured prompts sent to Gemini API
 */
UCLASS(BlueprintType)
class AINIAGARA_API UVFXPromptBuilder : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Build the complete system prompt for VFX generation
	 * @return System prompt string
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Prompt")
	static FString BuildSystemPrompt();

	/**
	 * Build the user prompt with context from conversation history
	 * @param UserRequest User's natural language request
	 * @param ConversationHistory Previous conversation messages
	 * @return Complete user prompt string
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Prompt")
	static FString BuildUserPrompt(
		const FString& UserRequest,
		const TArray<FConversationMessage>& ConversationHistory
	);

	/**
	 * Get available tool functions for LLM
	 * @return Array of tool function definitions
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Prompt")
	static TArray<FVFXToolFunction> GetAvailableTools();

	/**
	 * Build DSL format specification documentation
	 * @return DSL format specification string
	 */
	static FString GetDSLFormatSpecification();

	/**
	 * Build tool function documentation
	 * @return Tool function documentation string
	 */
	static FString GetToolFunctionDocumentation();

private:
	/**
	 * Build VFX expert persona instructions
	 */
	static FString BuildExpertPersonaInstructions();

	/**
	 * Build DSL format instructions
	 */
	static FString BuildDSLFormatInstructions();

	/**
	 * Build tool usage instructions
	 */
	static FString BuildToolUsageInstructions();

	/**
	 * Build 3D model handling instructions
	 */
	static FString Build3DModelInstructions();
};

