// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/PreviewSystemManager.h"
#include "Core/NiagaraSystemGenerator.h"
#include "Core/CascadeSystemGenerator.h"
#include "Core/VFXDSLDiff.h"
#include "NiagaraSystem.h"
#include "Particles/ParticleSystem.h"
#include "UObject/Package.h"
#include "Engine/Engine.h"
#include "HAL/PlatformTime.h"
#include "Editor.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "AssetEditorSubsystem.h"
#include "Editor/EditorEngine.h"

UPreviewSystemManager* UPreviewSystemManager::Get()
{
	static UPreviewSystemManager* Instance = nullptr;
	if (!Instance)
	{
		Instance = NewObject<UPreviewSystemManager>();
		Instance->AddToRoot(); // Keep alive
	}
	return Instance;
}

bool UPreviewSystemManager::UpdatePreview(const FVFXDSL& DSL, bool bForceUpdate, FString* OutError)
{
	if (!bPreviewEnabled)
	{
		if (OutError)
		{
			*OutError = TEXT("Preview is disabled");
		}
		return false;
	}

	// Throttle updates
	double CurrentTime = FPlatformTime::Seconds();
	if (!bForceUpdate && (CurrentTime - LastUpdateTime) < MinUpdateInterval)
	{
		if (OutError)
		{
			*OutError = TEXT("Update throttled (too frequent)");
		}
		return false; // Skip this update (throttled)
	}

	// Check if DSL has changed
	if (!bForceUpdate && !HasDSLChanged(DSL))
	{
		if (OutError)
		{
			*OutError = TEXT("DSL has not changed");
		}
		return false; // No change, skip update
	}

	LastUpdateTime = CurrentTime;

	// Store previous preview in case of error (to restore it)
	UNiagaraSystem* PreviousNiagaraPreview = NiagaraPreview;
	UParticleSystem* PreviousCascadePreview = CascadePreview;
	FVFXDSL PreviousDSL = CurrentPreviewDSL;
	bool bHadPreviousPreview = (PreviousNiagaraPreview != nullptr || PreviousCascadePreview != nullptr);

	// Clean up old preview (but keep references for restoration if needed)
	// We'll only mark as garbage if creation succeeds
	UNiagaraSystem* OldNiagaraPreview = NiagaraPreview;
	UParticleSystem* OldCascadePreview = CascadePreview;
	NiagaraPreview = nullptr;
	CascadePreview = nullptr;

	// Create new preview based on DSL type
	FString Error;
	bool bSuccess = false;

	if (DSL.Effect.Type == EVFXEffectType::Niagara)
	{
		bSuccess = CreateNiagaraPreview(DSL, Error);
	}
	else if (DSL.Effect.Type == EVFXEffectType::Cascade)
	{
		bSuccess = CreateCascadePreview(DSL, Error);
	}
	else
	{
		Error = FString::Printf(TEXT("Unknown or invalid DSL effect type: %d"), (int32)DSL.Effect.Type);
		bSuccess = false;
	}

	if (bSuccess)
	{
		CurrentPreviewDSL = DSL;
		
		// Mark old previews as garbage now that new one succeeded
		if (OldNiagaraPreview)
		{
			OldNiagaraPreview->MarkAsGarbage();
		}
		if (OldCascadePreview)
		{
			OldCascadePreview->MarkAsGarbage();
		}
		
		// Update editor viewport with new preview
		UpdateEditorViewport();
	}
	else
	{
		// Restore previous preview on error
		NiagaraPreview = PreviousNiagaraPreview;
		CascadePreview = PreviousCascadePreview;
		CurrentPreviewDSL = PreviousDSL;
		
		// Mark failed preview attempts as garbage
		if (OldNiagaraPreview && OldNiagaraPreview != PreviousNiagaraPreview)
		{
			OldNiagaraPreview->MarkAsGarbage();
		}
		if (OldCascadePreview && OldCascadePreview != PreviousCascadePreview)
		{
			OldCascadePreview->MarkAsGarbage();
		}
		
		// Restore viewport with previous preview if it exists
		if (bHadPreviousPreview && (PreviousNiagaraPreview || PreviousCascadePreview))
		{
			UpdateEditorViewport();
		}
		
		if (OutError)
		{
			*OutError = Error.IsEmpty() ? TEXT("Failed to create preview system") : Error;
		}
	}

	return bSuccess;
}

