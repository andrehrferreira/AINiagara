// Copyright Epic Games, Inc. All Rights Reserved.

#include "AINiagaraEditorModeToolkit.h"
#include "AINiagaraEditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "AINiagaraEditorModeToolkit"

FAINiagaraEditorModeToolkit::FAINiagaraEditorModeToolkit()
{
}

void FAINiagaraEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FAINiagaraEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}


FName FAINiagaraEditorModeToolkit::GetToolkitFName() const
{
	return FName("AINiagaraEditorMode");
}

FText FAINiagaraEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "AINiagaraEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
