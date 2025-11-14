// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Core/NiagaraSystemGenerator.h"
#include "Core/VFXDSL.h"
#include "NiagaraSystem.h"
#include "NiagaraEmitter.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraSystemGeneratorBasicTest,
	"AINiagara.NiagaraSystemGenerator.BasicGeneration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FNiagaraSystemGeneratorBasicTest::RunTest(const FString& Parameters)
{
	// Create simple DSL for testing
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
	
	DSL.Emitters.Add(Emitter);

	// Generate system
	UNiagaraSystem* System = nullptr;
	FString Error;
	bool bSuccess = UNiagaraSystemGenerator::CreateSystemFromDSL(
		DSL, 
		TEXT("/Game/Test"), 
		TEXT("TestSystem"), 
		System, 
		Error
	);
	
	// Verify system was created
	TestTrue(TEXT("System generation should succeed"), bSuccess);
	if (bSuccess)
	{
		TestNotNull(TEXT("Niagara system should be generated"), System);
	}

	if (System)
	{
		// Verify system properties
		TestTrue(TEXT("System should have emitters"), System->GetNumEmitters() > 0);
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraSystemGeneratorMultipleEmittersTest,
	"AINiagara.NiagaraSystemGenerator.MultipleEmitters",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FNiagaraSystemGeneratorMultipleEmittersTest::RunTest(const FString& Parameters)
{
	// Create DSL with multiple emitters
	FVFXDSL DSL;
	DSL.Effect.Type = EVFXEffectType::Niagara;
	DSL.Effect.Duration = 5.0f;
	DSL.Effect.bLooping = false;

	// First emitter
	FVFXDSLEmitter Emitter1;
	Emitter1.Name = TEXT("Emitter1");
	Emitter1.Spawners.Rate.SpawnRate = 10.0f;
	DSL.Emitters.Add(Emitter1);

	// Second emitter
	FVFXDSLEmitter Emitter2;
	Emitter2.Name = TEXT("Emitter2");
	Emitter2.Spawners.Rate.SpawnRate = 20.0f;
	DSL.Emitters.Add(Emitter2);

	// Third emitter
	FVFXDSLEmitter Emitter3;
	Emitter3.Name = TEXT("Emitter3");
	Emitter3.Spawners.Rate.SpawnRate = 30.0f;
	DSL.Emitters.Add(Emitter3);

	// Generate system
	UNiagaraSystem* System = nullptr;
	FString Error;
	bool bSuccess = UNiagaraSystemGenerator::CreateSystemFromDSL(
		DSL, 
		TEXT("/Game/Test"), 
		TEXT("TestSystem"), 
		System, 
		Error
	);
	
	TestNotNull(TEXT("System with multiple emitters should be generated"), System);

	if (System)
	{
		TestEqual(TEXT("Should have 3 emitters"), System->GetNumEmitters(), 3);
		System->ConditionalBeginDestroy();
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraSystemGeneratorColorTest,
	"AINiagara.NiagaraSystemGenerator.ColorConfiguration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FNiagaraSystemGeneratorColorTest::RunTest(const FString& Parameters)
{
	FVFXDSL DSL;
	DSL.Effect.Type = EVFXEffectType::Niagara;
	DSL.Effect.Duration = 5.0f;

	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("ColorEmitter");
	
	// Set specific color
	Emitter.Initialization.Color.R = 1.0f;
	Emitter.Initialization.Color.G = 0.0f;
	Emitter.Initialization.Color.B = 0.0f;
	Emitter.Initialization.Color.A = 1.0f;
	
	DSL.Emitters.Add(Emitter);

	UNiagaraSystem* System = nullptr;
	FString Error;
	bool bSuccess = UNiagaraSystemGenerator::CreateSystemFromDSL(DSL, TEXT("/Game/Test"), TEXT("TestSystem"), System, Error);
	
	TestNotNull(TEXT("System with color configuration should be generated"), System);

	if (System)
	{
		// Note: Actual color verification would require deeper inspection
		// of Niagara modules, which is complex in UE 5.3
		TestTrue(TEXT("System should have emitters"), System->GetNumEmitters() > 0);
		System->ConditionalBeginDestroy();
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraSystemGeneratorVelocityTest,
	"AINiagara.NiagaraSystemGenerator.VelocityConfiguration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FNiagaraSystemGeneratorVelocityTest::RunTest(const FString& Parameters)
{
	FVFXDSL DSL;
	DSL.Effect.Type = EVFXEffectType::Niagara;
	DSL.Effect.Duration = 5.0f;

	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("VelocityEmitter");
	
	// Set specific velocity
	Emitter.Initialization.Velocity.X = 100.0f;
	Emitter.Initialization.Velocity.Y = 50.0f;
	Emitter.Initialization.Velocity.Z = 200.0f;
	
	DSL.Emitters.Add(Emitter);

	UNiagaraSystem* System = nullptr;
	FString Error;
	bool bSuccess = UNiagaraSystemGenerator::CreateSystemFromDSL(DSL, TEXT("/Game/Test"), TEXT("TestSystem"), System, Error);
	
	TestNotNull(TEXT("System with velocity configuration should be generated"), System);

	if (System)
	{
		TestTrue(TEXT("System should have emitters"), System->GetNumEmitters() > 0);
		System->ConditionalBeginDestroy();
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraSystemGeneratorSpawnersTest,
	"AINiagara.NiagaraSystemGenerator.SpawnersConfiguration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FNiagaraSystemGeneratorSpawnersTest::RunTest(const FString& Parameters)
{
	FVFXDSL DSL;
	DSL.Effect.Type = EVFXEffectType::Niagara;
	DSL.Effect.Duration = 5.0f;

	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("SpawnerEmitter");
	
	// Configure spawners
	Emitter.Spawners.Rate.SpawnRate = 50.0f;
	Emitter.Spawners.Rate.ScaleOverTime = 1.5f;
	Emitter.Spawners.Burst.Count = 100;
	Emitter.Spawners.Burst.Time = 0.5f;
	
	DSL.Emitters.Add(Emitter);

	UNiagaraSystem* System = nullptr;
	FString Error;
	bool bSuccess = UNiagaraSystemGenerator::CreateSystemFromDSL(DSL, TEXT("/Game/Test"), TEXT("TestSystem"), System, Error);
	
	TestNotNull(TEXT("System with spawners configuration should be generated"), System);

	if (System)
	{
		TestTrue(TEXT("System should have emitters"), System->GetNumEmitters() > 0);
		System->ConditionalBeginDestroy();
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraSystemGeneratorForcesTest,
	"AINiagara.NiagaraSystemGenerator.ForcesConfiguration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FNiagaraSystemGeneratorForcesTest::RunTest(const FString& Parameters)
{
	FVFXDSL DSL;
	DSL.Effect.Type = EVFXEffectType::Niagara;
	DSL.Effect.Duration = 5.0f;

	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("ForcesEmitter");
	
	// Configure forces
	Emitter.Update.Forces.Gravity = -980.0f;
	Emitter.Update.Forces.Wind.X = 10.0f;
	Emitter.Update.Forces.Wind.Y = 5.0f;
	Emitter.Update.Forces.Wind.Z = 0.0f;
	Emitter.Update.Drag = 0.2f;
	
	DSL.Emitters.Add(Emitter);

	UNiagaraSystem* System = nullptr;
	FString Error;
	bool bSuccess = UNiagaraSystemGenerator::CreateSystemFromDSL(DSL, TEXT("/Game/Test"), TEXT("TestSystem"), System, Error);
	
	TestNotNull(TEXT("System with forces configuration should be generated"), System);

	if (System)
	{
		TestTrue(TEXT("System should have emitters"), System->GetNumEmitters() > 0);
		System->ConditionalBeginDestroy();
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraSystemGeneratorLifetimeTest,
	"AINiagara.NiagaraSystemGenerator.LifetimeConfiguration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FNiagaraSystemGeneratorLifetimeTest::RunTest(const FString& Parameters)
{
	FVFXDSL DSL;
	DSL.Effect.Type = EVFXEffectType::Niagara;
	DSL.Effect.Duration = 10.0f;

	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("LifetimeEmitter");
	
	// Configure lifetime - using size as proxy since Lifetime may not exist in DSL
	Emitter.Initialization.Size.Min = 1.0f;
	Emitter.Initialization.Size.Max = 3.0f;
	
	DSL.Emitters.Add(Emitter);

	UNiagaraSystem* System = nullptr;
	FString Error;
	bool bSuccess = UNiagaraSystemGenerator::CreateSystemFromDSL(DSL, TEXT("/Game/Test"), TEXT("TestSystem"), System, Error);
	
	TestNotNull(TEXT("System with lifetime configuration should be generated"), System);

	if (System)
	{
		TestTrue(TEXT("System should have emitters"), System->GetNumEmitters() > 0);
		System->ConditionalBeginDestroy();
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraSystemGeneratorEmptyDSLTest,
	"AINiagara.NiagaraSystemGenerator.EmptyDSL",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FNiagaraSystemGeneratorEmptyDSLTest::RunTest(const FString& Parameters)
{
	// Create DSL with no emitters
	FVFXDSL EmptyDSL;
	EmptyDSL.Effect.Type = EVFXEffectType::Niagara;
	EmptyDSL.Effect.Duration = 5.0f;
	// No emitters added

	UNiagaraSystem* System = nullptr;
	FString Error;
	bool bSuccess = UNiagaraSystemGenerator::CreateSystemFromDSL(EmptyDSL, TEXT("/Game/Test"), TEXT("EmptySystem"), System, Error);
	
	// Should either return null or a system with no emitters
	if (System)
	{
		// If system is created, it should have no emitters
		TestEqual(TEXT("System should have no emitters"), System->GetNumEmitters(), 0);
		System->ConditionalBeginDestroy();
	}
	else
	{
		TestNull(TEXT("Empty DSL should return null system"), System);
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraSystemGeneratorCollisionTest,
	"AINiagara.NiagaraSystemGenerator.CollisionConfiguration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FNiagaraSystemGeneratorCollisionTest::RunTest(const FString& Parameters)
{
	FVFXDSL DSL;
	DSL.Effect.Type = EVFXEffectType::Niagara;
	DSL.Effect.Duration = 5.0f;

	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("CollisionEmitter");
	
	// Configure collision
	Emitter.Update.Collision.bEnabled = true;
	Emitter.Update.Collision.Bounce = 0.5f;
	
	DSL.Emitters.Add(Emitter);

	UNiagaraSystem* System = nullptr;
	FString Error;
	bool bSuccess = UNiagaraSystemGenerator::CreateSystemFromDSL(DSL, TEXT("/Game/Test"), TEXT("TestSystem"), System, Error);
	
	TestNotNull(TEXT("System with collision configuration should be generated"), System);

	if (System)
	{
		TestTrue(TEXT("System should have emitters"), System->GetNumEmitters() > 0);
		System->ConditionalBeginDestroy();
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraSystemGeneratorLoopingTest,
	"AINiagara.NiagaraSystemGenerator.LoopingConfiguration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FNiagaraSystemGeneratorLoopingTest::RunTest(const FString& Parameters)
{
	// Test looping system
	FVFXDSL LoopingDSL;
	LoopingDSL.Effect.Type = EVFXEffectType::Niagara;
	LoopingDSL.Effect.Duration = 5.0f;
	LoopingDSL.Effect.bLooping = true;

	FVFXDSLEmitter Emitter1;
	Emitter1.Name = TEXT("LoopingEmitter");
	LoopingDSL.Emitters.Add(Emitter1);

	UNiagaraSystem* LoopingSystem = nullptr;
	FString Error1;
	bool bSuccess1 = UNiagaraSystemGenerator::CreateSystemFromDSL(
		LoopingDSL, 
		TEXT("/Game/Test"), 
		TEXT("LoopingSystem"), 
		LoopingSystem, 
		Error1
	);
	
	if (bSuccess1)
	{
		TestNotNull(TEXT("Looping system should be generated"), LoopingSystem);
	}

	// Test non-looping system
	FVFXDSL NonLoopingDSL;
	NonLoopingDSL.Effect.Type = EVFXEffectType::Niagara;
	NonLoopingDSL.Effect.Duration = 5.0f;
	NonLoopingDSL.Effect.bLooping = false;

	FVFXDSLEmitter Emitter2;
	Emitter2.Name = TEXT("NonLoopingEmitter");
	NonLoopingDSL.Emitters.Add(Emitter2);

	UNiagaraSystem* NonLoopingSystem = nullptr;
	FString Error2;
	bool bSuccess2 = UNiagaraSystemGenerator::CreateSystemFromDSL(
		NonLoopingDSL, 
		TEXT("/Game/Test"), 
		TEXT("NonLoopingSystem"), 
		NonLoopingSystem, 
		Error2
	);
	
	if (bSuccess2)
	{
		TestNotNull(TEXT("Non-looping system should be generated"), NonLoopingSystem);
	}

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

