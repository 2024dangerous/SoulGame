// Copyright Epic Games, Inc. All Rights Reserved.

#include "AddPrefixEditorModeCommands.h"
#include "AddPrefixEditorMode.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "AddPrefixEditorModeCommands"

FAddPrefixEditorModeCommands::FAddPrefixEditorModeCommands()
	: TCommands<FAddPrefixEditorModeCommands>("AddPrefixEditorMode",
		NSLOCTEXT("AddPrefixEditorMode", "AddPrefixEditorModeCommands", "AddPrefix Editor Mode"),
		NAME_None,
		FEditorStyle::GetStyleSetName())
{
}

void FAddPrefixEditorModeCommands::RegisterCommands()
{
	TArray <TSharedPtr<FUICommandInfo>>& ToolCommands = Commands.FindOrAdd(NAME_Default);

	UI_COMMAND(SimpleTool, "Show Actor Info", "Opens message box with info about a clicked actor", EUserInterfaceActionType::Button, FInputChord());
	ToolCommands.Add(SimpleTool);

	UI_COMMAND(InteractiveTool, "Measure Distance", "Measures distance between 2 points (click to set origin, shift-click to set end point)", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(InteractiveTool);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FAddPrefixEditorModeCommands::GetCommands()
{
	return FAddPrefixEditorModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE
