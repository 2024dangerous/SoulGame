// Copyright Epic Games, Inc. All Rights Reserved.

#include "AddPrefixEditorModeToolkit.h"
#include "AddPrefixEditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "AddPrefixEditorModeToolkit"

FAddPrefixEditorModeToolkit::FAddPrefixEditorModeToolkit()
{
}

void FAddPrefixEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FAddPrefixEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}


FName FAddPrefixEditorModeToolkit::GetToolkitFName() const
{
	return FName("AddPrefixEditorMode");
}

FText FAddPrefixEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "AddPrefixEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
