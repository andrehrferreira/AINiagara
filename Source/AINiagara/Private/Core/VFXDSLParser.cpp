// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/VFXDSLParser.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Misc/EnumRange.h"

/**
 * Parses a VFX DSL specification from a JSON string.
 * 
 * This function validates and parses the JSON structure into a FVFXDSL structure.
 * The expected JSON format is:
 * {
 *   "effect": { "type": "Niagara|Cascade", "duration": number, "looping": boolean },
 *   "emitters": [
 *     { "name": "...", "spawners": {...}, "initialization": {...}, "update": {...}, "render": {...} },
 *     ...
 *   ]
 * }
 * 
 * @param JsonString The JSON string to parse (typically from LLM response)
 * @param OutDSL Output parameter - the parsed DSL structure
 * @param OutError Output parameter - error message if parsing fails
 * @return true if parsing was successful, false otherwise
 * 
 * @note Requires at least one emitter in the emitters array
 * @note Validates JSON structure and required fields before parsing
 * @note On failure, OutError contains a descriptive error message
 */
bool UVFXDSLParser::ParseFromJSON(const FString& JsonString, FVFXDSL& OutDSL, FString& OutError)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		OutError = TEXT("Failed to parse JSON string");
		return false;
	}
	
	// Parse effect
	const TSharedPtr<FJsonObject>* EffectObject;
	if (!JsonObject->TryGetObjectField(TEXT("effect"), EffectObject) || !EffectObject->IsValid())
	{
		OutError = TEXT("Missing or invalid 'effect' field");
		return false;
	}
	
	if (!ParseEffect(*EffectObject, OutDSL.Effect))
	{
		OutError = TEXT("Failed to parse effect");
		return false;
	}
	
	// Parse emitters
	const TArray<TSharedPtr<FJsonValue>>* EmittersArray;
	if (!JsonObject->TryGetArrayField(TEXT("emitters"), EmittersArray))
	{
		OutError = TEXT("Missing 'emitters' array");
		return false;
	}
	
	if (EmittersArray->Num() == 0)
	{
		OutError = TEXT("Emitters array is empty - at least one emitter is required");
		return false;
	}
	
	OutDSL.Emitters.Empty();
	for (const TSharedPtr<FJsonValue>& EmitterValue : *EmittersArray)
	{
		FVFXDSLEmitter Emitter;
		if (!ParseEmitter(EmitterValue->AsObject(), Emitter))
		{
			OutError = FString::Printf(TEXT("Failed to parse emitter at index %d"), OutDSL.Emitters.Num());
			return false;
		}
		OutDSL.Emitters.Add(Emitter);
	}
	
	return true;
}

