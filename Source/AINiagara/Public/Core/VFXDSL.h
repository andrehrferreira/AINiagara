// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/StructOnScope.h"
#include "VFXDSL.generated.h"

/**
 * DSL Effect type enumeration
 */
UENUM(BlueprintType)
enum class EVFXEffectType : uint8
{
	Niagara		UMETA(DisplayName = "Niagara"),
	Cascade		UMETA(DisplayName = "Cascade")
};

/**
 * DSL Effect structure
 */
USTRUCT(BlueprintType)
struct FVFXDSLEffect
{
	GENERATED_BODY()

	/** Effect type: Niagara or Cascade */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVFXEffectType Type = EVFXEffectType::Niagara;

	/** Effect duration in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = 5.0f;

	/** Whether the effect loops */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLooping = false;
};

/**
 * DSL Burst spawn configuration
 */
USTRUCT(BlueprintType)
struct FVFXDSLBurst
{
	GENERATED_BODY()

	/** Number of particles to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 10;

	/** Time at which to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Time = 0.0f;

	/** Burst intervals (for multiple bursts) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> Intervals;
};

/**
 * DSL Rate spawn configuration
 */
USTRUCT(BlueprintType)
struct FVFXDSLRate
{
	GENERATED_BODY()

	/** Particles per second spawn rate */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnRate = 10.0f;

	/** Scale factor over time */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScaleOverTime = 1.0f;
};

/**
 * DSL Spawners structure
 */
USTRUCT(BlueprintType)
struct FVFXDSLSpawners
{
	GENERATED_BODY()

	/** Burst spawn configuration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVFXDSLBurst Burst;

	/** Rate spawn configuration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVFXDSLRate Rate;
};

/**
 * DSL Color structure (RGBA)
 */
USTRUCT(BlueprintType)
struct FVFXDSLColor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float R = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float G = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float B = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float A = 1.0f;

	/** Convert to FLinearColor */
	FLinearColor ToLinearColor() const
	{
		return FLinearColor(R, G, B, A);
	}
};

/**
 * DSL Size structure
 */
USTRUCT(BlueprintType)
struct FVFXDSLSize
{
	GENERATED_BODY()

	/** Minimum size */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Min = 1.0f;

	/** Maximum size */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Max = 1.0f;
};

/**
 * DSL Velocity structure
 */
USTRUCT(BlueprintType)
struct FVFXDSLVelocity
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float X = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Y = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Z = 0.0f;

	/** Convert to FVector */
	FVector ToVector() const
	{
		return FVector(X, Y, Z);
	}
};

/**
 * DSL Initialization structure
 */
USTRUCT(BlueprintType)
struct FVFXDSLInitialization
{
	GENERATED_BODY()

	/** Particle color */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVFXDSLColor Color;

	/** Particle size range */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVFXDSLSize Size;

	/** Particle initial velocity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVFXDSLVelocity Velocity;
};

/**
 * DSL Forces structure
 */
USTRUCT(BlueprintType)
struct FVFXDSLForces
{
	GENERATED_BODY()

	/** Gravity force */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Gravity = 0.0f;

	/** Wind force vector */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVFXDSLVelocity Wind;
};

/**
 * DSL Collision structure
 */
USTRUCT(BlueprintType)
struct FVFXDSLCollision
{
	GENERATED_BODY()

	/** Whether collision is enabled */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnabled = false;

	/** Bounce coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Bounce = 0.5f;
};

/**
 * DSL Update structure
 */
USTRUCT(BlueprintType)
struct FVFXDSLUpdate
{
	GENERATED_BODY()

	/** Applied forces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVFXDSLForces Forces;

	/** Drag coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Drag = 0.0f;

	/** Collision settings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVFXDSLCollision Collision;
};

/**
 * DSL Render structure
 */
USTRUCT(BlueprintType)
struct FVFXDSLRender
{
	GENERATED_BODY()

	/** Material path or name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Material;

	/** Texture path or name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Texture;

	/** Blend mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString BlendMode = TEXT("Translucent");

	/** Sort mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Sort = TEXT("ViewDepth");
};

/**
 * DSL Emitter structure
 */
USTRUCT(BlueprintType)
struct FVFXDSLEmitter
{
	GENERATED_BODY()

	/** Emitter name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name = TEXT("Emitter");

	/** Spawner configuration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVFXDSLSpawners Spawners;

	/** Initialization parameters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVFXDSLInitialization Initialization;

	/** Update parameters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVFXDSLUpdate Update;

	/** Render parameters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVFXDSLRender Render;
};

/**
 * Complete DSL structure
 */
USTRUCT(BlueprintType)
struct FVFXDSL
{
	GENERATED_BODY()

	/** Effect configuration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVFXDSLEffect Effect;

	/** List of emitters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVFXDSLEmitter> Emitters;
};

/**
 * DSL validation result
 */
USTRUCT(BlueprintType)
struct FVFXDSLValidationResult
{
	GENERATED_BODY()

	/** Whether validation passed */
	UPROPERTY(BlueprintReadOnly)
	bool bIsValid = false;

	/** Error messages if validation failed */
	UPROPERTY(BlueprintReadOnly)
	TArray<FString> ErrorMessages;

	/** Add an error message */
	void AddError(const FString& ErrorMessage)
	{
		bIsValid = false;
		ErrorMessages.Add(ErrorMessage);
	}

	/** Clear all errors */
	void Clear()
	{
		bIsValid = true;
		ErrorMessages.Empty();
	}
};

