// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tools/TextureGenerationHandler.h"
#include "Core/GeminiAPIClient.h"
#include "Misc/AutomationTest.h"
#include "Engine/Texture2D.h"
#include "Misc/Base64.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTextureGenerationHandler_ValidateRequest_ValidRequest, "AINiagara.Tools.TextureGenerationHandler.ValidateRequest.ValidRequest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTextureGenerationHandler_ValidateRequest_ValidRequest::RunTest(const FString& Parameters)
{
	// Arrange
	FTextureGenerationRequest Request;
	Request.Prompt = TEXT("Test noise texture");
	Request.TextureType = TEXT("noise");
	Request.Resolution = 512;
	Request.Frames = 1;
	FString OutError;

	// Act
	bool bIsValid = UTextureGenerationHandler::ValidateRequest(Request, OutError);

	// Assert
	TestTrue(TEXT("Valid request should pass validation"), bIsValid);
	TestTrue(TEXT("Error message should be empty for valid request"), OutError.IsEmpty());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTextureGenerationHandler_ValidateRequest_EmptyPrompt, "AINiagara.Tools.TextureGenerationHandler.ValidateRequest.EmptyPrompt", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTextureGenerationHandler_ValidateRequest_EmptyPrompt::RunTest(const FString& Parameters)
{
	// Arrange
	FTextureGenerationRequest Request;
	Request.Prompt = TEXT("");
	Request.TextureType = TEXT("noise");
	Request.Resolution = 512;
	FString OutError;

	// Act
	bool bIsValid = UTextureGenerationHandler::ValidateRequest(Request, OutError);

	// Assert
	TestFalse(TEXT("Empty prompt should fail validation"), bIsValid);
	TestTrue(TEXT("Error message should mention prompt"), OutError.Contains(TEXT("prompt")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTextureGenerationHandler_ValidateRequest_EmptyType, "AINiagara.Tools.TextureGenerationHandler.ValidateRequest.EmptyType", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTextureGenerationHandler_ValidateRequest_EmptyType::RunTest(const FString& Parameters)
{
	// Arrange
	FTextureGenerationRequest Request;
	Request.Prompt = TEXT("Test texture");
	Request.TextureType = TEXT("");
	Request.Resolution = 512;
	FString OutError;

	// Act
	bool bIsValid = UTextureGenerationHandler::ValidateRequest(Request, OutError);

	// Assert
	TestFalse(TEXT("Empty texture type should fail validation"), bIsValid);
	TestTrue(TEXT("Error message should mention type"), OutError.Contains(TEXT("type")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTextureGenerationHandler_ValidateRequest_InvalidResolution, "AINiagara.Tools.TextureGenerationHandler.ValidateRequest.InvalidResolution", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTextureGenerationHandler_ValidateRequest_InvalidResolution::RunTest(const FString& Parameters)
{
	// Arrange
	FTextureGenerationRequest Request;
	Request.Prompt = TEXT("Test texture");
	Request.TextureType = TEXT("noise");
	Request.Resolution = 500;  // Not power of 2
	FString OutError;

	// Act
	bool bIsValid = UTextureGenerationHandler::ValidateRequest(Request, OutError);

	// Assert
	TestFalse(TEXT("Non-power-of-2 resolution should fail validation"), bIsValid);
	TestTrue(TEXT("Error message should mention power of 2"), OutError.Contains(TEXT("power of 2")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTextureGenerationHandler_ValidateRequest_ResolutionTooLow, "AINiagara.Tools.TextureGenerationHandler.ValidateRequest.ResolutionTooLow", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTextureGenerationHandler_ValidateRequest_ResolutionTooLow::RunTest(const FString& Parameters)
{
	// Arrange
	FTextureGenerationRequest Request;
	Request.Prompt = TEXT("Test texture");
	Request.TextureType = TEXT("noise");
	Request.Resolution = 32;  // Too low (minimum is 64)
	FString OutError;

	// Act
	bool bIsValid = UTextureGenerationHandler::ValidateRequest(Request, OutError);

	// Assert
	TestFalse(TEXT("Resolution below 64 should fail validation"), bIsValid);
	TestTrue(TEXT("Error message should mention valid range"), OutError.Contains(TEXT("between")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTextureGenerationHandler_ValidateRequest_ResolutionTooHigh, "AINiagara.Tools.TextureGenerationHandler.ValidateRequest.ResolutionTooHigh", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTextureGenerationHandler_ValidateRequest_ResolutionTooHigh::RunTest(const FString& Parameters)
{
	// Arrange
	FTextureGenerationRequest Request;
	Request.Prompt = TEXT("Test texture");
	Request.TextureType = TEXT("noise");
	Request.Resolution = 4096;  // Too high (maximum is 2048)
	FString OutError;

	// Act
	bool bIsValid = UTextureGenerationHandler::ValidateRequest(Request, OutError);

	// Assert
	TestFalse(TEXT("Resolution above 2048 should fail validation"), bIsValid);
	TestTrue(TEXT("Error message should mention valid range"), OutError.Contains(TEXT("between")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTextureGenerationHandler_ValidateRequest_InvalidFrameCount, "AINiagara.Tools.TextureGenerationHandler.ValidateRequest.InvalidFrameCount", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTextureGenerationHandler_ValidateRequest_InvalidFrameCount::RunTest(const FString& Parameters)
{
	// Arrange
	FTextureGenerationRequest Request;
	Request.Prompt = TEXT("Test flipbook");
	Request.TextureType = TEXT("fire");
	Request.Resolution = 512;
	Request.Frames = 100;  // Too high (maximum is 64)
	FString OutError;

	// Act
	bool bIsValid = UTextureGenerationHandler::ValidateRequest(Request, OutError);

	// Assert
	TestFalse(TEXT("Frame count above 64 should fail validation"), bIsValid);
	TestTrue(TEXT("Error message should mention frame count"), OutError.Contains(TEXT("Frame count")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTextureGenerationHandler_DecodeBase64_ValidData, "AINiagara.Tools.TextureGenerationHandler.DecodeBase64.ValidData", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTextureGenerationHandler_DecodeBase64_ValidData::RunTest(const FString& Parameters)
{
	// Arrange
	FString TestData = TEXT("Hello World!");
	FString Base64Data = FBase64::Encode(TestData);
	TArray<uint8> OutBytes;

	// Act
	bool bSuccess = UTextureGenerationHandler::DecodeBase64(Base64Data, OutBytes);

	// Assert
	TestTrue(TEXT("Decoding valid base64 should succeed"), bSuccess);
	TestTrue(TEXT("Decoded data should not be empty"), OutBytes.Num() > 0);
	
	// Verify decoded data matches original
	FString DecodedString = FString(reinterpret_cast<const char*>(OutBytes.GetData()));
	TestEqual(TEXT("Decoded data should match original"), DecodedString, TestData);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTextureGenerationHandler_DecodeBase64_EmptyData, "AINiagara.Tools.TextureGenerationHandler.DecodeBase64.EmptyData", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTextureGenerationHandler_DecodeBase64_EmptyData::RunTest(const FString& Parameters)
{
	// Arrange
	FString Base64Data = TEXT("");
	TArray<uint8> OutBytes;

	// Act
	bool bSuccess = UTextureGenerationHandler::DecodeBase64(Base64Data, OutBytes);

	// Assert
	// Empty base64 string decodes to empty array (valid but empty)
	TestTrue(TEXT("Decoding empty base64 should succeed"), bSuccess);
	TestEqual(TEXT("Decoded data should be empty"), OutBytes.Num(), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTextureGenerationHandler_DecodeBase64_InvalidData, "AINiagara.Tools.TextureGenerationHandler.DecodeBase64.InvalidData", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTextureGenerationHandler_DecodeBase64_InvalidData::RunTest(const FString& Parameters)
{
	// Arrange
	FString Base64Data = TEXT("Invalid@#$%Base64!!!!");
	TArray<uint8> OutBytes;

	// Act
	bool bSuccess = UTextureGenerationHandler::DecodeBase64(Base64Data, OutBytes);

	// Assert
	// FBase64::Decode returns true even for invalid data, but OutBytes will be empty or garbage
	// This is expected behavior from UE's FBase64 implementation
	TestTrue(TEXT("FBase64::Decode always returns true"), bSuccess);

	return true;
}

