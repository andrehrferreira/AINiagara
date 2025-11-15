// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CascadeSystemGenerator.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleEmitter.h"
#include "Particles/ParticleModule.h"
#include "Particles/ParticleModuleRequired.h"
// Note: ParticleModuleSpawn may not exist in all UE versions, use base class
#include "Particles/ParticleModuleLifetime.h"
#include "Particles/ParticleModuleSize.h"
#include "Particles/ParticleModuleColor.h"
#include "Particles/ParticleModuleColorOverLife.h"
#include "Particles/ParticleModuleVelocity.h"
#include "Particles/ParticleModuleVelocityOverLifetime.h"
#include "Particles/ParticleModuleRotation.h"
#include "Particles/ParticleModuleRotationRate.h"
#include "Particles/ParticleModuleTypeDataBase.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/Class.h"
#include "UObject/PropertyAccessUtil.h"
#include "UObject/UnrealType.h"
#include "Misc/MessageDialog.h"

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
	OutSystem->Duration = DSL.Effect.Duration;
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
	
	if (!SpawnModuleClass)
	{
		// Fallback: try to find any existing spawn module
		for (UParticleModule* Module : Emitter->SpawnModules)
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
		for (UParticleModule* Module : Emitter->SpawnModules)
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
			SpawnModule = NewObject<UParticleModule>(Emitter, SpawnModuleClass);
			if (!SpawnModule)
			{
				OutError = TEXT("Failed to create UParticleModuleSpawn");
				return false;
			}
			Emitter->SpawnModules.Add(SpawnModule);
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
		FStructProperty* RateProperty = FindField<FStructProperty>(SpawnModule->GetClass(), TEXT("Rate"));
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
		FArrayProperty* BurstListProperty = FindField<FArrayProperty>(SpawnModule->GetClass(), TEXT("BurstList"));
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
				FIntProperty* CountProperty = FindField<FIntProperty>(BurstStructProperty->Struct, TEXT("Count"));
				FIntProperty* CountLowProperty = FindField<FIntProperty>(BurstStructProperty->Struct, TEXT("CountLow"));
				FFloatProperty* TimeProperty = FindField<FFloatProperty>(BurstStructProperty->Struct, TEXT("Time"));
				
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
					FIntProperty* CountProperty = FindField<FIntProperty>(BurstStructProperty->Struct, TEXT("Count"));
					FIntProperty* CountLowProperty = FindField<FIntProperty>(BurstStructProperty->Struct, TEXT("CountLow"));
					FFloatProperty* TimeProperty = FindField<FFloatProperty>(BurstStructProperty->Struct, TEXT("Time"));
					
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

	// Find or create required module (contains initialization parameters)
	UParticleModuleRequired* RequiredModule = nullptr;
	for (UParticleModule* Module : Emitter->RequiredModules)
	{
		if (UParticleModuleRequired* Required = Cast<UParticleModuleRequired>(Module))
		{
			RequiredModule = Required;
			break;
		}
	}

	if (!RequiredModule)
	{
		RequiredModule = NewObject<UParticleModuleRequired>(Emitter);
		if (!RequiredModule)
		{
			OutError = TEXT("Failed to create UParticleModuleRequired");
			return false;
		}
		Emitter->RequiredModules.Add(RequiredModule);
	}

	// Configure material (will be set in render module)
	// Configure emitter time (duration)
	RequiredModule->EmitterDuration = 5.0f; // Default, can be overridden

	// Find or create size module
	UParticleModuleSize* SizeModule = nullptr;
	for (UParticleModule* Module : Emitter->SpawnModules)
	{
		if (UParticleModuleSize* Size = Cast<UParticleModuleSize>(Module))
		{
			SizeModule = Size;
			break;
		}
	}

	if (!SizeModule)
	{
		SizeModule = NewObject<UParticleModuleSize>(Emitter);
		if (SizeModule)
		{
			Emitter->SpawnModules.Add(SizeModule);
		}
	}

	if (SizeModule)
	{
		SizeModule->StartSize = FVector2D(InitializationDSL.Size.Min, InitializationDSL.Size.Max);
	}

	// Find or create color module
	UParticleModuleColor* ColorModule = nullptr;
	for (UParticleModule* Module : Emitter->SpawnModules)
	{
		if (UParticleModuleColor* Color = Cast<UParticleModuleColor>(Module))
		{
			ColorModule = Color;
			break;
		}
	}

	if (!ColorModule)
	{
		ColorModule = NewObject<UParticleModuleColor>(Emitter);
		if (ColorModule)
		{
			Emitter->SpawnModules.Add(ColorModule);
		}
	}

	if (ColorModule)
	{
		FLinearColor StartColor(
			InitializationDSL.Color.R,
			InitializationDSL.Color.G,
			InitializationDSL.Color.B,
			InitializationDSL.Color.A
		);
		ColorModule->StartColor = FVector4(StartColor.R, StartColor.G, StartColor.B, StartColor.A);
	}

	// Find or create velocity module
	UParticleModuleVelocity* VelocityModule = nullptr;
	for (UParticleModule* Module : Emitter->SpawnModules)
	{
		if (UParticleModuleVelocity* Velocity = Cast<UParticleModuleVelocity>(Module))
		{
			VelocityModule = Velocity;
			break;
		}
	}

	if (!VelocityModule)
	{
		VelocityModule = NewObject<UParticleModuleVelocity>(Emitter);
		if (VelocityModule)
		{
			Emitter->SpawnModules.Add(VelocityModule);
		}
	}

	if (VelocityModule)
	{
		FVector StartVelocity(
			InitializationDSL.Velocity.X,
			InitializationDSL.Velocity.Y,
			InitializationDSL.Velocity.Z
		);
		VelocityModule->StartVelocity = StartVelocity;
	}

	// Find or create rotation module
	UParticleModuleRotation* RotationModule = nullptr;
	for (UParticleModule* Module : Emitter->SpawnModules)
	{
		if (UParticleModuleRotation* Rotation = Cast<UParticleModuleRotation>(Module))
		{
			RotationModule = Rotation;
			break;
		}
	}

	if (!RotationModule)
	{
		RotationModule = NewObject<UParticleModuleRotation>(Emitter);
		if (RotationModule)
		{
			Emitter->SpawnModules.Add(RotationModule);
		}
	}

	if (RotationModule)
	{
		RotationModule->StartRotation = FVector2D(InitializationDSL.Rotation.Min, InitializationDSL.Rotation.Max);
	}

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

	// Find or create velocity over lifetime module for forces
	UParticleModuleVelocityOverLifetime* VelocityOverLifetimeModule = nullptr;
	for (UParticleModule* Module : Emitter->UpdateModules)
	{
		if (UParticleModuleVelocityOverLifetime* VelocityOverLifetime = Cast<UParticleModuleVelocityOverLifetime>(Module))
		{
			VelocityOverLifetimeModule = VelocityOverLifetime;
			break;
		}
	}

	if (!VelocityOverLifetimeModule)
	{
		VelocityOverLifetimeModule = NewObject<UParticleModuleVelocityOverLifetime>(Emitter);
		if (VelocityOverLifetimeModule)
		{
			Emitter->UpdateModules.Add(VelocityOverLifetimeModule);
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
		VelocityOverLifetimeModule->VelOverLife = VelOverLife;
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

	// Find required module to set material
	UParticleModuleRequired* RequiredModule = nullptr;
	for (UParticleModule* Module : Emitter->RequiredModules)
	{
		if (UParticleModuleRequired* Required = Cast<UParticleModuleRequired>(Module))
		{
			RequiredModule = Required;
			break;
		}
	}

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

		// Configure blend mode
		if (RenderDSL.BlendMode.Equals(TEXT("Additive"), ESearchCase::IgnoreCase))
		{
			RequiredModule->BlendMode = BLEND_Additive;
		}
		else if (RenderDSL.BlendMode.Equals(TEXT("Translucent"), ESearchCase::IgnoreCase))
		{
			RequiredModule->BlendMode = BLEND_Translucent;
		}
		else if (RenderDSL.BlendMode.Equals(TEXT("Opaque"), ESearchCase::IgnoreCase))
		{
			RequiredModule->BlendMode = BLEND_Opaque;
		}
		else if (RenderDSL.BlendMode.Equals(TEXT("Modulate"), ESearchCase::IgnoreCase))
		{
			RequiredModule->BlendMode = BLEND_Modulate;
		}

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
					if (UMeshDetectionHandler::LoadSimpleMesh(SimpleMeshType, MeshAsset, MeshError))
					{
						// Mesh loaded successfully
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
				UMeshDetectionHandler::LoadSimpleMesh(SimpleMeshType, MeshAsset, MeshError);
			}

			// Apply mesh if loaded
			if (MeshAsset)
			{
				// Use dynamic class loading for UE 5.3 compatibility
				UClass* MeshTypeDataClass = FindObject<UClass>(ANY_PACKAGE, TEXT("ParticleModuleTypeDataMesh"));
				if (MeshTypeDataClass)
				{
					// Find or create TypeData mesh module
					UParticleModule* MeshTypeData = nullptr;
					for (UParticleModule* Module : Emitter->TypeDataModules)
					{
						if (Module && Module->GetClass() == MeshTypeDataClass)
						{
							MeshTypeData = Module;
							break;
						}
					}

					if (!MeshTypeData)
					{
						MeshTypeData = NewObject<UParticleModule>(Emitter, MeshTypeDataClass);
						if (MeshTypeData)
						{
							Emitter->TypeDataModules.Add(MeshTypeData);
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

	return true;
}

