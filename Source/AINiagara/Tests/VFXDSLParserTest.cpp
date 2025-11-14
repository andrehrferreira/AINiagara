// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Core/VFXDSLParser.h"
#include "Core/VFXDSL.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXDSLParserTest,
	"AINiagara.VFXDSLParser.ParseValidJSON",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXDSLParserTest::RunTest(const FString& Parameters)
{
	// Test valid DSL JSON parsing
	FString ValidJSON = TEXT(R"({
		"effect": {
			"type": "Niagara",
			"duration": 5.0,
			"looping": true
		},
		"emitters": [
			{
				"name": "TestEmitter",
				"spawners": {
					"burst": {
						"count": 10,
						"time": 0.0,
						"intervals": []
					},
					"rate": {
						"spawnRate": 20.0,
						"scaleOverTime": 1.0
					}
				},
				"initialization": {
					"color": {
						"r": 1.0,
						"g": 0.5,
						"b": 0.0,
						"a": 1.0
					},
					"size": {
						"min": 1.0,
						"max": 2.0
					},
					"velocity": {
						"x": 0.0,
						"y": 0.0,
						"z": 100.0
					}
				},
				"update": {
					"forces": {
						"gravity": -980.0,
						"wind": {
							"x": 0.0,
							"y": 0.0,
							"z": 0.0
						}
					},
					"drag": 0.1,
					"collision": {
						"enabled": false,
						"bounce": 0.5
					}
				},
				"render": {
					"material": "",
					"texture": "",
					"blendMode": "Translucent",
					"sort": "ViewDepth"
				}
			}
		]
	})");

	FVFXDSL DSL;
	FString ParseError;
	bool bParseSuccess = UVFXDSLParser::ParseFromJSON(ValidJSON, DSL, ParseError);

	TestTrue(TEXT("Valid JSON should parse successfully"), bParseSuccess);
	TestTrue(TEXT("Parse error should be empty on success"), ParseError.IsEmpty());
	TestEqual(TEXT("Effect type should be Niagara"), DSL.Effect.Type, EVFXEffectType::Niagara);
	TestEqual(TEXT("Effect duration should be 5.0"), DSL.Effect.Duration, 5.0f);
	TestTrue(TEXT("Effect should be looping"), DSL.Effect.bLooping);
	TestEqual(TEXT("Should have one emitter"), DSL.Emitters.Num(), 1);
	
	if (DSL.Emitters.Num() > 0)
	{
		const FVFXDSLEmitter& Emitter = DSL.Emitters[0];
		TestEqual(TEXT("Emitter name should match"), Emitter.Name, TEXT("TestEmitter"));
		TestEqual(TEXT("Spawn rate should be 20.0"), Emitter.Spawners.Rate.SpawnRate, 20.0f);
		TestEqual(TEXT("Burst count should be 10"), Emitter.Spawners.Burst.Count, 10);
		TestEqual(TEXT("Color R should be 1.0"), Emitter.Initialization.Color.R, 1.0f);
		TestEqual(TEXT("Color G should be 0.5"), Emitter.Initialization.Color.G, 0.5f);
		TestEqual(TEXT("Velocity Z should be 100.0"), Emitter.Initialization.Velocity.Z, 100.0f);
		TestEqual(TEXT("Gravity should be -980.0"), Emitter.Update.Forces.Gravity, -980.0f);
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXDSLParserInvalidJSONTest,
	"AINiagara.VFXDSLParser.ParseInvalidJSON",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXDSLParserInvalidJSONTest::RunTest(const FString& Parameters)
{
	// Test invalid JSON parsing
	FString InvalidJSON = TEXT("{ invalid json }");
	
	FVFXDSL DSL;
	FString ParseError;
	bool bParseSuccess = UVFXDSLParser::ParseFromJSON(InvalidJSON, DSL, ParseError);

	TestFalse(TEXT("Invalid JSON should fail to parse"), bParseSuccess);
	TestFalse(TEXT("Parse error should not be empty on failure"), ParseError.IsEmpty());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXDSLParserEmptyEmitterTest,
	"AINiagara.VFXDSLParser.ParseEmptyEmitters",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXDSLParserEmptyEmitterTest::RunTest(const FString& Parameters)
{
	// Test JSON with empty emitters array
	FString JSONWithEmptyEmitters = TEXT(R"({
		"effect": {
			"type": "Niagara",
			"duration": 5.0,
			"looping": false
		},
		"emitters": []
	})");

	FVFXDSL DSL;
	FString ParseError;
	bool bParseSuccess = UVFXDSLParser::ParseFromJSON(JSONWithEmptyEmitters, DSL, ParseError);

	TestTrue(TEXT("JSON with empty emitters should parse"), bParseSuccess);
	TestEqual(TEXT("Emitters array should be empty"), DSL.Emitters.Num(), 0);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

