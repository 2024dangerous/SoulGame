// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameGAS/Abilities/SoulAbility_MeleeAttack.h"
#include "SoulGameGAS/SoulAbilitySystemComponent.h"
#include "SoulGameGAS/SoulAttributeSet.h"
#include "SoulGameCharacter/SoulBaseCharacter.h"
#include "GameFramework/Character.h"

USoulAbility_MeleeAttack::USoulAbility_MeleeAttack()
{
	// 默认配置
	ActivationPolicy = ESoulAbilityActivationPolicy::OnInputTriggered;
	StaminaCost = 10.0f;
	MontagePlayRate = 1.0f;

	// 技能标签配置
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Skill.Melee.Combo")));

	// 激活时阻止其他攻击技能
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Attack")));

	// 激活时添加攻击状态标签
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Attack")));

	// 翻滚和受伤状态下不能攻击
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Rolling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Injury")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Dead")));
}

void USoulAbility_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// 调用父类（处理 CommitAbility 和资源消耗）
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 播放随机攻击动画
	UAnimMontage* PlayedMontage = PlayRandomMontage();
	if (!PlayedMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("SoulAbility_MeleeAttack::ActivateAbility - 没有可播放的攻击动画"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 摄像机震动反馈
	if (bCameraShake)
	{
		if (ASoulBaseCharacter* Character = Cast<ASoulBaseCharacter>(GetOwnerCharacter()))
		{
			Character->CameraShakeFeedBack(false);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("SoulAbility_MeleeAttack::ActivateAbility - 攻击技能激活，伤害: %.1f"), AttackDamage);
}

void USoulAbility_MeleeAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	UE_LOG(LogTemp, Log, TEXT("SoulAbility_MeleeAttack::EndAbility - 攻击技能结束"));

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
