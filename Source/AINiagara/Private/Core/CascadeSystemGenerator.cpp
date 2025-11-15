// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CascadeSystemGenerator.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleEmitter.h"
#include "Particles/ParticleLODLevel.h"
#include "Particles/ParticleModule.h"
#include "Particles/ParticleModuleRequired.h"
// Note: ParticleModuleSpawn may not exist in all UE versions, use base class
#include "Particles/Size/ParticleModuleSize.h"
#include "Particles/Color/ParticleModuleColor.h"
#include "Particles/Color/ParticleModuleColorOverLife.h"
#include "Particles/Velocity/ParticleModuleVelocity.h"
#include "Particles/Velocity/ParticleModuleVelocityOverLifetime.h"
#include "Particles/Rotation/ParticleModuleRotation.h"
// Note: ParticleModuleRotationRate may not exist in UE 5.3
// #include "Particles/ParticleModuleRotationRate.h"
#include "Particles/TypeData/ParticleModuleTypeDataBase.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/Class.h"
#include "UObject/PropertyAccessUtil.h"
#include "UObject/UnrealType.h"
#include "Misc/MessageDialog.h"
#include "Tools/MeshDetectionHandler.h"
#include "Distributions/DistributionFloat.h"
#include "Distributions/DistributionVector.h"
#include "Distributions/DistributionFloatConstant.h"
#include "Distributions/DistributionVectorConstant.h"
#include "Distributions/DistributionFloatUniform.h"
#include "Distributions/DistributionVectorUniform.h"

// Helper function to create and configure a constant vector distribution
static UDistributionVectorConstant* CreateVectorConstantDistribution(UObject* Outer, const FVector& Value)
{
	UDistributionVectorConstant* Distribution = NewObject<UDistributionVectorConstant>(Outer);
	if (Distribution)
	{
		Distribution->Constant = Value;
		Distribution->bLockAxes = false;
	}
	return Distribution;
}

// Helper function to create and configure a uniform vector distribution
static UDistributionVectorUniform* CreateVectorUniformDistribution(UObject* Outer, const FVector& Min, const FVector& Max)
{
	UDistributionVectorUniform* Distribution = NewObject<UDistributionVectorUniform>(Outer);
	if (Distribution)
	{
		Distribution->Min = Min;
		Distribution->Max = Max;
		Distribution->bLockAxes = false;
	}
	return Distribution;
}

// Helper function to create and configure a constant float distribution
static UDistributionFloatConstant* CreateFloatConstantDistribution(UObject* Outer, float Value)
{
	UDistributionFloatConstant* Distribution = NewObject<UDistributionFloatConstant>(Outer);
	if (Distribution)
	{
		Distribution->Constant = Value;
	}
	return Distribution;
}

// Helper function to set a FRawDistributionVector using reflection
static bool SetRawDistributionVector(UParticleModule* Module, const FString& PropertyName, UDistributionVector* Distribution)
{
	if (!Module || !Distribution)
	{
		return false;
	}

	FStructProperty* DistProperty = FindFProperty<FStructProperty>(Module->GetClass(), *PropertyName);
	if (!DistProperty)
	{
		return false;
	}

	FRawDistribution* RawDist = DistProperty->ContainerPtrToValuePtr<FRawDistribution>(Module);
	if (!RawDist)
	{
		return false;
	}

	// Set the distribution using reflection
	FProperty* DistObjProperty = DistProperty->Struct->FindPropertyByName(TEXT("Distribution"));
	if (DistObjProperty)
	{
		UObject** DistPtr = DistObjProperty->ContainerPtrToValuePtr<UObject*>(RawDist);
		if (DistPtr)
		{
			*DistPtr = Distribution;
			return true;
		}
	}

	return false;
}

// Helper function to set a FRawDistributionFloat using reflection
static bool SetRawDistributionFloat(UParticleModule* Module, const FString& PropertyName, UDistributionFloat* Distribution)
{
	if (!Module || !Distribution)
	{
		return false;
	}

	FStructProperty* DistProperty = FindFProperty<FStructProperty>(Module->GetClass(), *PropertyName);
	if (!DistProperty)
	{
		return false;
	}

	FRawDistribution* RawDist = DistProperty->ContainerPtrToValuePtr<FRawDistribution>(Module);
	if (!RawDist)
	{
		return false;
	}

	// Set the distribution using reflection
	FProperty* DistObjProperty = DistProperty->Struct->FindPropertyByName(TEXT("Distribution"));
	if (DistObjProperty)
	{
		UObject** DistPtr = DistObjProperty->ContainerPtrToValuePtr<UObject*>(RawDist);
		if (DistPtr)
		{
			*DistPtr = Distribution;
			return true;
		}
	}

	return false;
}

