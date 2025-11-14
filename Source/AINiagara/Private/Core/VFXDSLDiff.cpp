// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/VFXDSLDiff.h"
#include "Misc/DefaultValueHelper.h"

FVFXDSLDiffResult UVFXDSLDiff::Compare(const FVFXDSL& OldDSL, const FVFXDSL& NewDSL)
{
	FVFXDSLDiffResult Result;

	// Compare effect type first
	if (OldDSL.Effect.Type != NewDSL.Effect.Type)
	{
		FString OldType = OldDSL.Effect.Type == EVFXEffectType::Niagara ? TEXT("Niagara") : TEXT("Cascade");
		FString NewType = NewDSL.Effect.Type == EVFXEffectType::Niagara ? TEXT("Niagara") : TEXT("Cascade");
		Result.AddChange(
			TEXT("Effect.Type"),
			EVFXDSLChangeType::TypeChanged,
			OldType,
			NewType,
			FString::Printf(TEXT("Effect type changed from %s to %s"), *OldType, *NewType)
		);
	}

	// Compare effect configuration
	CompareEffect(OldDSL.Effect, NewDSL.Effect, Result);

	// Compare emitters
	CompareEmitters(OldDSL.Emitters, NewDSL.Emitters, Result);

	// Generate summary
	Result.GenerateSummary();

	return Result;
}

void UVFXDSLDiff::CompareEffect(const FVFXDSLEffect& OldEffect, const FVFXDSLEffect& NewEffect, FVFXDSLDiffResult& OutResult)
{
	CompareFloat(TEXT("Effect.Duration"), OldEffect.Duration, NewEffect.Duration, 0.001f, OutResult);
	
	if (OldEffect.bLooping != NewEffect.bLooping)
	{
		OutResult.AddChange(
			TEXT("Effect.Looping"),
			EVFXDSLChangeType::Modified,
			OldEffect.bLooping ? TEXT("true") : TEXT("false"),
			NewEffect.bLooping ? TEXT("true") : TEXT("false"),
			FString::Printf(TEXT("Looping changed from %s to %s"), 
				OldEffect.bLooping ? TEXT("enabled") : TEXT("disabled"),
				NewEffect.bLooping ? TEXT("enabled") : TEXT("disabled"))
		);
	}
}

void UVFXDSLDiff::CompareEmitters(const TArray<FVFXDSLEmitter>& OldEmitters, const TArray<FVFXDSLEmitter>& NewEmitters, FVFXDSLDiffResult& OutResult)
{
	int32 MaxCount = FMath::Max(OldEmitters.Num(), NewEmitters.Num());

	// Compare common emitters
	for (int32 i = 0; i < FMath::Min(OldEmitters.Num(), NewEmitters.Num()); i++)
	{
		CompareEmitter(OldEmitters[i], NewEmitters[i], i, OutResult);
	}

	// Check for removed emitters
	for (int32 i = NewEmitters.Num(); i < OldEmitters.Num(); i++)
	{
		OutResult.AddChange(
			FString::Printf(TEXT("Emitters[%d]"), i),
			EVFXDSLChangeType::Removed,
			OldEmitters[i].Name,
			TEXT(""),
			FString::Printf(TEXT("Emitter '%s' removed"), *OldEmitters[i].Name)
		);
	}

	// Check for added emitters
	for (int32 i = OldEmitters.Num(); i < NewEmitters.Num(); i++)
	{
		OutResult.AddChange(
			FString::Printf(TEXT("Emitters[%d]"), i),
			EVFXDSLChangeType::Added,
			TEXT(""),
			NewEmitters[i].Name,
			FString::Printf(TEXT("Emitter '%s' added"), *NewEmitters[i].Name)
		);
	}
}

void UVFXDSLDiff::CompareEmitter(const FVFXDSLEmitter& OldEmitter, const FVFXDSLEmitter& NewEmitter, int32 Index, FVFXDSLDiffResult& OutResult)
{
	FString BasePath = FString::Printf(TEXT("Emitters[%d]"), Index);

	// Compare name
	CompareString(BasePath + TEXT(".Name"), OldEmitter.Name, NewEmitter.Name, OutResult);

	// Compare spawners
	CompareSpawners(OldEmitter.Spawners, NewEmitter.Spawners, BasePath + TEXT(".Spawners"), OutResult);

	// Compare initialization
	CompareInitialization(OldEmitter.Initialization, NewEmitter.Initialization, BasePath + TEXT(".Initialization"), OutResult);

	// Compare update
	CompareUpdate(OldEmitter.Update, NewEmitter.Update, BasePath + TEXT(".Update"), OutResult);

	// Compare render
	CompareRender(OldEmitter.Render, NewEmitter.Render, BasePath + TEXT(".Render"), OutResult);
}

void UVFXDSLDiff::CompareSpawners(const FVFXDSLSpawners& OldSpawners, const FVFXDSLSpawners& NewSpawners, const FString& BasePath, FVFXDSLDiffResult& OutResult)
{
	CompareFloat(BasePath + TEXT(".Rate.SpawnRate"), OldSpawners.Rate.SpawnRate, NewSpawners.Rate.SpawnRate, 0.001f, OutResult);
	CompareFloat(BasePath + TEXT(".Rate.ScaleOverTime"), OldSpawners.Rate.ScaleOverTime, NewSpawners.Rate.ScaleOverTime, 0.001f, OutResult);
	
	CompareFloat(BasePath + TEXT(".Burst.Count"), (float)OldSpawners.Burst.Count, (float)NewSpawners.Burst.Count, 0.5f, OutResult);
	CompareFloat(BasePath + TEXT(".Burst.Time"), OldSpawners.Burst.Time, NewSpawners.Burst.Time, 0.001f, OutResult);
}

