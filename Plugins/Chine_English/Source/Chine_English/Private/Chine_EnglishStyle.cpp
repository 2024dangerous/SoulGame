// Copyright Epic Games, Inc. All Rights Reserved.

#include "Chine_EnglishStyle.h"
#include "Chine_English.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FChine_EnglishStyle::StyleInstance = nullptr;

void FChine_EnglishStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FChine_EnglishStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FChine_EnglishStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("Chine_EnglishStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FChine_EnglishStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("Chine_EnglishStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("Chine_English")->GetBaseDir() / TEXT("Resources"));


	Style->Set("Chine_English.PluginAction", new IMAGE_BRUSH(TEXT("Chine_English"), Icon20x20));
	return Style;
}

void FChine_EnglishStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FChine_EnglishStyle::Get()
{
	return *StyleInstance;
}
