// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameData/SoulGameConfigBase.h"

USoulGameConfigBase::USoulGameConfigBase()
	: ConfigVersion(0)
	, bIsLoaded(false)
	, bHasError(false)
{
}

bool USoulGameConfigBase::LoadConfig(FText& OutError)
{
	if (bHasError)
	{
		OutError = LastError;
		return false;
	}

	if (!ValidateConfig(OutError))
	{
		bHasError = true;
		LastError = OutError;
		return false;
	}

	bIsLoaded = true;
	return true;
}

bool USoulGameConfigBase::ReloadConfig(FText& OutError)
{
	UE_LOG(LogTemp, Log, TEXT("USoulGameConfigBase::ReloadConfig - Reloading config: %s"), *ConfigId.ToString());

	bIsLoaded = false;
	bHasError = false;
	LastError = FText();

	return LoadConfig(OutError);
}

bool USoulGameConfigBase::ValidateConfig_Implementation(FText& OutError)
{
	// 榛樿瀹炵幇锛氭鏌onfigId鏄惁鏈夋晥
	if (ConfigId.IsNone())
	{
		OutError = FText::FromString(TEXT("ConfigId cannot be None"));
		return false;
	}

	if (ConfigVersion <= 0)
	{
		OutError = FText::FromString(TEXT("ConfigVersion must be greater than 0"));
		return false;
	}

	return true;
}