void UPreviewSystemManager::ClearPreview()
{
	// Restore original system if it exists
	if (OriginalSystem && GEditor)
	{
		UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		if (AssetEditorSubsystem)
		{
			// Close preview editor
			if (NiagaraPreview)
			{
				AssetEditorSubsystem->CloseAllEditorsForAsset(NiagaraPreview);
			}
			if (CascadePreview)
			{
				AssetEditorSubsystem->CloseAllEditorsForAsset(CascadePreview);
			}
			
			// Restore original system
			AssetEditorSubsystem->OpenEditorForAsset(OriginalSystem);
			OriginalSystem = nullptr;
		}
	}
	
	// Clean up preview systems
	if (NiagaraPreview)
	{
		NiagaraPreview->MarkAsGarbage();
		NiagaraPreview = nullptr;
	}
	if (CascadePreview)
	{
		CascadePreview->MarkAsGarbage();
		CascadePreview = nullptr;
	}
	
	CurrentPreviewDSL = FVFXDSL();
}

void UPreviewSystemManager::SetPreviewEnabled(bool bEnabled)
{
	bPreviewEnabled = bEnabled;
	if (!bEnabled)
	{
		ClearPreview();
	}
}

bool UPreviewSystemManager::IsPreviewActive() const
{
	return (NiagaraPreview != nullptr) || (CascadePreview != nullptr);
}

bool UPreviewSystemManager::SavePreviewAsPermanent(const FString& PackagePath, const FString& SystemName, UObject*& OutSystem, FString& OutError)
{
	if (!IsPreviewActive())
	{
		OutError = TEXT("No preview system to save");
		return false;
	}

	if (NiagaraPreview)
	{
		// TODO: Implement save Niagara preview as permanent
		// This would duplicate the preview system to a permanent location
		OutError = TEXT("Saving Niagara preview as permanent not yet implemented");
		return false;
	}
	else if (CascadePreview)
	{
		// TODO: Implement save Cascade preview as permanent
		OutError = TEXT("Saving Cascade preview as permanent not yet implemented");
		return false;
	}

	return false;
}

void UPreviewSystemManager::Cleanup()
{
	ClearPreview();
}

FVFXDSLDiffResult UPreviewSystemManager::GetDSLDiff(const FVFXDSL& NewDSL) const
{
	return UVFXDSLDiff::Compare(CurrentPreviewDSL, NewDSL);
}

bool UPreviewSystemManager::HasDSLChanged(const FVFXDSL& NewDSL) const
{
	// Simple comparison - check if effect type or emitter count changed
	if (CurrentPreviewDSL.Effect.Type != NewDSL.Effect.Type)
	{
		return true;
	}

	if (CurrentPreviewDSL.Emitters.Num() != NewDSL.Emitters.Num())
	{
		return true;
	}

	// TODO: More sophisticated comparison could be added here
	// For now, we'll regenerate if type or emitter count changes
	
	return false; // Assume no change if basic properties match
}

bool UPreviewSystemManager::CreateNiagaraPreview(const FVFXDSL& DSL, FString& OutError)
{
	// Validate DSL has emitters
	if (DSL.Emitters.Num() == 0)
	{
		OutError = TEXT("DSL has no emitters");
		return false;
	}
	
	// Create temporary package for preview
	FString TempPackagePath = TEXT("/Temp/AINiagaraPreview");
	FString TempSystemName = TEXT("PreviewSystem");
	FString FullPackagePath = TempPackagePath + TEXT("/") + TempSystemName;

	UPackage* TempPackage = CreatePackage(*FullPackagePath);
	if (!TempPackage)
	{
		OutError = TEXT("Failed to create temporary package for preview");
		return false;
	}

	// Mark package as temporary (don't save)
	TempPackage->SetFlags(RF_Transient);

	// Create preview system
	UNiagaraSystem* PreviewSystem = nullptr;
	FString GenerationError;
	if (!UNiagaraSystemGenerator::CreateSystemFromDSL(DSL, TempPackagePath, TempSystemName, PreviewSystem, GenerationError))
	{
		OutError = FString::Printf(TEXT("Failed to create Niagara preview system: %s"), *GenerationError);
		return false;
	}

	// Mark system as transient (don't save)
	if (PreviewSystem)
	{
		PreviewSystem->SetFlags(RF_Transient);
		NiagaraPreview = PreviewSystem;
	}
	else
	{
		OutError = TEXT("Niagara system generator returned null system");
		return false;
	}

	return PreviewSystem != nullptr;
}

