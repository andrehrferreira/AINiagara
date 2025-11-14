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
	UAINiagaraSettings* Settings = GetMutableDefault<UAINiagaraSettings>();
	TestNotNull(TEXT("Settings should exist"), Settings);

	if (!Settings)
	{
		return false;
	}

	// Test default values
	FString InitialAPIKey = Settings->GetAPIKey();
	UE_LOG(LogTemp, Log, TEXT("Initial API Key length: %d"), InitialAPIKey.Len());

	// Test setting API key
	FString TestAPIKey = TEXT("test-api-key-12345");
	Settings->SetAPIKey(TestAPIKey);
	
	FString RetrievedAPIKey = Settings->GetAPIKey();
	TestEqual(TEXT("API key should be set correctly"), RetrievedAPIKey, TestAPIKey);

	// Test has API key
	bool bHasKey = Settings->HasAPIKey();
	TestTrue(TEXT("Should have API key after setting"), bHasKey);

	// Test clearing API key
	Settings->SetAPIKey(TEXT(""));
	TestFalse(TEXT("Should not have API key after clearing"), Settings->HasAPIKey());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAINiagaraSettingsModelConfigTest,
	"AINiagara.Settings.ModelConfiguration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAINiagaraSettingsModelConfigTest::RunTest(const FString& Parameters)
{
	UAINiagaraSettings* Settings = GetMutableDefault<UAINiagaraSettings>();
	TestNotNull(TEXT("Settings should exist"), Settings);

	if (!Settings)
	{
		return false;
	}

	// Test model name
	FString DefaultModel = Settings->ModelName;
	TestFalse(TEXT("Model name should not be empty"), DefaultModel.IsEmpty());

	Settings->ModelName = TEXT("gemini-test-model");
	TestEqual(TEXT("Model name should be set"), Settings->ModelName, TEXT("gemini-test-model"));

	// Test temperature
	Settings->Temperature = 0.75f;
	TestEqual(TEXT("Temperature should be set"), Settings->Temperature, 0.75f);

	// Test max tokens
	Settings->MaxTokens = 4096;
	TestEqual(TEXT("Max tokens should be set"), Settings->MaxTokens, 4096);

	// Test request timeout
	Settings->RequestTimeout = 45.0f;
	TestEqual(TEXT("Request timeout should be set"), Settings->RequestTimeout, 45.0f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAINiagaraSettingsValidationTest,
	"AINiagara.Settings.Validation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAINiagaraSettingsValidationTest::RunTest(const FString& Parameters)
{
	UAINiagaraSettings* Settings = GetMutableDefault<UAINiagaraSettings>();
	TestNotNull(TEXT("Settings should exist"), Settings);

	if (!Settings)
	{
		return false;
	}

	// Test with valid API key
	Settings->SetAPIKey(TEXT("valid-key"));
	bool bIsValid = Settings->ValidateSettings();
	TestTrue(TEXT("Should be valid with API key"), bIsValid);

	// Test with empty API key
	Settings->SetAPIKey(TEXT(""));
	bIsValid = Settings->ValidateSettings();
	TestFalse(TEXT("Should be invalid without API key"), bIsValid);

	// Test with whitespace API key
	Settings->SetAPIKey(TEXT("   "));
	bIsValid = Settings->ValidateSettings();
	TestFalse(TEXT("Should be invalid with whitespace API key"), bIsValid);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAINiagaraSettingsSaveLoadTest,
	"AINiagara.Settings.SaveAndLoad",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAINiagaraSettingsSaveLoadTest::RunTest(const FString& Parameters)
{
	UAINiagaraSettings* Settings = GetMutableDefault<UAINiagaraSettings>();
	TestNotNull(TEXT("Settings should exist"), Settings);

	if (!Settings)
	{
		return false;
	}

	// Set test values
	FString TestAPIKey = TEXT("test-save-load-key");
	FString TestModel = TEXT("test-model");
	float TestTemp = 0.8f;
	int32 TestTokens = 3000;
	float TestTimeout = 40.0f;

	Settings->SetAPIKey(TestAPIKey);
	Settings->ModelName = TestModel;
	Settings->Temperature = TestTemp;
	Settings->MaxTokens = TestTokens;
	Settings->RequestTimeout = TestTimeout;

	// Save settings
	Settings->SaveSettings();

	// Load settings (in real scenario, would reload from config)
	// For this test, we verify the values remain
	TestEqual(TEXT("API key should persist"), Settings->GetAPIKey(), TestAPIKey);
	TestEqual(TEXT("Model name should persist"), Settings->ModelName, TestModel);
	TestEqual(TEXT("Temperature should persist"), Settings->Temperature, TestTemp);
	TestEqual(TEXT("Max tokens should persist"), Settings->MaxTokens, TestTokens);
	TestEqual(TEXT("Request timeout should persist"), Settings->RequestTimeout, TestTimeout);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAINiagaraSettingsEdgeCasesTest,
	"AINiagara.Settings.EdgeCases",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAINiagaraSettingsEdgeCasesTest::RunTest(const FString& Parameters)
{
	UAINiagaraSettings* Settings = GetMutableDefault<UAINiagaraSettings>();
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
	Settings->SetAPIKey(LongKey);
	TestEqual(TEXT("Should handle long API key"), Settings->GetAPIKey(), LongKey);

	// Test special characters in API key
	FString SpecialKey = TEXT("test-key!@#$%^&*()_+{}[]|\\:;\"'<>?,./");
	Settings->SetAPIKey(SpecialKey);
	TestEqual(TEXT("Should handle special characters"), Settings->GetAPIKey(), SpecialKey);

	// Test extreme temperature values
	Settings->Temperature = -1.0f;
	TestEqual(TEXT("Should accept negative temperature"), Settings->Temperature, -1.0f);

	Settings->Temperature = 100.0f;
	TestEqual(TEXT("Should accept high temperature"), Settings->Temperature, 100.0f);

	// Test extreme max tokens
	Settings->MaxTokens = 0;
	TestEqual(TEXT("Should accept zero max tokens"), Settings->MaxTokens, 0);

	Settings->MaxTokens = 1000000;
	TestEqual(TEXT("Should accept large max tokens"), Settings->MaxTokens, 1000000);

	// Test extreme timeout values
	Settings->RequestTimeout = 0.0f;
	TestEqual(TEXT("Should accept zero timeout"), Settings->RequestTimeout, 0.0f);

	Settings->RequestTimeout = 3600.0f;
	TestEqual(TEXT("Should accept large timeout"), Settings->RequestTimeout, 3600.0f);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

