// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoulLODManager.generated.h"

/**
 * LOD 分级配置
 */
USTRUCT(BlueprintType)
struct FSoulLODLevel
{
	GENERATED_BODY()

	// LOD 级别名称（调试用）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LevelName;

	// 距离阈值（超过此距离切换到此 LOD）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
	float DistanceThreshold = 0.0f;

	// 是否禁用 Tick
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDisableTick = false;

	// 是否禁用碰撞
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDisableCollision = false;

	// 是否隐藏 Actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHideActor = false;

	// 动画更新频率（0 = 不限制）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
	float AnimUpdateRate = 0.0f;

	// 是否禁用阴影
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDisableShadow = false;
};

/**
 * SoulLODManager - LOD 与视锥剔除管理组件
 * 
 * 根据与摄像机的距离自动调整 Actor 的：
 * - Tick 频率
 * - 碰撞检测
 * - 可见性
 * - 动画更新频率
 * - 阴影投射
 * 
 * 使用方法：
 * 1. 添加到需要 LOD 管理的 Actor 上
 * 2. 在编辑器中配置 LODLevels 数组
 * 3. 组件会自动根据距离切换 LOD 级别
 */
UCLASS(ClassGroup = (SoulGame), meta = (BlueprintSpawnableComponent))
class SOULGAME_API USoulLODManager : public UActorComponent
{
	GENERATED_BODY()

public:
	USoulLODManager();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ============ 配置 ============

	// LOD 级别配置（按距离从近到远排列）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoulGame|LOD")
	TArray<FSoulLODLevel> LODLevels;

	// LOD 检查间隔（秒，避免每帧检查）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoulGame|LOD", meta = (ClampMin = "0.1"))
	float CheckInterval = 0.5f;

	// 是否启用视锥剔除
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoulGame|LOD")
	bool bEnableFrustumCulling = true;

	// 视锥剔除边距（度，防止边缘闪烁）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoulGame|LOD", meta = (ClampMin = "0"))
	float FrustumCullingMargin = 5.0f;

	// 最大管理距离（超过此距离直接隐藏）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoulGame|LOD", meta = (ClampMin = "0"))
	float MaxDistance = 10000.0f;

	// ============ 状态查询 ============

	// 获取当前 LOD 级别索引
	UFUNCTION(BlueprintCallable, Category = "SoulGame|LOD")
	int32 GetCurrentLODLevel() const { return CurrentLODLevel; }

	// 获取与摄像机的距离
	UFUNCTION(BlueprintCallable, Category = "SoulGame|LOD")
	float GetDistanceToCamera() const { return CachedDistance; }

	// 是否在视锥内
	UFUNCTION(BlueprintCallable, Category = "SoulGame|LOD")
	bool IsInFrustum() const { return bIsInFrustum; }

	// 强制更新 LOD
	UFUNCTION(BlueprintCallable, Category = "SoulGame|LOD")
	void ForceUpdate();

protected:
	// 当前 LOD 级别
	int32 CurrentLODLevel;

	// 缓存的距离
	float CachedDistance;

	// 是否在视锥内
	bool bIsInFrustum;

	// 上次检查时间
	float LastCheckTime;

	// 原始 Tick 状态
	bool bOriginalTickEnabled;

	// 原始碰撞状态
	bool bOriginalCollisionEnabled;

	// 更新 LOD 级别
	void UpdateLOD();

	// 应用 LOD 设置
	void ApplyLODSettings(const FSoulLODLevel& LODConfig);

	// 恢复原始设置
	void RestoreOriginalSettings();

	// 检查是否在视锥内
	bool CheckFrustumVisibility() const;

	// 获取摄像机位置
	FVector GetCameraLocation() const;
};
