// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tools/ShaderGenerationHandler.h"
#include "Tools/MaterialGenerationHandler.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShaderGenerationHandler_ValidateRequest_ValidRequest, "AINiagara.Tools.ShaderGenerationHandler.ValidateRequest.ValidRequest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FShaderGenerationHandler_ValidateRequest_ValidRequest::RunTest(const FString& Parameters)
{
	// Arrange
	FShaderGenerationRequest Request;
	Request.Specifications = TEXT("Particle color shader");
	Request.Functionality = TEXT("Color manipulation");
	Request.ShaderType = TEXT("PixelShader");
	FString OutError;

	// Act
	bool bIsValid = UShaderGenerationHandler::ValidateRequest(Request, OutError);

	// Assert
	TestTrue(TEXT("Valid request should pass validation"), bIsValid);
	TestTrue(TEXT("Error message should be empty for valid request"), OutError.IsEmpty());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShaderGenerationHandler_ValidateRequest_EmptySpecifications, "AINiagara.Tools.ShaderGenerationHandler.ValidateRequest.EmptySpecifications", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FShaderGenerationHandler_ValidateRequest_EmptySpecifications::RunTest(const FString& Parameters)
{
	// Arrange
	FShaderGenerationRequest Request;
	Request.Specifications = TEXT("");
	Request.Functionality = TEXT("Color manipulation");
	Request.ShaderType = TEXT("PixelShader");
	FString OutError;

	// Act
	bool bIsValid = UShaderGenerationHandler::ValidateRequest(Request, OutError);

	// Assert
	TestFalse(TEXT("Empty specifications should fail validation"), bIsValid);
	TestTrue(TEXT("Error message should mention specifications"), OutError.Contains(TEXT("specifications")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShaderGenerationHandler_ValidateRequest_EmptyFunctionality, "AINiagara.Tools.ShaderGenerationHandler.ValidateRequest.EmptyFunctionality", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FShaderGenerationHandler_ValidateRequest_EmptyFunctionality::RunTest(const FString& Parameters)
{
	// Arrange
	FShaderGenerationRequest Request;
	Request.Specifications = TEXT("Particle color shader");
	Request.Functionality = TEXT("");
	Request.ShaderType = TEXT("PixelShader");
	FString OutError;

	// Act
	bool bIsValid = UShaderGenerationHandler::ValidateRequest(Request, OutError);

	// Assert
	TestFalse(TEXT("Empty functionality should fail validation"), bIsValid);
	TestTrue(TEXT("Error message should mention functionality"), OutError.Contains(TEXT("functionality")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShaderGenerationHandler_ValidateRequest_InvalidShaderType, "AINiagara.Tools.ShaderGenerationHandler.ValidateRequest.InvalidShaderType", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FShaderGenerationHandler_ValidateRequest_InvalidShaderType::RunTest(const FString& Parameters)
{
	// Arrange
	FShaderGenerationRequest Request;
	Request.Specifications = TEXT("Particle color shader");
	Request.Functionality = TEXT("Color manipulation");
	Request.ShaderType = TEXT("InvalidShader");
	FString OutError;

	// Act
	bool bIsValid = UShaderGenerationHandler::ValidateRequest(Request, OutError);

	// Assert
	TestFalse(TEXT("Invalid shader type should fail validation"), bIsValid);
	TestTrue(TEXT("Error message should mention shader type"), OutError.Contains(TEXT("shader type")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMaterialGenerationHandler_ValidateRequest_ValidRequest, "AINiagara.Tools.MaterialGenerationHandler.ValidateRequest.ValidRequest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FMaterialGenerationHandler_ValidateRequest_ValidRequest::RunTest(const FString& Parameters)
{
	// Arrange
	FMaterialGenerationRequest Request;
	Request.MaterialName = TEXT("M_TestMaterial");
	Request.BlendMode = TEXT("Translucent");
	FString OutError;

	// Act
	bool bIsValid = UMaterialGenerationHandler::ValidateRequest(Request, OutError);

	// Assert
	TestTrue(TEXT("Valid request should pass validation"), bIsValid);
	TestTrue(TEXT("Error message should be empty for valid request"), OutError.IsEmpty());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMaterialGenerationHandler_ValidateRequest_InvalidBlendMode, "AINiagara.Tools.MaterialGenerationHandler.ValidateRequest.InvalidBlendMode", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FMaterialGenerationHandler_ValidateRequest_InvalidBlendMode::RunTest(const FString& Parameters)
{
	// Arrange
	FMaterialGenerationRequest Request;
	Request.MaterialName = TEXT("M_TestMaterial");
	Request.BlendMode = TEXT("InvalidBlendMode");
	FString OutError;

	// Act
	bool bIsValid = UMaterialGenerationHandler::ValidateRequest(Request, OutError);

	// Assert
	TestFalse(TEXT("Invalid blend mode should fail validation"), bIsValid);
	TestTrue(TEXT("Error message should mention blend mode"), OutError.Contains(TEXT("blend mode")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShaderGenerationHandler_ExtractHLSL_CodeBlock, "AINiagara.Tools.ShaderGenerationHandler.ExtractHLSL.CodeBlock", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FShaderGenerationHandler_ExtractHLSL_CodeBlock::RunTest(const FString& Parameters)
{
	// Arrange
	FString AIResponse = TEXT("Here's your shader:\n```hlsl\nfloat4 MainPS() { return float4(1,1,1,1); }\n```\nDone!");
	FString OutHLSLCode;

	// Act
	bool bSuccess = UShaderGenerationHandler::ExtractHLSLFromAIResponse(AIResponse, OutHLSLCode);

	// Assert
	TestTrue(TEXT("Extraction should succeed"), bSuccess);
	TestTrue(TEXT("HLSL code should be extracted"), OutHLSLCode.Contains(TEXT("MainPS")));
	TestFalse(TEXT("HLSL code should not contain markdown"), OutHLSLCode.Contains(TEXT("```")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShaderGenerationHandler_ExtractHLSL_GenericCodeBlock, "AINiagara.Tools.ShaderGenerationHandler.ExtractHLSL.GenericCodeBlock", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FShaderGenerationHandler_ExtractHLSL_GenericCodeBlock::RunTest(const FString& Parameters)
{
	// Arrange
	FString AIResponse = TEXT("Shader code:\n```\nfloat4 MainPS() { return float4(1,1,1,1); }\n```");
	FString OutHLSLCode;

	// Act
	bool bSuccess = UShaderGenerationHandler::ExtractHLSLFromAIResponse(AIResponse, OutHLSLCode);

	// Assert
	TestTrue(TEXT("Extraction should succeed"), bSuccess);
	TestTrue(TEXT("HLSL code should be extracted"), OutHLSLCode.Contains(TEXT("MainPS")));

	return true;
}

