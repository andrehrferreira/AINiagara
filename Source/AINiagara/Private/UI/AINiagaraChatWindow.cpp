// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/AINiagaraChatWindow.h"
#include "UI/Widgets/SAINiagaraChatWidget.h"
#include "UI/Widgets/SAINiagaraAPIKeyDialog.h"
#include "Core/AINiagaraSettings.h"
#include "Core/PreviewSystemManager.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "Framework/Docking/TabManager.h"
#include "Styling/AppStyle.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Text.h"

TSharedPtr<SWindow> FAINiagaraChatWindow::ChatWindow = nullptr;

void FAINiagaraChatWindow::OpenChatWindow()
{
	// Check if API key is configured
	if (UAINiagaraSettings* Settings = UAINiagaraSettings::Get())
	{
		if (!Settings->IsAPIKeyConfigured())
		{
			// Show API key configuration dialog first
			ShowAPIKeyDialog();
			return;
		}
	}

	// If window already exists, bring it to front
	if (ChatWindow.IsValid())
	{
		ChatWindow->BringToFront();
		return;
	}

	// Create new window
	const FVector2D WindowSize(600.0f, 800.0f);
	const FVector2D WindowPosition = FSlateApplication::Get().GetCursorPos();

	ChatWindow = SNew(SWindow)
		.Title(NSLOCTEXT("AINiagara", "AINiagaraChatWindowTitle", "AINiagara - AI Chat"))
		.ClientSize(WindowSize)
		.ScreenPosition(WindowPosition)
		.SizingRule(ESizingRule::UserSized)
		.AutoCenter(EAutoCenter::None)
		.SupportsMinimize(true)
		.SupportsMaximize(true)
		[
			CreateChatWidget()
		];

	// Add to Slate application
	FSlateApplication::Get().AddWindow(ChatWindow.ToSharedRef(), true);
}

void FAINiagaraChatWindow::ShowAPIKeyDialog(FSimpleDelegate OnConfiguredCallback)
{
	const FVector2D DialogSize(550.0f, 300.0f);
	const FVector2D DialogPosition = FSlateApplication::Get().GetCursorPos();

	// Use provided callback if available, otherwise default to opening chat window
	FSimpleDelegate FinalCallback = OnConfiguredCallback.IsBound() 
		? OnConfiguredCallback 
		: FSimpleDelegate::CreateLambda([]()
		{
			// After API key is configured, open chat window (default behavior)
			FAINiagaraChatWindow::OpenChatWindow();
		});

	// Create dialog widget first
	TSharedRef<SAINiagaraAPIKeyDialog> DialogWidget = SNew(SAINiagaraAPIKeyDialog)
		.OnAPIKeyConfigured(FinalCallback);

	// Create window with close callback to invalidate widget
	TSharedRef<SWindow> DialogWindow = SNew(SWindow)
		.Title(NSLOCTEXT("AINiagara", "APIKeyDialogWindowTitle", "AINiagara - API Key Configuration"))
		.ClientSize(DialogSize)
		.ScreenPosition(DialogPosition)
		.SizingRule(ESizingRule::FixedSize)
		.AutoCenter(EAutoCenter::None)
		.SupportsMinimize(false)
		.SupportsMaximize(false)
		.OnWindowClosed_Lambda([DialogWidget](const TSharedRef<SWindow>&)
		{
			// Invalidate widget when window is closed
			DialogWidget->Invalidate();
		})
		[
			DialogWidget
		];

	FSlateApplication::Get().AddModalWindow(DialogWindow, nullptr);
}

void FAINiagaraChatWindow::CloseChatWindow()
{
	if (ChatWindow.IsValid())
	{
		// Cleanup preview when closing chat window
		if (UPreviewSystemManager* PreviewManager = UPreviewSystemManager::Get())
		{
			PreviewManager->Cleanup();
		}
		
		ChatWindow->RequestDestroyWindow();
		ChatWindow.Reset();
	}
}

bool FAINiagaraChatWindow::IsChatWindowOpen()
{
	return ChatWindow.IsValid() && ChatWindow->IsVisible();
}

TSharedRef<SWidget> FAINiagaraChatWindow::CreateChatWidget()
{
	// TODO: Get current asset path from active editor (Niagara or Cascade)
	// For now, pass empty string - widget will try to detect it or use default
	FString CurrentAssetPath = TEXT("");
	
	// Check if API key is configured - if not, the widget will handle showing the dialog
	// For tab-based usage, we create the widget anyway and let it handle the API key check
	return SNew(SAINiagaraChatWidget)
		.AssetPath(CurrentAssetPath);
}