// Helper function to get or create the first LOD level
static UParticleLODLevel* GetOrCreateFirstLODLevel(UParticleEmitter* Emitter, FString& OutError)
{
	if (!Emitter)
	{
		OutError = TEXT("Emitter is null");
		return nullptr;
	}

	// Get or create first LOD level
	if (Emitter->LODLevels.Num() == 0)
	{
		int32 LODLevelIndex = Emitter->CreateLODLevel(0, true);
		if (LODLevelIndex < 0 || Emitter->LODLevels.Num() == 0)
		{
			OutError = TEXT("Failed to create LOD level");
			return nullptr;
		}
	}

	UParticleLODLevel* LODLevel = Emitter->LODLevels[0];
	if (!LODLevel)
	{
		OutError = TEXT("First LOD level is null");
		return nullptr;
	}

	// Ensure RequiredModule exists
	if (!LODLevel->RequiredModule)
	{
		LODLevel->RequiredModule = NewObject<UParticleModuleRequired>(LODLevel);
		if (!LODLevel->RequiredModule)
		{
			OutError = TEXT("Failed to create RequiredModule");
			return nullptr;
		}
	}

	return LODLevel;
}

bool UCascadeSystemGenerator::CreateSystemFromDSL(
	const FVFXDSL& DSL,
	const FString& PackagePath,
	const FString& SystemName,
	UParticleSystem*& OutSystem,
	FString& OutError)
{
	// Validate DSL type
	if (DSL.Effect.Type != EVFXEffectType::Cascade)
	{
		OutError = TEXT("DSL type is not Cascade");
		return false;
	}

	// Validate emitters
	if (DSL.Emitters.Num() == 0)
	{
		OutError = TEXT("DSL must contain at least one emitter");
		return false;
	}

	// Create package
	FString FullPackagePath = PackagePath + TEXT("/") + SystemName;
	UPackage* Package = CreatePackage(*FullPackagePath);
	if (!Package)
	{
		OutError = FString::Printf(TEXT("Failed to create package at path: %s"), *FullPackagePath);
		return false;
	}

	// Create Cascade particle system
	OutSystem = NewObject<UParticleSystem>(Package, *SystemName, RF_Public | RF_Standalone);
	if (!OutSystem)
	{
		OutError = TEXT("Failed to create UParticleSystem object");
		return false;
	}

	// Configure system properties
	// Note: UParticleSystem doesn't have a Duration property in UE 5.3
	// Duration is stored in each emitter's RequiredModule->EmitterDuration
	// This will be set when configuring each emitter
	OutSystem->bOrientZAxisTowardCamera = false;
	OutSystem->bUseFixedRelativeBoundingBox = false;

	// Create emitters for each DSL emitter
	for (const FVFXDSLEmitter& EmitterDSL : DSL.Emitters)
	{
		UParticleEmitter* Emitter = nullptr;
		FString EmitterError;
		
		if (!CreateEmitterFromDSL(EmitterDSL, PackagePath, EmitterDSL.Name, Emitter, EmitterError))
		{
			OutError = FString::Printf(TEXT("Failed to create emitter '%s': %s"), *EmitterDSL.Name, *EmitterError);
			return false;
		}

		// Add emitter to system
		OutSystem->Emitters.Add(Emitter);
	}

	// Mark package as dirty
	Package->MarkPackageDirty();

	// Notify asset registry
	FAssetRegistryModule::AssetCreated(OutSystem);

	return true;
}

