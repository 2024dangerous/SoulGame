// Copyright Epic Games, Inc. All Rights Reserved.

#include "Chine_English.h"
#include "Chine_EnglishStyle.h"
#include "Chine_EnglishCommands.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Culture.h" 
#include "Misc/ConfigCacheIni.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName Chine_EnglishTabName("Chine_English");

#define LOCTEXT_NAMESPACE "FChine_EnglishModule"

void FChine_EnglishModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FChine_EnglishStyle::Initialize();
	FChine_EnglishStyle::ReloadTextures();

	FChine_EnglishCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FChine_EnglishCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FChine_EnglishModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FChine_EnglishModule::RegisterMenus));
}

void FChine_EnglishModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module. 

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FChine_EnglishStyle::Shutdown();

	FChine_EnglishCommands::Unregister();
}

void FChine_EnglishModule::PluginButtonClicked()
{
	// 确保调用指针返回的文化对象并获取当前语言的名称
	TSharedPtr<FCulture> CurrentCulture = FInternationalization::Get().GetCurrentCulture();
	FString LanguageCode =  CurrentCulture->GetName();
	if (LanguageCode == TEXT("zh-Hans"))
	{
		// 设置当前的语言文化
		FInternationalization::Get().SetCurrentCulture("en");

		// 重新加载编辑器配置文件以应用更改
		GConfig->Flush(false, GEditorPerProjectIni);
	} 
	else if (LanguageCode == TEXT("en"))
	{
		// 设置当前的语言文化
		FInternationalization::Get().SetCurrentCulture("zh-Hans");
		// 重新加载编辑器配置文件以应用更改
		GConfig->Flush(false, GEditorPerProjectIni);
	}
}

void FChine_EnglishModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FChine_EnglishCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FChine_EnglishCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}

	{
		UToolMenu* BlueprintToolbarMenu = UToolMenus::Get()->ExtendMenu("AssetEditor.BlueprintEditor.ToolBar");
		{
			FToolMenuSection& Section = BlueprintToolbarMenu->FindOrAddSection("PluginToolbar");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FChine_EnglishCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}

	{
		UToolMenu* BlueprintToolbarMenu = UToolMenus::Get()->ExtendMenu("AssetEditor.WidgetBlueprintEditor.ToolBar");
		{
			FToolMenuSection& Section = BlueprintToolbarMenu->FindOrAddSection("PluginToolbar");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FChine_EnglishCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FChine_EnglishModule, Chine_English)