bool UVFXDSLParser::ToJSON(const FVFXDSL& DSL, FString& OutJsonString)
{
	TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);
	
	// Serialize effect
	TSharedPtr<FJsonObject> EffectObject = MakeShareable(new FJsonObject);
	EffectObject->SetStringField(TEXT("type"), DSL.Effect.Type == EVFXEffectType::Niagara ? TEXT("Niagara") : TEXT("Cascade"));
	EffectObject->SetNumberField(TEXT("duration"), DSL.Effect.Duration);
	EffectObject->SetBoolField(TEXT("looping"), DSL.Effect.bLooping);
	RootObject->SetObjectField(TEXT("effect"), EffectObject);
	
	// Serialize emitters
	TArray<TSharedPtr<FJsonValue>> EmittersArray;
	for (const FVFXDSLEmitter& Emitter : DSL.Emitters)
	{
		TSharedPtr<FJsonObject> EmitterObject = MakeShareable(new FJsonObject);
		EmitterObject->SetStringField(TEXT("name"), Emitter.Name);
		
		// Serialize spawners
		TSharedPtr<FJsonObject> SpawnersObject = MakeShareable(new FJsonObject);
		
		TSharedPtr<FJsonObject> BurstObject = MakeShareable(new FJsonObject);
		BurstObject->SetNumberField(TEXT("count"), Emitter.Spawners.Burst.Count);
		BurstObject->SetNumberField(TEXT("time"), Emitter.Spawners.Burst.Time);
		
		TArray<TSharedPtr<FJsonValue>> IntervalsArray;
		for (float Interval : Emitter.Spawners.Burst.Intervals)
		{
			IntervalsArray.Add(MakeShareable(new FJsonValueNumber(Interval)));
		}
		BurstObject->SetArrayField(TEXT("intervals"), IntervalsArray);
		SpawnersObject->SetObjectField(TEXT("burst"), BurstObject);
		
		TSharedPtr<FJsonObject> RateObject = MakeShareable(new FJsonObject);
		RateObject->SetNumberField(TEXT("spawnRate"), Emitter.Spawners.Rate.SpawnRate);
		RateObject->SetNumberField(TEXT("scaleOverTime"), Emitter.Spawners.Rate.ScaleOverTime);
		SpawnersObject->SetObjectField(TEXT("rate"), RateObject);
		
		EmitterObject->SetObjectField(TEXT("spawners"), SpawnersObject);
		
		// Serialize initialization
		TSharedPtr<FJsonObject> InitObject = MakeShareable(new FJsonObject);
		
		TSharedPtr<FJsonObject> ColorObject = MakeShareable(new FJsonObject);
		ColorObject->SetNumberField(TEXT("r"), Emitter.Initialization.Color.R);
		ColorObject->SetNumberField(TEXT("g"), Emitter.Initialization.Color.G);
		ColorObject->SetNumberField(TEXT("b"), Emitter.Initialization.Color.B);
		ColorObject->SetNumberField(TEXT("a"), Emitter.Initialization.Color.A);
		InitObject->SetObjectField(TEXT("color"), ColorObject);
		
		TSharedPtr<FJsonObject> SizeObject = MakeShareable(new FJsonObject);
		SizeObject->SetNumberField(TEXT("min"), Emitter.Initialization.Size.Min);
		SizeObject->SetNumberField(TEXT("max"), Emitter.Initialization.Size.Max);
		InitObject->SetObjectField(TEXT("size"), SizeObject);
		
		TSharedPtr<FJsonObject> VelocityObject = MakeShareable(new FJsonObject);
		VelocityObject->SetNumberField(TEXT("x"), Emitter.Initialization.Velocity.X);
		VelocityObject->SetNumberField(TEXT("y"), Emitter.Initialization.Velocity.Y);
		VelocityObject->SetNumberField(TEXT("z"), Emitter.Initialization.Velocity.Z);
		InitObject->SetObjectField(TEXT("velocity"), VelocityObject);
		
		EmitterObject->SetObjectField(TEXT("initialization"), InitObject);
		
		// Serialize update
		TSharedPtr<FJsonObject> UpdateObject = MakeShareable(new FJsonObject);
		
		TSharedPtr<FJsonObject> ForcesObject = MakeShareable(new FJsonObject);
		ForcesObject->SetNumberField(TEXT("gravity"), Emitter.Update.Forces.Gravity);
		
		TSharedPtr<FJsonObject> WindObject = MakeShareable(new FJsonObject);
		WindObject->SetNumberField(TEXT("x"), Emitter.Update.Forces.Wind.X);
		WindObject->SetNumberField(TEXT("y"), Emitter.Update.Forces.Wind.Y);
		WindObject->SetNumberField(TEXT("z"), Emitter.Update.Forces.Wind.Z);
		ForcesObject->SetObjectField(TEXT("wind"), WindObject);
		
		UpdateObject->SetObjectField(TEXT("forces"), ForcesObject);
		UpdateObject->SetNumberField(TEXT("drag"), Emitter.Update.Drag);
		
		TSharedPtr<FJsonObject> CollisionObject = MakeShareable(new FJsonObject);
		CollisionObject->SetBoolField(TEXT("enabled"), Emitter.Update.Collision.bEnabled);
		CollisionObject->SetNumberField(TEXT("bounce"), Emitter.Update.Collision.Bounce);
		UpdateObject->SetObjectField(TEXT("collision"), CollisionObject);
		
		EmitterObject->SetObjectField(TEXT("update"), UpdateObject);
		
		// Serialize render
		TSharedPtr<FJsonObject> RenderObject = MakeShareable(new FJsonObject);
		RenderObject->SetStringField(TEXT("material"), Emitter.Render.Material);
		RenderObject->SetStringField(TEXT("texture"), Emitter.Render.Texture);
		RenderObject->SetStringField(TEXT("blendMode"), Emitter.Render.BlendMode);
		RenderObject->SetStringField(TEXT("sort"), Emitter.Render.Sort);
		
		EmitterObject->SetObjectField(TEXT("render"), RenderObject);
		
		EmittersArray.Add(MakeShareable(new FJsonValueObject(EmitterObject)));
	}
	
	RootObject->SetArrayField(TEXT("emitters"), EmittersArray);
	
	// Convert to JSON string
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJsonString);
	FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);
	
	return true;
}

