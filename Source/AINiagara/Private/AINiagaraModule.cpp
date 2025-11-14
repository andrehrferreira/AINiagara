// Copyright Epic Games, Inc. All Rights Reserved.

#include "AINiagaraModule.h"
#include "AINiagaraEditorModeCommands.h"
#include "UI/AINiagaraEditorToolbar.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "AINiagaraModule"

void FAINiagaraModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FAINiagaraEditorModeCommands::Register();
	
	// Initialize toolbar extensions
	FAINiagaraEditorToolbar::Initialize();
}

void FAINiagaraModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Shutdown toolbar extensions
	FAINiagaraEditorToolbar::Shutdown();
	
	FAINiagaraEditorModeCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAINiagaraModule, AINiagara)