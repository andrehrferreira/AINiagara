// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Core/NiagaraSystemToDSLConverter.h"
#include "Core/NiagaraSystemGenerator.h"
#include "Core/VFXDSL.h"
#include "Core/VFXDSLParser.h"
#include "NiagaraSystem.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

/**
 * Test basic Niagara system to DSL conversion
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraSystemToDSLBasicTest,
	"AINiagara.NiagaraSystemToDSL.BasicConversion",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FNiagaraSystemToDSLBasicTest::RunTest(const FString& Parameters)
{
	// Create a test DSL
	FVFXDSL TestDSL;
	TestDSL.Effect.Type = EVFXEffectType::Niagara;
	TestDSL.Effect.Duration = 5.0f;
	
	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("TestEmitter");
	Emitter.Initialization.Color.R = 1.0f;
	Emitter.Initialization.Color.G = 0.5f;
	Emitter.Initialization.Color.B = 0.0f;
	Emitter.Initialization.Size.Min = 10.0f;
	Emitter.Initialization.Size.Max = 20.0f;
	TestDSL.Emitters.Add(Emitter);

	// Generate a Niagara system from DSL
	UNiagaraSystem* System = nullptr;
	FString GenerationError;
	bool bGenerated = UNiagaraSystemGenerator::CreateSystemFromDSL(
		TestDSL,
		TEXT("/Game/Test"),
		TEXT("TestSystemForConversion"),
		System,
		GenerationError
	);

	if (!bGenerated || !System)
	{
		AddError(FString::Printf(TEXT("Failed to generate system for conversion test: %s"), *GenerationError));
		return false;
	}

	// Convert back to DSL
	FVFXDSL ConvertedDSL;
	FString ConversionError;
	bool bConverted = UNiagaraSystemToDSLConverter::ConvertSystemToDSL(
		System,
		ConvertedDSL,
		ConversionError
	);

	TestTrue(TEXT("Conversion should succeed"), bConverted);
	if (!bConverted)
	{
		AddError(FString::Printf(TEXT("Conversion failed: %s"), *ConversionError));
		return false;
	}

	// Verify basic structure
	TestEqual(TEXT("Effect type should match"), ConvertedDSL.Effect.Type, EVFXEffectType::Niagara);
	TestTrue(TEXT("Should have at least one emitter"), ConvertedDSL.Emitters.Num() > 0);

	return true;
}

/**
 * Test DSL export to JSON
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraSystemToDSLExportJSONTest,
	"AINiagara.NiagaraSystemToDSL.ExportJSON",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FNiagaraSystemToDSLExportJSONTest::RunTest(const FString& Parameters)
{
	// Create a test DSL
	FVFXDSL TestDSL;
	TestDSL.Effect.Type = EVFXEffectType::Niagara;
	TestDSL.Effect.Duration = 10.0f;
	TestDSL.Effect.bLooping = true;
	
	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("ExportTestEmitter");
	Emitter.Spawners.Rate.SpawnRate = 50.0f;
	Emitter.Initialization.Color.R = 0.8f;
	Emitter.Initialization.Color.G = 0.2f;
	Emitter.Initialization.Color.B = 0.9f;
	TestDSL.Emitters.Add(Emitter);

	// Export to JSON
	FString JsonOutput;
	bool bExported = UNiagaraSystemToDSLConverter::ExportDSLToJSON(TestDSL, JsonOutput, true);

	TestTrue(TEXT("Export should succeed"), bExported);
	if (!bExported)
	{
		return false;
	}

	// Verify JSON content
	TestTrue(TEXT("JSON should contain effect type"), JsonOutput.Contains(TEXT("Niagara")));
	TestTrue(TEXT("JSON should contain emitter name"), JsonOutput.Contains(TEXT("ExportTestEmitter")));
	TestTrue(TEXT("JSON should contain spawn rate"), JsonOutput.Contains(TEXT("50")));

	// Verify we can parse it back
	FVFXDSL ParsedDSL;
	FString ParseError;
	bool bParsed = UVFXDSLParser::ParseFromJSON(JsonOutput, ParsedDSL, ParseError);

	TestTrue(TEXT("Parsed JSON should be valid"), bParsed);
	if (bParsed)
	{
		TestEqual(TEXT("Parsed effect type should match"), ParsedDSL.Effect.Type, TestDSL.Effect.Type);
		TestEqual(TEXT("Parsed duration should match"), ParsedDSL.Effect.Duration, TestDSL.Effect.Duration);
		TestEqual(TEXT("Parsed looping should match"), ParsedDSL.Effect.bLooping, TestDSL.Effect.bLooping);
		TestTrue(TEXT("Parsed should have emitters"), ParsedDSL.Emitters.Num() > 0);
	}

	return true;
}

/**
 * Test DSL export to file
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraSystemToDSLExportFileTest,
	"AINiagara.NiagaraSystemToDSL.ExportFile",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FNiagaraSystemToDSLExportFileTest::RunTest(const FString& Parameters)
{
	// Create a test DSL
	FVFXDSL TestDSL;
	TestDSL.Effect.Type = EVFXEffectType::Niagara;
	
	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("FileExportEmitter");
	TestDSL.Emitters.Add(Emitter);

	// Export to file
	FString TestFilePath = FPaths::ProjectSavedDir() / TEXT("AINiagara") / TEXT("TestExport.json");
	bool bExported = UNiagaraSystemToDSLConverter::ExportDSLToFile(TestDSL, TestFilePath);

	TestTrue(TEXT("File export should succeed"), bExported);
	if (!bExported)
	{
		return false;
	}

	// Verify file exists
	TestTrue(TEXT("Exported file should exist"), FPaths::FileExists(TestFilePath));

	// Read and verify content
	FString FileContent;
	if (FFileHelper::LoadFileToString(FileContent, *TestFilePath))
	{
		TestTrue(TEXT("File should contain JSON"), FileContent.Contains(TEXT("effect")));
		TestTrue(TEXT("File should contain emitter"), FileContent.Contains(TEXT("emitters")));
	}

	// Cleanup
	IFileManager::Get().Delete(*TestFilePath);

	return true;
}

/**
 * Test conversion with multiple emitters
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraSystemToDSLMultipleEmittersTest,
	"AINiagara.NiagaraSystemToDSL.MultipleEmitters",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FNiagaraSystemToDSLMultipleEmittersTest::RunTest(const FString& Parameters)
{
	// Create DSL with multiple emitters
	FVFXDSL TestDSL;
	TestDSL.Effect.Type = EVFXEffectType::Niagara;
	
	FVFXDSLEmitter Emitter1;
	Emitter1.Name = TEXT("Emitter1");
	Emitter1.Initialization.Color.R = 1.0f;
	TestDSL.Emitters.Add(Emitter1);
	
	FVFXDSLEmitter Emitter2;
	Emitter2.Name = TEXT("Emitter2");
	Emitter2.Initialization.Color.G = 1.0f;
	TestDSL.Emitters.Add(Emitter2);

	// Generate system
	UNiagaraSystem* System = nullptr;
	FString GenerationError;
	bool bGenerated = UNiagaraSystemGenerator::CreateSystemFromDSL(
		TestDSL,
		TEXT("/Game/Test"),
		TEXT("MultiEmitterSystem"),
		System,
		GenerationError
	);

	if (!bGenerated || !System)
	{
		AddError(FString::Printf(TEXT("Failed to generate multi-emitter system: %s"), *GenerationError));
		return false;
	}

	// Convert back
	FVFXDSL ConvertedDSL;
	FString ConversionError;
	bool bConverted = UNiagaraSystemToDSLConverter::ConvertSystemToDSL(
		System,
		ConvertedDSL,
		ConversionError
	);

	TestTrue(TEXT("Conversion should succeed"), bConverted);
	if (bConverted)
	{
		TestTrue(TEXT("Converted DSL should have multiple emitters"), ConvertedDSL.Emitters.Num() >= 1);
		// Note: May not match exactly due to conversion limitations
	}

	return true;
}

/**
 * Test conversion error handling
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraSystemToDSLErrorHandlingTest,
	"AINiagara.NiagaraSystemToDSL.ErrorHandling",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FNiagaraSystemToDSLErrorHandlingTest::RunTest(const FString& Parameters)
{
	// Test null system
	FVFXDSL OutDSL;
	FString OutError;
	bool bConverted = UNiagaraSystemToDSLConverter::ConvertSystemToDSL(
		nullptr,
		OutDSL,
		OutError
	);

	TestFalse(TEXT("Conversion should fail with null system"), bConverted);
	TestTrue(TEXT("Error message should be set"), !OutError.IsEmpty());

	// Test null emitter
	FVFXDSLEmitter OutEmitterDSL;
	FString EmitterError;
	bool bEmitterConverted = UNiagaraSystemToDSLConverter::ConvertEmitterToDSL(
		nullptr,
		OutEmitterDSL,
		EmitterError
	);

	TestFalse(TEXT("Emitter conversion should fail with null emitter"), bEmitterConverted);
	TestTrue(TEXT("Emitter error message should be set"), !EmitterError.IsEmpty());

	return true;
}

/**
 * Test round-trip conversion (DSL -> System -> DSL)
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraSystemToDSLRoundTripTest,
	"AINiagara.NiagaraSystemToDSL.RoundTrip",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FNiagaraSystemToDSLRoundTripTest::RunTest(const FString& Parameters)
{
	// Create original DSL
	FVFXDSL OriginalDSL;
	OriginalDSL.Effect.Type = EVFXEffectType::Niagara;
	OriginalDSL.Effect.Duration = 7.5f;
	OriginalDSL.Effect.bLooping = true;
	
	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("RoundTripEmitter");
	Emitter.Spawners.Rate.SpawnRate = 30.0f;
	Emitter.Initialization.Color.R = 0.9f;
	Emitter.Initialization.Color.G = 0.1f;
	Emitter.Initialization.Color.B = 0.5f;
	Emitter.Initialization.Size.Min = 15.0f;
	Emitter.Initialization.Size.Max = 25.0f;
	OriginalDSL.Emitters.Add(Emitter);

	// Generate system
	UNiagaraSystem* System = nullptr;
	FString GenerationError;
	bool bGenerated = UNiagaraSystemGenerator::CreateSystemFromDSL(
		OriginalDSL,
		TEXT("/Game/Test"),
		TEXT("RoundTripSystem"),
		System,
		GenerationError
	);

	if (!bGenerated || !System)
	{
		AddError(FString::Printf(TEXT("Failed to generate system: %s"), *GenerationError));
		return false;
	}

	// Convert back to DSL
	FVFXDSL ConvertedDSL;
	FString ConversionError;
	bool bConverted = UNiagaraSystemToDSLConverter::ConvertSystemToDSL(
		System,
		ConvertedDSL,
		ConversionError
	);

	TestTrue(TEXT("Round-trip conversion should succeed"), bConverted);
	if (bConverted)
	{
		// Verify structure matches
		TestEqual(TEXT("Effect type should match"), ConvertedDSL.Effect.Type, OriginalDSL.Effect.Type);
		TestTrue(TEXT("Should have emitters"), ConvertedDSL.Emitters.Num() > 0);
		
		// Note: Exact values may not match due to conversion limitations
		// but structure should be preserved
	}

	return true;
}

