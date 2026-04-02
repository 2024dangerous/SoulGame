// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameGAS/Abilities/SoulAbility_Defense.h"
#include "SoulGameGAS/SoulAbilitySystemComponent.h"
#include "SoulGameGAS/SoulAttributeSet.h"
#include "SoulGameCharacter/SoulBaseCharacter.h"

USoulAbility_Defense::USoulAbility_Defense()
{
	ActivationPolicy = ESoulAbilityActivationPolicy::OnInputTriggered;
	StaminaCost = 0.0f; // 防御不消耗体力（受击时消耗）
	MontagePlayRate = 1.0f;

	// 技能标签
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Skill.Defense.Block")));

	// 激活时添加防御状态标签
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Defense")));

	// 攻击、翻滚、受伤、死亡状态下不能防御
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Attack")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Rolling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Injury")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Dead")));
}

void USoulAbility_Defense::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 检查是否需要装备剑
	if (bRequiresSword)
	{
		if (ASoulBaseCharacter* Character = Cast<ASoulBaseCharacter>(GetOwnerCharacter()))
		{
			if (Character->WeaponType != EWeaponType::SWORD)
			{
				UE_LOG(LogTemp, Log, TEXT("SoulAbility_Defense::ActivateAbility - 需要装备剑才能防御"));
				EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
				return;
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("SoulAbility_Defense::ActivateAbility - 防御技能激活，减伤: %.0f%%"), DamageReduction * 100.0f);
}

void USoulAbility_Defense::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	UE_LOG(LogTemp, Log, TEXT("SoulAbility_Defense::EndAbility - 防御技能结束"));

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USoulAbility_Defense::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	// 松开按键时结束防御
	if (IsActive())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}