void UVFXDSLDiff::CompareInitialization(const FVFXDSLInitialization& OldInit, const FVFXDSLInitialization& NewInit, const FString& BasePath, FVFXDSLDiffResult& OutResult)
{
	CompareColor(BasePath + TEXT(".Color"), OldInit.Color, NewInit.Color, OutResult);
	CompareFloat(BasePath + TEXT(".Size.Min"), OldInit.Size.Min, NewInit.Size.Min, 0.001f, OutResult);
	CompareFloat(BasePath + TEXT(".Size.Max"), OldInit.Size.Max, NewInit.Size.Max, 0.001f, OutResult);
	CompareVelocity(BasePath + TEXT(".Velocity"), OldInit.Velocity, NewInit.Velocity, OutResult);
}

void UVFXDSLDiff::CompareUpdate(const FVFXDSLUpdate& OldUpdate, const FVFXDSLUpdate& NewUpdate, const FString& BasePath, FVFXDSLDiffResult& OutResult)
{
	CompareFloat(BasePath + TEXT(".Forces.Gravity"), OldUpdate.Forces.Gravity, NewUpdate.Forces.Gravity, 0.001f, OutResult);
	CompareVelocity(BasePath + TEXT(".Forces.Wind"), OldUpdate.Forces.Wind, NewUpdate.Forces.Wind, OutResult);
	CompareFloat(BasePath + TEXT(".Drag"), OldUpdate.Drag, NewUpdate.Drag, 0.001f, OutResult);
	
	if (OldUpdate.Collision.bEnabled != NewUpdate.Collision.bEnabled)
	{
		OutResult.AddChange(
			BasePath + TEXT(".Collision.Enabled"),
			EVFXDSLChangeType::Modified,
			OldUpdate.Collision.bEnabled ? TEXT("true") : TEXT("false"),
			NewUpdate.Collision.bEnabled ? TEXT("true") : TEXT("false"),
			TEXT("Collision enabled state changed")
		);
	}
	CompareFloat(BasePath + TEXT(".Collision.Bounce"), OldUpdate.Collision.Bounce, NewUpdate.Collision.Bounce, 0.001f, OutResult);
}

void UVFXDSLDiff::CompareRender(const FVFXDSLRender& OldRender, const FVFXDSLRender& NewRender, const FString& BasePath, FVFXDSLDiffResult& OutResult)
{
	CompareString(BasePath + TEXT(".Material"), OldRender.Material, NewRender.Material, OutResult);
	CompareString(BasePath + TEXT(".Texture"), OldRender.Texture, NewRender.Texture, OutResult);
	CompareString(BasePath + TEXT(".BlendMode"), OldRender.BlendMode, NewRender.BlendMode, OutResult);
	CompareString(BasePath + TEXT(".Sort"), OldRender.Sort, NewRender.Sort, OutResult);
}

void UVFXDSLDiff::CompareFloat(const FString& Path, float OldValue, float NewValue, float Tolerance, FVFXDSLDiffResult& OutResult)
{
	if (FMath::Abs(OldValue - NewValue) > Tolerance)
	{
		OutResult.AddChange(
			Path,
			EVFXDSLChangeType::Modified,
			FString::SanitizeFloat(OldValue),
			FString::SanitizeFloat(NewValue),
			FString::Printf(TEXT("%s changed from %s to %s"), *Path, *FString::SanitizeFloat(OldValue), *FString::SanitizeFloat(NewValue))
		);
	}
}

void UVFXDSLDiff::CompareString(const FString& Path, const FString& OldValue, const FString& NewValue, FVFXDSLDiffResult& OutResult)
{
	if (OldValue != NewValue)
	{
		OutResult.AddChange(
			Path,
			EVFXDSLChangeType::Modified,
			OldValue,
			NewValue,
			FString::Printf(TEXT("%s changed from '%s' to '%s'"), *Path, *OldValue, *NewValue)
		);
	}
}

void UVFXDSLDiff::CompareColor(const FString& Path, const FVFXDSLColor& OldColor, const FVFXDSLColor& NewColor, FVFXDSLDiffResult& OutResult)
{
	CompareFloat(Path + TEXT(".R"), OldColor.R, NewColor.R, 0.01f, OutResult);
	CompareFloat(Path + TEXT(".G"), OldColor.G, NewColor.G, 0.01f, OutResult);
	CompareFloat(Path + TEXT(".B"), OldColor.B, NewColor.B, 0.01f, OutResult);
	CompareFloat(Path + TEXT(".A"), OldColor.A, NewColor.A, 0.01f, OutResult);
}

void UVFXDSLDiff::CompareVelocity(const FString& Path, const FVFXDSLVelocity& OldVel, const FVFXDSLVelocity& NewVel, FVFXDSLDiffResult& OutResult)
{
	CompareFloat(Path + TEXT(".X"), OldVel.X, NewVel.X, 0.001f, OutResult);
	CompareFloat(Path + TEXT(".Y"), OldVel.Y, NewVel.Y, 0.001f, OutResult);
	CompareFloat(Path + TEXT(".Z"), OldVel.Z, NewVel.Z, 0.001f, OutResult);
}

