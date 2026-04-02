// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SoulGameGAS/SoulGameplayAbility.h"
#include "SoulAbility_Dodge.generated.h"

/**
 * SoulAbility_Dodge - 闪避/翻滚技能
 * 
 * 根据输入方向播放不同的翻滚动画
 * 自动处理：
 * - 体力消耗
 * - 方向计算
 * - 无敌帧（通过 GameplayTag）
 * - 摄像机震动
 */
UCLASS()
class SOULGAME_API USoulAbility_Dodge : public USoulGameplayAbility
{
	GENERATED_BODY()

public:
	USoulAbility_Dodge();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	/**
	 * 根据输入方向获取翻滚动画索引
	 * 0=前, 1=后, 2=左, 3=右
	 */
	int32 GetRollingAnimIndex() const;

	/** 是否触发摄像机震动 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SoulGame|Ability|Dodge")
	bool bCameraShake = true;
};