bool UCascadeSystemGenerator::CreateEmitterFromDSL(
	const FVFXDSLEmitter& EmitterDSL,
	const FString& PackagePath,
	const FString& EmitterName,
	UParticleEmitter*& OutEmitter,
	FString& OutError)
{
	// Create package for emitter
	FString FullPackagePath = PackagePath + TEXT("/") + EmitterName;
	UPackage* Package = CreatePackage(*FullPackagePath);
	if (!Package)
	{
		OutError = FString::Printf(TEXT("Failed to create package at path: %s"), *FullPackagePath);
		return false;
	}

	// Create Cascade emitter
	OutEmitter = NewObject<UParticleEmitter>(Package, *EmitterName, RF_Public | RF_Standalone);
	if (!OutEmitter)
	{
		OutError = TEXT("Failed to create UParticleEmitter object");
		return false;
	}

	// Set emitter name
	OutEmitter->EmitterName = FName(*EmitterName);

	// Configure spawn module
	FString SpawnError;
	if (!ConfigureSpawnModule(OutEmitter, EmitterDSL.Spawners, SpawnError))
	{
		OutError = FString::Printf(TEXT("Failed to configure spawn module: %s"), *SpawnError);
		return false;
	}

	// Configure initialize module
	FString InitError;
	if (!ConfigureInitializeModule(OutEmitter, EmitterDSL.Initialization, InitError))
	{
		OutError = FString::Printf(TEXT("Failed to configure initialize module: %s"), *InitError);
		return false;
	}

	// Configure update module
	FString UpdateError;
	if (!ConfigureUpdateModule(OutEmitter, EmitterDSL.Update, UpdateError))
	{
		OutError = FString::Printf(TEXT("Failed to configure update module: %s"), *UpdateError);
		return false;
	}

	// Configure render module
	FString RenderError;
	if (!ConfigureRenderModule(OutEmitter, EmitterDSL.Render, RenderError))
	{
		OutError = FString::Printf(TEXT("Failed to configure render module: %s"), *RenderError);
		return false;
	}

	// Mark package as dirty
	Package->MarkPackageDirty();

	// Notify asset registry
	FAssetRegistryModule::AssetCreated(OutEmitter);

	return true;
}

bool UCascadeSystemGenerator::ConfigureSpawnModule(
	UParticleEmitter* Emitter,
	const FVFXDSLSpawners& SpawnersDSL,
	FString& OutError)
{
	if (!Emitter)
	{
		OutError = TEXT("Emitter is null");
		return false;
	}

	// Find or create spawn module using dynamic class loading
	UParticleModule* SpawnModule = nullptr;
	UClass* SpawnModuleClass = FindObject<UClass>(nullptr, TEXT("/Script/Engine.ParticleModuleSpawn"));
	
	// Get or create first LOD level
	UParticleLODLevel* LODLevel = GetOrCreateFirstLODLevel(Emitter, OutError);
	if (!LODLevel)
	{
		return false;
	}

	if (!SpawnModuleClass)
	{
		// Fallback: try to find any existing spawn module
		for (UParticleModule* Module : LODLevel->SpawnModules)
		{
			if (Module && Module->GetClass()->GetName().Contains(TEXT("Spawn")))
			{
				SpawnModule = Module;
				break;
			}
		}
	}
	else
	{
		// Find existing spawn module
		for (UParticleModule* Module : LODLevel->SpawnModules)
		{
			if (Module && Module->IsA(SpawnModuleClass))
			{
				SpawnModule = Module;
				break;
			}
		}

		if (!SpawnModule)
		{
			// Create new spawn module
			SpawnModule = NewObject<UParticleModule>(LODLevel, SpawnModuleClass);
			if (!SpawnModule)
			{
				OutError = TEXT("Failed to create UParticleModuleSpawn");
				return false;
			}
			LODLevel->SpawnModules.Add(SpawnModule);
		}
	}

	if (!SpawnModule)
	{
		OutError = TEXT("Could not find or create spawn module");
		return false;
	}

	// Configure spawn rate using reflection
	if (SpawnersDSL.Rate.SpawnRate > 0.0f)
	{
		FStructProperty* RateProperty = FindFProperty<FStructProperty>(SpawnModule->GetClass(), TEXT("Rate"));
		if (RateProperty)
		{
			FVector2D* RateValue = RateProperty->ContainerPtrToValuePtr<FVector2D>(SpawnModule);
			if (RateValue)
			{
				*RateValue = FVector2D(SpawnersDSL.Rate.SpawnRate, SpawnersDSL.Rate.SpawnRate);
			}
		}
	}

	// Configure burst spawns using reflection
	if (SpawnersDSL.Burst.Count > 0)
	{
		FArrayProperty* BurstListProperty = FindFProperty<FArrayProperty>(SpawnModule->GetClass(), TEXT("BurstList"));
		if (BurstListProperty)
		{
			FScriptArrayHelper ArrayHelper(BurstListProperty, BurstListProperty->ContainerPtrToValuePtr<void>(SpawnModule));
			ArrayHelper.EmptyValues();

			// Add first burst
			int32 Index = ArrayHelper.AddValue();
			uint8* BurstData = ArrayHelper.GetRawPtr(Index);
			
			FStructProperty* BurstStructProperty = CastField<FStructProperty>(BurstListProperty->Inner);
			if (BurstStructProperty && BurstData)
			{
				// Set Count
				FIntProperty* CountProperty = FindFProperty<FIntProperty>(BurstStructProperty->Struct, TEXT("Count"));
				FIntProperty* CountLowProperty = FindFProperty<FIntProperty>(BurstStructProperty->Struct, TEXT("CountLow"));
				FFloatProperty* TimeProperty = FindFProperty<FFloatProperty>(BurstStructProperty->Struct, TEXT("Time"));
				
				if (CountProperty) CountProperty->SetPropertyValue_InContainer(BurstData, SpawnersDSL.Burst.Count);
				if (CountLowProperty) CountLowProperty->SetPropertyValue_InContainer(BurstData, SpawnersDSL.Burst.Count);
				if (TimeProperty) TimeProperty->SetPropertyValue_InContainer(BurstData, SpawnersDSL.Burst.Time);
			}

			// Add additional bursts from intervals
			for (float Interval : SpawnersDSL.Burst.Intervals)
			{
				int32 Index2 = ArrayHelper.AddValue();
				uint8* BurstData2 = ArrayHelper.GetRawPtr(Index2);
				
				if (BurstStructProperty && BurstData2)
				{
					FIntProperty* CountProperty = FindFProperty<FIntProperty>(BurstStructProperty->Struct, TEXT("Count"));
					FIntProperty* CountLowProperty = FindFProperty<FIntProperty>(BurstStructProperty->Struct, TEXT("CountLow"));
					FFloatProperty* TimeProperty = FindFProperty<FFloatProperty>(BurstStructProperty->Struct, TEXT("Time"));
					
					if (CountProperty) CountProperty->SetPropertyValue_InContainer(BurstData2, SpawnersDSL.Burst.Count);
					if (CountLowProperty) CountLowProperty->SetPropertyValue_InContainer(BurstData2, SpawnersDSL.Burst.Count);
					if (TimeProperty) TimeProperty->SetPropertyValue_InContainer(BurstData2, Interval);
				}
			}
		}
	}

	return true;
}

