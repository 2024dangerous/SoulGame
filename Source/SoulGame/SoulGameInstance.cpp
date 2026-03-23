// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameInstance.h"
#include "SoulObjectPool/ObjectPoolManager.h"
#include "SoulObjectPool/PickupItemPool.h"
#include "SoulGameItem/PickupItem.h"
#include "SoulGameDebug/DebugTools.h"

void USoulGameInstance::Init()
{
    Super::Init();

    UE_LOG(LogTemp, Log, TEXT("USoulGameInstance initialized."));

    // 创建对象池管理器
    PoolManager = NewObject<UObjectPoolManager>(this);

    // 注册延迟初始化
    FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &USoulGameInstance::OnWorldInitialized);
   
}



void USoulGameInstance::OnWorldInitialized(UWorld* World, const UWorld::InitializationValues IVS)
{
    // 只处理世界，跳过 PIE 编辑器预览世界
    if (!World || World->IsPreviewWorld()) return;

    UE_LOG(LogTemp, Log, TEXT("World Initialized: %s"), *World->GetName());

    PoolManager->CreatePool(
        FName("PickupItemPool"),
        UPickupItemPool::StaticClass(),
        APickupItem::StaticClass(),
        10,
        World
    );
}