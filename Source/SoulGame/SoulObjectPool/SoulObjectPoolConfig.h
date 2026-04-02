// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SoulObjectPoolConfig.generated.h"

class UObjectPoolBase;

/**
 * 单个对象池的配置条目
 */
USTRUCT(BlueprintType)
struct FSoulPoolEntry
{
	GENERATED_BODY()

	// 池名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PoolName;

	// 池类（必须继承自 UObjectPoolBase）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UObjectPoolBase> PoolClass;

	// 池化的 Actor 类
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ActorClass;

	// 初始预热数量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "500"))
	int32 InitialSize = 10;

	// 最大容量（0 = 无限制）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
	int32 MaxSize = 50;

	// 自动缩容阈值（空闲对象超过此数量时自动回收）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
	int32 ShrinkThreshold = 30;

	// 自动缩容检查间隔（秒）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1.0"))
	float ShrinkCheckInterval = 30.0f;

	// 是否启用自动缩容
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutoShrink = true;

	// 是否在关卡加载时自动预热
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPrewarmOnLoad = true;

	// 适用的关卡名称（空 = 所有关卡）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> ApplicableLevels;
};

/**
 * SoulObjectPoolConfig - 对象池配置数据资产
 * 
 * 策划可在编辑器中配置所有对象池的参数
 * 支持：
 * - 按关卡配置不同的池
 * - 自动预热和缩容
 * - 最大容量限制
 */
UCLASS(BlueprintType)
class SOULGAME_API USoulObjectPoolConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	// 所有对象池配置
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ObjectPool")
	TArray<FSoulPoolEntry> PoolEntries;

	// 全局缩容检查间隔（秒）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ObjectPool|Global")
	float GlobalShrinkInterval = 60.0f;

	// 是否启用池统计日志
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ObjectPool|Debug")
	bool bEnablePoolStats = false;

	/**
	 * 获取指定关卡适用的池配置
	 * @param LevelName 关卡名称
	 * @return 适用的池配置列表
	 */
	UFUNCTION(BlueprintCallable, Category = "ObjectPool")
	TArray<FSoulPoolEntry> GetPoolsForLevel(FName LevelName) const;
};
