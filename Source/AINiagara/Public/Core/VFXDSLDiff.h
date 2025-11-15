// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/VFXDSL.h"
#include "VFXDSLDiff.generated.h"

/**
 * Type of change detected in DSL comparison
 */
UENUM(BlueprintType)
enum class EVFXDSLChangeType : uint8
{
	None			UMETA(DisplayName = "No Change"),
	Added			UMETA(DisplayName = "Added"),
	Removed			UMETA(DisplayName = "Removed"),
	Modified		UMETA(DisplayName = "Modified"),
	TypeChanged		UMETA(DisplayName = "Type Changed")
};

/**
 * Represents a single change in DSL comparison
 */
USTRUCT(BlueprintType)
struct FVFXDSLChange
{
	GENERATED_BODY()

	/** Path to the changed property (e.g., "Effect.Duration", "Emitters[0].Name") */
	UPROPERTY(BlueprintReadOnly, Category = "VFX")
	FString PropertyPath;

	/** Type of change */
	UPROPERTY(BlueprintReadOnly, Category = "VFX")
	EVFXDSLChangeType ChangeType = EVFXDSLChangeType::None;

	/** Old value (as string) */
	UPROPERTY(BlueprintReadOnly, Category = "VFX")
	FString OldValue;

	/** New value (as string) */
	UPROPERTY(BlueprintReadOnly, Category = "VFX")
	FString NewValue;

	/** Human-readable description of the change */
	UPROPERTY(BlueprintReadOnly, Category = "VFX")
	FString Description;
};

/**
 * Result of DSL comparison
 */
USTRUCT(BlueprintType)
struct FVFXDSLDiffResult
{
	GENERATED_BODY()

	/** List of all changes detected */
	UPROPERTY(BlueprintReadOnly, Category = "VFX")
	TArray<FVFXDSLChange> Changes;

	/** Whether any changes were detected */
	UPROPERTY(BlueprintReadOnly, Category = "VFX")
	bool bHasChanges = false;

	/** Summary message */
	UPROPERTY(BlueprintReadOnly, Category = "VFX")
	FString Summary;

	/** Add a change */
	void AddChange(const FString& PropertyPath, EVFXDSLChangeType ChangeType, const FString& OldValue, const FString& NewValue, const FString& Description)
	{
		FVFXDSLChange Change;
		Change.PropertyPath = PropertyPath;
		Change.ChangeType = ChangeType;
		Change.OldValue = OldValue;
		Change.NewValue = NewValue;
		Change.Description = Description;
		Changes.Add(Change);
		bHasChanges = true;
	}

	/** Generate summary message */
	void GenerateSummary()
	{
		if (!bHasChanges)
		{
			Summary = TEXT("No changes detected");
			return;
		}

		int32 AddedCount = 0;
		int32 RemovedCount = 0;
		int32 ModifiedCount = 0;
		int32 TypeChangedCount = 0;

		for (const FVFXDSLChange& Change : Changes)
		{
			switch (Change.ChangeType)
			{
			case EVFXDSLChangeType::Added:
				AddedCount++;
				break;
			case EVFXDSLChangeType::Removed:
				RemovedCount++;
				break;
			case EVFXDSLChangeType::Modified:
				ModifiedCount++;
				break;
			case EVFXDSLChangeType::TypeChanged:
				TypeChangedCount++;
				break;
			}
		}

		Summary = FString::Printf(
			TEXT("Detected %d change(s): %d added, %d removed, %d modified, %d type changes"),
			Changes.Num(),
			AddedCount,
			RemovedCount,
			ModifiedCount,
			TypeChangedCount
		);
	}
};

/**
 * Utility class for comparing DSL versions
 */
UCLASS(BlueprintType)
class AINIAGARA_API UVFXDSLDiff : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Compare two DSL specifications and return differences
	 * @param OldDSL Previous DSL version
	 * @param NewDSL New DSL version
	 * @return Diff result with all detected changes
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|DSL")
	static FVFXDSLDiffResult Compare(const FVFXDSL& OldDSL, const FVFXDSL& NewDSL);

private:
	/** Compare effect configurations */
	static void CompareEffect(const FVFXDSLEffect& OldEffect, const FVFXDSLEffect& NewEffect, FVFXDSLDiffResult& OutResult);

	/** Compare emitters */
	static void CompareEmitters(const TArray<FVFXDSLEmitter>& OldEmitters, const TArray<FVFXDSLEmitter>& NewEmitters, FVFXDSLDiffResult& OutResult);

	/** Compare a single emitter */
	static void CompareEmitter(const FVFXDSLEmitter& OldEmitter, const FVFXDSLEmitter& NewEmitter, int32 Index, FVFXDSLDiffResult& OutResult);

	/** Compare spawners */
	static void CompareSpawners(const FVFXDSLSpawners& OldSpawners, const FVFXDSLSpawners& NewSpawners, const FString& BasePath, FVFXDSLDiffResult& OutResult);

	/** Compare initialization */
	static void CompareInitialization(const FVFXDSLInitialization& OldInit, const FVFXDSLInitialization& NewInit, const FString& BasePath, FVFXDSLDiffResult& OutResult);

	/** Compare update */
	static void CompareUpdate(const FVFXDSLUpdate& OldUpdate, const FVFXDSLUpdate& NewUpdate, const FString& BasePath, FVFXDSLDiffResult& OutResult);

	/** Compare render */
	static void CompareRender(const FVFXDSLRender& OldRender, const FVFXDSLRender& NewRender, const FString& BasePath, FVFXDSLDiffResult& OutResult);

	/** Helper to compare float values */
	static void CompareFloat(const FString& Path, float OldValue, float NewValue, float Tolerance, FVFXDSLDiffResult& OutResult);

	/** Helper to compare string values */
	static void CompareString(const FString& Path, const FString& OldValue, const FString& NewValue, FVFXDSLDiffResult& OutResult);

	/** Helper to compare color values */
	static void CompareColor(const FString& Path, const FVFXDSLColor& OldColor, const FVFXDSLColor& NewColor, FVFXDSLDiffResult& OutResult);

	/** Helper to compare velocity values */
	static void CompareVelocity(const FString& Path, const FVFXDSLVelocity& OldVel, const FVFXDSLVelocity& NewVel, FVFXDSLDiffResult& OutResult);
};