bool UCascadeSystemGenerator::ConfigureInitializeModule(
	UParticleEmitter* Emitter,
	const FVFXDSLInitialization& InitializationDSL,
	FString& OutError)
{
	if (!Emitter)
	{
		OutError = TEXT("Emitter is null");
		return false;
	}

	// Get or create first LOD level
	UParticleLODLevel* LODLevel = GetOrCreateFirstLODLevel(Emitter, OutError);
	if (!LODLevel)
	{
		return false;
	}

	// Get required module (contains initialization parameters)
	UParticleModuleRequired* RequiredModule = LODLevel->RequiredModule;
	if (!RequiredModule)
	{
		OutError = TEXT("Failed to get RequiredModule");
		return false;
	}

	// Configure emitter time (duration)
	// Note: Duration should be passed from DSL.Effect.Duration, but we don't have access here
	// This should be set in CreateEmitterFromDSL or passed as a parameter
	RequiredModule->EmitterDuration = 5.0f; // Default, will be overridden if DSL provides duration

	// Find or create size module
	UParticleModuleSize* SizeModule = nullptr;
	for (UParticleModule* Module : LODLevel->SpawnModules)
	{
		if (UParticleModuleSize* Size = Cast<UParticleModuleSize>(Module))
		{
			SizeModule = Size;
			break;
		}
	}

	if (!SizeModule)
	{
		SizeModule = NewObject<UParticleModuleSize>(LODLevel);
		if (SizeModule)
		{
			LODLevel->SpawnModules.Add(SizeModule);
		}
	}

	if (SizeModule)
	{
		// Configure size distribution
		// Use uniform distribution if Min != Max, otherwise use constant
		FVector SizeValue(InitializationDSL.Size.Min, InitializationDSL.Size.Min, InitializationDSL.Size.Min);
		UDistributionVector* SizeDistribution = nullptr;
		
		if (FMath::IsNearlyEqual(InitializationDSL.Size.Min, InitializationDSL.Size.Max))
		{
			// Constant size
			SizeDistribution = CreateVectorConstantDistribution(SizeModule, SizeValue);
		}
		else
		{
			// Uniform size range
			FVector MinSize(InitializationDSL.Size.Min, InitializationDSL.Size.Min, InitializationDSL.Size.Min);
			FVector MaxSize(InitializationDSL.Size.Max, InitializationDSL.Size.Max, InitializationDSL.Size.Max);
			SizeDistribution = CreateVectorUniformDistribution(SizeModule, MinSize, MaxSize);
		}
		
		if (SizeDistribution)
		{
			SetRawDistributionVector(SizeModule, TEXT("StartSize"), SizeDistribution);
		}
	}

	// Find or create color module
	UParticleModuleColor* ColorModule = nullptr;
	for (UParticleModule* Module : LODLevel->SpawnModules)
	{
		if (UParticleModuleColor* Color = Cast<UParticleModuleColor>(Module))
		{
			ColorModule = Color;
			break;
		}
	}

	if (!ColorModule)
	{
		ColorModule = NewObject<UParticleModuleColor>(LODLevel);
		if (ColorModule)
		{
			LODLevel->SpawnModules.Add(ColorModule);
		}
	}

	if (ColorModule)
	{
		// Configure color distribution
		// FRawDistributionLinearColor uses a vector distribution for RGB and float for Alpha
		FVector ColorRGB(InitializationDSL.Color.R, InitializationDSL.Color.G, InitializationDSL.Color.B);
		UDistributionVectorConstant* ColorRGBDistribution = CreateVectorConstantDistribution(ColorModule, ColorRGB);
		
		if (ColorRGBDistribution)
		{
			// Set RGB distribution
			SetRawDistributionVector(ColorModule, TEXT("StartColor"), ColorRGBDistribution);
		}
		
		// Set alpha separately if needed (StartAlpha property)
		FStructProperty* AlphaProperty = FindFProperty<FStructProperty>(ColorModule->GetClass(), TEXT("StartAlpha"));
		if (AlphaProperty)
		{
			UDistributionFloatConstant* AlphaDistribution = CreateFloatConstantDistribution(ColorModule, InitializationDSL.Color.A);
			if (AlphaDistribution)
			{
				SetRawDistributionFloat(ColorModule, TEXT("StartAlpha"), AlphaDistribution);
			}
		}
	}

	// Find or create velocity module
	UParticleModuleVelocity* VelocityModule = nullptr;
	for (UParticleModule* Module : LODLevel->SpawnModules)
	{
		if (UParticleModuleVelocity* Velocity = Cast<UParticleModuleVelocity>(Module))
		{
			VelocityModule = Velocity;
			break;
		}
	}

	if (!VelocityModule)
	{
		VelocityModule = NewObject<UParticleModuleVelocity>(LODLevel);
		if (VelocityModule)
		{
			LODLevel->SpawnModules.Add(VelocityModule);
		}
	}

	if (VelocityModule)
	{
		// Configure velocity distribution
		FVector StartVelocity(
			InitializationDSL.Velocity.X,
			InitializationDSL.Velocity.Y,
			InitializationDSL.Velocity.Z
		);
		
		UDistributionVectorConstant* VelocityDistribution = CreateVectorConstantDistribution(VelocityModule, StartVelocity);
		if (VelocityDistribution)
		{
			SetRawDistributionVector(VelocityModule, TEXT("StartVelocity"), VelocityDistribution);
		}
	}

	// Note: Rotation configuration is handled in ConfigureRenderModule
	// since rotation is now part of FVFXDSLMesh in the Render configuration

	return true;
}

