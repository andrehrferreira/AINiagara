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

#endif // WITH_DEV_AUTOMATION_TESTS

