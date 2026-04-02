// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTags.h"
#include "BehaviorTree/BehaviorTree.h"
#include "SoulAIConfig.generated.h"

class USoulGameplayAbility;

/**
 * AI 攻击配置条目
 * 定义一个攻击行为的权重和条件
 */
USTRUCT(BlueprintType)
struct FSoulAIAttackEntry
{
	GENERATED_BODY()

	// 攻击标签（用于标识攻击类型）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AttackTag;

	// 攻击名称（调试用）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AttackName;

	// 攻击权重（用于随机选择）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "100"))
	int32 Weight = 50;

	// 攻击距离范围
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinRange = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxRange = 200.0f;

	// 攻击冷却时间
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Cooldown = 1.0f;

	// 攻击动画蒙太奇（如果不使用 GAS）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAnimMontage*> AttackMontages;

	// GAS 技能类（如果使用 GAS）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<USoulGameplayAbility> AbilityClass;

	// 需要的条件标签
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer RequiredTags;

	// 阻止的条件标签
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer BlockedTags;
};

/**
 * AI 距离范围配置
 */
USTRUCT(BlueprintType)
struct FSoulAIRangeConfig
{
	GENERATED_BODY()

	// 近距离阈值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CloseRange = 180.0f;

	// 中距离阈值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MediumRange = 500.0f;

	// 追击最大距离（超过此距离返回初始位置）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxChaseDistance = 4000.0f;

	// 返回初始位置的距离阈值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ResetDistance = 100.0f;
};

/**
 * AI 移动配置
 */
USTRUCT(BlueprintType)
struct FSoulAIMoveConfig
{
	GENERATED_BODY()

	// 行走速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WalkSpeed = 200.0f;

	// 奔跑速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RunSpeed = 600.0f;

	// 绕行速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StrafeSpeed = 150.0f;

	// 观察时间范围（秒）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinObserveTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxObserveTime = 3.0f;
};

/**
 * SoulAIConfig - AI 配置数据资产
 * 
 * 策划可在编辑器中配置 AI 的所有行为参数
 * 无需修改代码即可调整 AI 行为
 */
UCLASS(BlueprintType)
class SOULGAME_API USoulAIConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	// AI 名称
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Basic")
	FName AIName;

	// 行为树资产
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Basic")
	UBehaviorTree* BehaviorTree;

	// 距离范围配置
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Range")
	FSoulAIRangeConfig RangeConfig;

	// 移动配置
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Movement")
	FSoulAIMoveConfig MoveConfig;

	// 攻击配置列表
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Attack")
	TArray<FSoulAIAttackEntry> AttackEntries;

	// 观察行为权重（vs 攻击）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Behavior", meta = (ClampMin = "0", ClampMax = "100"))
	int32 ObserveWeight = 30;

	// 默认授予的 GAS 技能
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|GAS")
	TArray<TSubclassOf<USoulGameplayAbility>> DefaultAbilities;

	// ============ 便捷方法 ============

	/**
	 * 根据距离和权重随机选择一个攻击
	 * @param Distance 与目标的距离
	 * @param OwnerTags 拥有者的标签（用于条件检查）
	 * @return 选中的攻击条目索引（-1 表示无可用攻击）
	 */
	UFUNCTION(BlueprintCallable, Category = "AI|Config")
	int32 SelectAttack(float Distance, const FGameplayTagContainer& OwnerTags) const;

	/**
	 * 判断是否应该观察（vs 攻击）
	 */
	UFUNCTION(BlueprintCallable, Category = "AI|Config")
	bool ShouldObserve() const;

	/**
	 * 获取距离类型
	 * @return 0=近, 1=中, 2=远
	 */
	UFUNCTION(BlueprintCallable, Category = "AI|Config")
	int32 GetRangeType(float Distance) const;
};
