// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "Containers/Map.h"
#include "UObject/ObjectMacros.h"

DECLARE_DELEGATE_OneParam(FOnGeminiResponse, const FString& ResponseText);
DECLARE_DELEGATE_TwoParams(FOnGeminiError, int32 ErrorCode, const FString& ErrorMessage);

/**
 * Message structure for conversation history
 */
USTRUCT()
struct FConversationMessage
{
	GENERATED_BODY()

	/** Message role: "user" or "assistant" */
	UPROPERTY()
	FString Role;

	/** Message content */
	UPROPERTY()
	FString Content;

	/** Timestamp of the message */
	UPROPERTY()
	FDateTime Timestamp;

	FConversationMessage()
		: Role(TEXT("user"))
		, Timestamp(FDateTime::Now())
	{
	}

	FConversationMessage(const FString& InRole, const FString& InContent)
		: Role(InRole)
		, Content(InContent)
		, Timestamp(FDateTime::Now())
	{
	}
};

/**
 * Tool function definition for LLM function calling
 */
USTRUCT()
struct FVFXToolFunction
{
	GENERATED_BODY()

	/** Tool function name (e.g., "tool:texture") */
	UPROPERTY()
	FString Name;

	/** Tool function description */
	UPROPERTY()
	FString Description;

	/** Tool function parameters schema */
	UPROPERTY()
	TMap<FString, FString> Parameters;
};

/**
 * Gemini API client for making requests to Google Gemini API
 */
class AINIAGARA_API FGeminiAPIClient
{
public:
	/** Constructor */
	FGeminiAPIClient();

	/** Destructor */
	~FGeminiAPIClient();

	/**
	 * Set the API key for authentication
	 * @param InAPIKey The API key to use
	 */
	void SetAPIKey(const FString& InAPIKey);

	/**
	 * Load API key from settings
	 */
	void LoadAPIKeyFromSettings();

	/**
	 * Get the current API key (masked for security)
	 * @return Masked API key string
	 */
	FString GetAPIKey() const;

	/**
	 * Test if the API key is valid by making a simple request
	 * @param InAPIKey The API key to test
	 * @param OnResponse Callback when test succeeds
	 * @param OnError Callback when test fails
	 */
	void TestAPIKey(
		const FString& InAPIKey,
		FOnGeminiResponse OnResponse,
		FOnGeminiError OnError
	);

	/**
	 * Send a chat completion request to Gemini API
	 * @param Prompt The user's prompt
	 * @param ConversationHistory Previous messages in the conversation
	 * @param AvailableTools List of available tool functions
	 * @param OnResponse Callback when request succeeds
	 * @param OnError Callback when request fails
	 */
	void SendChatCompletion(
		const FString& Prompt,
		const TArray<FConversationMessage>& ConversationHistory,
		const TArray<FVFXToolFunction>& AvailableTools,
		FOnGeminiResponse OnResponse,
		FOnGeminiError OnError
	);

	/**
	 * Send a request to generate texture using Imagen 3
	 * @param Prompt Description of the texture to generate
	 * @param TextureType Type of texture (noise, fire, smoke, sparks, distortion)
	 * @param Resolution Texture resolution
	 * @param OnResponse Callback with generated image data
	 * @param OnError Callback when request fails
	 */
	void GenerateTexture(
		const FString& Prompt,
		const FString& TextureType,
		int32 Resolution,
		FOnGeminiResponse OnResponse,
		FOnGeminiError OnError
	);

private:
	/** API key for authentication */
	FString APIKey;

	/** Base URL for Gemini API */
	static const FString BaseURL;

	/** Model endpoint for chat completion */
	static const FString ChatCompletionEndpoint;

	/** Model endpoint for image generation */
	static const FString ImageGenerationEndpoint;

	/** Maximum number of retry attempts */
	static const int32 MaxRetries;

	/** Initial retry delay in seconds */
	static const float InitialRetryDelay;

	/**
	 * Build the request payload for chat completion
	 * @param Prompt User prompt
	 * @param ConversationHistory Conversation history
	 * @param AvailableTools Available tool functions
	 * @return JSON string payload
	 */
	FString BuildChatCompletionPayload(
		const FString& Prompt,
		const TArray<FConversationMessage>& ConversationHistory,
		const TArray<FVFXToolFunction>& AvailableTools
	) const;

	/**
	 * Build the request payload for texture generation
	 * @param Prompt Texture description
	 * @param TextureType Type of texture
	 * @param Resolution Texture resolution
	 * @return JSON string payload
	 */
	FString BuildTextureGenerationPayload(
		const FString& Prompt,
		const FString& TextureType,
		int32 Resolution
	) const;

	/**
	 * Parse response from Gemini API
	 * @param ResponseBody Response body JSON string
	 * @param OutResponseText Parsed response text
	 * @return True if parsing succeeded
	 */
	bool ParseResponse(const FString& ResponseBody, FString& OutResponseText) const;

	/**
	 * Handle HTTP request completion
	 * @param Request The HTTP request
	 * @param Response The HTTP response
	 * @param bWasSuccessful Whether the request was successful
	 * @param OnResponse Success callback
	 * @param OnError Error callback
	 */
	void HandleRequestComplete(
		FHttpRequestPtr Request,
		FHttpResponsePtr Response,
		bool bWasSuccessful,
		FOnGeminiResponse OnResponse,
		FOnGeminiError OnError
	);

	/**
	 * Retry a failed request with exponential backoff
	 * @param URL Request URL
	 * @param Payload Request payload
	 * @param RetryCount Current retry count
	 * @param OnResponse Success callback
	 * @param OnError Error callback
	 */
	void RetryRequest(
		const FString& URL,
		const FString& Payload,
		int32 RetryCount,
		FOnGeminiResponse OnResponse,
		FOnGeminiError OnError
	);
};

