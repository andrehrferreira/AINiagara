// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/AINiagaraEditorCommands.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "AINiagaraEditorCommands"

FAINiagaraEditorCommands::FAINiagaraEditorCommands()
	: TCommands<FAINiagaraEditorCommands>(
		TEXT("AINiagaraEditor"),
		NSLOCTEXT("AINiagaraEditor", "AINiagaraEditorCommands", "AINiagara Editor Commands"),
		NAME_None,
		FAppStyle::GetAppStyleSetName()
	)
{
}

void FAINiagaraEditorCommands::RegisterCommands()
{
	UI_COMMAND(
		OpenAIChatWindow,
		"AI Chat",
		"Open AI Chat interface for generating VFX",
		EUserInterfaceActionType::Button,
		FInputChord(EModifierKey::Control | EModifierKey::Shift, EKeys::C)
	);
}

#undef LOCTEXT_NAMESPACE

