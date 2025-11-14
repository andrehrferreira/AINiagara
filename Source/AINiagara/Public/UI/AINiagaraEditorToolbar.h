// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "ToolMenus.h"

/**
 * Toolbar extension for Niagara and Cascade editors
 */
class AINIAGARA_API FAINiagaraEditorToolbar
{
public:
	/**
	 * Initialize toolbar extensions
	 */
	static void Initialize();

	/**
	 * Shutdown toolbar extensions
	 */
	static void Shutdown();

private:
	/**
	 * Register toolbar button for Niagara editor
	 */
	static void RegisterNiagaraToolbarButton();

	/**
	 * Register toolbar button for Cascade editor
	 */
	static void RegisterCascadeToolbarButton();

	/**
	 * Callback for opening AI chat window
	 */
	static void OnOpenAIChat();

	/**
	 * Check if API key is configured
	 */
	static bool IsAPIKeyConfigured();

	/** Command list for toolbar actions */
	static TSharedPtr<FUICommandList> CommandList;
};

