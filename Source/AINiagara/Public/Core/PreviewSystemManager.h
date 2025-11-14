// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Core/VFXDSL.h"
#include "Core/VFXDSLDiff.h"
#include "PreviewSystemManager.generated.h"

class UNiagaraSystem;
class UParticleSystem;

/**
 * Manager for real-time preview systems
 * Creates and manages temporary preview systems that update as DSL changes
 */
UCLASS()
class AINIAGARA_API UPreviewSystemManager : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Get singleton instance
	 */
	static UPreviewSystemManager* Get();

	/**
	 * Update preview system from DSL and display in editor viewport
	 * @param DSL The DSL specification to preview
	 * @param bForceUpdate Force update even if DSL hasn't changed
	 * @param OutError Output parameter - error message if update failed
	 * @return true if preview was updated successfully
	 */
	bool UpdatePreview(const FVFXDSL& DSL, bool bForceUpdate = false, FString* OutError = nullptr);

	/**
	 * Update viewport with current preview system
	 * This replaces the system being viewed in the Niagara/Cascade editor viewport
	 */
	void UpdateEditorViewport();

	/**
	 * Clear current preview
	 */
	void ClearPreview();

	/**
	 * Enable or disable preview
	 */
	void SetPreviewEnabled(bool bEnabled);

	/**
	 * Check if preview is enabled
	 */
	bool IsPreviewEnabled() const { return bPreviewEnabled; }

	/**
	 * Get current Niagara preview system
	 */
	UNiagaraSystem* GetNiagaraPreview() const { return NiagaraPreview; }

	/**
	 * Get current Cascade preview system
	 */
	UParticleSystem* GetCascadePreview() const { return CascadePreview; }

	/**
	 * Check if preview is active
	 */
	bool IsPreviewActive() const;

	/**
	 * Save current preview as permanent system
	 * @param PackagePath Package path for the new system
	 * @param SystemName Name for the new system
	 * @param OutSystem Output parameter - the saved system
	 * @param OutError Output parameter - error message if save failed
	 * @return true if save was successful
	 */
	bool SavePreviewAsPermanent(const FString& PackagePath, const FString& SystemName, UObject*& OutSystem, FString& OutError);

	/**
	 * Cleanup preview systems (called on widget close)
	 */
	void Cleanup();

	/**
	 * Get DSL diff between current preview and new DSL
	 * @param NewDSL New DSL to compare
	 * @return Diff result with all changes
	 */
	FVFXDSLDiffResult GetDSLDiff(const FVFXDSL& NewDSL) const;

private:
	/** Current Niagara preview system */
	UPROPERTY()
	UNiagaraSystem* NiagaraPreview;

	/** Current Cascade preview system */
	UPROPERTY()
	UParticleSystem* CascadePreview;

	/** Original system being viewed (to restore when preview is disabled) */
	UPROPERTY()
	UObject* OriginalSystem;

	/** Whether preview is enabled */
	bool bPreviewEnabled = true;

	/** Current preview DSL (for change detection) */
	FVFXDSL CurrentPreviewDSL;

	/** Last update time (for throttling) */
	double LastUpdateTime = 0.0;

	/** Minimum time between updates (in seconds) */
	static constexpr double MinUpdateInterval = 0.5;

	/**
	 * Check if DSL has changed
	 */
	bool HasDSLChanged(const FVFXDSL& NewDSL) const;

	/**
	 * Create temporary Niagara preview system
	 */
	bool CreateNiagaraPreview(const FVFXDSL& DSL, FString& OutError);

	/**
	 * Create temporary Cascade preview system
	 */
	bool CreateCascadePreview(const FVFXDSL& DSL, FString& OutError);

	/**
	 * Clean up old preview system
	 */
	void CleanupOldPreview();
};

