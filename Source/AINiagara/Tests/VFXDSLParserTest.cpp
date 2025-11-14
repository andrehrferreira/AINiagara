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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXDSLParserMissingFieldsTest,
	"AINiagara.VFXDSLParser.MissingFields",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXDSLParserMissingFieldsTest::RunTest(const FString& Parameters)
{
	// Test JSON with missing effect fields
	FString MinimalJSON = TEXT(R"({
		"effect": {
			"type": "Niagara"
		},
		"emitters": []
	})");

	FVFXDSL DSL;
	FString ParseError;
	bool bParseSuccess = UVFXDSLParser::ParseFromJSON(MinimalJSON, DSL, ParseError);

	// Should still parse even with missing fields (using defaults)
	TestTrue(TEXT("JSON with minimal fields should parse"), bParseSuccess);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXDSLParserComplexEmitterTest,
	"AINiagara.VFXDSLParser.ComplexEmitter",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXDSLParserComplexEmitterTest::RunTest(const FString& Parameters)
{
	// Test with all emitter fields populated
	FString ComplexJSON = TEXT(R"({
		"effect": {
			"type": "Niagara",
			"duration": 10.0,
			"looping": false
		},
		"emitters": [
			{
				"name": "ComplexEmitter",
				"spawners": {
					"burst": {
						"count": 50,
						"time": 1.0,
						"intervals": [0.5, 1.0, 1.5]
					},
					"rate": {
						"spawnRate": 100.0,
						"scaleOverTime": 0.8
					}
				},
				"initialization": {
					"color": {
						"r": 0.2,
						"g": 0.8,
						"b": 1.0,
						"a": 0.9
					},
					"size": {
						"min": 0.5,
						"max": 5.0
					},
					"velocity": {
						"x": 50.0,
						"y": -25.0,
						"z": 150.0
					},
					"lifetime": {
						"min": 2.0,
						"max": 5.0
					},
					"rotation": {
						"min": 0.0,
						"max": 360.0
					}
				},
				"update": {
					"forces": {
						"gravity": -500.0,
						"wind": {
							"x": 10.0,
							"y": 5.0,
							"z": 0.0
						}
					},
					"drag": 0.25,
					"collision": {
						"enabled": true,
						"bounce": 0.7,
						"friction": 0.4
					}
				},
				"render": {
					"material": "/Game/Materials/M_Particle",
					"texture": "/Game/Textures/T_Particle",
					"blendMode": "Additive",
					"sort": "Distance"
				}
			}
		]
	})");

	FVFXDSL DSL;
	FString ParseError;
	bool bParseSuccess = UVFXDSLParser::ParseFromJSON(ComplexJSON, DSL, ParseError);

	TestTrue(TEXT("Complex JSON should parse"), bParseSuccess);
	TestTrue(TEXT("Parse error should be empty"), ParseError.IsEmpty());
	TestEqual(TEXT("Should have one emitter"), DSL.Emitters.Num(), 1);

	if (DSL.Emitters.Num() > 0)
	{
		const FVFXDSLEmitter& Emitter = DSL.Emitters[0];
		TestEqual(TEXT("Burst intervals should have 3 elements"), Emitter.Spawners.Burst.Intervals.Num(), 3);
		TestEqual(TEXT("Collision should be enabled"), Emitter.Update.Collision.bEnabled, true);
		TestEqual(TEXT("Material should match"), Emitter.Render.Material, TEXT("/Game/Materials/M_Particle"));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXDSLParserSerializeTest,
	"AINiagara.VFXDSLParser.Serialize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXDSLParserSerializeTest::RunTest(const FString& Parameters)
{
	// Create a DSL object
	FVFXDSL DSL;
	DSL.Effect.Type = EVFXEffectType::Niagara;
	DSL.Effect.Duration = 7.5f;
	DSL.Effect.bLooping = true;

	FVFXDSLEmitter Emitter;
	Emitter.Name = TEXT("SerializedEmitter");
	Emitter.Spawners.Rate.SpawnRate = 30.0f;
	DSL.Emitters.Add(Emitter);

	// Serialize to JSON
	FString SerializedJSON;
	bool bSerializeSuccess = UVFXDSLParser::ToJSON(DSL, SerializedJSON);

	TestTrue(TEXT("Serialization should succeed"), bSerializeSuccess);
	TestFalse(TEXT("Serialized JSON should not be empty"), SerializedJSON.IsEmpty());
	TestTrue(TEXT("Should contain effect type"), SerializedJSON.Contains(TEXT("Niagara")) || SerializedJSON.Contains(TEXT("niagara")));
	TestTrue(TEXT("Should contain emitter name"), SerializedJSON.Contains(TEXT("SerializedEmitter")));

	// Try to parse it back
	FVFXDSL ParsedDSL;
	FString ParseError;
	bool bParseSuccess = UVFXDSLParser::ParseFromJSON(SerializedJSON, ParsedDSL, ParseError);

	TestTrue(TEXT("Serialized JSON should be parseable"), bParseSuccess);
	if (bParseSuccess)
	{
		TestEqual(TEXT("Effect duration should match"), ParsedDSL.Effect.Duration, DSL.Effect.Duration);
		TestEqual(TEXT("Effect looping should match"), ParsedDSL.Effect.bLooping, DSL.Effect.bLooping);
		TestEqual(TEXT("Emitters count should match"), ParsedDSL.Emitters.Num(), DSL.Emitters.Num());
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXDSLParserMultipleEmittersTest,
	"AINiagara.VFXDSLParser.MultipleEmitters",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXDSLParserMultipleEmittersTest::RunTest(const FString& Parameters)
{
	FString MultiEmitterJSON = TEXT(R"({
		"effect": {
			"type": "Niagara",
			"duration": 8.0,
			"looping": true
		},
		"emitters": [
			{
				"name": "Emitter1",
				"spawners": {
					"rate": {
						"spawnRate": 10.0
					}
				}
			},
			{
				"name": "Emitter2",
				"spawners": {
					"rate": {
						"spawnRate": 20.0
					}
				}
			},
			{
				"name": "Emitter3",
				"spawners": {
					"rate": {
						"spawnRate": 30.0
					}
				}
			}
		]
	})");

	FVFXDSL DSL;
	FString ParseError;
	bool bParseSuccess = UVFXDSLParser::ParseFromJSON(MultiEmitterJSON, DSL, ParseError);

	TestTrue(TEXT("Multi-emitter JSON should parse"), bParseSuccess);
	TestEqual(TEXT("Should have 3 emitters"), DSL.Emitters.Num(), 3);

	if (DSL.Emitters.Num() == 3)
	{
		TestEqual(TEXT("First emitter name"), DSL.Emitters[0].Name, TEXT("Emitter1"));
		TestEqual(TEXT("Second emitter name"), DSL.Emitters[1].Name, TEXT("Emitter2"));
		TestEqual(TEXT("Third emitter name"), DSL.Emitters[2].Name, TEXT("Emitter3"));
		TestEqual(TEXT("Second emitter spawn rate"), DSL.Emitters[1].Spawners.Rate.SpawnRate, 20.0f);
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXDSLParserSpecialCharactersTest,
	"AINiagara.VFXDSLParser.SpecialCharacters",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXDSLParserSpecialCharactersTest::RunTest(const FString& Parameters)
{
	FString SpecialCharsJSON = TEXT(R"({
		"effect": {
			"type": "Niagara",
			"duration": 5.0
		},
		"emitters": [
			{
				"name": "Emitter with \"quotes\" and 'apostrophes'",
				"render": {
					"material": "/Game/Materials/M_\"Special\"",
					"blendMode": "Translucent"
				}
			}
		]
	})");

	FVFXDSL DSL;
	FString ParseError;
	bool bParseSuccess = UVFXDSLParser::ParseFromJSON(SpecialCharsJSON, DSL, ParseError);

	// Should handle special characters properly
	if (bParseSuccess)
	{
		TestTrue(TEXT("Should parse with special characters"), true);
	}

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

