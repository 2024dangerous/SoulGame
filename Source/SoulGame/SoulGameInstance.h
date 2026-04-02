// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SoulGameInstance.generated.h"

class UObjectPoolManager;
class USoulAsyncLoader;
class USoulObjectPoolConfig;

/**
 * SoulGameInstance - 游戏实例
 * 
 * 管理全局子系统：
 * - 对象池管理器
 * - 异步资源加载器
 */
UCLASS()
class SOULGAME_API USoulGameInstance : public UGameInstance
{
	GENERATED_BODY()

	virtual void Init() override;
	virtual void Shutdown() override;

public:
	// 对象池管理器
	UPROPERTY(BlueprintReadOnly, Category = "SoulGame|Systems")
	UObjectPoolManager* PoolManager;

	// 异步资源加载器
	UPROPERTY(BlueprintReadOnly, Category = "SoulGame|Systems")
	USoulAsyncLoader* AsyncLoader;

	// 对象池配置（在编辑器中设置）
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SoulGame|Config")
	USoulObjectPoolConfig* PoolConfig;

	// 关卡管理
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName CurrentLevelName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName TargetLevelName;

	// ============ 便捷访问 ============

	/**
	 * 获取异步加载器
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Systems")
	USoulAsyncLoader* GetAsyncLoader() const { return AsyncLoader; }

	/**
	 * 获取对象池管理器
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Systems")
	UObjectPoolManager* GetPoolManager() const { return PoolManager; }

	// 世界初始化回调
	void OnWorldInitialized(UWorld* World, const UWorld::InitializationValues IVS);
};