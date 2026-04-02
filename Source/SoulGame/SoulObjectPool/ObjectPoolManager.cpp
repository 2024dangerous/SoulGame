// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulObjectPool/ObjectPoolManager.h"
#include "SoulObjectPool/ObjectPoolBase.h"
#include "SoulObjectPool/SoulObjectPoolConfig.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UObjectPoolManager::Init()
{
    CurrentConfig = nullptr;
    UE_LOG(LogTemp, Log, TEXT("UObjectPoolManager::Init - 对象池管理器初始化"));
}

void UObjectPoolManager::InitializeFromConfig(USoulObjectPoolConfig* Config, UWorld* World)
{
    if (!Config || !World)
    {
        UE_LOG(LogTemp, Error, TEXT("UObjectPoolManager::InitializeFromConfig - Config 或 World 为空"));
        return;
    }

    CurrentConfig = Config;

    for (const FSoulPoolEntry& Entry : Config->PoolEntries)
    {
        if (Entry.bPrewarmOnLoad)
        {
            CreatePool(Entry.PoolName, Entry.PoolClass, Entry.ActorClass, Entry.InitialSize, World);
            PoolConfigCache.Add(Entry.PoolName, Entry);
        }
    }

    // 启动缩容定时器
    StartShrinkTimer(World);

    UE_LOG(LogTemp, Log, TEXT("UObjectPoolManager::InitializeFromConfig - 从配置初始化了 %d 个对象池"),
        Config->PoolEntries.Num());
}

void UObjectPoolManager::InitializeForLevel(USoulObjectPoolConfig* Config, FName LevelName, UWorld* World)
{
    if (!Config || !World)
    {
        UE_LOG(LogTemp, Error, TEXT("UObjectPoolManager::InitializeForLevel - Config 或 World 为空"));
        return;
    }

    CurrentConfig = Config;

    TArray<FSoulPoolEntry> LevelPools = Config->GetPoolsForLevel(LevelName);

    for (const FSoulPoolEntry& Entry : LevelPools)
    {
        if (Entry.bPrewarmOnLoad)
        {
            CreatePool(Entry.PoolName, Entry.PoolClass, Entry.ActorClass, Entry.InitialSize, World);
            PoolConfigCache.Add(Entry.PoolName, Entry);
        }
    }

    // 启动缩容定时器
    StartShrinkTimer(World);

    UE_LOG(LogTemp, Log, TEXT("UObjectPoolManager::InitializeForLevel - 为关卡 %s 初始化了 %d 个对象池"),
        *LevelName.ToString(), LevelPools.Num());
}

void UObjectPoolManager::CreatePool(FName PoolName, TSubclassOf<UObjectPoolBase> PoolClass, TSubclassOf<AActor> ActorClass, int32 InitialSize, UWorld* World)
{
    if (Pools.Contains(PoolName))
    {
        UE_LOG(LogTemp, Warning, TEXT("Pool '%s' already exists."), *PoolName.ToString());
        return;
    }

    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("World is null when creating pool '%s'."), *PoolName.ToString());
        return;
    }

    UObjectPoolBase* NewPool = NewObject<UObjectPoolBase>(this, PoolClass);
    if (!NewPool)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create pool '%s'."), *PoolName.ToString());
        return;
    }

    NewPool->SetupPool(ActorClass, World);
    NewPool->InitializePool(InitialSize);

    Pools.Add(PoolName, NewPool);

    // 初始化统计信息
    FPoolStats Stats;
    Stats.PoolName = PoolName;
    Stats.TotalCreated = InitialSize;
    Stats.CurrentPooled = InitialSize;
    PoolStatsMap.Add(PoolName, Stats);

    UE_LOG(LogTemp, Log, TEXT("Created pool '%s' with %d objects."), *PoolName.ToString(), InitialSize);
}

UObject* UObjectPoolManager::Acquire(FName PoolName, UObject* WorldContextObject)
{
    if (!WorldContextObject)
    {
        UE_LOG(LogTemp, Error, TEXT("WorldContextObject is null in Acquire."));
        return nullptr;
    }

    UWorld* World = WorldContextObject->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("World is null in Acquire for context object: %s"), *WorldContextObject->GetName());
        return nullptr;
    }

    if (UObjectPoolBase** FoundPool = Pools.Find(PoolName))
    {
        // 检查最大容量限制
        if (const FSoulPoolEntry* Config = PoolConfigCache.Find(PoolName))
        {
            if (Config->MaxSize > 0)
            {
                FPoolStats* Stats = PoolStatsMap.Find(PoolName);
                if (Stats && Stats->CurrentActive >= Config->MaxSize)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Pool '%s' reached max capacity: %d"), *PoolName.ToString(), Config->MaxSize);
                    return nullptr;
                }
            }
        }

        UObject* Result = (*FoundPool)->AcquireObject(World);
        if (Result)
        {
            UpdateStats(PoolName, true);
        }
        return Result;
    }

    UE_LOG(LogTemp, Warning, TEXT("No pool found with name: %s"), *PoolName.ToString());
    return nullptr;
}

