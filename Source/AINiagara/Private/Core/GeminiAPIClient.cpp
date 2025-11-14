// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/GeminiAPIClient.h"
#include "Core/AINiagaraSettings.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Misc/DateTime.h"
#include "HAL/PlatformProcess.h"
#include "Misc/App.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformFilemanager.h"

const FString FGeminiAPIClient::BaseURL = TEXT("https://generativelanguage.googleapis.com/v1beta");
const FString FGeminiAPIClient::ChatCompletionEndpoint = TEXT("/models/gemini-pro:generateContent");
const FString FGeminiAPIClient::ImageGenerationEndpoint = TEXT("/models/imagen-3-generate-001:generateContent");
const int32 FGeminiAPIClient::MaxRetries = 3;
const float FGeminiAPIClient::InitialRetryDelay = 1.0f;

FGeminiAPIClient::FGeminiAPIClient()
{
	// Load API key from settings on construction
	LoadAPIKeyFromSettings();
}

FGeminiAPIClient::~FGeminiAPIClient()
{
}

void FGeminiAPIClient::SetAPIKey(const FString& InAPIKey)
{
	APIKey = InAPIKey;
	
	// Also save to settings
	if (UAINiagaraSettings* Settings = UAINiagaraSettings::Get())
	{
		Settings->SetGeminiAPIKey(InAPIKey, true);
	}
}

void FGeminiAPIClient::LoadAPIKeyFromSettings()
{
	if (UAINiagaraSettings* Settings = UAINiagaraSettings::Get())
	{
		if (Settings->IsAPIKeyConfigured())
		{
			APIKey = Settings->GetGeminiAPIKey();
		}
	}
}

FString FGeminiAPIClient::GetAPIKey() const
{
	// Return masked API key for security
	if (APIKey.IsEmpty())
	{
		return FString();
	}
	
	// Mask all but last 4 characters
	if (APIKey.Len() > 4)
	{
		FString Masked = FString::ChrN(APIKey.Len() - 4, TEXT('*'));
		Masked += APIKey.Right(4);
		return Masked;
	}
	
	return TEXT("****");
}

void FGeminiAPIClient::TestAPIKey(
	const FString& InAPIKey,
	FOnGeminiResponse OnResponse,
	FOnGeminiError OnError
)
{
	// Create a simple test request
	const FString TestPrompt = TEXT("Say 'OK' if you receive this message.");
	
	TArray<FConversationMessage> EmptyHistory;
	TArray<FVFXToolFunction> EmptyTools;
	
	// Temporarily set the API key for testing
	FString SavedAPIKey = APIKey;
	SetAPIKey(InAPIKey);
	
	SendChatCompletion(
		TestPrompt,
		EmptyHistory,
		EmptyTools,
		OnResponse,
		OnError
	);
	
	// Restore original API key
	SetAPIKey(SavedAPIKey);
}

void FGeminiAPIClient::SendChatCompletion(
	const FString& Prompt,
	const TArray<FConversationMessage>& ConversationHistory,
	const TArray<FVFXToolFunction>& AvailableTools,
	FOnGeminiResponse OnResponse,
	FOnGeminiError OnError
)
{
	if (APIKey.IsEmpty())
	{
		OnError.ExecuteIfBound(401, TEXT("API key is not set"));
		return;
	}
	
	const FString URL = BaseURL + ChatCompletionEndpoint + TEXT("?key=") + APIKey;
	const FString Payload = BuildChatCompletionPayload(Prompt, ConversationHistory, AvailableTools);
	
	// Create HTTP request
	FHttpModule& HttpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();
	
	Request->SetURL(URL);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(Payload);
	
	// Bind completion callback
	Request->OnProcessRequestComplete().BindLambda(
		[this, OnResponse, OnError, URL, Payload](
			FHttpRequestPtr HttpRequest,
			FHttpResponsePtr HttpResponse,
			bool bWasSuccessful
		)
		{
			HandleRequestComplete(HttpRequest, HttpResponse, bWasSuccessful, OnResponse, OnError);
		}
	);
	
	// Send request
	Request->ProcessRequest();
}

