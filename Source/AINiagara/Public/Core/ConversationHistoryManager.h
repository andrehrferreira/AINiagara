// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Core/GeminiAPIClient.h"
#include "ConversationHistoryManager.generated.h"

/**
 * Manager for conversation history per asset
 */
UCLASS(BlueprintType)
class AINIAGARA_API UConversationHistoryManager : public UObject
{
	GENERATED_BODY()

public:
	UConversationHistoryManager(const FObjectInitializer& ObjectInitializer);

	/**
	 * Get the singleton instance
	 */
	static UConversationHistoryManager* Get();

	/**
	 * Get conversation history for a specific asset
	 * @param AssetPath Path to the asset (Niagara/Cascade system)
	 * @return Array of conversation messages
	 */
	TArray<FConversationMessage> GetHistory(const FString& AssetPath) const;

	/**
	 * Add a message to the conversation history
	 * @param AssetPath Path to the asset
	 * @param Role Message role ("user" or "assistant")
	 * @param Content Message content
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|History")
	void AddMessage(const FString& AssetPath, const FString& Role, const FString& Content);

	/**
	 * Clear conversation history for an asset
	 * @param AssetPath Path to the asset
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|History")
	void ClearHistory(const FString& AssetPath);

	/**
	 * Clear all conversation history
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|History")
	void ClearAllHistory();

	/**
	 * Load conversation history from disk for an asset
	 * @param AssetPath Path to the asset
	 * @return True if history was loaded successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|History")
	bool LoadHistory(const FString& AssetPath);

	/**
	 * Save conversation history to disk for an asset
	 * @param AssetPath Path to the asset
	 * @return True if history was saved successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|History")
	bool SaveHistory(const FString& AssetPath);

	/**
	 * Save all conversation history to disk
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|History")
	void SaveAllHistory();

	/**
	 * Get the number of messages in history for an asset
	 * @param AssetPath Path to the asset
	 * @return Number of messages
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|History")
	int32 GetHistoryCount(const FString& AssetPath) const;

	/**
	 * Check if asset has conversation history
	 * @param AssetPath Path to the asset
	 * @return True if history exists
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|History")
	bool HasHistory(const FString& AssetPath) const;

	/**
	 * Register asset event hooks for automatic history save/load
	 */
	void RegisterAssetEventHooks();

	/**
	 * Unregister asset event hooks
	 */
	void UnregisterAssetEventHooks();

	/**
	 * Enable or disable automatic history persistence
	 * @param bEnable True to enable automatic save/load
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|History")
	void SetAutoPersistence(bool bEnable);

	/**
	 * Check if automatic persistence is enabled
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara|History")
	bool IsAutoPersistenceEnabled() const { return bAutoPersistenceEnabled; }

private:
	/** Map of asset paths to conversation histories */
	TMap<FString, TArray<FConversationMessage>> ConversationHistories;

	/** Whether automatic persistence is enabled */
	bool bAutoPersistenceEnabled = true;

	/** Delegate handles for asset events */
	FDelegateHandle OnAssetSavedHandle;
	FDelegateHandle OnAssetOpenedHandle;

	/**
	 * Called when an asset is saved
	 */
	void OnAssetSaved(const FString& PackageName, UObject* Asset);

	/**
	 * Called when a package is saved (includes all assets in package)
	 */
	void OnPackageSaved(const FString& PackageFilename, UPackage* Package, FObjectPostSaveContext ObjectSaveContext);

	/**
	 * Called when an asset is opened in editor
	 */
	void OnAssetOpened(UObject* Asset);

	/**
	 * Get the file path for storing history
	 * @param AssetPath Path to the asset
	 * @return File path for history storage
	 */
	FString GetHistoryFilePath(const FString& AssetPath) const;

	/**
	 * Convert asset path to a safe filename
	 * @param AssetPath Path to the asset
	 * @return Safe filename
	 */
	FString AssetPathToFilename(const FString& AssetPath) const;
};

