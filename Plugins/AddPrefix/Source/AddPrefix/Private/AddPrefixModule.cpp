// Copyright Epic Games, Inc. All Rights Reserved.

#include "AddPrefixModule.h"
#include "AddPrefixEditorModeCommands.h"
#include "../../../../../../../Source/Editor/ContentBrowser/Public/ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "AssetToolsModule.h"
#include "Misc/MessageDialog.h"
#include "../../../../../../../Plugins/Editor/EditorScriptingUtilities/Source/EditorScriptingUtilities/Public/EditorAssetLibrary.h"
#include "../../../../../../../Source/Runtime/Slate/Public/Framework/Commands/GenericCommands.h"
#include "AssetPrefix.h"

#define LOCTEXT_NAMESPACE "AddPrefixModule"

void FAddPrefixModule::StartupModule()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
    TArray<FContentBrowserMenuExtender_SelectedAssets>& MenuExtenders = ContentBrowserModule.GetAllAssetViewContextMenuExtenders();

    MenuExtenders.Add(FContentBrowserMenuExtender_SelectedAssets::CreateRaw(this, &FAddPrefixModule::OnExtendAssetMenu));
	
}

void FAddPrefixModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FAddPrefixEditorModeCommands::Unregister();
}

TSharedRef<FExtender> FAddPrefixModule::OnExtendAssetMenu(const TArray<FAssetData>& SelectedAssets)
{
    TSharedRef<FExtender> Extender(new FExtender());

    if (SelectedAssets.Num() > 0)
    {
        Extender->AddMenuExtension(
            "Rename",
            EExtensionHook::After,
            nullptr,
            FMenuExtensionDelegate::CreateRaw(this, &FAddPrefixModule::AddAssetContextMenuEntry, SelectedAssets)
        );
        
    }
    return Extender;
}

void FAddPrefixModule::AddAssetContextMenuEntry(FMenuBuilder& MenuBuilder, TArray<FAssetData> SelectedAssets)
{
    MenuBuilder.AddMenuEntry(
        FText::FromString(TEXT("增加前缀")),
        FText::FromString(TEXT("给资产添加前缀")),
        FGenericCommands::Get().Rename->GetIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FAddPrefixModule::AddPrefixToAssets, SelectedAssets))
    );
}

void FAddPrefixModule::AddPrefixToAssets(TArray<FAssetData> SelectedAssets)
{
    for (const FAssetData& AssetData : SelectedAssets)
    {
        UClass* AssetClass = AssetData.GetClass();
        const FString Prefix = AssetPrefixUtils->GetPrefixForClass(AssetClass);
        const FString OldName = AssetData.AssetName.ToString();

        // 检查是否已经有该前缀
        if (OldName.StartsWith(Prefix))
        {
            // 已有前缀，跳过处理
            FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("已有前缀：" + OldName)));
            continue;
        }

        const FString NewName = Prefix + OldName;

        const FString AssetPath = AssetData.ObjectPath.ToString();
        const FString PackagePath = AssetData.PackagePath.ToString();

        const FString NewPackagePath = PackagePath + "/" + NewName;

        if (!UEditorAssetLibrary::RenameAsset(AssetPath, NewPackagePath))
        {
            FMessageDialog::Open(EAppMsgType::Ok, FText::FromString( FString::Printf(TEXT("错误：%s (未注册该前缀或重命名)"), *OldName)));
        }
    }
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAddPrefixModule, AddPrefixEditorMode)