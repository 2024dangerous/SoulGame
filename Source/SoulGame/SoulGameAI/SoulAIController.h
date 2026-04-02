// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTags.h"
#include "SoulAIController.generated.h"

class USoulAIConfig;
class UBehaviorTree;
class UBlackboardData;

/**
 * SoulAIController - 数据驱动的 AI 控制器
 * 
 * 从 SoulAIConfig DataAsset 读取配置
 * 支持：
 * - 数据驱动的行为树
 * - 可配置的距离检测
 * - 可配置的攻击选择
 * - 与 GAS 集成
 */
UCLASS()
class SOULGAME_API ASoulAIController : public AAIController
{
	GENERATED_BODY()

public:
	ASoulAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	// ============ 配置 ============

	/** AI 配置数据资产 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoulGame|AI")
	USoulAIConfig* AIConfig;

	// ============ 便捷方法 ============

	/**
	 * 获取 AI 配置
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|AI")
	USoulAIConfig* GetAIConfig() const { return AIConfig; }

	/**
	 * 根据配置选择攻击
	 * @param Distance 与目标的距离
	 * @return 攻击条目索引（-1 表示无可用攻击）
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|AI")
	int32 SelectAttackFromConfig(float Distance);

	/**
	 * 根据配置获取距离类型
	 * @return 0=近, 1=中, 2=远
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|AI")
	int32 GetRangeTypeFromConfig(float Distance);

	/**
	 * 根据配置判断是否应该观察
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|AI")
	bool ShouldObserveFromConfig();

	/**
	 * 设置移动速度（根据配置）
	 * @param bIsRunning 是否奔跑
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|AI")
	void SetMovementSpeed(bool bIsRunning);

protected:
	// 初始化行为树（从配置中读取）
	void InitializeBehaviorTree();
};