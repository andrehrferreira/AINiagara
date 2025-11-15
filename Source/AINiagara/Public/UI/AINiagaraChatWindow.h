// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Docking/TabManager.h"
#include "Widgets/SWindow.h"
#include "Delegates/Delegate.h"

class SWidget;

/**
 * Chat window for AI VFX generation
 */
class AINIAGARA_API FAINiagaraChatWindow
{
public:
	/**
	 * Create the chat window widget
	 * This is used by the module to create the widget for the tab
	 */
	static TSharedRef<SWidget> CreateChatWidget();

	/**
	 * Open the chat window
	 */
	static void OpenChatWindow();

	/**
	 * Close the chat window
	 */
	static void CloseChatWindow();

	/**
	 * Check if chat window is open
	 */
	static bool IsChatWindowOpen();

	/**
	 * Show API key configuration dialog
	 * @param OnConfiguredCallback Optional callback to execute after API key is configured (if null, opens chat window)
	 */
	static void ShowAPIKeyDialog(FSimpleDelegate OnConfiguredCallback = FSimpleDelegate());

private:
	/** Reference to the chat window */
	static TSharedPtr<SWindow> ChatWindow;
};
