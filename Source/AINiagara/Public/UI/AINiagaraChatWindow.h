// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Docking/TabManager.h"
#include "Widgets/SWindow.h"

class SWidget;

/**
 * Chat window for AI VFX generation
 */
class AINIAGARA_API FAINiagaraChatWindow
{
public:
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
	 */
	static void ShowAPIKeyDialog();

private:
	/**
	 * Create the chat window widget
	 */
	static TSharedRef<SWidget> CreateChatWidget();

	/** Reference to the chat window */
	static TSharedPtr<SWindow> ChatWindow;
};

