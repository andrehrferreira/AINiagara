// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "ToolMenus.h"
#include "Modules/ModuleInterface.h"

/**
 * Toolbar extension for Niagara and Cascade editors
 */
class AINIAGARA_API FAINiagaraEditorToolbar
{
public:
	/**
	 * Initialize toolbar extensions
	 * @param InModuleInstance The module instance to use as menu owner
	 */
	static void Initialize(IModuleInterface* InModuleInstance);

	/**
	 * Shutdown toolbar extensions
	 */
	static void Shutdown();

	/**
	 * Callback for opening AI chat window
	 */
	static void OnOpenAIChat();

	/**
	 * Register menu extensions (called by startup callback)
	 */
	static void RegisterMenuExtensions();

private:
	/**
	 * Register menus (called by RegisterMenuExtensions)
	 */
	static void RegisterMenus();
	/**
	 * Register toolbar button for Niagara editor
	 */
	static void RegisterNiagaraToolbarButton();

	/**
	 * Register toolbar button for Cascade editor
	 */
	static void RegisterCascadeToolbarButton();

	/**
	 * Check if API key is configured
	 */
	static bool IsAPIKeyConfigured();

	/** Command list for toolbar actions */
	static TSharedPtr<FUICommandList> CommandList;
	
	/** Module instance for menu ownership */
	static IModuleInterface* ModuleInstance;
};

