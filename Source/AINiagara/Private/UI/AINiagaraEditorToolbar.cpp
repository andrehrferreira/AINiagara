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

TSharedPtr<FUICommandList> FAINiagaraEditorToolbar::CommandList = nullptr;

void FAINiagaraEditorToolbar::Initialize()
{
	// Create command list
	CommandList = MakeShareable(new FUICommandList);

	// Register toolbar buttons
	RegisterNiagaraToolbarButton();
	RegisterCascadeToolbarButton();
}

void FAINiagaraEditorToolbar::Shutdown()
{
	CommandList.Reset();
}

void FAINiagaraEditorToolbar::RegisterNiagaraToolbarButton()
{
	UToolMenus* ToolMenus = UToolMenus::Get();
	if (!ToolMenus)
	{
		return;
	}

	// Register extension for Niagara editor toolbar
	// This will be called when Niagara editor is opened
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateLambda([]()
	{
		UToolMenus* ToolMenus = UToolMenus::Get();
		if (!ToolMenus)
		{
			return;
		}

		// Try to extend Niagara editor toolbar
		// Menu names may vary by Unreal Engine version
		FName NiagaraToolbarMenus[] = {
			"NiagaraEditor.NiagaraEditorToolbar",
			"NiagaraEditor.Toolbar",
			"NiagaraEditor.MainToolbar"
		};

		for (const FName& MenuName : NiagaraToolbarMenus)
		{
			UToolMenu* ToolbarMenu = ToolMenus->ExtendMenu(MenuName);
			if (ToolbarMenu)
			{
				FToolMenuSection& Section = ToolbarMenu->AddSection("AINiagara", NSLOCTEXT("AINiagara", "AINiagaraSection", "AINiagara"));
				
				Section.AddEntry(FToolMenuEntry::InitToolBarButton(
					"OpenAIChat",
					FUIAction(
						FExecuteAction::CreateStatic(&FAINiagaraEditorToolbar::OnOpenAIChat),
						FCanExecuteAction::CreateStatic(&FAINiagaraEditorToolbar::IsAPIKeyConfigured)
					),
					NSLOCTEXT("AINiagara", "OpenAIChat", "AI Chat"),
					NSLOCTEXT("AINiagara", "OpenAIChatTooltip", "Open AI Chat interface for generating VFX"),
					FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Message")
				));
				break; // Found and added, exit loop
			}
		}
	}));
}

void FAINiagaraEditorToolbar::RegisterCascadeToolbarButton()
{
	UToolMenus* ToolMenus = UToolMenus::Get();
	if (!ToolMenus)
	{
		return;
	}

	// Register extension for Cascade editor toolbar
	// This will be called when Cascade editor is opened
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateLambda([]()
	{
		UToolMenus* ToolMenus = UToolMenus::Get();
		if (!ToolMenus)
		{
			return;
		}

		// Try to extend Cascade editor toolbar
		// Menu names may vary by Unreal Engine version
		FName CascadeToolbarMenus[] = {
			"CascadeEditor.CascadeEditorToolbar",
			"CascadeEditor.Toolbar",
			"CascadeEditor.MainToolbar"
		};

		for (const FName& MenuName : CascadeToolbarMenus)
		{
			UToolMenu* ToolbarMenu = ToolMenus->ExtendMenu(MenuName);
			if (ToolbarMenu)
			{
				FToolMenuSection& Section = ToolbarMenu->AddSection("AINiagara", NSLOCTEXT("AINiagara", "AINiagaraSection", "AINiagara"));
				
				Section.AddEntry(FToolMenuEntry::InitToolBarButton(
					"OpenAIChat",
					FUIAction(
						FExecuteAction::CreateStatic(&FAINiagaraEditorToolbar::OnOpenAIChat),
						FCanExecuteAction::CreateStatic(&FAINiagaraEditorToolbar::IsAPIKeyConfigured)
					),
					NSLOCTEXT("AINiagara", "OpenAIChat", "AI Chat"),
					NSLOCTEXT("AINiagara", "OpenAIChatTooltip", "Open AI Chat interface for generating VFX"),
					FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Message")
				));
				break; // Found and added, exit loop
			}
		}
	}));
}

void FAINiagaraEditorToolbar::OnOpenAIChat()
{
	// Open the chat window (it will check API key internally)
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