bool UVFXDSLParser::ParseEffect(const TSharedPtr<FJsonObject>& JsonObject, FVFXDSLEffect& OutEffect)
{
	if (!JsonObject.IsValid())
	{
		return false;
	}
	
	// Parse type
	FString TypeString;
	if (JsonObject->TryGetStringField(TEXT("type"), TypeString))
	{
		OutEffect.Type = ParseEffectType(TypeString);
	}
	
	// Parse duration
	double DurationValue;
	if (JsonObject->TryGetNumberField(TEXT("duration"), DurationValue))
	{
		OutEffect.Duration = static_cast<float>(DurationValue);
	}
	
	// Parse looping
	bool bLoopingValue;
	if (JsonObject->TryGetBoolField(TEXT("looping"), bLoopingValue))
	{
		OutEffect.bLooping = bLoopingValue;
	}
	
	return true;
}

bool UVFXDSLParser::ParseEmitter(const TSharedPtr<FJsonObject>& JsonObject, FVFXDSLEmitter& OutEmitter)
{
	if (!JsonObject.IsValid())
	{
		return false;
	}
	
	// Parse name
	JsonObject->TryGetStringField(TEXT("name"), OutEmitter.Name);
	
	// Parse spawners
	const TSharedPtr<FJsonObject>* SpawnersObject;
	if (JsonObject->TryGetObjectField(TEXT("spawners"), SpawnersObject))
	{
		ParseSpawners(*SpawnersObject, OutEmitter.Spawners);
	}
	
	// Parse initialization
	const TSharedPtr<FJsonObject>* InitObject;
	if (JsonObject->TryGetObjectField(TEXT("initialization"), InitObject))
	{
		ParseInitialization(*InitObject, OutEmitter.Initialization);
	}
	
	// Parse update
	const TSharedPtr<FJsonObject>* UpdateObject;
	if (JsonObject->TryGetObjectField(TEXT("update"), UpdateObject))
	{
		ParseUpdate(*UpdateObject, OutEmitter.Update);
	}
	
	// Parse render
	const TSharedPtr<FJsonObject>* RenderObject;
	if (JsonObject->TryGetObjectField(TEXT("render"), RenderObject))
	{
		ParseRender(*RenderObject, OutEmitter.Render);
	}
	
	return true;
}

bool UVFXDSLParser::ParseSpawners(const TSharedPtr<FJsonObject>& JsonObject, FVFXDSLSpawners& OutSpawners)
{
	if (!JsonObject.IsValid())
	{
		return false;
	}
	
	// Parse burst
	const TSharedPtr<FJsonObject>* BurstObject;
	if (JsonObject->TryGetObjectField(TEXT("burst"), BurstObject))
	{
		double CountValue, TimeValue;
		if ((*BurstObject)->TryGetNumberField(TEXT("count"), CountValue))
		{
			OutSpawners.Burst.Count = static_cast<int32>(CountValue);
		}
		if ((*BurstObject)->TryGetNumberField(TEXT("time"), TimeValue))
		{
			OutSpawners.Burst.Time = static_cast<float>(TimeValue);
		}
		
		const TArray<TSharedPtr<FJsonValue>>* IntervalsArray;
		if ((*BurstObject)->TryGetArrayField(TEXT("intervals"), IntervalsArray))
		{
			OutSpawners.Burst.Intervals.Empty();
			for (const TSharedPtr<FJsonValue>& Value : *IntervalsArray)
			{
				OutSpawners.Burst.Intervals.Add(static_cast<float>(Value->AsNumber()));
			}
		}
	}
	
	// Parse rate
	const TSharedPtr<FJsonObject>* RateObject;
	if (JsonObject->TryGetObjectField(TEXT("rate"), RateObject))
	{
		double SpawnRateValue, ScaleOverTimeValue;
		if ((*RateObject)->TryGetNumberField(TEXT("spawnRate"), SpawnRateValue))
		{
			OutSpawners.Rate.SpawnRate = static_cast<float>(SpawnRateValue);
		}
		if ((*RateObject)->TryGetNumberField(TEXT("scaleOverTime"), ScaleOverTimeValue))
		{
			OutSpawners.Rate.ScaleOverTime = static_cast<float>(ScaleOverTimeValue);
		}
	}
	
	return true;
}

