// Copyright Epic Games, Inc. All Rights Reserved.

#include "AINiagaraModule.h"
#include "AINiagaraEditorModeCommands.h"
#include "UI/AINiagaraEditorToolbar.h"
#include "UI/AINiagaraEditorCommands.h"
#include "UI/AINiagaraChatWindow.h"
#include "UI/Widgets/SAINiagaraChatWidget.h"
#include "Core/AINiagaraSettings.h"
#include "Core/AINiagaraLogMonitor.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Docking/TabManager.h"
#include "Interfaces/IMainFrameModule.h"
#include "LevelEditor.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/AppStyle.h"
#include "Framework/MultiBox/MultiBoxExtender.h"

#define LOCTEXT_NAMESPACE "AINiagaraModule"

// Tab ID for the chat window
const FName FAINiagaraModule::ChatWindowTabId = FName(TEXT("AINiagaraChatWindow"));

void FAINiagaraModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Register editor mode commands
	FAINiagaraEditorModeCommands::Register();
	
	// Register editor commands (for chat window)
	FAINiagaraEditorCommands::Register();
	
	// Initialize log monitor for debugging
	FAINiagaraLogMonitor::Get().Initialize();
	
	// Register OnPostEngineInit delegate - this ensures menus are registered after engine is fully initialized
	OnPostEngineInitDelegateHandle = FCoreDelegates::OnPostEngineInit.AddRaw(this, &FAINiagaraModule::OnPostEngineInit);
	
	// Register Tab Spawner - this creates the tab in the editor
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		ChatWindowTabId,
		FOnSpawnTab::CreateRaw(this, &FAINiagaraModule::SpawnChatWindow),
		FCanSpawnTab::CreateLambda([this](const FSpawnTabArgs& Args) -> bool { return CanSpawnChatWindow(); })
	)
	.SetMenuType(ETabSpawnerMenuType::Hidden)
	.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Message"));
	
	UE_LOG(LogTemp, Log, TEXT("AINiagara: Module started, Tab Spawner registered"));
}

void FAINiagaraModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Unregister Tab Spawner
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ChatWindowTabId);
	
	// Remove OnPostEngineInit delegate
	if (OnPostEngineInitDelegateHandle.IsValid())
	{
		FCoreDelegates::OnPostEngineInit.Remove(OnPostEngineInitDelegateHandle);
		OnPostEngineInitDelegateHandle.Reset();
	}

	// Shutdown toolbar extensions
	FAINiagaraEditorToolbar::Shutdown();
	
	// Shutdown log monitor
	FAINiagaraLogMonitor::Get().Shutdown();
	
	// Unregister commands
	FAINiagaraEditorCommands::Unregister();
	FAINiagaraEditorModeCommands::Unregister();
}

void FAINiagaraModule::AddReferencedObjects(FReferenceCollector& Collector)
{
	// No objects to reference currently
}

#if (ENGINE_MAJOR_VERSION == 5)
FString FAINiagaraModule::GetReferencerName() const
{
	return TEXT("AINiagaraModuleGCObject");
}
#endif

