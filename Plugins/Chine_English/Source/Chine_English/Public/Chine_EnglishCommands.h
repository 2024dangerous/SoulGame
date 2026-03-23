// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Chine_EnglishStyle.h"

class FChine_EnglishCommands : public TCommands<FChine_EnglishCommands>
{
public:

	FChine_EnglishCommands()
		: TCommands<FChine_EnglishCommands>(TEXT("Chine_English"), NSLOCTEXT("Contexts", "Chine_English", "Chine_English Plugin"), NAME_None, FChine_EnglishStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