bool UVFXDSLParser::ParseInitialization(const TSharedPtr<FJsonObject>& JsonObject, FVFXDSLInitialization& OutInit)
{
	if (!JsonObject.IsValid())
	{
		return false;
	}
	
	// Parse color
	const TSharedPtr<FJsonObject>* ColorObject;
	if (JsonObject->TryGetObjectField(TEXT("color"), ColorObject))
	{
		ParseColor(*ColorObject, OutInit.Color);
	}
	
	// Parse size
	const TSharedPtr<FJsonObject>* SizeObject;
	if (JsonObject->TryGetObjectField(TEXT("size"), SizeObject))
	{
		double MinValue, MaxValue;
		if ((*SizeObject)->TryGetNumberField(TEXT("min"), MinValue))
		{
			OutInit.Size.Min = static_cast<float>(MinValue);
		}
		if ((*SizeObject)->TryGetNumberField(TEXT("max"), MaxValue))
		{
			OutInit.Size.Max = static_cast<float>(MaxValue);
		}
	}
	
	// Parse velocity
	const TSharedPtr<FJsonObject>* VelocityObject;
	if (JsonObject->TryGetObjectField(TEXT("velocity"), VelocityObject))
	{
		ParseVelocity(*VelocityObject, OutInit.Velocity);
	}
	
	return true;
}

bool UVFXDSLParser::ParseUpdate(const TSharedPtr<FJsonObject>& JsonObject, FVFXDSLUpdate& OutUpdate)
{
	if (!JsonObject.IsValid())
	{
		return false;
	}
	
	// Parse forces
	const TSharedPtr<FJsonObject>* ForcesObject;
	if (JsonObject->TryGetObjectField(TEXT("forces"), ForcesObject))
	{
		double GravityValue;
		if ((*ForcesObject)->TryGetNumberField(TEXT("gravity"), GravityValue))
		{
			OutUpdate.Forces.Gravity = static_cast<float>(GravityValue);
		}
		
		const TSharedPtr<FJsonObject>* WindObject;
		if ((*ForcesObject)->TryGetObjectField(TEXT("wind"), WindObject))
		{
			ParseVelocity(*WindObject, OutUpdate.Forces.Wind);
		}
	}
	
	// Parse drag
	double DragValue;
	if (JsonObject->TryGetNumberField(TEXT("drag"), DragValue))
	{
		OutUpdate.Drag = static_cast<float>(DragValue);
	}
	
	// Parse collision
	const TSharedPtr<FJsonObject>* CollisionObject;
	if (JsonObject->TryGetObjectField(TEXT("collision"), CollisionObject))
	{
		bool bEnabledValue;
		if ((*CollisionObject)->TryGetBoolField(TEXT("enabled"), bEnabledValue))
		{
			OutUpdate.Collision.bEnabled = bEnabledValue;
		}
		
		double BounceValue;
		if ((*CollisionObject)->TryGetNumberField(TEXT("bounce"), BounceValue))
		{
			OutUpdate.Collision.Bounce = static_cast<float>(BounceValue);
		}
	}
	
	return true;
}