void UObjectPoolManager::Release(FName PoolName, UObject* Object)
{
    if (UObjectPoolBase** FoundPool = Pools.Find(PoolName))
    {
        (*FoundPool)->ReleaseObject(Object);
        UpdateStats(PoolName, false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No pool found with name: %s"), *PoolName.ToString());
    }
}

void UObjectPoolManager::DestroyPool(FName PoolName)
{
    if (UObjectPoolBase** FoundPool = Pools.Find(PoolName))
    {
        Pools.Remove(PoolName);
        PoolStatsMap.Remove(PoolName);
        PoolConfigCache.Remove(PoolName);
        UE_LOG(LogTemp, Log, TEXT("Destroyed pool: %s"), *PoolName.ToString());
    }
}

void UObjectPoolManager::DestroyAllPools()
{
    Pools.Empty();
    PoolStatsMap.Empty();
    PoolConfigCache.Empty();
    UE_LOG(LogTemp, Log, TEXT("All pools destroyed."));
}

void UObjectPoolManager::PerformShrink()
{
    for (auto& Pair : PoolConfigCache)
    {
        const FName& PoolName = Pair.Key;
        const FSoulPoolEntry& Config = Pair.Value;

        if (!Config.bAutoShrink) continue;

        FPoolStats* Stats = PoolStatsMap.Find(PoolName);
        if (!Stats) continue;

        if (Stats->CurrentPooled > Config.ShrinkThreshold)
        {
            int32 ToRemove = Stats->CurrentPooled - Config.ShrinkThreshold;
            UE_LOG(LogTemp, Log, TEXT("UObjectPoolManager::PerformShrink - 池 %s 缩容: 回收 %d 个空闲对象"),
                *PoolName.ToString(), ToRemove);

            // 注意：实际的对象销毁需要在 ObjectPoolBase 中实现 ShrinkPool 方法
            // 这里只更新统计
            Stats->CurrentPooled = Config.ShrinkThreshold;
        }
    }

    if (CurrentConfig && CurrentConfig->bEnablePoolStats)
    {
        LogPoolStats();
    }
}

FPoolStats UObjectPoolManager::GetPoolStats(FName PoolName) const
{
    if (const FPoolStats* Stats = PoolStatsMap.Find(PoolName))
    {
        return *Stats;
    }
    return FPoolStats();
}

TArray<FPoolStats> UObjectPoolManager::GetAllPoolStats() const
{
    TArray<FPoolStats> Result;
    for (const auto& Pair : PoolStatsMap)
    {
        Result.Add(Pair.Value);
    }
    return Result;
}

void UObjectPoolManager::LogPoolStats() const
{
    UE_LOG(LogTemp, Log, TEXT("========== 对象池统计 =========="));
    for (const auto& Pair : PoolStatsMap)
    {
        const FPoolStats& Stats = Pair.Value;
        UE_LOG(LogTemp, Log, TEXT("  [%s] 总创建: %d | 活跃: %d | 池中: %d | 峰值: %d | 获取: %d | 归还: %d"),
            *Stats.PoolName.ToString(),
            Stats.TotalCreated,
            Stats.CurrentActive,
            Stats.CurrentPooled,
            Stats.PeakActive,
            Stats.TotalAcquires,
            Stats.TotalReleases);
    }
    UE_LOG(LogTemp, Log, TEXT("================================"));
}

void UObjectPoolManager::StartShrinkTimer(UWorld* World)
{
    if (!World) return;

    float Interval = CurrentConfig ? CurrentConfig->GlobalShrinkInterval : 60.0f;

    World->GetTimerManager().SetTimer(
        ShrinkTimerHandle,
        FTimerDelegate::CreateUObject(this, &UObjectPoolManager::PerformShrink),
        Interval,
        true // 循环
    );

    UE_LOG(LogTemp, Log, TEXT("UObjectPoolManager::StartShrinkTimer - 缩容定时器启动，间隔: %.1f 秒"), Interval);
}

void UObjectPoolManager::UpdateStats(FName PoolName, bool bAcquire)
{
    FPoolStats* Stats = PoolStatsMap.Find(PoolName);
    if (!Stats) return;

    if (bAcquire)
    {
        Stats->CurrentActive++;
        Stats->CurrentPooled = FMath::Max(0, Stats->CurrentPooled - 1);
        Stats->TotalAcquires++;
        Stats->PeakActive = FMath::Max(Stats->PeakActive, Stats->CurrentActive);
    }
    else
    {
        Stats->CurrentActive = FMath::Max(0, Stats->CurrentActive - 1);
        Stats->CurrentPooled++;
        Stats->TotalReleases++;
    }
}