void FGeminiAPIClient::GenerateTexture(
	const FString& Prompt,
	const FString& TextureType,
	int32 Resolution,
	FOnGeminiResponse OnResponse,
	FOnGeminiError OnError
)
{
	if (APIKey.IsEmpty())
	{
		OnError.ExecuteIfBound(401, TEXT("API key is not set"));
		return;
	}
	
	const FString URL = BaseURL + ImageGenerationEndpoint + TEXT("?key=") + APIKey;
	const FString Payload = BuildTextureGenerationPayload(Prompt, TextureType, Resolution);
	
	// Create HTTP request
	FHttpModule& HttpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();
	
	Request->SetURL(URL);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(Payload);
	
	// Bind completion callback
	Request->OnProcessRequestComplete().BindLambda(
		[this, OnResponse, OnError](
			FHttpRequestPtr HttpRequest,
			FHttpResponsePtr HttpResponse,
			bool bWasSuccessful
		)
		{
			HandleRequestComplete(HttpRequest, HttpResponse, bWasSuccessful, OnResponse, OnError);
		}
	);
	
	// Send request
	Request->ProcessRequest();
}

FString FGeminiAPIClient::BuildChatCompletionPayload(
	const FString& Prompt,
	const TArray<FConversationMessage>& ConversationHistory,
	const TArray<FVFXToolFunction>& AvailableTools
) const
{
	TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);
	
	// Build contents array
	TArray<TSharedPtr<FJsonValue>> ContentsArray;
	
	// Add conversation history
	for (const FConversationMessage& Message : ConversationHistory)
	{
		TSharedPtr<FJsonObject> MessageObject = MakeShareable(new FJsonObject);
		MessageObject->SetStringField(TEXT("role"), Message.Role);
		
		TSharedPtr<FJsonObject> PartObject = MakeShareable(new FJsonObject);
		PartObject->SetStringField(TEXT("text"), Message.Content);
		
		TArray<TSharedPtr<FJsonValue>> PartsArray;
		PartsArray.Add(MakeShareable(new FJsonValueObject(PartObject)));
		
		MessageObject->SetArrayField(TEXT("parts"), PartsArray);
		ContentsArray.Add(MakeShareable(new FJsonValueObject(MessageObject)));
	}
	
	// Add current user prompt
	TSharedPtr<FJsonObject> UserMessage = MakeShareable(new FJsonObject);
	UserMessage->SetStringField(TEXT("role"), TEXT("user"));
	
	TSharedPtr<FJsonObject> UserPart = MakeShareable(new FJsonObject);
	UserPart->SetStringField(TEXT("text"), Prompt);
	
	TArray<TSharedPtr<FJsonValue>> UserPartsArray;
	UserPartsArray.Add(MakeShareable(new FJsonValueObject(UserPart)));
	
	UserMessage->SetArrayField(TEXT("parts"), UserPartsArray);
	ContentsArray.Add(MakeShareable(new FJsonValueObject(UserMessage)));
	
	RootObject->SetArrayField(TEXT("contents"), ContentsArray);
	
	// Add tools if available
	if (AvailableTools.Num() > 0)
	{
		TArray<TSharedPtr<FJsonValue>> ToolsArray;
		
		for (const FVFXToolFunction& Tool : AvailableTools)
		{
			TSharedPtr<FJsonObject> ToolObject = MakeShareable(new FJsonObject);
			TSharedPtr<FJsonObject> FunctionDeclaration = MakeShareable(new FJsonObject);
			
			FunctionDeclaration->SetStringField(TEXT("name"), Tool.Name);
			FunctionDeclaration->SetStringField(TEXT("description"), Tool.Description);
			
			// Add parameters if available
			if (Tool.Parameters.Num() > 0)
			{
				TSharedPtr<FJsonObject> ParametersObject = MakeShareable(new FJsonObject);
				TSharedPtr<FJsonObject> PropertiesObject = MakeShareable(new FJsonObject);
				
				for (const auto& ParamPair : Tool.Parameters)
				{
					TSharedPtr<FJsonObject> ParamObject = MakeShareable(new FJsonObject);
					ParamObject->SetStringField(TEXT("type"), ParamPair.Value);
					PropertiesObject->SetObjectField(ParamPair.Key, ParamObject);
				}
				
				TArray<FString> RequiredArray;
				Tool.Parameters.GetKeys(RequiredArray);
				
				ParametersObject->SetObjectField(TEXT("properties"), PropertiesObject);
				ParametersObject->SetArrayField(TEXT("required"), TArray<TSharedPtr<FJsonValue>>());
				ParametersObject->SetStringField(TEXT("type"), TEXT("object"));
				
				FunctionDeclaration->SetObjectField(TEXT("parameters"), ParametersObject);
			}
			
			ToolObject->SetObjectField(TEXT("functionDeclaration"), FunctionDeclaration);
			ToolsArray.Add(MakeShareable(new FJsonValueObject(ToolObject)));
		}
		
		TSharedPtr<FJsonObject> ToolsObject = MakeShareable(new FJsonObject);
		ToolsObject->SetArrayField(TEXT("tools"), ToolsArray);
		RootObject->SetObjectField(TEXT("tools"), ToolsObject);
	}
	
	// Convert to JSON string
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);
	
	return OutputString;
}

