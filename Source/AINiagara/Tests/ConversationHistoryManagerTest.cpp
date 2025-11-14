// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Core/ConversationHistoryManager.h"
#include "Core/GeminiAPIClient.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FConversationHistoryManagerTest,
	"AINiagara.ConversationHistoryManager.AddAndGetMessages",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FConversationHistoryManagerTest::RunTest(const FString& Parameters)
{
	// Get history manager instance
	UConversationHistoryManager* HistoryManager = UConversationHistoryManager::Get();
	TestNotNull(TEXT("History manager should be created"), HistoryManager);

	if (!HistoryManager)
	{
		return false;
	}

	// Test asset path
	FString TestAssetPath = TEXT("/Game/Test/TestAsset");

	// Clear any existing history
	HistoryManager->ClearHistory(TestAssetPath);

	// Add messages
	HistoryManager->AddMessage(TestAssetPath, TEXT("user"), TEXT("Hello, AI!"));
	HistoryManager->AddMessage(TestAssetPath, TEXT("assistant"), TEXT("Hello! How can I help you?"));

	// Get history
	TArray<FConversationMessage> History = HistoryManager->GetHistory(TestAssetPath);
	TestEqual(TEXT("Should have 2 messages"), History.Num(), 2);

	if (History.Num() >= 2)
	{
		TestEqual(TEXT("First message role should be user"), History[0].Role, TEXT("user"));
		TestEqual(TEXT("First message content should match"), History[0].Content, TEXT("Hello, AI!"));
		TestEqual(TEXT("Second message role should be assistant"), History[1].Role, TEXT("assistant"));
		TestEqual(TEXT("Second message content should match"), History[1].Content, TEXT("Hello! How can I help you?"));
	}

	// Test history count
	int32 HistoryCount = HistoryManager->GetHistoryCount(TestAssetPath);
	TestEqual(TEXT("History count should be 2"), HistoryCount, 2);

	// Test has history
	bool bHasHistory = HistoryManager->HasHistory(TestAssetPath);
	TestTrue(TEXT("Should have history"), bHasHistory);

	// Clean up
	HistoryManager->ClearHistory(TestAssetPath);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FConversationHistoryManagerSaveLoadTest,
	"AINiagara.ConversationHistoryManager.SaveAndLoadHistory",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FConversationHistoryManagerSaveLoadTest::RunTest(const FString& Parameters)
{
	UConversationHistoryManager* HistoryManager = UConversationHistoryManager::Get();
	TestNotNull(TEXT("History manager should be created"), HistoryManager);

	if (!HistoryManager)
	{
		return false;
	}

	FString TestAssetPath = TEXT("/Game/Test/TestAssetSaveLoad");

	// Clear any existing history
	HistoryManager->ClearHistory(TestAssetPath);

	// Add messages
	HistoryManager->AddMessage(TestAssetPath, TEXT("user"), TEXT("Test message 1"));
	HistoryManager->AddMessage(TestAssetPath, TEXT("assistant"), TEXT("Test response 1"));

	// Save history
	bool bSaveSuccess = HistoryManager->SaveHistory(TestAssetPath);
	TestTrue(TEXT("Save history should succeed"), bSaveSuccess);

	// Clear in-memory history
	HistoryManager->ClearHistory(TestAssetPath);
	TestEqual(TEXT("History should be cleared"), HistoryManager->GetHistoryCount(TestAssetPath), 0);

	// Load history
	bool bLoadSuccess = HistoryManager->LoadHistory(TestAssetPath);
	TestTrue(TEXT("Load history should succeed"), bLoadSuccess);

	// Verify loaded history
	TArray<FConversationMessage> LoadedHistory = HistoryManager->GetHistory(TestAssetPath);
	TestEqual(TEXT("Loaded history should have 2 messages"), LoadedHistory.Num(), 2);

	if (LoadedHistory.Num() >= 2)
	{
		TestEqual(TEXT("First loaded message should match"), LoadedHistory[0].Content, TEXT("Test message 1"));
		TestEqual(TEXT("Second loaded message should match"), LoadedHistory[1].Content, TEXT("Test response 1"));
	}

	// Clean up
	HistoryManager->ClearHistory(TestAssetPath);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FConversationHistoryManagerClearTest,
	"AINiagara.ConversationHistoryManager.ClearHistory",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FConversationHistoryManagerClearTest::RunTest(const FString& Parameters)
{
	UConversationHistoryManager* HistoryManager = UConversationHistoryManager::Get();
	TestNotNull(TEXT("History manager should be created"), HistoryManager);

	if (!HistoryManager)
	{
		return false;
	}

	FString TestAssetPath = TEXT("/Game/Test/TestAssetClear");

	// Add multiple messages
	HistoryManager->AddMessage(TestAssetPath, TEXT("user"), TEXT("Message 1"));
	HistoryManager->AddMessage(TestAssetPath, TEXT("assistant"), TEXT("Response 1"));
	HistoryManager->AddMessage(TestAssetPath, TEXT("user"), TEXT("Message 2"));

	TestEqual(TEXT("Should have 3 messages"), HistoryManager->GetHistoryCount(TestAssetPath), 3);

	// Clear history
	HistoryManager->ClearHistory(TestAssetPath);
	
	TestEqual(TEXT("History should be cleared"), HistoryManager->GetHistoryCount(TestAssetPath), 0);
	TestFalse(TEXT("Should not have history"), HistoryManager->HasHistory(TestAssetPath));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FConversationHistoryManagerMultipleAssetsTest,
	"AINiagara.ConversationHistoryManager.MultipleAssets",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FConversationHistoryManagerMultipleAssetsTest::RunTest(const FString& Parameters)
{
	UConversationHistoryManager* HistoryManager = UConversationHistoryManager::Get();
	TestNotNull(TEXT("History manager should be created"), HistoryManager);

	if (!HistoryManager)
	{
		return false;
	}

	FString Asset1 = TEXT("/Game/Test/Asset1");
	FString Asset2 = TEXT("/Game/Test/Asset2");
	FString Asset3 = TEXT("/Game/Test/Asset3");

	// Clear existing
	HistoryManager->ClearHistory(Asset1);
	HistoryManager->ClearHistory(Asset2);
	HistoryManager->ClearHistory(Asset3);

	// Add messages to different assets
	HistoryManager->AddMessage(Asset1, TEXT("user"), TEXT("Asset1 Message 1"));
	HistoryManager->AddMessage(Asset1, TEXT("assistant"), TEXT("Asset1 Response 1"));
	
	HistoryManager->AddMessage(Asset2, TEXT("user"), TEXT("Asset2 Message 1"));
	HistoryManager->AddMessage(Asset2, TEXT("assistant"), TEXT("Asset2 Response 1"));
	HistoryManager->AddMessage(Asset2, TEXT("user"), TEXT("Asset2 Message 2"));
	
	HistoryManager->AddMessage(Asset3, TEXT("user"), TEXT("Asset3 Message 1"));

	// Verify each asset has correct history
	TestEqual(TEXT("Asset1 should have 2 messages"), HistoryManager->GetHistoryCount(Asset1), 2);
	TestEqual(TEXT("Asset2 should have 3 messages"), HistoryManager->GetHistoryCount(Asset2), 3);
	TestEqual(TEXT("Asset3 should have 1 message"), HistoryManager->GetHistoryCount(Asset3), 1);

	// Verify histories are independent
	TArray<FConversationMessage> Asset1History = HistoryManager->GetHistory(Asset1);
	TestEqual(TEXT("Asset1 first message should match"), Asset1History[0].Content, TEXT("Asset1 Message 1"));

	// Clean up
	HistoryManager->ClearHistory(Asset1);
	HistoryManager->ClearHistory(Asset2);
	HistoryManager->ClearHistory(Asset3);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FConversationHistoryManagerEmptyPathTest,
	"AINiagara.ConversationHistoryManager.EmptyPath",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FConversationHistoryManagerEmptyPathTest::RunTest(const FString& Parameters)
{
	UConversationHistoryManager* HistoryManager = UConversationHistoryManager::Get();
	TestNotNull(TEXT("History manager should be created"), HistoryManager);

	if (!HistoryManager)
	{
		return false;
	}

	FString EmptyPath = TEXT("");

	// Test with empty path
	HistoryManager->AddMessage(EmptyPath, TEXT("user"), TEXT("Test message"));
	
	// Should handle empty path gracefully
	int32 Count = HistoryManager->GetHistoryCount(EmptyPath);
	TestTrue(TEXT("Should handle empty path"), Count >= 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FConversationHistoryManagerLargeHistoryTest,
	"AINiagara.ConversationHistoryManager.LargeHistory",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FConversationHistoryManagerLargeHistoryTest::RunTest(const FString& Parameters)
{
	UConversationHistoryManager* HistoryManager = UConversationHistoryManager::Get();
	TestNotNull(TEXT("History manager should be created"), HistoryManager);

	if (!HistoryManager)
	{
		return false;
	}

	FString TestAssetPath = TEXT("/Game/Test/LargeHistory");
	HistoryManager->ClearHistory(TestAssetPath);

	// Add many messages
	for (int32 i = 0; i < 100; i++)
	{
		FString UserMsg = FString::Printf(TEXT("User message %d"), i);
		FString AssistantMsg = FString::Printf(TEXT("Assistant response %d"), i);
		
		HistoryManager->AddMessage(TestAssetPath, TEXT("user"), UserMsg);
		HistoryManager->AddMessage(TestAssetPath, TEXT("assistant"), AssistantMsg);
	}

	// Verify count
	TestEqual(TEXT("Should have 200 messages"), HistoryManager->GetHistoryCount(TestAssetPath), 200);

	// Verify retrieval
	TArray<FConversationMessage> History = HistoryManager->GetHistory(TestAssetPath);
	TestEqual(TEXT("Retrieved history should have 200 messages"), History.Num(), 200);

	// Verify first and last messages
	if (History.Num() >= 2)
	{
		TestEqual(TEXT("First message should match"), History[0].Content, TEXT("User message 0"));
		TestEqual(TEXT("Last message should match"), History[199].Content, TEXT("Assistant response 99"));
	}

	// Clean up
	HistoryManager->ClearHistory(TestAssetPath);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FConversationHistoryManagerSpecialCharactersTest,
	"AINiagara.ConversationHistoryManager.SpecialCharacters",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FConversationHistoryManagerSpecialCharactersTest::RunTest(const FString& Parameters)
{
	UConversationHistoryManager* HistoryManager = UConversationHistoryManager::Get();
	TestNotNull(TEXT("History manager should be created"), HistoryManager);

	if (!HistoryManager)
	{
		return false;
	}

	FString TestAssetPath = TEXT("/Game/Test/SpecialChars");
	HistoryManager->ClearHistory(TestAssetPath);

	// Test with special characters
	FString SpecialMsg = TEXT("Test with \"quotes\", 'apostrophes', {braces}, [brackets], and symbols: !@#$%^&*()");
	HistoryManager->AddMessage(TestAssetPath, TEXT("user"), SpecialMsg);

	TArray<FConversationMessage> History = HistoryManager->GetHistory(TestAssetPath);
	TestEqual(TEXT("Should handle special characters"), History[0].Content, SpecialMsg);

	// Test with Unicode
	FString UnicodeMsg = TEXT("Unicode test: 你好 🔥 ™©®");
	HistoryManager->AddMessage(TestAssetPath, TEXT("assistant"), UnicodeMsg);

	History = HistoryManager->GetHistory(TestAssetPath);
	TestEqual(TEXT("Should handle Unicode"), History[1].Content, UnicodeMsg);

	// Clean up
	HistoryManager->ClearHistory(TestAssetPath);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

