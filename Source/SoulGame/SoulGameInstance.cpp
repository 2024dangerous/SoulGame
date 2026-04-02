// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameInstance.h"
#include "SoulObjectPool/ObjectPoolManager.h"
#include "SoulObjectPool/SoulObjectPoolConfig.h"
#include "SoulObjectPool/PickupItemPool.h"
#include "SoulGameItem/PickupItem.h"
#include "SoulGamePerformance/SoulAsyncLoader.h"
#include "SoulGameDebug/DebugTools.h"

void USoulGameInstance::Init()
{
    Super::Init();

    UE_LOG(LogTemp, Log, TEXT("USoulGameInstance initialized."));

    // 创建对象池管理器
    PoolManager = NewObject<UObjectPoolManager>(this);
    PoolManager->Init();

    // 创建异步资源加载器
    AsyncLoader = NewObject<USoulAsyncLoader>(this);

    // 注册延迟初始化
    FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &USoulGameInstance::OnWorldInitialized);
}

void USoulGameInstance::Shutdown()
{
    // 解绑世界初始化委托，避免编辑器多次PIE时出现问题
    FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);

    Super::Shutdown();
}

void USoulGameInstance::OnWorldInitialized(UWorld* World, const UWorld::InitializationValues IVS)
{
    // 只处理游戏世界，跳过 PIE 编辑器预览世界
    if (!World || World->IsPreviewWorld()) return;

    UE_LOG(LogTemp, Log, TEXT("World Initialized: %s"), *World->GetName());

    // 优先使用配置化初始化
    if (PoolConfig)
    {
        FName LevelName = FName(*World->GetName());
        PoolManager->InitializeForLevel(PoolConfig, LevelName, World);
        UE_LOG(LogTemp, Log, TEXT("USoulGameInstance - 从配置初始化对象池 (关卡: %s)"), *LevelName.ToString());
    }
    else
    {
        // 回退到旧的硬编码方式（兼容）
        PoolManager->CreatePool(
            FName("PickupItemPool"),
            UPickupItemPool::StaticClass(),
            APickupItem::StaticClass(),
            10,
            World
        );
        UE_LOG(LogTemp, Log, TEXT("USoulGameInstance - 使用默认配置初始化对象池"));
    }
}