FString FGeminiAPIClient::BuildTextureGenerationPayload(
	const FString& Prompt,
	const FString& TextureType,
	int32 Resolution
) const
{
	TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);
	
	// Build prompt with texture type specification
	FString FullPrompt = FString::Printf(
		TEXT("Generate a %s texture. %s Resolution: %dx%d pixels."),
		*TextureType,
		*Prompt,
		Resolution,
		Resolution
	);
	
	TSharedPtr<FJsonObject> PromptObject = MakeShareable(new FJsonObject);
	PromptObject->SetStringField(TEXT("prompt"), FullPrompt);
	
	TSharedPtr<FJsonObject> InstancePromptObject = MakeShareable(new FJsonObject);
	InstancePromptObject->SetStringField(TEXT("prompt"), FullPrompt);
	
	// Add generation config
	TSharedPtr<FJsonObject> GenerationConfig = MakeShareable(new FJsonObject);
	GenerationConfig->SetNumberField(TEXT("numberOfImages"), 1);
	GenerationConfig->SetNumberField(TEXT("aspectRatio"), 1.0);  // 1:1 for textures
	GenerationConfig->SetStringField(TEXT("outputOptions"), TEXT("png"));
	
	RootObject->SetObjectField(TEXT("prompt"), PromptObject);
	RootObject->SetObjectField(TEXT("instancePrompt"), InstancePromptObject);
	RootObject->SetObjectField(TEXT("generationConfig"), GenerationConfig);
	
	// Convert to JSON string
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);
	
	return OutputString;
}

bool FGeminiAPIClient::ParseResponse(const FString& ResponseBody, FString& OutResponseText) const
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);
	
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		return false;
	}
	
	// Navigate to the response text
	// Gemini API response structure: { candidates: [{ content: { parts: [{ text: "..." }] } }] }
	const TArray<TSharedPtr<FJsonValue>>* CandidatesArray;
	if (JsonObject->TryGetArrayField(TEXT("candidates"), CandidatesArray) && 
		CandidatesArray->Num() > 0)
	{
		TSharedPtr<FJsonObject> CandidateObject = (*CandidatesArray)[0]->AsObject();
		if (CandidateObject.IsValid())
		{
			TSharedPtr<FJsonObject> ContentObject = CandidateObject->GetObjectField(TEXT("content"));
			if (ContentObject.IsValid())
			{
				const TArray<TSharedPtr<FJsonValue>>* PartsArray;
				if (ContentObject->TryGetArrayField(TEXT("parts"), PartsArray) &&
					PartsArray->Num() > 0)
				{
					TSharedPtr<FJsonObject> PartObject = (*PartsArray)[0]->AsObject();
					if (PartObject.IsValid())
					{
						PartObject->TryGetStringField(TEXT("text"), OutResponseText);
						return true;
					}
				}
			}
		}
	}
	
	return false;
}

void FGeminiAPIClient::HandleRequestComplete(
	FHttpRequestPtr Request,
	FHttpResponsePtr Response,
	bool bWasSuccessful,
	FOnGeminiResponse OnResponse,
	FOnGeminiError OnError
)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		// Network error - could retry, but for now just report
		OnError.ExecuteIfBound(
			Response.IsValid() ? Response->GetResponseCode() : 0,
			TEXT("Network error: Request failed or no response received")
		);
		return;
	}
	
	const int32 ResponseCode = Response->GetResponseCode();
	
	if (ResponseCode == 200)
	{
		FString ResponseBody = Response->GetContentAsString();
		FString ResponseText;
		
		if (ParseResponse(ResponseBody, ResponseText))
		{
			OnResponse.ExecuteIfBound(ResponseText);
		}
		else
		{
			OnError.ExecuteIfBound(500, TEXT("Failed to parse response from API"));
		}
	}
	else if (ResponseCode == 401)
	{
		// Unauthorized - API key issue
		OnError.ExecuteIfBound(ResponseCode, TEXT("Unauthorized: Invalid or missing API key. Please check your API key configuration."));
	}
	else if (ResponseCode == 403)
	{
		// Forbidden - API key doesn't have permission
		OnError.ExecuteIfBound(ResponseCode, TEXT("Forbidden: API key does not have permission for this operation."));
	}
	else if (ResponseCode == 429)
	{
		// Rate limited
		OnError.ExecuteIfBound(ResponseCode, TEXT("Rate limit exceeded: Too many requests. Please wait before trying again."));
	}
	else if (ResponseCode >= 500)
	{
		// Server error - could retry
		FString ErrorMessage = FString::Printf(TEXT("Server error (%d): %s"), ResponseCode, *Response->GetContentAsString());
		OnError.ExecuteIfBound(ResponseCode, ErrorMessage);
	}
	else
	{
		// Other client errors
		FString ErrorMessage = Response->GetContentAsString();
		if (ErrorMessage.IsEmpty())
		{
			ErrorMessage = FString::Printf(TEXT("Request failed with status code %d"), ResponseCode);
		}
		OnError.ExecuteIfBound(ResponseCode, ErrorMessage);
	}
}

