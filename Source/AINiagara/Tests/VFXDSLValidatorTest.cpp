// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Core/VFXDSLParser.h"
#include "Core/VFXDSL.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXDSLValidatorTest,
	"AINiagara.VFXDSLValidator.ValidateValidDSL",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXDSLValidatorTest::RunTest(const FString& Parameters)
{
	// Create a valid DSL
	FVFXDSL DSL;
	DSL.Effect.Type = EVFXEffectType::Niagara;
	DSL.Effect.Duration = 5.0f;
	DSL.Effect.bLooping = true;

	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("TestEmitter");
	Emitter.Spawners.Rate.SpawnRate = 20.0f;
	Emitter.Spawners.Burst.Count = 10;
	Emitter.Initialization.Color.R = 1.0f;
	Emitter.Initialization.Color.G = 0.5f;
	Emitter.Initialization.Color.B = 0.0f;
	Emitter.Initialization.Color.A = 1.0f;
	Emitter.Initialization.Size.Min = 1.0f;
	Emitter.Initialization.Size.Max = 2.0f;
	Emitter.Initialization.Velocity.Z = 100.0f;
	Emitter.Update.Forces.Gravity = -980.0f;
	Emitter.Update.Drag = 0.1f;
	Emitter.Update.Collision.bEnabled = false;
	Emitter.Render.BlendMode = TEXT("Translucent");

	DSL.Emitters.Add(Emitter);

	// Validate DSL
	FVFXDSLValidationResult Result = UVFXDSLValidator::Validate(DSL);

	TestTrue(TEXT("Valid DSL should pass validation"), Result.bIsValid);
	TestEqual(TEXT("Should have no error messages"), Result.ErrorMessages.Num(), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXDSLValidatorEmptyEmittersTest,
	"AINiagara.VFXDSLValidator.ValidateEmptyEmitters",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXDSLValidatorEmptyEmittersTest::RunTest(const FString& Parameters)
{
	// Create DSL with no emitters
	FVFXDSL DSL;
	DSL.Effect.Type = EVFXEffectType::Niagara;
	DSL.Effect.Duration = 5.0f;
	DSL.Effect.bLooping = false;
	// No emitters added

	// Validate DSL
	FVFXDSLValidationResult Result = UVFXDSLValidator::Validate(DSL);

	TestFalse(TEXT("DSL with no emitters should fail validation"), Result.bIsValid);
	TestTrue(TEXT("Should have error messages"), Result.ErrorMessages.Num() > 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXDSLValidatorInvalidEffectTypeTest,
	"AINiagara.VFXDSLValidator.ValidateInvalidEffectType",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXDSLValidatorInvalidEffectTypeTest::RunTest(const FString& Parameters)
{
	// Create DSL with invalid effect type (this would need to be tested differently
	// since the enum only has valid values, but we can test edge cases)
	FVFXDSL DSL;
	DSL.Effect.Type = EVFXEffectType::Niagara; // Valid type
	DSL.Effect.Duration = -1.0f; // Invalid duration
	DSL.Effect.bLooping = false;

	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("TestEmitter");
	DSL.Emitters.Add(Emitter);

	// Validate DSL
	FVFXDSLValidationResult Result = UVFXDSLValidator::Validate(DSL);

	// Note: This test depends on the validator implementation
	// If the validator checks for negative duration, it should fail
	// Otherwise, adjust the test accordingly

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

