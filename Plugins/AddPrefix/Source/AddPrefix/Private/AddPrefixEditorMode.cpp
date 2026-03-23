// Copyright Epic Games, Inc. All Rights Reserved.

#include "AddPrefixEditorMode.h"
#include "AddPrefixEditorModeToolkit.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "AddPrefixEditorModeCommands.h"


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
// AddYourTool Step 1 - include the header file for your Tools here
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
#include "Tools/AddPrefixSimpleTool.h"
#include "Tools/AddPrefixInteractiveTool.h"

// step 2: register a ToolBuilder in FAddPrefixEditorMode::Enter() below


#define LOCTEXT_NAMESPACE "AddPrefixEditorMode"

const FEditorModeID UAddPrefixEditorMode::EM_AddPrefixEditorModeId = TEXT("EM_AddPrefixEditorMode");

FString UAddPrefixEditorMode::SimpleToolName = TEXT("AddPrefix_ActorInfoTool");
FString UAddPrefixEditorMode::InteractiveToolName = TEXT("AddPrefix_MeasureDistanceTool");


UAddPrefixEditorMode::UAddPrefixEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	// appearance and icon in the editing mode ribbon can be customized here
	Info = FEditorModeInfo(UAddPrefixEditorMode::EM_AddPrefixEditorModeId,
		LOCTEXT("ModeName", "AddPrefix"),
		FSlateIcon(),
		true);
}


UAddPrefixEditorMode::~UAddPrefixEditorMode()
{
}


void UAddPrefixEditorMode::ActorSelectionChangeNotify()
{
}

void UAddPrefixEditorMode::Enter()
{
	UEdMode::Enter();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// AddYourTool Step 2 - register the ToolBuilders for your Tools here.
	// The string name you pass to the ToolManager is used to select/activate your ToolBuilder later.
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const FAddPrefixEditorModeCommands& SampleToolCommands = FAddPrefixEditorModeCommands::Get();

	RegisterTool(SampleToolCommands.SimpleTool, SimpleToolName, NewObject<UAddPrefixSimpleToolBuilder>(this));
	RegisterTool(SampleToolCommands.InteractiveTool, InteractiveToolName, NewObject<UAddPrefixInteractiveToolBuilder>(this));

	// active tool type is not relevant here, we just set to default
	GetToolManager()->SelectActiveToolType(EToolSide::Left, SimpleToolName);
}

void UAddPrefixEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FAddPrefixEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UAddPrefixEditorMode::GetModeCommands() const
{
	return FAddPrefixEditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
