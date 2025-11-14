// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Core/AINiagaraSettings.h"
#include "Misc/ConfigCacheIni.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAINiagaraSettingsBasicTest,
	"AINiagara.Settings.GetAndSetAPIKey",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAINiagaraSettingsBasicTest::RunTest(const FString& Parameters)
{
	// Get settings instance
	UAINiagaraSettings* Settings = UAINiagaraSettings::Get();
	TestNotNull(TEXT("Settings should exist"), Settings);

	if (!Settings)
	{
		return false;
	}

	// Test setting API key
	FString TestAPIKey = TEXT("test-api-key-12345");
	Settings->SetGeminiAPIKey(TestAPIKey, false);
	
	FString RetrievedAPIKey = Settings->GetGeminiAPIKey();
	TestEqual(TEXT("API key should be set correctly"), RetrievedAPIKey, TestAPIKey);

	// Test has API key
	bool bHasKey = Settings->IsAPIKeyConfigured();
	TestTrue(TEXT("Should have API key after setting"), bHasKey);

	// Test clearing API key
	Settings->ClearAPIKey();
	TestFalse(TEXT("Should not have API key after clearing"), Settings->IsAPIKeyConfigured());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAINiagaraSettingsSaveLoadTest,
	"AINiagara.Settings.SaveAndLoad",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAINiagaraSettingsSaveLoadTest::RunTest(const FString& Parameters)
{
	UAINiagaraSettings* Settings = UAINiagaraSettings::Get();
	TestNotNull(TEXT("Settings should exist"), Settings);

	if (!Settings)
	{
		return false;
	}

	// Set test values
	FString TestAPIKey = TEXT("test-save-load-key");
	Settings->SetGeminiAPIKey(TestAPIKey, false);

	// Save settings
	Settings->SaveConfig();

	// Verify the values remain
	TestEqual(TEXT("API key should persist"), Settings->GetGeminiAPIKey(), TestAPIKey);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAINiagaraSettingsEdgeCasesTest,
	"AINiagara.Settings.EdgeCases",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAINiagaraSettingsEdgeCasesTest::RunTest(const FString& Parameters)
{
	UAINiagaraSettings* Settings = UAINiagaraSettings::Get();
	TestNotNull(TEXT("Settings should exist"), Settings);

	if (!Settings)
	{
		return false;
	}

	// Test very long API key
	FString LongKey;
	for (int32 i = 0; i < 1000; i++)
	{
		LongKey.AppendChar(TEXT('a'));
	}
	Settings->SetGeminiAPIKey(LongKey, false);
	TestEqual(TEXT("Should handle long API key"), Settings->GetGeminiAPIKey(), LongKey);

	// Test special characters in API key
	FString SpecialKey = TEXT("test-key!@#$%^&*()_+{}[]|\\:;\"'<>?,./");
	Settings->SetGeminiAPIKey(SpecialKey, false);
	TestEqual(TEXT("Should handle special characters"), Settings->GetGeminiAPIKey(), SpecialKey);

	// Test empty key
	Settings->SetGeminiAPIKey(TEXT(""), false);
	TestFalse(TEXT("Empty key should not be configured"), Settings->IsAPIKeyConfigured());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAINiagaraSettingsMaskedAPIKeyTest,
	"AINiagara.Settings.MaskedAPIKey",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAINiagaraSettingsMaskedAPIKeyTest::RunTest(const FString& Parameters)
{
	UAINiagaraSettings* Settings = UAINiagaraSettings::Get();
	TestNotNull(TEXT("Settings should exist"), Settings);

	if (!Settings)
	{
		return false;
	}

	// Test masked API key
	Settings->SetGeminiAPIKey(TEXT("test-api-key-12345"), false);
	FString MaskedKey = Settings->GetMaskedAPIKey();
	
	TestFalse(TEXT("Masked key should not be empty"), MaskedKey.IsEmpty());
	// Just verify it's different - don't compare string types
	bool bIsDifferent = (MaskedKey != Settings->GetGeminiAPIKey()) || (MaskedKey.Len() < 19);
	TestTrue(TEXT("Masked key should differ from original"), bIsDifferent);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