bool UVFXDSLParser::ParseRender(const TSharedPtr<FJsonObject>& JsonObject, FVFXDSLRender& OutRender)
{
	if (!JsonObject.IsValid())
	{
		return false;
	}
	
	JsonObject->TryGetStringField(TEXT("material"), OutRender.Material);
	JsonObject->TryGetStringField(TEXT("texture"), OutRender.Texture);
	JsonObject->TryGetStringField(TEXT("blendMode"), OutRender.BlendMode);
	JsonObject->TryGetStringField(TEXT("sort"), OutRender.Sort);
	
	// Parse mesh configuration
	const TSharedPtr<FJsonObject>* MeshObject;
	if (JsonObject->TryGetObjectField(TEXT("mesh"), MeshObject))
	{
		(*MeshObject)->TryGetStringField(TEXT("meshPath"), OutRender.Mesh.MeshPath);
		(*MeshObject)->TryGetStringField(TEXT("meshType"), OutRender.Mesh.MeshType);
		
		double ScaleValue;
		if ((*MeshObject)->TryGetNumberField(TEXT("scale"), ScaleValue))
		{
			OutRender.Mesh.Scale = static_cast<float>(ScaleValue);
		}
		
		const TSharedPtr<FJsonObject>* RotationObject;
		if ((*MeshObject)->TryGetObjectField(TEXT("rotation"), RotationObject))
		{
			ParseVelocity(*RotationObject, OutRender.Mesh.Rotation);
		}
		
		bool bUseMeshValue;
		if ((*MeshObject)->TryGetBoolField(TEXT("useMesh"), bUseMeshValue))
		{
			OutRender.Mesh.bUseMesh = bUseMeshValue;
		}
	}
	
	return true;
}

bool UVFXDSLParser::ParseColor(const TSharedPtr<FJsonObject>& JsonObject, FVFXDSLColor& OutColor)
{
	if (!JsonObject.IsValid())
	{
		return false;
	}
	
	double RValue, GValue, BValue, AValue;
	if (JsonObject->TryGetNumberField(TEXT("r"), RValue))
	{
		OutColor.R = static_cast<float>(RValue);
	}
	if (JsonObject->TryGetNumberField(TEXT("g"), GValue))
	{
		OutColor.G = static_cast<float>(GValue);
	}
	if (JsonObject->TryGetNumberField(TEXT("b"), BValue))
	{
		OutColor.B = static_cast<float>(BValue);
	}
	if (JsonObject->TryGetNumberField(TEXT("a"), AValue))
	{
		OutColor.A = static_cast<float>(AValue);
	}
	
	return true;
}

bool UVFXDSLParser::ParseVelocity(const TSharedPtr<FJsonObject>& JsonObject, FVFXDSLVelocity& OutVelocity)
{
	if (!JsonObject.IsValid())
	{
		return false;
	}
	
	double XValue, YValue, ZValue;
	if (JsonObject->TryGetNumberField(TEXT("x"), XValue))
	{
		OutVelocity.X = static_cast<float>(XValue);
	}
	if (JsonObject->TryGetNumberField(TEXT("y"), YValue))
	{
		OutVelocity.Y = static_cast<float>(YValue);
	}
	if (JsonObject->TryGetNumberField(TEXT("z"), ZValue))
	{
		OutVelocity.Z = static_cast<float>(ZValue);
	}
	
	return true;
}

EVFXEffectType UVFXDSLParser::ParseEffectType(const FString& TypeString)
{
	if (TypeString.Equals(TEXT("Niagara"), ESearchCase::IgnoreCase))
	{
		return EVFXEffectType::Niagara;
	}
	else if (TypeString.Equals(TEXT("Cascade"), ESearchCase::IgnoreCase))
	{
		return EVFXEffectType::Cascade;
	}
	
	// Default to Niagara
	return EVFXEffectType::Niagara;
}

FVFXDSLValidationResult UVFXDSLValidator::Validate(const FVFXDSL& DSL)
{
	FVFXDSLValidationResult Result;
	Result.bIsValid = true;
	
	// Validate effect
	ValidateEffect(DSL.Effect, Result);
	
	// Validate emitters
	if (DSL.Emitters.Num() == 0)
	{
		Result.AddError(TEXT("DSL must contain at least one emitter"));
	}
	
	for (int32 i = 0; i < DSL.Emitters.Num(); ++i)
	{
		ValidateEmitter(DSL.Emitters[i], i, Result);
	}
	
	return Result;
}