void FAINiagaraModule::OnPostEngineInit()
{
	// This function is for registering UICommand to the engine, so it can be executed via keyboard shortcut.
	// This will also add this UICommand to the menu, so it can also be executed from there.
	
	// This function is valid only if no Commandlet or game is running. It also requires Slate Application to be initialized.
	if ((IsRunningCommandlet() == false) && (IsRunningGame() == false) && FSlateApplication::IsInitialized())
	{
		if (FLevelEditorModule* LevelEditor = FModuleManager::LoadModulePtr<FLevelEditorModule>(TEXT("LevelEditor")))
		{
			// Create a UICommandList and map editor spawning function to the UICommand of opening Chat Window.
			TSharedPtr<FUICommandList> Commands = MakeShareable(new FUICommandList());
			Commands->MapAction(
				FAINiagaraEditorCommands::Get().OpenAIChatWindow,
				FExecuteAction::CreateRaw(this, &FAINiagaraModule::InvokeChatWindowSpawn),
				FCanExecuteAction::CreateRaw(this, &FAINiagaraModule::CanSpawnChatWindow),
				FIsActionChecked::CreateRaw(this, &FAINiagaraModule::IsChatWindowSpawned)
			);

			// Register this UICommandList to the MainFrame.
			// Otherwise nothing will handle the input to trigger this command.
			IMainFrameModule& MainFrame = FModuleManager::Get().LoadModuleChecked<IMainFrameModule>("MainFrame");
			MainFrame.GetMainFrameCommandBindings()->Append(Commands.ToSharedRef());

#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 4))
			// UE5.4+ uses ToolMenus for menu entries
			UToolMenu* Menu = UToolMenus::Get()->FindMenu("LevelEditor.MainMenu.Window");
			if (Menu)
			{
				FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
				Section.AddMenuEntry(
					"OpenAIChat",
					NSLOCTEXT("AINiagara", "OpenAIChat", "AI Chat"),
					NSLOCTEXT("AINiagara", "OpenAIChatTooltip", "Open AI Chat interface for generating VFX"),
					FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Message"),
					FUIAction(
						FExecuteAction::CreateRaw(this, &FAINiagaraModule::InvokeChatWindowSpawn),
						FCanExecuteAction::CreateRaw(this, &FAINiagaraModule::CanSpawnChatWindow)
					)
				);
				UE_LOG(LogTemp, Log, TEXT("AINiagara: Successfully added AI Chat to Window menu (UE5.4+)"));
			}
#else
			// Create a Menu Extender, which adds a button that executes the UICommandList of opening Chat Window.
			TSharedPtr<FExtender> MainMenuExtender = MakeShareable(new FExtender);
			MainMenuExtender->AddMenuExtension(
#if (ENGINE_MAJOR_VERSION == 5)
				FName(TEXT("Window")),
#else
				FName(TEXT("General")),
#endif
				EExtensionHook::After, 
				Commands,
				FMenuExtensionDelegate::CreateLambda([](FMenuBuilder& MenuBuilder)
				{
					MenuBuilder.AddMenuEntry(
						FAINiagaraEditorCommands::Get().OpenAIChatWindow,
						NAME_None,
						NSLOCTEXT("AINiagara", "OpenAIChat", "AI Chat"),
						NSLOCTEXT("AINiagara", "OpenAIChatTooltip", "Open AI Chat interface for generating VFX"),
						FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Message")
					);
				})
			);

			// Extend Editors menu with the created Menu Extender.
			LevelEditor->GetMenuExtensibilityManager()->AddExtender(MainMenuExtender);
#endif
			
			UE_LOG(LogTemp, Log, TEXT("AINiagara: OnPostEngineInit completed, menu registered"));
		}
	}
}

bool FAINiagaraModule::CanSpawnChatWindow()
{
	// Chat window can always be spawned
	return true;
}

TSharedRef<SDockTab> FAINiagaraModule::SpawnChatWindow(const FSpawnTabArgs& Args)
{
	// Spawn the Chat Window only when we can.
	if (CanSpawnChatWindow())
	{
		// Create the chat widget directly (workaround for PCH cache issue)
		// TODO: Get current asset path from active editor (Niagara or Cascade)
		FString CurrentAssetPath = TEXT("");
		TSharedRef<SWidget> ChatWidget = SNew(SAINiagaraChatWidget)
			.AssetPath(CurrentAssetPath);
		
		// Spawn new DockTab and fill it with newly created chat widget.
		TSharedRef<SDockTab> NewTab = SAssignNew(ChatWindowTab, SDockTab)
			.TabRole(ETabRole::NomadTab)
			.Label(NSLOCTEXT("AINiagara", "ChatWindowTitle", "AI Chat"))
			[
				ChatWidget
			];

		// Return the DockTab to the Global Tab Manager.
		return NewTab;
	}

	// If chat window can't be spawned - create an empty tab.
	return SAssignNew(ChatWindowTab, SDockTab).TabRole(ETabRole::NomadTab);
}

bool FAINiagaraModule::IsChatWindowSpawned()
{
	// Checks if the chat window tab is already existing in the editor
	return FGlobalTabmanager::Get()->FindExistingLiveTab(ChatWindowTabId).IsValid();
}

void FAINiagaraModule::InvokeChatWindowSpawn()
{
	// Check if API key is configured
	if (UAINiagaraSettings* Settings = UAINiagaraSettings::Get())
	{
		if (!Settings->IsAPIKeyConfigured())
		{
			// Show API key configuration dialog first
			// After API key is configured, open the chat window tab
			FAINiagaraChatWindow::ShowAPIKeyDialog(
				FSimpleDelegate::CreateRaw(this, &FAINiagaraModule::InvokeChatWindowSpawn)
			);
			return;
		}
	}
	
	// Tries to invoke opening the chat window tab
	FGlobalTabmanager::Get()->TryInvokeTab(ChatWindowTabId);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAINiagaraModule, AINiagara)