void FGeminiAPIClient::RetryRequest(
	const FString& URL,
	const FString& Payload,
	int32 RetryCount,
	FOnGeminiResponse OnResponse,
	FOnGeminiError OnError
)
{
	if (RetryCount >= MaxRetries)
	{
		OnError.ExecuteIfBound(500, FString::Printf(TEXT("Request failed after %d retry attempts"), MaxRetries));
		return;
	}
	
	// Calculate exponential backoff delay
	float DelaySeconds = InitialRetryDelay * FMath::Pow(2.0f, static_cast<float>(RetryCount));
	
	// Schedule retry after delay
	FTimerHandle RetryTimerHandle;
	FTimerDelegate RetryDelegate = FTimerDelegate::CreateLambda([this, URL, Payload, RetryCount, OnResponse, OnError]()
	{
		// Create HTTP request
		FHttpModule& HttpModule = FHttpModule::Get();
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();
		
		Request->SetURL(URL);
		Request->SetVerb(TEXT("POST"));
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
		Request->SetContentAsString(Payload);
		
		// Bind completion callback with retry logic
		Request->OnProcessRequestComplete().BindLambda(
			[this, URL, Payload, RetryCount, OnResponse, OnError](
				FHttpRequestPtr HttpRequest,
				FHttpResponsePtr HttpResponse,
				bool bWasSuccessful
			)
			{
				if (!bWasSuccessful || !HttpResponse.IsValid())
				{
					// Retry on network errors
					RetryRequest(URL, Payload, RetryCount + 1, OnResponse, OnError);
					return;
				}
				
				const int32 ResponseCode = HttpResponse->GetResponseCode();
				
				// Retry on 5xx server errors and 429 (rate limit)
				if (ResponseCode >= 500 || ResponseCode == 429)
				{
					RetryRequest(URL, Payload, RetryCount + 1, OnResponse, OnError);
					return;
				}
				
				// Handle other responses normally
				HandleRequestComplete(HttpRequest, HttpResponse, bWasSuccessful, OnResponse, OnError);
			}
		);
		
		// Send request
		Request->ProcessRequest();
	});
	
	// Schedule retry using FTSTicker for delayed execution
	// This works in both editor and game
	FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateLambda([this, URL, Payload, RetryCount, OnResponse, OnError](float DeltaTime) -> bool
		{
			// Create HTTP request
			FHttpModule& HttpModule = FHttpModule::Get();
			TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();
			
			Request->SetURL(URL);
			Request->SetVerb(TEXT("POST"));
			Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
			Request->SetContentAsString(Payload);
			
			// Bind completion callback with retry logic
			Request->OnProcessRequestComplete().BindLambda(
				[this, URL, Payload, RetryCount, OnResponse, OnError](
					FHttpRequestPtr HttpRequest,
					FHttpResponsePtr HttpResponse,
					bool bWasSuccessful
				)
				{
					if (!bWasSuccessful || !HttpResponse.IsValid())
					{
						// Retry on network errors
						RetryRequest(URL, Payload, RetryCount + 1, OnResponse, OnError);
						return;
					}
					
					const int32 ResponseCode = HttpResponse->GetResponseCode();
					
					// Retry on 5xx server errors and 429 (rate limit)
					if (ResponseCode >= 500 || ResponseCode == 429)
					{
						RetryRequest(URL, Payload, RetryCount + 1, OnResponse, OnError);
						return;
					}
					
					// Handle other responses normally
					HandleRequestComplete(HttpRequest, HttpResponse, bWasSuccessful, OnResponse, OnError);
				}
			);
			
			// Send request
			Request->ProcessRequest();
			
			return false; // Don't repeat
		}),
		DelaySeconds
	);
}

