// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FAssetPrefixUtils;
/**
 * This is the module definition for the editor mode. You can implement custom functionality
 * as your plugin module starts up and shuts down. See IModuleInterface for more extensibility options.
 */
class FAddPrefixModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TSharedRef<FExtender> OnExtendAssetMenu(const TArray<FAssetData>& SelectedAssets);

    FAssetPrefixUtils* AssetPrefixUtils;

	void AddAssetContextMenuEntry(FMenuBuilder& MenuBuilder, TArray<FAssetData> SelectedAssets);

	void AddPrefixToAssets(TArray<FAssetData> SelectedAssets);
};
