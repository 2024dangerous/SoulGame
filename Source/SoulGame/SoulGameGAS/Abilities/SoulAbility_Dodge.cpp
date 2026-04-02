// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameGAS/Abilities/SoulAbility_Dodge.h"
#include "SoulGameGAS/SoulAbilitySystemComponent.h"
#include "SoulGameGAS/SoulAttributeSet.h"
#include "SoulGameCharacter/SoulBaseCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

USoulAbility_Dodge::USoulAbility_Dodge()
{
	ActivationPolicy = ESoulAbilityActivationPolicy::OnInputTriggered;
	StaminaCost = 20.0f;
	MontagePlayRate = 1.0f;

	// 技能标签
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Skill.Dodge.Roll")));

	// 激活时添加翻滚状态标签
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Rolling")));

	// 攻击、受伤、死亡状态下不能翻滚
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Attack")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Injury")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Dead")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Rolling")));
}

bool USoulAbility_Dodge::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void USoulAbility_Dodge::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 根据方向选择翻滚动画
	int32 AnimIndex = GetRollingAnimIndex();
	UAnimMontage* PlayedMontage = PlayMontageByIndex(AnimIndex);

	if (!PlayedMontage)
	{
		// 如果指定方向没有动画，尝试播放随机的
		PlayedMontage = PlayRandomMontage();
		if (!PlayedMontage)
		{
			UE_LOG(LogTemp, Warning, TEXT("SoulAbility_Dodge::ActivateAbility - 没有可播放的翻滚动画"));
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
	}

	// 摄像机震动
	if (bCameraShake)
	{
		if (ASoulBaseCharacter* Character = Cast<ASoulBaseCharacter>(GetOwnerCharacter()))
		{
			Character->CameraShakeFeedBack(false);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("SoulAbility_Dodge::ActivateAbility - 翻滚技能激活，方向索引: %d"), AnimIndex);
}

void USoulAbility_Dodge::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	UE_LOG(LogTemp, Log, TEXT("SoulAbility_Dodge::EndAbility - 翻滚技能结束"));

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

int32 USoulAbility_Dodge::GetRollingAnimIndex() const
{
	ACharacter* Character = GetOwnerCharacter();
	if (!Character)
	{
		return 0;
	}

	// 获取最后的输入方向
	FVector LastInput = Character->GetCharacterMovement()->GetLastInputVector();
	if (LastInput.IsNearlyZero())
	{
		return 0; // 默认前翻
	}

	// 转换为角色本地空间
	FVector Forward = Character->GetActorForwardVector();
	FVector Right = Character->GetActorRightVector();

	float ForwardDot = FVector::DotProduct(LastInput.GetSafeNormal(), Forward);
	float RightDot = FVector::DotProduct(LastInput.GetSafeNormal(), Right);

	// 判断主要方向
	if (FMath::Abs(ForwardDot) > FMath::Abs(RightDot))
	{
		return ForwardDot >= 0 ? 0 : 1; // 0=前, 1=后
	}
	else
	{
		return RightDot >= 0 ? 3 : 2; // 2=左, 3=右
	}
}
