// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Engine/Engine.h"
#include "AINiagaraSettings.generated.h"

/**
 * Settings class for AINiagara plugin
 * Stores configuration including API keys
 */
UCLASS(Config = EditorPerProjectUserSettings)
class AINIAGARA_API UAINiagaraSettings : public UObject
{
	GENERATED_BODY()

public:
	UAINiagaraSettings(const FObjectInitializer& ObjectInitializer);

	/**
	 * Get the singleton instance of settings
	 * @return Settings instance
	 */
	static UAINiagaraSettings* Get();

	/**
	 * Get the Gemini API key
	 * @return API key (may be empty if not configured)
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara")
	FString GetGeminiAPIKey() const { return GeminiAPIKey; }

	/**
	 * Set the Gemini API key
	 * @param InAPIKey The API key to set
	 * @param bSaveConfig Whether to save the configuration immediately
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara")
	void SetGeminiAPIKey(const FString& InAPIKey, bool bSaveConfig = true);

	/**
	 * Check if API key is configured
	 * @return True if API key is set and not empty
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara")
	bool IsAPIKeyConfigured() const { return !GeminiAPIKey.IsEmpty(); }

	/**
	 * Clear the API key
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara")
	void ClearAPIKey();

	/**
	 * Get masked API key for display (shows only last 4 characters)
	 * @return Masked API key string
	 */
	UFUNCTION(BlueprintCallable, Category = "AINiagara")
	FString GetMaskedAPIKey() const;

	/**
	 * Save the current configuration
	 */
	void SaveConfig();

private:
	/** Gemini API key - stored in EditorPerProjectUserSettings config */
	UPROPERTY(Config)
	FString GeminiAPIKey;

	/** Config file name */
	static const FString ConfigSectionName;
	static const FString ConfigFileName;
};

