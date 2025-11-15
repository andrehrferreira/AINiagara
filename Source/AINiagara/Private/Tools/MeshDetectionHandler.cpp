// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tools/MeshDetectionHandler.h"
#include "Engine/StaticMesh.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/BasicShapeMesh.h"
#include "UObject/ConstructorHelpers.h"

bool UMeshDetectionHandler::DetectMeshRequirement(const FString& UserRequest, FMeshDetectionResult& OutResult)
{
	OutResult.bMeshRequired = false;
	OutResult.DetectedKeywords.Empty();

	FString LowerRequest = UserRequest.ToLower();

	// Get mesh keywords
	TMap<FString, FString> MeshKeywords = GetMeshKeywords();

	// Check for mesh-related keywords
	for (const auto& KeywordPair : MeshKeywords)
	{
		if (LowerRequest.Contains(KeywordPair.Key, ESearchCase::IgnoreCase))
		{
			OutResult.bMeshRequired = true;
			OutResult.DetectedKeywords.Add(KeywordPair.Key);
			OutResult.MeshType = KeywordPair.Value;
		}
	}

	if (OutResult.bMeshRequired)
	{
		OutResult.RecommendedSimpleMesh = GetRecommendedSimpleMesh(OutResult.MeshType);
	}

	return OutResult.bMeshRequired;
}

TMap<FString, FString> UMeshDetectionHandler::GetMeshKeywords()
{
	TMap<FString, FString> Keywords;

	// Debris-related
	Keywords.Add(TEXT("debris"), TEXT("Debris"));
	Keywords.Add(TEXT("rock"), TEXT("Debris"));
	Keywords.Add(TEXT("stone"), TEXT("Debris"));
	Keywords.Add(TEXT("chunk"), TEXT("Debris"));
	Keywords.Add(TEXT("fragment"), TEXT("Debris"));
	Keywords.Add(TEXT("piece"), TEXT("Debris"));

	// Foliage-related
	Keywords.Add(TEXT("leaf"), TEXT("Foliage"));
	Keywords.Add(TEXT("foliage"), TEXT("Foliage"));
	Keywords.Add(TEXT("tree"), TEXT("Foliage"));
	Keywords.Add(TEXT("branch"), TEXT("Foliage"));
	Keywords.Add(TEXT("grass"), TEXT("Foliage"));
	Keywords.Add(TEXT("petal"), TEXT("Foliage"));

	// Projectile-related
	Keywords.Add(TEXT("projectile"), TEXT("Projectile"));
	Keywords.Add(TEXT("bullet"), TEXT("Projectile"));
	Keywords.Add(TEXT("missile"), TEXT("Projectile"));
	Keywords.Add(TEXT("arrow"), TEXT("Projectile"));
	Keywords.Add(TEXT("shell"), TEXT("Projectile"));
	Keywords.Add(TEXT("bolt"), TEXT("Projectile"));

	// Object-related
	Keywords.Add(TEXT("object"), TEXT("Object"));
	Keywords.Add(TEXT("mesh"), TEXT("Object"));
	Keywords.Add(TEXT("3d model"), TEXT("Object"));
	Keywords.Add(TEXT("static mesh"), TEXT("Object"));
	Keywords.Add(TEXT("geometry"), TEXT("Object"));

	// Vehicle-related
	Keywords.Add(TEXT("vehicle"), TEXT("Vehicle"));
	Keywords.Add(TEXT("car"), TEXT("Vehicle"));
	Keywords.Add(TEXT("ship"), TEXT("Vehicle"));

	// Creature-related
	Keywords.Add(TEXT("creature"), TEXT("Creature"));
	Keywords.Add(TEXT("monster"), TEXT("Creature"));
	Keywords.Add(TEXT("enemy"), TEXT("Creature"));

	return Keywords;
}

FString UMeshDetectionHandler::GetRecommendedSimpleMesh(const FString& MeshType)
{
	FString LowerType = MeshType.ToLower();

	if (LowerType.Contains(TEXT("debris")) || LowerType.Contains(TEXT("rock")) || LowerType.Contains(TEXT("stone")))
	{
		return TEXT("Sphere");
	}
	else if (LowerType.Contains(TEXT("foliage")) || LowerType.Contains(TEXT("leaf")))
	{
		return TEXT("Billboard");
	}
	else if (LowerType.Contains(TEXT("projectile")) || LowerType.Contains(TEXT("arrow")) || LowerType.Contains(TEXT("bullet")))
	{
		return TEXT("Cone");
	}
	else if (LowerType.Contains(TEXT("vehicle")) || LowerType.Contains(TEXT("car")))
	{
		return TEXT("Cube");
	}

	return TEXT("Sphere"); // Default
}

FString UMeshDetectionHandler::GetSimpleMeshPath(ESimpleMeshType MeshType)
{
	switch (MeshType)
	{
	case ESimpleMeshType::Sphere:
		return TEXT("/Engine/BasicShapes/Sphere");
	case ESimpleMeshType::Cone:
		return TEXT("/Engine/BasicShapes/Cone");
	case ESimpleMeshType::Cube:
		return TEXT("/Engine/BasicShapes/Cube");
	case ESimpleMeshType::Cylinder:
		return TEXT("/Engine/BasicShapes/Cylinder");
	case ESimpleMeshType::Billboard:
	default:
		return TEXT(""); // Billboard is not a mesh, it's a sprite
	}
}

bool UMeshDetectionHandler::LoadSimpleMesh(ESimpleMeshType MeshType, UStaticMesh*& OutMesh, FString& OutError)
{
	FString MeshPath = GetSimpleMeshPath(MeshType);
	
	if (MeshPath.IsEmpty())
	{
		OutError = TEXT("No mesh path for Billboard type (use sprite rendering instead)");
		return false;
	}

	// Load mesh asset
	OutMesh = LoadObject<UStaticMesh>(nullptr, *MeshPath);
	
	if (!OutMesh)
	{
		OutError = FString::Printf(TEXT("Failed to load mesh at path: %s"), *MeshPath);
		return false;
	}

	return true;
}

bool UMeshDetectionHandler::MeshPathExists(const FString& MeshPath)
{
	if (MeshPath.IsEmpty())
	{
		return false;
	}

	// Try to load mesh
	UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, *MeshPath);
	return Mesh != nullptr;
}

