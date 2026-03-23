#pragma once

#include "CoreMinimal.h"
//#include "AssetPrefix.generated.h"
#include "UObject/Class.h" // for UClass*
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialFunctionInterface.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundWave.h"
#include "Components/SkeletalMeshComponent.h"
#include "Blueprint/UserWidget.h"
#include "../../../../../../../Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraSystem.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Blueprint/UserWidgetBlueprint.h"

class UBlueprint; // 前向声明即可

/**
 * 静态工具类，用于根据资源类型获取前缀
 */
class FAssetPrefixUtils
{
public:
    static const TMap<UClass*, FString>& GetPrefixMap()
    {
        static const TMap<UClass*, FString> PrefixMap = {
            {UStaticMesh::StaticClass(), TEXT("SM_")},
            {UMaterial::StaticClass(), TEXT("M_")},
            {UMaterialInstanceConstant::StaticClass(), TEXT("MI_")},
            {UMaterialFunctionInterface::StaticClass(), TEXT("MF_")},
            {USoundCue::StaticClass(), TEXT("SC_")},
            {USoundWave::StaticClass(), TEXT("SW_")},
            {UTexture::StaticClass(), TEXT("T_")},
            {UTexture2D::StaticClass(), TEXT("T_")},
            {UUserWidget::StaticClass(), TEXT("WBP_")},
            {UUserWidgetBlueprint::StaticClass(), TEXT("WBP_")},
            {USkeletalMeshComponent::StaticClass(), TEXT("SK_")},
            {UBlueprint::StaticClass(), TEXT("BP_")},
            {UNiagaraSystem::StaticClass(), TEXT("NS_")},
        };
        return PrefixMap;
    }

    static FString GetPrefixForClass(UClass* Class)
    {
        const auto& Map = GetPrefixMap();

        while (Class)
        {
            UE_LOG(LogTemp, Warning, TEXT("Class: %s"), *Class->GetName());
            if (const FString* Prefix = Map.Find(Class))
            {
                return *Prefix;
            }
            // 向父类查找
            Class = Class->GetSuperClass();
        }

        return FString(); // 没找到
    }
};
