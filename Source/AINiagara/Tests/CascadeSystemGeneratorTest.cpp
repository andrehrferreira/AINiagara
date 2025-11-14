// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Core/CascadeSystemGenerator.h"
#include "Core/CascadeSystemToDSLConverter.h"
#include "Core/VFXDSL.h"
#include "Core/VFXDSLParser.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleEmitter.h"

/**
 * Test basic Cascade system generation from DSL
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCascadeSystemGeneratorBasicTest,
	"AINiagara.CascadeSystemGenerator.BasicGeneration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FCascadeSystemGeneratorBasicTest::RunTest(const FString& Parameters)
{
	FVFXDSL DSL;
	DSL.Effect.Type = EVFXEffectType::Cascade;
	DSL.Effect.Duration = 5.0f;
	
	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("TestCascadeEmitter");
	Emitter.Initialization.Color.R = 1.0f;
	Emitter.Initialization.Color.G = 0.5f;
	Emitter.Initialization.Color.B = 0.0f;
	Emitter.Initialization.Size.Min = 10.0f;
	Emitter.Initialization.Size.Max = 20.0f;
	DSL.Emitters.Add(Emitter);

	UParticleSystem* System = nullptr;
	FString Error;
	bool bSuccess = UCascadeSystemGenerator::CreateSystemFromDSL(
		DSL, 
		TEXT("/Game/Test"), 
		TEXT("TestCascadeSystem"), 
		System, 
		Error
	);
	
	TestTrue(TEXT("System generation should succeed"), bSuccess);
	if (bSuccess)
	{
		TestNotNull(TEXT("Cascade system should be generated"), System);
	}

	if (System)
	{
		TestTrue(TEXT("System should have emitters"), System->Emitters.Num() > 0);
		TestEqual(TEXT("System duration should match"), System->Duration, DSL.Effect.Duration);
	}

	return true;
}

/**
 * Test Cascade system with multiple emitters
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCascadeSystemGeneratorMultipleEmittersTest,
	"AINiagara.CascadeSystemGenerator.MultipleEmitters",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FCascadeSystemGeneratorMultipleEmittersTest::RunTest(const FString& Parameters)
{
	FVFXDSL DSL;
	DSL.Effect.Type = EVFXEffectType::Cascade;
	
	FVFXDSLEmitter Emitter1;
	Emitter1.Name = TEXT("Emitter1");
	Emitter1.Initialization.Color.R = 1.0f;
	DSL.Emitters.Add(Emitter1);
	
	FVFXDSLEmitter Emitter2;
	Emitter2.Name = TEXT("Emitter2");
	Emitter2.Initialization.Color.G = 1.0f;
	DSL.Emitters.Add(Emitter2);

	UParticleSystem* System = nullptr;
	FString Error;
	bool bSuccess = UCascadeSystemGenerator::CreateSystemFromDSL(
		DSL, 
		TEXT("/Game/Test"), 
		TEXT("MultiEmitterCascadeSystem"), 
		System, 
		Error
	);
	
	TestTrue(TEXT("System generation should succeed"), bSuccess);
	if (bSuccess && System)
	{
		TestTrue(TEXT("System should have multiple emitters"), System->Emitters.Num() >= 2);
	}

	return true;
}

/**
 * Test Cascade system with spawn rate configuration
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCascadeSystemGeneratorSpawnRateTest,
	"AINiagara.CascadeSystemGenerator.SpawnRate",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FCascadeSystemGeneratorSpawnRateTest::RunTest(const FString& Parameters)
{
	FVFXDSL DSL;
	DSL.Effect.Type = EVFXEffectType::Cascade;
	
	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("SpawnRateEmitter");
	Emitter.Spawners.Rate.SpawnRate = 50.0f;
	DSL.Emitters.Add(Emitter);

	UParticleSystem* System = nullptr;
	FString Error;
	bool bSuccess = UCascadeSystemGenerator::CreateSystemFromDSL(
		DSL, 
		TEXT("/Game/Test"), 
		TEXT("SpawnRateCascadeSystem"), 
		System, 
		Error
	);
	
	TestTrue(TEXT("System generation should succeed"), bSuccess);
	if (bSuccess && System && System->Emitters.Num() > 0)
	{
		UParticleEmitter* GeneratedEmitter = System->Emitters[0];
		TestNotNull(TEXT("Emitter should be valid"), GeneratedEmitter);
	}

	return true;
}

/**
 * Test Cascade system with burst spawn configuration
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCascadeSystemGeneratorBurstTest,
	"AINiagara.CascadeSystemGenerator.BurstSpawn",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FCascadeSystemGeneratorBurstTest::RunTest(const FString& Parameters)
{
	FVFXDSL DSL;
	DSL.Effect.Type = EVFXEffectType::Cascade;
	
	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("BurstEmitter");
	Emitter.Spawners.Burst.Count = 100;
	Emitter.Spawners.Burst.Time = 0.0f;
	Emitter.Spawners.Burst.Intervals.Add(1.0f);
	Emitter.Spawners.Burst.Intervals.Add(2.0f);
	DSL.Emitters.Add(Emitter);

	UParticleSystem* System = nullptr;
	FString Error;
	bool bSuccess = UCascadeSystemGenerator::CreateSystemFromDSL(
		DSL, 
		TEXT("/Game/Test"), 
		TEXT("BurstCascadeSystem"), 
		System, 
		Error
	);
	
	TestTrue(TEXT("System generation should succeed"), bSuccess);
	if (bSuccess && System && System->Emitters.Num() > 0)
	{
		UParticleEmitter* GeneratedEmitter = System->Emitters[0];
		TestNotNull(TEXT("Emitter should be valid"), GeneratedEmitter);
	}

	return true;
}

/**
 * Test Cascade system with color configuration
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCascadeSystemGeneratorColorTest,
	"AINiagara.CascadeSystemGenerator.Color",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FCascadeSystemGeneratorColorTest::RunTest(const FString& Parameters)
{
	FVFXDSL DSL;
	DSL.Effect.Type = EVFXEffectType::Cascade;
	
	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("ColorEmitter");
	Emitter.Initialization.Color.R = 0.8f;
	Emitter.Initialization.Color.G = 0.2f;
	Emitter.Initialization.Color.B = 0.9f;
	Emitter.Initialization.Color.A = 1.0f;
	DSL.Emitters.Add(Emitter);

	UParticleSystem* System = nullptr;
	FString Error;
	bool bSuccess = UCascadeSystemGenerator::CreateSystemFromDSL(
		DSL, 
		TEXT("/Game/Test"), 
		TEXT("ColorCascadeSystem"), 
		System, 
		Error
	);
	
	TestTrue(TEXT("System generation should succeed"), bSuccess);
	if (bSuccess && System && System->Emitters.Num() > 0)
	{
		UParticleEmitter* GeneratedEmitter = System->Emitters[0];
		TestNotNull(TEXT("Emitter should be valid"), GeneratedEmitter);
	}

	return true;
}

/**
 * Test Cascade system with velocity configuration
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCascadeSystemGeneratorVelocityTest,
	"AINiagara.CascadeSystemGenerator.Velocity",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FCascadeSystemGeneratorVelocityTest::RunTest(const FString& Parameters)
{
	FVFXDSL DSL;
	DSL.Effect.Type = EVFXEffectType::Cascade;
	
	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("VelocityEmitter");
	Emitter.Initialization.Velocity.X = 0.0f;
	Emitter.Initialization.Velocity.Y = 0.0f;
	Emitter.Initialization.Velocity.Z = 200.0f;
	DSL.Emitters.Add(Emitter);

	UParticleSystem* System = nullptr;
	FString Error;
	bool bSuccess = UCascadeSystemGenerator::CreateSystemFromDSL(
		DSL, 
		TEXT("/Game/Test"), 
		TEXT("VelocityCascadeSystem"), 
		System, 
		Error
	);
	
	TestTrue(TEXT("System generation should succeed"), bSuccess);
	if (bSuccess && System && System->Emitters.Num() > 0)
	{
		UParticleEmitter* GeneratedEmitter = System->Emitters[0];
		TestNotNull(TEXT("Emitter should be valid"), GeneratedEmitter);
	}

	return true;
}

/**
 * Test Cascade system with forces configuration
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCascadeSystemGeneratorForcesTest,
	"AINiagara.CascadeSystemGenerator.Forces",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FCascadeSystemGeneratorForcesTest::RunTest(const FString& Parameters)
{
	FVFXDSL DSL;
	DSL.Effect.Type = EVFXEffectType::Cascade;
	
	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("ForcesEmitter");
	Emitter.Update.Forces.Gravity = -980.0f;
	Emitter.Update.Forces.Wind.X = 50.0f;
	Emitter.Update.Forces.Wind.Y = 0.0f;
	Emitter.Update.Forces.Wind.Z = 0.0f;
	DSL.Emitters.Add(Emitter);

	UParticleSystem* System = nullptr;
	FString Error;
	bool bSuccess = UCascadeSystemGenerator::CreateSystemFromDSL(
		DSL, 
		TEXT("/Game/Test"), 
		TEXT("ForcesCascadeSystem"), 
		System, 
		Error
	);
	
	TestTrue(TEXT("System generation should succeed"), bSuccess);
	if (bSuccess && System && System->Emitters.Num() > 0)
	{
		UParticleEmitter* GeneratedEmitter = System->Emitters[0];
		TestNotNull(TEXT("Emitter should be valid"), GeneratedEmitter);
	}

	return true;
}

/**
 * Test Cascade system with blend mode configuration
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCascadeSystemGeneratorBlendModeTest,
	"AINiagara.CascadeSystemGenerator.BlendMode",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FCascadeSystemGeneratorBlendModeTest::RunTest(const FString& Parameters)
{
	FVFXDSL DSL;
	DSL.Effect.Type = EVFXEffectType::Cascade;
	
	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("BlendModeEmitter");
	Emitter.Render.BlendMode = TEXT("Additive");
	DSL.Emitters.Add(Emitter);

	UParticleSystem* System = nullptr;
	FString Error;
	bool bSuccess = UCascadeSystemGenerator::CreateSystemFromDSL(
		DSL, 
		TEXT("/Game/Test"), 
		TEXT("BlendModeCascadeSystem"), 
		System, 
		Error
	);
	
	TestTrue(TEXT("System generation should succeed"), bSuccess);
	if (bSuccess && System && System->Emitters.Num() > 0)
	{
		UParticleEmitter* GeneratedEmitter = System->Emitters[0];
		TestNotNull(TEXT("Emitter should be valid"), GeneratedEmitter);
	}

	return true;
}

/**
 * Test Cascade system error handling
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCascadeSystemGeneratorErrorHandlingTest,
	"AINiagara.CascadeSystemGenerator.ErrorHandling",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FCascadeSystemGeneratorErrorHandlingTest::RunTest(const FString& Parameters)
{
	// Test null DSL (invalid type)
	FVFXDSL WrongTypeDSL;
	WrongTypeDSL.Effect.Type = EVFXEffectType::Niagara; // Wrong type
	
	UParticleSystem* System = nullptr;
	FString Error;
	bool bSuccess = UCascadeSystemGenerator::CreateSystemFromDSL(
		WrongTypeDSL, 
		TEXT("/Game/Test"), 
		TEXT("WrongTypeSystem"), 
		System, 
		Error
	);
	
	TestFalse(TEXT("Should fail with wrong DSL type"), bSuccess);
	TestTrue(TEXT("Error message should be set"), !Error.IsEmpty());

	// Test empty emitters
	FVFXDSL EmptyEmittersDSL;
	EmptyEmittersDSL.Effect.Type = EVFXEffectType::Cascade;
	
	bSuccess = UCascadeSystemGenerator::CreateSystemFromDSL(
		EmptyEmittersDSL, 
		TEXT("/Game/Test"), 
		TEXT("EmptyEmittersSystem"), 
		System, 
		Error
	);
	
	TestFalse(TEXT("Should fail with no emitters"), bSuccess);
	TestTrue(TEXT("Error message should be set"), !Error.IsEmpty());

	return true;
}

/**
 * Test Cascade system round-trip conversion (DSL → System → DSL)
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCascadeSystemRoundTripTest,
	"AINiagara.CascadeSystemGenerator.RoundTrip",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FCascadeSystemRoundTripTest::RunTest(const FString& Parameters)
{
	// Create original DSL
	FVFXDSL OriginalDSL;
	OriginalDSL.Effect.Type = EVFXEffectType::Cascade;
	OriginalDSL.Effect.Duration = 7.5f;
	
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
	UParticleSystem* System = nullptr;
	FString GenerationError;
	bool bGenerated = UCascadeSystemGenerator::CreateSystemFromDSL(
		OriginalDSL,
		TEXT("/Game/Test"),
		TEXT("RoundTripCascadeSystem"),
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
	bool bConverted = UCascadeSystemToDSLConverter::ConvertSystemToDSL(
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

/**
 * Test Cascade system to DSL conversion
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCascadeSystemToDSLBasicTest,
	"AINiagara.CascadeSystemToDSL.BasicConversion",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FCascadeSystemToDSLBasicTest::RunTest(const FString& Parameters)
{
	// Create a test DSL
	FVFXDSL TestDSL;
	TestDSL.Effect.Type = EVFXEffectType::Cascade;
	TestDSL.Effect.Duration = 5.0f;
	
	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("TestCascadeEmitter");
	Emitter.Initialization.Color.R = 1.0f;
	Emitter.Initialization.Size.Min = 10.0f;
	Emitter.Initialization.Size.Max = 20.0f;
	TestDSL.Emitters.Add(Emitter);

	// Generate a Cascade system from DSL
	UParticleSystem* System = nullptr;
	FString GenerationError;
	bool bGenerated = UCascadeSystemGenerator::CreateSystemFromDSL(
		TestDSL,
		TEXT("/Game/Test"),
		TEXT("TestSystemForCascadeConversion"),
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
	bool bConverted = UCascadeSystemToDSLConverter::ConvertSystemToDSL(
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
	TestEqual(TEXT("Effect type should match"), ConvertedDSL.Effect.Type, EVFXEffectType::Cascade);
	TestTrue(TEXT("Should have at least one emitter"), ConvertedDSL.Emitters.Num() > 0);

	return true;
}

/**
 * Test Cascade system to DSL conversion error handling
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCascadeSystemToDSLErrorHandlingTest,
	"AINiagara.CascadeSystemToDSL.ErrorHandling",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FCascadeSystemToDSLErrorHandlingTest::RunTest(const FString& Parameters)
{
	// Test null system
	FVFXDSL OutDSL;
	FString OutError;
	bool bConverted = UCascadeSystemToDSLConverter::ConvertSystemToDSL(
		nullptr,
		OutDSL,
		OutError
	);

	TestFalse(TEXT("Conversion should fail with null system"), bConverted);
	TestTrue(TEXT("Error message should be set"), !OutError.IsEmpty());

	// Test null emitter
	FVFXDSLEmitter OutEmitterDSL;
	FString EmitterError;
	bool bEmitterConverted = UCascadeSystemToDSLConverter::ConvertEmitterToDSL(
		nullptr,
		OutEmitterDSL,
		EmitterError
	);

	TestFalse(TEXT("Emitter conversion should fail with null emitter"), bEmitterConverted);
	TestTrue(TEXT("Emitter error message should be set"), !EmitterError.IsEmpty());

	return true;
}

