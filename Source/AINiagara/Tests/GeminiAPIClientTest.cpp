// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Core/GeminiAPIClient.h"
#include "Core/AINiagaraSettings.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGeminiAPIClientBasicTest,
	"AINiagara.GeminiAPIClient.BasicFunctionality",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FGeminiAPIClientBasicTest::RunTest(const FString& Parameters)
{
	// Create client instance
	TSharedPtr<FGeminiAPIClient> Client = MakeShared<FGeminiAPIClient>();
	TestNotNull(TEXT("Client should be created"), Client.Get());

	if (!Client.IsValid())
	{
		return false;
	}

	// Test setting API key
	FString TestAPIKey = TEXT("test-api-key-12345");
	Client->SetAPIKey(TestAPIKey);
	
	// Get masked API key
	FString MaskedKey = Client->GetAPIKey();
	TestFalse(TEXT("Masked key should not be empty"), MaskedKey.IsEmpty());

	// Test clearing API key
	Client->SetAPIKey(TEXT(""));
	FString EmptyMaskedKey = Client->GetAPIKey();
	TestTrue(TEXT("Empty key should return empty string"), EmptyMaskedKey.IsEmpty() || EmptyMaskedKey == TEXT("<not set>"));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGeminiAPIClientLoadFromSettingsTest,
	"AINiagara.GeminiAPIClient.LoadFromSettings",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FGeminiAPIClientLoadFromSettingsTest::RunTest(const FString& Parameters)
{
	// Set up settings with test API key
	UAINiagaraSettings* Settings = UAINiagaraSettings::Get();
	TestNotNull(TEXT("Settings should exist"), Settings);

	if (!Settings)
	{
		return false;
	}

	FString TestAPIKey = TEXT("settings-test-key");
	Settings->SetGeminiAPIKey(TestAPIKey, false);

	// Create client and load from settings
	TSharedPtr<FGeminiAPIClient> Client = MakeShared<FGeminiAPIClient>();
	Client->LoadAPIKeyFromSettings();

	// Verify key was loaded (masked)
	FString MaskedKey = Client->GetAPIKey();
	TestFalse(TEXT("Key should be loaded from settings"), MaskedKey.IsEmpty());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGeminiAPIClientConversationHistoryTest,
	"AINiagara.GeminiAPIClient.ConversationHistory",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FGeminiAPIClientConversationHistoryTest::RunTest(const FString& Parameters)
{
	// Test conversation message creation
	FConversationMessage Msg1(TEXT("user"), TEXT("Hello"));
	TestEqual(TEXT("Message role should be user"), Msg1.Role, TEXT("user"));
	TestEqual(TEXT("Message content should be Hello"), Msg1.Content, TEXT("Hello"));
	TestTrue(TEXT("Message should have timestamp"), Msg1.Timestamp != FDateTime::MinValue());

	// Test default constructor
	FConversationMessage Msg2;
	TestEqual(TEXT("Default role should be user"), Msg2.Role, TEXT("user"));
	TestTrue(TEXT("Default timestamp should be set"), Msg2.Timestamp != FDateTime::MinValue());

	// Test array of messages
	TArray<FConversationMessage> History;
	History.Add(FConversationMessage(TEXT("user"), TEXT("First message")));
	History.Add(FConversationMessage(TEXT("assistant"), TEXT("Response")));
	History.Add(FConversationMessage(TEXT("user"), TEXT("Follow-up")));

	TestEqual(TEXT("History should have 3 messages"), History.Num(), 3);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGeminiAPIClientToolFunctionTest,
	"AINiagara.GeminiAPIClient.ToolFunction",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FGeminiAPIClientToolFunctionTest::RunTest(const FString& Parameters)
{
	// Create tool function
	FVFXToolFunction Tool;
	Tool.Name = TEXT("tool:texture");
	Tool.Description = TEXT("Load a texture asset");
	Tool.Parameters.Add(TEXT("path"), TEXT("string"));
	Tool.Parameters.Add(TEXT("type"), TEXT("string"));

	TestEqual(TEXT("Tool name should be set"), Tool.Name, TEXT("tool:texture"));
	TestEqual(TEXT("Tool should have 2 parameters"), Tool.Parameters.Num(), 2);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGeminiAPIClientEdgeCasesTest,
	"AINiagara.GeminiAPIClient.EdgeCases",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FGeminiAPIClientEdgeCasesTest::RunTest(const FString& Parameters)
{
	TSharedPtr<FGeminiAPIClient> Client = MakeShared<FGeminiAPIClient>();
	
	// Test with empty strings
	Client->SetAPIKey(TEXT(""));
	FString EmptyResult = Client->GetAPIKey();
	TestTrue(TEXT("Empty API key should be handled"), EmptyResult.IsEmpty() || EmptyResult == TEXT("<not set>"));

	// Test with very long API key
	FString LongKey;
	for (int32 i = 0; i < 1000; i++)
	{
		LongKey.AppendChar(TEXT('a'));
	}
	Client->SetAPIKey(LongKey);
	FString MaskedLongKey = Client->GetAPIKey();
	TestFalse(TEXT("Long key should be masked"), MaskedLongKey.IsEmpty());

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
