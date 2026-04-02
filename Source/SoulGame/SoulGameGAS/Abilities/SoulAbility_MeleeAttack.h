// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SoulGameGAS/SoulGameplayAbility.h"
#include "SoulAbility_MeleeAttack.generated.h"

/**
 * SoulAbility_MeleeAttack - 近战攻击技能
 * 
 * 支持拳法和剑术两种攻击模式
 * 自动处理：
 * - 体力消耗检查
 * - 随机动画播放（避免连续重复）
 * - 攻击状态切换
 * - 摄像机震动反馈
 */
UCLASS()
class SOULGAME_API USoulAbility_MeleeAttack : public USoulGameplayAbility
{
	GENERATED_BODY()

public:
	USoulAbility_MeleeAttack();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	/** 攻击伤害值 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SoulGame|Ability|Attack")
	float AttackDamage = 30.0f;

	/** 是否触发摄像机震动 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SoulGame|Ability|Attack")
	bool bCameraShake = true;

	/** 退出攻击状态的延迟时间（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SoulGame|Ability|Attack")
	float ExitAttackStateDelay = 10.0f;
};