void UVFXDSLValidator::ValidateEffect(const FVFXDSLEffect& Effect, FVFXDSLValidationResult& Result)
{
	if (Effect.Duration < 0.0f)
	{
		Result.AddError(FString::Printf(TEXT("Effect duration must be non-negative, got: %f"), Effect.Duration));
	}
}

void UVFXDSLValidator::ValidateEmitter(const FVFXDSLEmitter& Emitter, int32 EmitterIndex, FVFXDSLValidationResult& Result)
{
	FString EmitterContext = FString::Printf(TEXT("Emitter[%d]"), EmitterIndex);
	
	// Validate emitter name
	if (Emitter.Name.IsEmpty())
	{
		Result.AddError(FString::Printf(TEXT("%s: Name is empty"), *EmitterContext));
	}
	
	// Validate initialization
	ValidateColor(Emitter.Initialization.Color, FString::Printf(TEXT("%s.Initialization.Color"), *EmitterContext), Result);
	ValidateSize(Emitter.Initialization.Size, FString::Printf(TEXT("%s.Initialization.Size"), *EmitterContext), Result);
	
	// Validate spawners
	if (Emitter.Spawners.Burst.Count < 0)
	{
		Result.AddError(FString::Printf(TEXT("%s.Spawners.Burst: Count must be non-negative, got: %d"), *EmitterContext, Emitter.Spawners.Burst.Count));
	}
	
	if (Emitter.Spawners.Rate.SpawnRate < 0.0f)
	{
		Result.AddError(FString::Printf(TEXT("%s.Spawners.Rate: SpawnRate must be non-negative, got: %f"), *EmitterContext, Emitter.Spawners.Rate.SpawnRate));
	}
	
	// Validate collision bounce
	if (Emitter.Update.Collision.bEnabled)
	{
		if (Emitter.Update.Collision.Bounce < 0.0f || Emitter.Update.Collision.Bounce > 1.0f)
		{
			Result.AddError(FString::Printf(TEXT("%s.Update.Collision: Bounce must be between 0 and 1, got: %f"), *EmitterContext, Emitter.Update.Collision.Bounce));
		}
	}
}

void UVFXDSLValidator::ValidateColor(const FVFXDSLColor& Color, const FString& Context, FVFXDSLValidationResult& Result)
{
	if (Color.R < 0.0f || Color.R > 1.0f)
	{
		Result.AddError(FString::Printf(TEXT("%s.R: Color component must be between 0 and 1, got: %f"), *Context, Color.R));
	}
	if (Color.G < 0.0f || Color.G > 1.0f)
	{
		Result.AddError(FString::Printf(TEXT("%s.G: Color component must be between 0 and 1, got: %f"), *Context, Color.G));
	}
	if (Color.B < 0.0f || Color.B > 1.0f)
	{
		Result.AddError(FString::Printf(TEXT("%s.B: Color component must be between 0 and 1, got: %f"), *Context, Color.B));
	}
	if (Color.A < 0.0f || Color.A > 1.0f)
	{
		Result.AddError(FString::Printf(TEXT("%s.A: Color component must be between 0 and 1, got: %f"), *Context, Color.A));
	}
}

void UVFXDSLValidator::ValidateSize(const FVFXDSLSize& Size, const FString& Context, FVFXDSLValidationResult& Result)
{
	if (Size.Min < 0.0f)
	{
		Result.AddError(FString::Printf(TEXT("%s.Min: Size must be non-negative, got: %f"), *Context, Size.Min));
	}
	if (Size.Max < 0.0f)
	{
		Result.AddError(FString::Printf(TEXT("%s.Max: Size must be non-negative, got: %f"), *Context, Size.Max));
	}
	if (Size.Min > Size.Max)
	{
		Result.AddError(FString::Printf(TEXT("%s: Min (%f) must be less than or equal to Max (%f)"), *Context, Size.Min, Size.Max));
	}
}

