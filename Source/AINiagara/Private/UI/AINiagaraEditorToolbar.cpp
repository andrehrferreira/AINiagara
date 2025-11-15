// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/AINiagaraEditorToolbar.h"
#include "Core/AINiagaraSettings.h"
#include "UI/AINiagaraChatWindow.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ToolMenus.h"
#include "Styling/AppStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Text.h"
#include "Misc/MessageDialog.h"
#include "Logging/LogMacros.h"
#include "HAL/IConsoleManager.h"
#include "LevelEditor.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Modules/ModuleManager.h"

TSharedPtr<FUICommandList> FAINiagaraEditorToolbar::CommandList = nullptr;
IModuleInterface* FAINiagaraEditorToolbar::ModuleInstance = nullptr;

void FAINiagaraEditorToolbar::Initialize(IModuleInterface* InModuleInstance)
{
	// Store module instance for menu ownership
	ModuleInstance = InModuleInstance;

	// Create command list
	CommandList = MakeShareable(new FUICommandList);

	// Log initialization
	UE_LOG(LogTemp, Log, TEXT("AINiagara: Toolbar Initialize called"));

	// Register console command first - this always works
	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("AINiagara.OpenChat"),
		TEXT("Open AINiagara Chat Window"),
		FConsoleCommandDelegate::CreateStatic(&FAINiagaraEditorToolbar::OnOpenAIChat),
		ECVF_Default
	);

	UE_LOG(LogTemp, Log, TEXT("AINiagara: Console command registered"));

	// Register menu extensions using ToolMenus system (following minifloppy.it tutorial pattern)
	RegisterNiagaraToolbarButton();
	RegisterCascadeToolbarButton();
	
	UE_LOG(LogTemp, Log, TEXT("AINiagara: Toolbar registration complete"));
}

void FAINiagaraEditorToolbar::RegisterMenuExtensions()
{
	UE_LOG(LogTemp, Log, TEXT("AINiagara: RegisterMenuExtensions called"));
	
	if (!ModuleInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("AINiagara: ModuleInstance is null"));
		return;
	}

	// Use the module instance as the owner of the menus
	// This allows us to remove all our custom menus when the module is unloaded
	FToolMenuOwnerScoped OwnerScoped(ModuleInstance);
	
	UE_LOG(LogTemp, Log, TEXT("AINiagara: OwnerScoped created, calling RegisterMenus"));
	
	RegisterMenus();
}

void FAINiagaraEditorToolbar::RegisterMenus()
{
	if (!ModuleInstance)
	{
		return;
	}

	UToolMenus* ToolMenus = UToolMenus::Get();
	if (!ToolMenus)
	{
		UE_LOG(LogTemp, Error, TEXT("AINiagara: ToolMenus is null"));
		return;
	}

	// Note: Window menu is now registered by the module using OnPostEngineInit
	// This function only registers toolbar buttons for Niagara editor

	// Extend Niagara editor toolbar - try multiple possible menu names
	FName NiagaraToolbarMenus[] = {
		"NiagaraEditor.NiagaraSystemEditor.ToolBar",
		"AssetEditor.NiagaraEditor.ToolBar",
		"NiagaraEditor.ToolBar",
		"NiagaraSystemEditor.ToolBar",
		"AssetEditor.NiagaraSystemEditor.ToolBar"
	};

	bool bRegistered = false;
	for (const FName& MenuName : NiagaraToolbarMenus)
	{
		UToolMenu* ToolbarMenu = ToolMenus->ExtendMenu(MenuName);
		if (ToolbarMenu)
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("AINiagara");
			
			Section.AddEntry(FToolMenuEntry::InitToolBarButton(
				"OpenAIChat",
				FUIAction(
					FExecuteAction::CreateStatic(&FAINiagaraEditorToolbar::OnOpenAIChat)
				),
				INVTEXT("AI Chat"),
				INVTEXT("Open AI Chat interface for generating VFX"),
				FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Message")
			));
			bRegistered = true;
			UE_LOG(LogTemp, Log, TEXT("AINiagara: Successfully registered toolbar button to menu: %s"), *MenuName.ToString());
			break;
		}
		else
		{
			UE_LOG(LogTemp, Verbose, TEXT("AINiagara: Menu '%s' not found, trying next..."), *MenuName.ToString());
		}
	}

	if (!bRegistered)
	{
		UE_LOG(LogTemp, Warning, TEXT("AINiagara: Could not find Niagara editor toolbar menu. Tried: NiagaraEditor.NiagaraSystemEditor.ToolBar, AssetEditor.NiagaraEditor.ToolBar, etc. Use Window > AI Chat or console command 'AINiagara.OpenChat' instead."));
		UE_LOG(LogTemp, Warning, TEXT("AINiagara: To find the correct menu name, use 'ToolMenus.Edit' command in the editor console."));
	}
}

void FAINiagaraEditorToolbar::Shutdown()
{
	if (ModuleInstance)
	{
		// Unregister the startup function
		UToolMenus::UnRegisterStartupCallback(ModuleInstance);

		// Unregister all our menu extensions
		UToolMenus::UnregisterOwner(ModuleInstance);
	}

	CommandList.Reset();
	ModuleInstance = nullptr;
}

void FAINiagaraEditorToolbar::RegisterNiagaraToolbarButton()
{
	UE_LOG(LogTemp, Log, TEXT("AINiagara: RegisterNiagaraToolbarButton called"));
	
	// Register startup callback following the official pattern
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateStatic(
		&FAINiagaraEditorToolbar::RegisterMenuExtensions));
	
	UE_LOG(LogTemp, Log, TEXT("AINiagara: Startup callback registered"));
}

void FAINiagaraEditorToolbar::RegisterCascadeToolbarButton()
{
	// Cascade toolbar registration is handled in RegisterMenuExtensions
	// No separate registration needed
}

void FAINiagaraEditorToolbar::OnOpenAIChat()
{
	// This is called from the toolbar button or console command
	// For toolbar buttons, we use the tab system (module handles it)
	// For console commands, we can still use the window system
	// Try to use the module's tab system first, fallback to window if needed
	
	// Check if we can use the module's tab system
	// Since the module handles everything through Commands, we'll use the window system
	// as a fallback for console commands and toolbar buttons
	FAINiagaraChatWindow::OpenChatWindow();
}

bool FAINiagaraEditorToolbar::IsAPIKeyConfigured()
{
	if (UAINiagaraSettings* Settings = UAINiagaraSettings::Get())
	{
		return Settings->IsAPIKeyConfigured();
	}
	return false;
}

