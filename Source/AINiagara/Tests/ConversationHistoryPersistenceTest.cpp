// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Core/ConversationHistoryManager.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"

/**
 * Test automatic persistence enable/disable
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FConversationHistoryAutoPersistenceTest,
	"AINiagara.ConversationHistory.AutoPersistence",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FConversationHistoryAutoPersistenceTest::RunTest(const FString& Parameters)
{
	UConversationHistoryManager* Manager = UConversationHistoryManager::Get();
	TestNotNull(TEXT("Manager should be valid"), Manager);

	// Test default is enabled
	TestTrue(TEXT("Auto-persistence should be enabled by default"), Manager->IsAutoPersistenceEnabled());

	// Test disable
	Manager->SetAutoPersistence(false);
	TestFalse(TEXT("Auto-persistence should be disabled"), Manager->IsAutoPersistenceEnabled());

	// Test re-enable
	Manager->SetAutoPersistence(true);
	TestTrue(TEXT("Auto-persistence should be re-enabled"), Manager->IsAutoPersistenceEnabled());

	return true;
}

/**
 * Test automatic save when adding messages with auto-persistence enabled
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FConversationHistoryAutoSaveTest,
	"AINiagara.ConversationHistory.AutoSave",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FConversationHistoryAutoSaveTest::RunTest(const FString& Parameters)
{
	UConversationHistoryManager* Manager = UConversationHistoryManager::Get();
	TestNotNull(TEXT("Manager should be valid"), Manager);

	FString TestAssetPath = TEXT("/Game/Test/AutoSaveTest");
	
	// Clear any existing history
	Manager->ClearHistory(TestAssetPath);
	
	// Ensure auto-persistence is enabled
	Manager->SetAutoPersistence(true);

	// Add a message
	Manager->AddMessage(TestAssetPath, TEXT("user"), TEXT("Test auto-save message"));

	// Give it a moment to save
	FPlatformProcess::Sleep(0.1f);

	// Check if file was created
	FString HistoryFilePath = FPaths::ProjectSavedDir() / TEXT("AINiagara") / TEXT("History") / TEXT("Game_Test_AutoSaveTest.json");
	TestTrue(TEXT("History file should exist after adding message"), FPaths::FileExists(HistoryFilePath));

	// Load and verify
	FString JsonString;
	if (FFileHelper::LoadFileToString(JsonString, *HistoryFilePath))
	{
		TestTrue(TEXT("JSON content should contain the message"), JsonString.Contains(TEXT("Test auto-save message")));
	}

	// Cleanup
	Manager->ClearHistory(TestAssetPath);

	return true;
}

/**
 * Test that auto-save does not occur when disabled
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FConversationHistoryAutoSaveDisabledTest,
	"AINiagara.ConversationHistory.AutoSaveDisabled",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FConversationHistoryAutoSaveDisabledTest::RunTest(const FString& Parameters)
{
	UConversationHistoryManager* Manager = UConversationHistoryManager::Get();
	TestNotNull(TEXT("Manager should be valid"), Manager);

	FString TestAssetPath = TEXT("/Game/Test/AutoSaveDisabledTest");
	
	// Clear any existing history
	Manager->ClearHistory(TestAssetPath);
	
	// Disable auto-persistence
	Manager->SetAutoPersistence(false);

	// Add a message
	Manager->AddMessage(TestAssetPath, TEXT("user"), TEXT("This should not auto-save"));

	// Give it a moment
	FPlatformProcess::Sleep(0.1f);

	// Check if file was NOT created
	FString HistoryFilePath = FPaths::ProjectSavedDir() / TEXT("AINiagara") / TEXT("History") / TEXT("Game_Test_AutoSaveDisabledTest.json");
	TestFalse(TEXT("History file should NOT exist when auto-save is disabled"), FPaths::FileExists(HistoryFilePath));

	// Re-enable auto-persistence
	Manager->SetAutoPersistence(true);
	
	// Cleanup
	Manager->ClearHistory(TestAssetPath);

	return true;
}

/**
 * Test persistence across manager "sessions"
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FConversationHistoryPersistenceSessionTest,
	"AINiagara.ConversationHistory.PersistenceSession",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FConversationHistoryPersistenceSessionTest::RunTest(const FString& Parameters)
{
	UConversationHistoryManager* Manager = UConversationHistoryManager::Get();
	TestNotNull(TEXT("Manager should be valid"), Manager);

	FString TestAssetPath = TEXT("/Game/Test/PersistenceSessionTest");
	
	// Clear any existing history
	Manager->ClearHistory(TestAssetPath);
	
	// Enable auto-persistence
	Manager->SetAutoPersistence(true);

	// Add messages
	Manager->AddMessage(TestAssetPath, TEXT("user"), TEXT("Message 1"));
	Manager->AddMessage(TestAssetPath, TEXT("assistant"), TEXT("Response 1"));
	Manager->AddMessage(TestAssetPath, TEXT("user"), TEXT("Message 2"));

	// Save explicitly
	TestTrue(TEXT("Save should succeed"), Manager->SaveHistory(TestAssetPath));

	// Clear in-memory history (simulate session end)
	Manager->ClearHistory(TestAssetPath);
	TestEqual(TEXT("In-memory history should be empty"), Manager->GetHistoryCount(TestAssetPath), 0);

	// Load from disk (simulate new session)
	TestTrue(TEXT("Load should succeed"), Manager->LoadHistory(TestAssetPath));
	TestEqual(TEXT("Loaded history should have 3 messages"), Manager->GetHistoryCount(TestAssetPath), 3);

	// Verify message content
	TArray<FConversationMessage> History = Manager->GetHistory(TestAssetPath);
	if (History.Num() >= 3)
	{
		TestEqual(TEXT("First message role"), History[0].Role, TEXT("user"));
		TestEqual(TEXT("First message content"), History[0].Content, TEXT("Message 1"));
		TestEqual(TEXT("Second message role"), History[1].Role, TEXT("assistant"));
		TestEqual(TEXT("Third message role"), History[2].Role, TEXT("user"));
	}

	// Cleanup
	Manager->ClearHistory(TestAssetPath);

	return true;
}

/**
 * Test independent tracking for multiple assets
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FConversationHistoryIndependentTrackingTest,
	"AINiagara.ConversationHistory.IndependentTracking",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FConversationHistoryIndependentTrackingTest::RunTest(const FString& Parameters)
{
	UConversationHistoryManager* Manager = UConversationHistoryManager::Get();
	TestNotNull(TEXT("Manager should be valid"), Manager);

	FString Asset1 = TEXT("/Game/Test/Asset1");
	FString Asset2 = TEXT("/Game/Test/Asset2");
	FString Asset3 = TEXT("/Game/Test/Asset3");
	
	// Clear any existing history
	Manager->ClearHistory(Asset1);
	Manager->ClearHistory(Asset2);
	Manager->ClearHistory(Asset3);
	
	// Enable auto-persistence
	Manager->SetAutoPersistence(true);

	// Add different messages to each asset
	Manager->AddMessage(Asset1, TEXT("user"), TEXT("Asset 1 message"));
	Manager->AddMessage(Asset2, TEXT("user"), TEXT("Asset 2 message"));
	Manager->AddMessage(Asset2, TEXT("assistant"), TEXT("Asset 2 response"));
	Manager->AddMessage(Asset3, TEXT("user"), TEXT("Asset 3 message"));
	Manager->AddMessage(Asset3, TEXT("assistant"), TEXT("Asset 3 response 1"));
	Manager->AddMessage(Asset3, TEXT("user"), TEXT("Asset 3 message 2"));

	// Verify counts
	TestEqual(TEXT("Asset 1 should have 1 message"), Manager->GetHistoryCount(Asset1), 1);
	TestEqual(TEXT("Asset 2 should have 2 messages"), Manager->GetHistoryCount(Asset2), 2);
	TestEqual(TEXT("Asset 3 should have 3 messages"), Manager->GetHistoryCount(Asset3), 3);

	// Verify content independence
	TArray<FConversationMessage> History1 = Manager->GetHistory(Asset1);
	TArray<FConversationMessage> History2 = Manager->GetHistory(Asset2);
	TArray<FConversationMessage> History3 = Manager->GetHistory(Asset3);

	TestTrue(TEXT("Asset 1 history should contain its message"), History1[0].Content.Contains(TEXT("Asset 1")));
	TestTrue(TEXT("Asset 2 history should contain its message"), History2[0].Content.Contains(TEXT("Asset 2")));
	TestTrue(TEXT("Asset 3 history should contain its messages"), History3[0].Content.Contains(TEXT("Asset 3")));

	// Save all
	Manager->SaveAllHistory();

	// Clear in-memory
	Manager->ClearHistory(Asset1);
	Manager->ClearHistory(Asset2);
	Manager->ClearHistory(Asset3);

	// Load back and verify independence maintained
	Manager->LoadHistory(Asset1);
	Manager->LoadHistory(Asset2);
	Manager->LoadHistory(Asset3);

	TestEqual(TEXT("Asset 1 loaded count"), Manager->GetHistoryCount(Asset1), 1);
	TestEqual(TEXT("Asset 2 loaded count"), Manager->GetHistoryCount(Asset2), 2);
	TestEqual(TEXT("Asset 3 loaded count"), Manager->GetHistoryCount(Asset3), 3);

	// Cleanup
	Manager->ClearHistory(Asset1);
	Manager->ClearHistory(Asset2);
	Manager->ClearHistory(Asset3);

	return true;
}

/**
 * Test persistence file format and structure
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FConversationHistoryFileFormatTest,
	"AINiagara.ConversationHistory.FileFormat",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)
bool FConversationHistoryFileFormatTest::RunTest(const FString& Parameters)
{
	UConversationHistoryManager* Manager = UConversationHistoryManager::Get();
	TestNotNull(TEXT("Manager should be valid"), Manager);

	FString TestAssetPath = TEXT("/Game/Test/FileFormatTest");
	
	// Clear any existing history
	Manager->ClearHistory(TestAssetPath);
	
	// Enable auto-persistence
	Manager->SetAutoPersistence(true);

	// Add messages with known content
	Manager->AddMessage(TestAssetPath, TEXT("user"), TEXT("Test user message"));
	Manager->AddMessage(TestAssetPath, TEXT("assistant"), TEXT("Test assistant response"));

	// Save
	Manager->SaveHistory(TestAssetPath);

	// Read file directly
	FString HistoryFilePath = FPaths::ProjectSavedDir() / TEXT("AINiagara") / TEXT("History") / TEXT("Game_Test_FileFormatTest.json");
	FString JsonContent;
	
	if (FFileHelper::LoadFileToString(JsonContent, *HistoryFilePath))
	{
		// Verify JSON structure
		TestTrue(TEXT("JSON should contain 'messages' array"), JsonContent.Contains(TEXT("\"messages\"")));
		TestTrue(TEXT("JSON should contain 'assetPath'"), JsonContent.Contains(TEXT("\"assetPath\"")));
		TestTrue(TEXT("JSON should contain 'role' field"), JsonContent.Contains(TEXT("\"role\"")));
		TestTrue(TEXT("JSON should contain 'content' field"), JsonContent.Contains(TEXT("\"content\"")));
		TestTrue(TEXT("JSON should contain 'timestamp' field"), JsonContent.Contains(TEXT("\"timestamp\"")));
		
		// Verify content
		TestTrue(TEXT("JSON should contain user message"), JsonContent.Contains(TEXT("Test user message")));
		TestTrue(TEXT("JSON should contain assistant response"), JsonContent.Contains(TEXT("Test assistant response")));
		TestTrue(TEXT("JSON should contain user role"), JsonContent.Contains(TEXT("\"user\"")));
		TestTrue(TEXT("JSON should contain assistant role"), JsonContent.Contains(TEXT("\"assistant\"")));
	}
	else
	{
		AddError(TEXT("Failed to read history file"));
	}

	// Cleanup
	Manager->ClearHistory(TestAssetPath);

	return true;
}

