// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SoulGameConfigBase.h"
#include "SoulGameplayConfig.generated.h"

/**
 * 玩家属性配置
 */
USTRUCT(BlueprintType)
struct SOULGAME_API FPlayerAttributeConfig
{
	GENERATED_BODY()

public:
	// 最大生命值
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute")
	float MaxHealth = 100.0f;

	// 最大法力值
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute")
	float MaxMana = 50.0f;

	// 最大体力值
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute")
	float MaxStamina = 100.0f;

	// 生命恢复速率
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute")
	float HealthRegenRate = 1.0f;

	// 体力恢复速率
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute")
	float StaminaRegenRate = 10.0f;
};

/**
 * 战斗配置
 */
USTRUCT(BlueprintType)
struct SOULGAME_API FCombatConfig
{
	GENERATED_BODY()

public:
	// 锁定目标最大距离
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float LockOnMaxDistance = 1000.0f;

	// 闪避消耗体力
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float DodgeStaminaCost = 20.0f;

	// 防御消耗体力
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float DefenseStaminaCost = 5.0f;

	// 攻击消耗体力
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float AttackStaminaCost = 10.0f;

	// 硬直时间（受击）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float HitStunDuration = 0.5f;

	// 死亡时间
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float DeathDuration = 3.0f;
};

/**
 * 拾取配置
 */
USTRUCT(BlueprintType)
struct SOULGAME_API FPickupConfig
{
	GENERATED_BODY()

public:
	// 拾取检测半径
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	float PickupRadius = 200.0f;

	// 拾取冷却时间
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	float PickupCooldown = 0.5f;

	// 最大拾取距离
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	float MaxPickupDistance = 300.0f;
};

/**
 * AI 配置
 */
USTRUCT(BlueprintType)
struct SOULGAME_API FAIConfig
{
	GENERATED_BODY()

public:
	// 敌人感知半径
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	float PerceptionRadius = 500.0f;

	// 敌人攻击范围
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	float AttackRange = 200.0f;

	// 敌人追击范围
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	float ChaseRange = 1000.0f;

	// 巡逻范围
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	float PatrolRange = 500.0f;
};

/**
 * SoulGameplayConfig - 游戏玩法配置 DataAsset
 * 
 * 集中管理游戏的各种配置参数
 * 支持通过 DataAsset 编辑器进行配置
 * 支持热更新
 */
UCLASS(BlueprintType, meta = (DisplayName = "Soul Gameplay Config"))
class SOULGAME_API USoulGameplayConfig : public USoulGameConfigBase
{
	GENERATED_BODY()

public:
	USoulGameplayConfig();

	// ============ 配置内容 ============

	// 玩家属性配置
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	FPlayerAttributeConfig PlayerAttributes;

	// 战斗配置
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	FCombatConfig Combat;

	// 拾取配置
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	FPickupConfig Pickup;

	// AI 配置
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	FAIConfig AI;

protected:
	// ============ USoulGameConfigBase ============

	virtual bool ValidateConfig_Implementation(FText& OutError) override;
};