bool UPreviewSystemManager::CreateCascadePreview(const FVFXDSL& DSL, FString& OutError)
{
	// Validate DSL has emitters
	if (DSL.Emitters.Num() == 0)
	{
		OutError = TEXT("DSL has no emitters");
		return false;
	}
	
	// Create temporary package for preview
	FString TempPackagePath = TEXT("/Temp/AINiagaraPreview");
	FString TempSystemName = TEXT("PreviewSystem");
	FString FullPackagePath = TempPackagePath + TEXT("/") + TempSystemName;

	UPackage* TempPackage = CreatePackage(*FullPackagePath);
	if (!TempPackage)
	{
		OutError = TEXT("Failed to create temporary package for preview");
		return false;
	}

	// Mark package as temporary (don't save)
	TempPackage->SetFlags(RF_Transient);

	// Create preview system
	UParticleSystem* PreviewSystem = nullptr;
	FString GenerationError;
	if (!UCascadeSystemGenerator::CreateSystemFromDSL(DSL, TempPackagePath, TempSystemName, PreviewSystem, GenerationError))
	{
		OutError = FString::Printf(TEXT("Failed to create Cascade preview system: %s"), *GenerationError);
		return false;
	}

	// Mark system as transient (don't save)
	if (PreviewSystem)
	{
		PreviewSystem->SetFlags(RF_Transient);
		CascadePreview = PreviewSystem;
	}
	else
	{
		OutError = TEXT("Cascade system generator returned null system");
		return false;
	}

	return PreviewSystem != nullptr;
}

void UPreviewSystemManager::CleanupOldPreview()
{
	if (NiagaraPreview)
	{
		NiagaraPreview->MarkAsGarbage();
		NiagaraPreview = nullptr;
	}

	if (CascadePreview)
	{
		CascadePreview->MarkAsGarbage();
		CascadePreview = nullptr;
	}
}

void UPreviewSystemManager::UpdateEditorViewport()
{
	if (!GEditor)
	{
		return;
	}

	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	if (!AssetEditorSubsystem)
	{
		return;
	}

	// Try to update Niagara editor viewport
	if (NiagaraPreview)
	{
		// Find currently open Niagara editor
		TArray<UObject*> AllAssets;
		AssetEditorSubsystem->GetAllEditedAssets(AllAssets);
		
		bool bFoundNiagaraEditor = false;
		for (UObject* Asset : AllAssets)
		{
			if (UNiagaraSystem* NiagaraSystem = Cast<UNiagaraSystem>(Asset))
			{
				// Found an open Niagara editor - store original if not already stored
				if (!OriginalSystem)
				{
					OriginalSystem = NiagaraSystem;
				}
				bFoundNiagaraEditor = true;
				break;
			}
		}
		
		// If we found an open Niagara editor, replace it with preview
		// Otherwise, open preview in a new editor
		if (bFoundNiagaraEditor && OriginalSystem)
		{
			// Close original and open preview in the same editor window
			AssetEditorSubsystem->CloseAllEditorsForAsset(OriginalSystem);
		}
		
		// Open preview system - will use existing editor window if available
		AssetEditorSubsystem->OpenEditorForAsset(NiagaraPreview);
	}

	// Try to update Cascade editor viewport
	if (CascadePreview)
	{
		// Find currently open Cascade editor
		TArray<UObject*> AllAssets;
		AssetEditorSubsystem->GetAllEditedAssets(AllAssets);
		
		bool bFoundCascadeEditor = false;
		for (UObject* Asset : AllAssets)
		{
			if (UParticleSystem* ParticleSystem = Cast<UParticleSystem>(Asset))
			{
				// Found an open Cascade editor - store original if not already stored
				if (!OriginalSystem)
				{
					OriginalSystem = ParticleSystem;
				}
				bFoundCascadeEditor = true;
				break;
			}
		}
		
		// If we found an open Cascade editor, replace it with preview
		if (bFoundCascadeEditor && OriginalSystem)
		{
			// Close original and open preview in the same editor window
			AssetEditorSubsystem->CloseAllEditorsForAsset(OriginalSystem);
		}
		
		// Open preview system - will use existing editor window if available
		AssetEditorSubsystem->OpenEditorForAsset(CascadePreview);
	}
}

