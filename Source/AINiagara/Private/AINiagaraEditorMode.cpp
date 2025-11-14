// Copyright Epic Games, Inc. All Rights Reserved.

#include "AINiagaraEditorMode.h"
#include "AINiagaraEditorModeToolkit.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "AINiagaraEditorModeCommands.h"
#include "Modules/ModuleManager.h"


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
// AddYourTool Step 1 - include the header file for your Tools here
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
#include "Tools/AINiagaraSimpleTool.h"
#include "Tools/AINiagaraInteractiveTool.h"

// step 2: register a ToolBuilder in FAINiagaraEditorMode::Enter() below


#define LOCTEXT_NAMESPACE "AINiagaraEditorMode"

const FEditorModeID UAINiagaraEditorMode::EM_AINiagaraEditorModeId = TEXT("EM_AINiagaraEditorMode");

FString UAINiagaraEditorMode::SimpleToolName = TEXT("AINiagara_ActorInfoTool");
FString UAINiagaraEditorMode::InteractiveToolName = TEXT("AINiagara_MeasureDistanceTool");


UAINiagaraEditorMode::UAINiagaraEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	// appearance and icon in the editing mode ribbon can be customized here
	Info = FEditorModeInfo(UAINiagaraEditorMode::EM_AINiagaraEditorModeId,
		LOCTEXT("ModeName", "AINiagara"),
		FSlateIcon(),
		true);
}


UAINiagaraEditorMode::~UAINiagaraEditorMode()
{
}


void UAINiagaraEditorMode::ActorSelectionChangeNotify()
{
}

void UAINiagaraEditorMode::Enter()
{
	UEdMode::Enter();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// AddYourTool Step 2 - register the ToolBuilders for your Tools here.
	// The string name you pass to the ToolManager is used to select/activate your ToolBuilder later.
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const FAINiagaraEditorModeCommands& SampleToolCommands = FAINiagaraEditorModeCommands::Get();

	RegisterTool(SampleToolCommands.SimpleTool, SimpleToolName, NewObject<UAINiagaraSimpleToolBuilder>(this));
	RegisterTool(SampleToolCommands.InteractiveTool, InteractiveToolName, NewObject<UAINiagaraInteractiveToolBuilder>(this));

	// active tool type is not relevant here, we just set to default
	GetToolManager()->SelectActiveToolType(EToolSide::Left, SimpleToolName);
}

void UAINiagaraEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FAINiagaraEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UAINiagaraEditorMode::GetModeCommands() const
{
	return FAINiagaraEditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
