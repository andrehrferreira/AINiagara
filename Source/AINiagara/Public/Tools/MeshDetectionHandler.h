// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/StaticMesh.h"
#include "MeshDetectionHandler.generated.h"

/**
 * Mesh detection result
 */
USTRUCT()
struct FMeshDetectionResult
{
	GENERATED_BODY()

	/** Whether mesh is required */
	UPROPERTY()
	bool bMeshRequired = false;

	/** Detected mesh type (Debris, Foliage, Projectile, etc.) */
	UPROPERTY()
	FString MeshType;

	/** Detected keywords that indicate mesh requirement */
	UPROPERTY()
	TArray<FString> DetectedKeywords;

	/** Recommended simple mesh (Billboard, Cone, Sphere) */
	UPROPERTY()
	FString RecommendedSimpleMesh;
};

/**
 * Simple mesh types available by default
 */
UENUM(BlueprintType)
enum class ESimpleMeshType : uint8
{
	Billboard	UMETA(DisplayName = "Billboard"),
	Cone		UMETA(DisplayName = "Cone"),
	Sphere		UMETA(DisplayName = "Sphere"),
	Cube		UMETA(DisplayName = "Cube"),
	Cylinder	UMETA(DisplayName = "Cylinder")
};

/**
 * Handler for detecting 3D model requirements in user requests
 * Processes natural language to determine if mesh-based particles are needed
 */
UCLASS()
class AINIAGARA_API UMeshDetectionHandler : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Detect if user request requires 3D models/meshes
	 * @param UserRequest Natural language request
	 * @param OutResult Detection result with mesh requirements
	 * @return True if mesh was detected as required
	 */
	static bool DetectMeshRequirement(const FString& UserRequest, FMeshDetectionResult& OutResult);

	/**
	 * Get default simple mesh asset path
	 * @param MeshType Simple mesh type
	 * @return Asset path to default mesh
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Mesh")
	static FString GetSimpleMeshPath(ESimpleMeshType MeshType);

	/**
	 * Load or create simple mesh
	 * @param MeshType Simple mesh type
	 * @param OutMesh Loaded mesh asset
	 * @param OutError Error message if failed
	 * @return True if mesh was loaded/created successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Mesh")
	static bool LoadSimpleMesh(ESimpleMeshType MeshType, UStaticMesh*& OutMesh, FString& OutError);

	/**
	 * Check if a mesh path exists
	 * @param MeshPath Asset path to check
	 * @return True if mesh exists at path
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|Mesh")
	static bool MeshPathExists(const FString& MeshPath);

private:
	/**
	 * Get mesh-related keywords and their types
	 * @return Map of keywords to mesh types
	 */
	static TMap<FString, FString> GetMeshKeywords();

	/**
	 * Get recommended simple mesh for a mesh type
	 * @param MeshType Detected mesh type
	 * @return Recommended simple mesh type
	 */
	static FString GetRecommendedSimpleMesh(const FString& MeshType);
};

