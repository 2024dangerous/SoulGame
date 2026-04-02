// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SoulGameGAS/SoulGameplayAbility.h"
#include "SoulAbility_Defense.generated.h"

/**
 * SoulAbility_Defense - 防御技能
 * 
 * 持续型技能：按住激活，松开结束
 * 自动处理：
 * - 防御状态标签管理
 * - 持续体力消耗（可选）
 * - 受击减伤
 */
UCLASS()
class SOULGAME_API USoulAbility_Defense : public USoulGameplayAbility
{
	GENERATED_BODY()

public:
	USoulAbility_Defense();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	/** 防御时的减伤比例（0.0 ~ 1.0） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SoulGame|Ability|Defense", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DamageReduction = 0.5f;

	/** 是否需要装备剑才能防御 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SoulGame|Ability|Defense")
	bool bRequiresSword = true;
};
