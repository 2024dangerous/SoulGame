// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoulObjectPoolConfig.h"
#include "ObjectPoolManager.generated.h"

class UObjectPoolBase;

/**
 * 姹犵粺璁′俊鎭?
 */
USTRUCT(BlueprintType)
struct FPoolStats
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FName PoolName;

    UPROPERTY(BlueprintReadOnly)
    int32 TotalCreated = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentActive = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentPooled = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 PeakActive = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 TotalAcquires = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 TotalReleases = 0;
};

/**
 * ObjectPoolManager - 閰嶇疆鍖栧璞℃睜绠＄悊鍣?
 * 
 * 鏀寔锛?
 * - 浠?DataAsset 閰嶇疆鑷姩鍒涘缓姹?
 * - 鑷姩缂╁锛堝畾鏃跺洖鏀剁┖闂插璞★級
 * - 姹犵粺璁′俊鎭?
 * - 鎸夊叧鍗¤嚜鍔ㄧ鐞?
 */
UCLASS()
class SOULGAME_API UObjectPoolManager : public UObject
{
    GENERATED_BODY()

public:
    // 鍒濆鍖栵紙鐢?GameInstance Init 涓皟鐢級
    void Init();

    // ============ 閰嶇疆鍖栨帴鍙?============

    /**
     * 浠庨厤缃?DataAsset 鍒濆鍖栨墍鏈夊璞℃睜
     * @param Config 瀵硅薄姹犻厤缃?
     * @param World 当前世界
     */
    UFUNCTION(BlueprintCallable, Category = "ObjectPool")
    void InitializeFromConfig(USoulObjectPoolConfig* Config, UWorld* World);

    /**
     * 鏍规嵁鍏冲崱鍚嶇О鍒濆鍖栭€傜敤鐨勫璞℃睜
     * @param Config 瀵硅薄姹犻厤缃?
     * @param LevelName 关卡名称
     * @param World 当前世界
     */
    UFUNCTION(BlueprintCallable, Category = "ObjectPool")
    void InitializeForLevel(USoulObjectPoolConfig* Config, FName LevelName, UWorld* World);

    // ============ 鍩虹鎺ュ彛 ============

    // 鍒涘缓涓€涓柊鐨勫璞℃睜
    UFUNCTION(BlueprintCallable, Category = "ObjectPool")
    void CreatePool(FName PoolName, TSubclassOf<UObjectPoolBase> PoolClass, TSubclassOf<AActor> ActorClass, int32 InitialSize, UWorld* World);

    // 获取池中对象
    UFUNCTION(BlueprintCallable, Category = "ObjectPool")
    UObject* Acquire(FName PoolName, UObject* WorldContextObject);

    // 归还对象到池
    UFUNCTION(BlueprintCallable, Category = "ObjectPool")
    void Release(FName PoolName, UObject* Object);

    // ============ 管理接口 ============

    /**
     * 閿€姣佹寚瀹氱殑瀵硅薄姹?
     */
    UFUNCTION(BlueprintCallable, Category = "ObjectPool")
    void DestroyPool(FName PoolName);

    /**
     * 閿€姣佹墍鏈夊璞℃睜
     */
    UFUNCTION(BlueprintCallable, Category = "ObjectPool")
    void DestroyAllPools();

    /**
     * 鎵ц缂╁妫€鏌ワ紙鍥炴敹澶氫綑鐨勭┖闂插璞★級
     */
    UFUNCTION(BlueprintCallable, Category = "ObjectPool")
    void PerformShrink();

    // ============ 缁熻鎺ュ彛 ============

    /**
     * 鑾峰彇鎸囧畾姹犵殑缁熻淇℃伅
     */
    UFUNCTION(BlueprintCallable, Category = "ObjectPool|Stats")
    FPoolStats GetPoolStats(FName PoolName) const;

    /**
     * 鑾峰彇鎵€鏈夋睜鐨勭粺璁′俊鎭?
     */
    UFUNCTION(BlueprintCallable, Category = "ObjectPool|Stats")
    TArray<FPoolStats> GetAllPoolStats() const;

    /**
     * 鎵撳嵃鎵€鏈夋睜鐨勭粺璁′俊鎭埌鏃ュ織
     */
    UFUNCTION(BlueprintCallable, Category = "ObjectPool|Stats")
    void LogPoolStats() const;

private:
    // 鎵€鏈夊璞℃睜
    UPROPERTY()
    TMap<FName, UObjectPoolBase*> Pools;

    // 姹犵粺璁′俊鎭?
    TMap<FName, FPoolStats> PoolStatsMap;

    // 姹犻厤缃紦瀛?
    TMap<FName, FSoulPoolEntry> PoolConfigCache;

    // 当前配置
    UPROPERTY()
    USoulObjectPoolConfig* CurrentConfig;

    // 缂╁瀹氭椂鍣ㄥ彞鏌?
    FTimerHandle ShrinkTimerHandle;

    // 鍚姩缂╁瀹氭椂鍣?
    void StartShrinkTimer(UWorld* World);

    // 鏇存柊缁熻淇℃伅
    void UpdateStats(FName PoolName, bool bAcquire);
};