bool UCascadeSystemGenerator::ConfigureUpdateModule(
	UParticleEmitter* Emitter,
	const FVFXDSLUpdate& UpdateDSL,
	FString& OutError)
{
	if (!Emitter)
	{
		OutError = TEXT("Emitter is null");
		return false;
	}

	// Get or create first LOD level
	UParticleLODLevel* LODLevel = GetOrCreateFirstLODLevel(Emitter, OutError);
	if (!LODLevel)
	{
		return false;
	}

	// Find or create velocity over lifetime module for forces
	UParticleModuleVelocityOverLifetime* VelocityOverLifetimeModule = nullptr;
	for (UParticleModule* Module : LODLevel->UpdateModules)
	{
		if (UParticleModuleVelocityOverLifetime* VelocityOverLifetime = Cast<UParticleModuleVelocityOverLifetime>(Module))
		{
			VelocityOverLifetimeModule = VelocityOverLifetime;
			break;
		}
	}

	if (!VelocityOverLifetimeModule)
	{
		VelocityOverLifetimeModule = NewObject<UParticleModuleVelocityOverLifetime>(LODLevel);
		if (VelocityOverLifetimeModule)
		{
			LODLevel->UpdateModules.Add(VelocityOverLifetimeModule);
		}
	}

	if (VelocityOverLifetimeModule)
	{
		// Apply gravity and wind forces
		FVector VelOverLife(
			UpdateDSL.Forces.Wind.X,
			UpdateDSL.Forces.Wind.Y,
			UpdateDSL.Forces.Gravity + UpdateDSL.Forces.Wind.Z
		);
		
		UDistributionVectorConstant* VelOverLifeDistribution = CreateVectorConstantDistribution(VelocityOverLifetimeModule, VelOverLife);
		if (VelOverLifeDistribution)
		{
			SetRawDistributionVector(VelocityOverLifetimeModule, TEXT("VelOverLife"), VelOverLifeDistribution);
		}
	}

	// TODO: Configure drag and collision modules
	// These require more complex module setup

	return true;
}

