// Copyright Epic Games, Inc. All Rights Reserved.

#include "Chine_EnglishCommands.h"

#define LOCTEXT_NAMESPACE "FChine_EnglishModule"

void FChine_EnglishCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Chine_English", "一键切换中英文", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
