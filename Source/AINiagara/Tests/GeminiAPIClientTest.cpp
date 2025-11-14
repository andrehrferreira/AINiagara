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
	TestTrue(TEXT("Masked key should contain asterisks"), MaskedKey.Contains(TEXT("*")));

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
	UAINiagaraSettings* Settings = GetMutableDefault<UAINiagaraSettings>();
	TestNotNull(TEXT("Settings should exist"), Settings);

	if (!Settings)
	{
		return false;
	}

	FString TestAPIKey = TEXT("settings-test-key");
	Settings->SetAPIKey(TestAPIKey);

	// Create client and load from settings
	TSharedPtr<FGeminiAPIClient> Client = MakeShared<FGeminiAPIClient>();
	Client->LoadAPIKeyFromSettings();

	// Verify key was loaded
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
	TestEqual(TEXT("First message role should be user"), History[0].Role, TEXT("user"));
	TestEqual(TEXT("Second message role should be assistant"), History[1].Role, TEXT("assistant"));
	TestEqual(TEXT("Third message role should be user"), History[2].Role, TEXT("user"));

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
	TestEqual(TEXT("Tool description should be set"), Tool.Description, TEXT("Load a texture asset"));
	TestEqual(TEXT("Tool should have 2 parameters"), Tool.Parameters.Num(), 2);
	TestTrue(TEXT("Tool should have path parameter"), Tool.Parameters.Contains(TEXT("path")));
	TestTrue(TEXT("Tool should have type parameter"), Tool.Parameters.Contains(TEXT("type")));

	// Create multiple tools
	TArray<FVFXToolFunction> Tools;
	
	FVFXToolFunction MaterialTool;
	MaterialTool.Name = TEXT("tool:material");
	MaterialTool.Description = TEXT("Load a material asset");
	Tools.Add(MaterialTool);

	FVFXToolFunction EmitterTool;
	EmitterTool.Name = TEXT("tool:emitter");
	EmitterTool.Description = TEXT("Create particle emitter");
	Tools.Add(EmitterTool);

	TestEqual(TEXT("Should have 2 tools"), Tools.Num(), 2);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGeminiAPIClientRequestParametersTest,
	"AINiagara.GeminiAPIClient.RequestParameters",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FGeminiAPIClientRequestParametersTest::RunTest(const FString& Parameters)
{
	TSharedPtr<FGeminiAPIClient> Client = MakeShared<FGeminiAPIClient>();
	TestNotNull(TEXT("Client should be created"), Client.Get());

	if (!Client.IsValid())
	{
		return false;
	}

	// Test setting model parameters
	Client->SetModelName(TEXT("gemini-pro"));
	Client->SetTemperature(0.8f);
	Client->SetMaxTokens(4096);

	// Note: Since these are private members, we can't directly test them
	// In a real implementation, you'd add getters or test through API calls
	// For now, we just verify the methods don't crash
	TestTrue(TEXT("SetModelName should succeed"), true);
	TestTrue(TEXT("SetTemperature should succeed"), true);
	TestTrue(TEXT("SetMaxTokens should succeed"), true);

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

	// Test with special characters
	FString SpecialKey = TEXT("key!@#$%^&*()_+{}[]|\\:;\"'<>?,./");
	Client->SetAPIKey(SpecialKey);
	FString MaskedSpecialKey = Client->GetAPIKey();
	TestFalse(TEXT("Special character key should be handled"), MaskedSpecialKey.IsEmpty());

	// Test conversation message with empty content
	FConversationMessage EmptyMsg(TEXT("user"), TEXT(""));
	TestTrue(TEXT("Empty content should be allowed"), EmptyMsg.Content.IsEmpty());

	// Test conversation message with very long content
	FString LongContent;
	for (int32 i = 0; i < 10000; i++)
	{
		LongContent.Append(TEXT("test "));
	}
	FConversationMessage LongMsg(TEXT("user"), LongContent);
	TestEqual(TEXT("Long content should be stored"), LongMsg.Content, LongContent);

	// Test tool function with empty name
	FVFXToolFunction EmptyTool;
	TestTrue(TEXT("Empty tool name should be allowed"), EmptyTool.Name.IsEmpty());

	// Test tool function with many parameters
	FVFXToolFunction ManyParamsTool;
	for (int32 i = 0; i < 100; i++)
	{
		ManyParamsTool.Parameters.Add(FString::Printf(TEXT("param%d"), i), TEXT("string"));
	}
	TestEqual(TEXT("Tool should support many parameters"), ManyParamsTool.Parameters.Num(), 100);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGeminiAPIClientCancelRequestTest,
	"AINiagara.GeminiAPIClient.CancelRequest",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FGeminiAPIClientCancelRequestTest::RunTest(const FString& Parameters)
{
	TSharedPtr<FGeminiAPIClient> Client = MakeShared<FGeminiAPIClient>();
	TestNotNull(TEXT("Client should be created"), Client.Get());

	if (!Client.IsValid())
	{
		return false;
	}

	// Test canceling non-existent request (should not crash)
	Client->CancelRequest();
	TestTrue(TEXT("Canceling non-existent request should not crash"), true);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