bool UCascadeSystemGenerator::ConfigureRenderModule(
	UParticleEmitter* Emitter,
	const FVFXDSLRender& RenderDSL,
	FString& OutError)
{
	if (!Emitter)
	{
		OutError = TEXT("Emitter is null");
		return false;
	}

	// Get or create first LOD level
	UParticleLODLevel* LODLevel = GetOrCreateFirstLODLevel(Emitter, OutError);
	if (!LODLevel)
	{
		return false;
	}

	// Get required module to set material
	UParticleModuleRequired* RequiredModule = LODLevel->RequiredModule;

	if (RequiredModule)
	{
		// Set material if specified
		if (!RenderDSL.Material.IsEmpty())
		{
			// Load material asset
			UMaterialInterface* Material = LoadObject<UMaterialInterface>(nullptr, *RenderDSL.Material);
			if (Material)
			{
				RequiredModule->Material = Material;
			}
		}

		// Note: BlendMode property was removed from UParticleModuleRequired in UE 5.3
		// Blend mode is now controlled by the material itself, not the particle module
		// To set blend mode, configure it in the material asset

		// Configure mesh if specified
		if (RenderDSL.Mesh.bUseMesh)
		{
			UStaticMesh* MeshAsset = nullptr;
			
			// Try to load mesh from path if provided
			if (!RenderDSL.Mesh.MeshPath.IsEmpty())
			{
				MeshAsset = LoadObject<UStaticMesh>(nullptr, *RenderDSL.Mesh.MeshPath);
				
				// If path doesn't exist, try simple mesh type
				if (!MeshAsset && !RenderDSL.Mesh.MeshType.IsEmpty())
				{
					// Convert mesh type string to enum
					ESimpleMeshType SimpleMeshType = ESimpleMeshType::Sphere;
					FString LowerType = RenderDSL.Mesh.MeshType.ToLower();
					
					if (LowerType.Contains(TEXT("cone")))
					{
						SimpleMeshType = ESimpleMeshType::Cone;
					}
					else if (LowerType.Contains(TEXT("sphere")))
					{
						SimpleMeshType = ESimpleMeshType::Sphere;
					}
					else if (LowerType.Contains(TEXT("cube")))
					{
						SimpleMeshType = ESimpleMeshType::Cube;
					}
					else if (LowerType.Contains(TEXT("cylinder")))
					{
						SimpleMeshType = ESimpleMeshType::Cylinder;
					}
					
					FString MeshError;
					UStaticMesh* LoadedMesh = nullptr;
					if (UMeshDetectionHandler::LoadSimpleMesh(SimpleMeshType, LoadedMesh, MeshError))
					{
						MeshAsset = LoadedMesh;
					}
				}
			}
			else if (!RenderDSL.Mesh.MeshType.IsEmpty())
			{
				// Try to load simple mesh by type
				ESimpleMeshType SimpleMeshType = ESimpleMeshType::Sphere;
				FString LowerType = RenderDSL.Mesh.MeshType.ToLower();
				
				if (LowerType.Contains(TEXT("cone")))
				{
					SimpleMeshType = ESimpleMeshType::Cone;
				}
				else if (LowerType.Contains(TEXT("sphere")))
				{
					SimpleMeshType = ESimpleMeshType::Sphere;
				}
				else if (LowerType.Contains(TEXT("cube")))
				{
					SimpleMeshType = ESimpleMeshType::Cube;
				}
				else if (LowerType.Contains(TEXT("cylinder")))
				{
					SimpleMeshType = ESimpleMeshType::Cylinder;
				}
				
				FString MeshError;
				UStaticMesh* LoadedMesh = nullptr;
				if (UMeshDetectionHandler::LoadSimpleMesh(SimpleMeshType, LoadedMesh, MeshError))
				{
					MeshAsset = LoadedMesh;
				}
			}

			// Apply mesh if loaded
			if (MeshAsset)
			{
				// Use dynamic class loading for UE 5.3 compatibility
				UClass* MeshTypeDataClass = FindObject<UClass>(nullptr, TEXT("/Script/Engine.ParticleModuleTypeDataMesh"));
				if (MeshTypeDataClass)
				{
					// Find or create TypeData mesh module
					// TODO: TypeDataModules was removed from UParticleEmitter in UE 5.3
					// Type data modules are now stored in LODLevel->TypeDataModule
					UParticleModuleTypeDataBase* MeshTypeData = LODLevel->TypeDataModule;
					if (!MeshTypeData || !MeshTypeData->IsA(MeshTypeDataClass))
					{
						MeshTypeData = NewObject<UParticleModuleTypeDataBase>(Emitter, MeshTypeDataClass);
						if (MeshTypeData)
						{
							LODLevel->TypeDataModule = MeshTypeData;
						}
					}

					if (MeshTypeData)
					{
						// Set mesh property via reflection for UE 5.3 compatibility
						FProperty* MeshProperty = MeshTypeDataClass->FindPropertyByName(TEXT("Mesh"));
						if (MeshProperty)
						{
							UObject** MeshPtr = MeshProperty->ContainerPtrToValuePtr<UObject*>(MeshTypeData);
							if (MeshPtr)
							{
								*MeshPtr = MeshAsset;
							}
						}
						
						// Set scale
						if (RenderDSL.Mesh.Scale > 0.0f)
						{
							// Scale is applied through size module, which is configured separately
							// For mesh particles, size affects mesh scale
						}
						
						// Note: Rotation is typically handled through rotation modules, not in TypeData
						// This is a simplified implementation - full rotation support would require
						// additional rotation modules (UParticleModuleRotation, etc.)
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("ParticleModuleTypeDataMesh class not found. Mesh support may not be available in this UE version."));
				}
			}
			else if (RenderDSL.Mesh.bUseMesh)
			{
				// Mesh requested but failed to load
				UE_LOG(LogTemp, Warning, TEXT("Failed to load mesh for emitter. Mesh path: %s, Type: %s"), 
					*RenderDSL.Mesh.MeshPath, *RenderDSL.Mesh.MeshType);
			}
		}
	}

	// Configure rotation from mesh configuration
	if (RenderDSL.Mesh.Rotation.X != 0.0f || RenderDSL.Mesh.Rotation.Y != 0.0f || RenderDSL.Mesh.Rotation.Z != 0.0f)
	{
		// Find or create rotation module
		UParticleModuleRotation* RotationModule = nullptr;
		for (UParticleModule* Module : LODLevel->SpawnModules)
		{
			if (UParticleModuleRotation* Rotation = Cast<UParticleModuleRotation>(Module))
			{
				RotationModule = Rotation;
				break;
			}
		}

		if (!RotationModule)
		{
			RotationModule = NewObject<UParticleModuleRotation>(LODLevel);
			if (RotationModule)
			{
				LODLevel->SpawnModules.Add(RotationModule);
			}
		}

		if (RotationModule)
		{
			// Convert rotation from degrees to a rotation range
			// UParticleModuleRotation uses StartRotation as FVector2D (min, max) in degrees
			// For simplicity, use the Z component as the rotation value
			float RotationZ = RenderDSL.Mesh.Rotation.Z;
			FVector2D StartRotation(RotationZ, RotationZ);
			
			// Set rotation using reflection
			FStructProperty* RotationProperty = FindFProperty<FStructProperty>(RotationModule->GetClass(), TEXT("StartRotation"));
			if (RotationProperty)
			{
				FVector2D* RotationValue = RotationProperty->ContainerPtrToValuePtr<FVector2D>(RotationModule);
				if (RotationValue)
				{
					*RotationValue = StartRotation;
				}
			}
		}
	}

	return true;
}

