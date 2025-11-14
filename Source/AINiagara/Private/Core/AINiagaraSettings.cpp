// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/AINiagaraSettings.h"
#include "Misc/ConfigCacheIni.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"

const FString UAINiagaraSettings::ConfigSectionName = TEXT("AINiagara");
const FString UAINiagaraSettings::ConfigFileName = TEXT("EditorPerProjectUserSettings");

UAINiagaraSettings::UAINiagaraSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Load existing config
	LoadConfig(nullptr, *ConfigFileName);
}

UAINiagaraSettings* UAINiagaraSettings::Get()
{
	static UAINiagaraSettings* SettingsInstance = nullptr;
	
	if (!SettingsInstance)
	{
		SettingsInstance = NewObject<UAINiagaraSettings>();
		SettingsInstance->AddToRoot(); // Prevent garbage collection
	}
	
	return SettingsInstance;
}

void UAINiagaraSettings::SetGeminiAPIKey(const FString& InAPIKey, bool bSaveConfig)
{
	// Trim whitespace
	FString TrimmedKey = InAPIKey.TrimStartAndEnd();
	
	// Basic validation: API keys are typically not empty and have minimum length
	if (TrimmedKey.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("AINiagara: Attempted to set empty API key"));
		return;
	}
	
	GeminiAPIKey = TrimmedKey;
	
	if (bSaveConfig)
	{
		SaveConfig();
	}
}

void UAINiagaraSettings::ClearAPIKey()
{
	GeminiAPIKey.Empty();
	SaveConfig();
}

FString UAINiagaraSettings::GetMaskedAPIKey() const
{
	if (GeminiAPIKey.IsEmpty())
	{
		return FString();
	}
	
	// Mask all but last 4 characters for security
	if (GeminiAPIKey.Len() > 4)
	{
		FString Masked = FString::ChrN(GeminiAPIKey.Len() - 4, TEXT('*'));
		Masked += GeminiAPIKey.Right(4);
		return Masked;
	}
	
	return TEXT("****");
}

void UAINiagaraSettings::SaveConfig()
{
	// Save to EditorPerProjectUserSettings config file
	Super::SaveConfig(CPF_Config, *ConfigFileName);
}

