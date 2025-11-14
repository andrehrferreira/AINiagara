// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "UI/Widgets/SAINiagaraChatWidget.h"
#include "Core/ConversationHistoryManager.h"
#include "Core/AINiagaraSettings.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"

#if WITH_DEV_AUTOMATION_TESTS

/**
 * Test chat widget construction
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSAINiagaraChatWidgetConstructionTest,
	"AINiagara.UI.ChatWidget.Construction",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FSAINiagaraChatWidgetConstructionTest::RunTest(const FString& Parameters)
{
	// Ensure Slate is initialized
	if (!FSlateApplication::IsInitialized())
	{
		AddError(TEXT("Slate application not initialized"));
		return false;
	}

	// Create widget
	TSharedRef<SAINiagaraChatWidget> ChatWidget = SNew(SAINiagaraChatWidget)
		.AssetPath(TEXT("/Game/Test/TestAsset"));

	TestNotNull(TEXT("Chat widget should be created"), ChatWidget.Get());

	// Widget should be valid
	if (!ChatWidget.IsValid())
	{
		return false;
	}

	return true;
}

/**
 * Test chat widget with empty asset path
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSAINiagaraChatWidgetEmptyAssetPathTest,
	"AINiagara.UI.ChatWidget.EmptyAssetPath",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FSAINiagaraChatWidgetEmptyAssetPathTest::RunTest(const FString& Parameters)
{
	if (!FSlateApplication::IsInitialized())
	{
		AddError(TEXT("Slate application not initialized"));
		return false;
	}

	// Create widget with empty asset path
	TSharedRef<SAINiagaraChatWidget> ChatWidget = SNew(SAINiagaraChatWidget)
		.AssetPath(FString());

	TestNotNull(TEXT("Chat widget should be created even with empty asset path"), ChatWidget.Get());

	return true;
}

/**
 * Test message history loading
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSAINiagaraChatWidgetHistoryLoadingTest,
	"AINiagara.UI.ChatWidget.HistoryLoading",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FSAINiagaraChatWidgetHistoryLoadingTest::RunTest(const FString& Parameters)
{
	if (!FSlateApplication::IsInitialized())
	{
		AddError(TEXT("Slate application not initialized"));
		return false;
	}

	// Setup test asset path
	FString TestAssetPath = TEXT("/Game/Test/TestAssetHistory");

	// Clear any existing history
	if (UConversationHistoryManager* HistoryManager = UConversationHistoryManager::Get())
	{
		HistoryManager->ClearHistory(TestAssetPath);

		// Add some test messages
		HistoryManager->AddMessage(TestAssetPath, TEXT("user"), TEXT("Test message 1"));
		HistoryManager->AddMessage(TestAssetPath, TEXT("assistant"), TEXT("Test response 1"));
		HistoryManager->AddMessage(TestAssetPath, TEXT("user"), TEXT("Test message 2"));

		// Verify history was added
		TArray<FConversationMessage> History = HistoryManager->GetHistory(TestAssetPath);
		TestEqual(TEXT("Should have 3 messages in history"), History.Num(), 3);
	}

	// Create widget with asset path
	TSharedRef<SAINiagaraChatWidget> ChatWidget = SNew(SAINiagaraChatWidget)
		.AssetPath(TestAssetPath);

	TestNotNull(TEXT("Chat widget should be created"), ChatWidget.Get());

	// Clean up
	if (UConversationHistoryManager* HistoryManager = UConversationHistoryManager::Get())
	{
		HistoryManager->ClearHistory(TestAssetPath);
	}

	return true;
}

/**
 * Test widget with multiple asset paths
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSAINiagaraChatWidgetMultipleAssetsTest,
	"AINiagara.UI.ChatWidget.MultipleAssets",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FSAINiagaraChatWidgetMultipleAssetsTest::RunTest(const FString& Parameters)
{
	if (!FSlateApplication::IsInitialized())
	{
		AddError(TEXT("Slate application not initialized"));
		return false;
	}

	// Create multiple widgets with different asset paths
	FString AssetPath1 = TEXT("/Game/Test/Asset1");
	FString AssetPath2 = TEXT("/Game/Test/Asset2");

	TSharedRef<SAINiagaraChatWidget> Widget1 = SNew(SAINiagaraChatWidget)
		.AssetPath(AssetPath1);

	TSharedRef<SAINiagaraChatWidget> Widget2 = SNew(SAINiagaraChatWidget)
		.AssetPath(AssetPath2);

	TestNotNull(TEXT("First widget should be created"), Widget1.Get());
	TestNotNull(TEXT("Second widget should be created"), Widget2.Get());

	// Add different messages to each asset's history
	if (UConversationHistoryManager* HistoryManager = UConversationHistoryManager::Get())
	{
		HistoryManager->ClearHistory(AssetPath1);
		HistoryManager->ClearHistory(AssetPath2);

		HistoryManager->AddMessage(AssetPath1, TEXT("user"), TEXT("Message for Asset 1"));
		HistoryManager->AddMessage(AssetPath2, TEXT("user"), TEXT("Message for Asset 2"));

		// Verify histories are independent
		TArray<FConversationMessage> History1 = HistoryManager->GetHistory(AssetPath1);
		TArray<FConversationMessage> History2 = HistoryManager->GetHistory(AssetPath2);

		TestEqual(TEXT("Asset 1 should have 1 message"), History1.Num(), 1);
		TestEqual(TEXT("Asset 2 should have 1 message"), History2.Num(), 1);
		TestEqual(TEXT("Asset 1 message should be different"), History1[0].Content, TEXT("Message for Asset 1"));
		TestEqual(TEXT("Asset 2 message should be different"), History2[0].Content, TEXT("Message for Asset 2"));

		// Clean up
		HistoryManager->ClearHistory(AssetPath1);
		HistoryManager->ClearHistory(AssetPath2);
	}

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

