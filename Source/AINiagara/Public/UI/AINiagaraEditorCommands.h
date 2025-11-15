// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

/**
 * Commands for AINiagara Editor
 */
class AINIAGARA_API FAINiagaraEditorCommands : public TCommands<FAINiagaraEditorCommands>
{
public:
	FAINiagaraEditorCommands();
	
	virtual void RegisterCommands() override;
	
	/** Command to open AI Chat window */
	TSharedPtr<FUICommandInfo> OpenAIChatWindow;
};

