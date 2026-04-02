// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameGAS/SoulGameplayAbility.h"
#include "SoulGameGAS/SoulAbilitySystemComponent.h"
#include "SoulGameGAS/SoulAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"

USoulGameplayAbility::USoulGameplayAbility()
{
	// 默认设置
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// 默认提示文本
	InsufficientStaminaText = FText::FromString(TEXT("体力不足"));
	InsufficientManaText = FText::FromString(TEXT("法力不足"));
}

bool USoulGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	// 检查体力
	if (StaminaCost > 0.0f && !HasEnoughStamina())
	{
		return false;
	}

	// 检查法力
	if (ManaCost > 0.0f && !HasEnoughMana())
	{
		return false;
	}

	return true;
}

void USoulGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 消耗资源
	if (StaminaCost > 0.0f)
	{
		ConsumeStamina();
	}
	if (ManaCost > 0.0f)
	{
		ConsumeMana();
	}

	UE_LOG(LogTemp, Log, TEXT("SoulGameplayAbility::ActivateAbility - 技能激活: %s"), *GetName());
}

void USoulGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	UE_LOG(LogTemp, Log, TEXT("SoulGameplayAbility::EndAbility - 技能结束: %s (取消: %s)"),
		*GetName(), bWasCancelled ? TEXT("是") : TEXT("否"));

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USoulGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	// 如果策略是授予时激活，则立即尝试激活
	if (ActivationPolicy == ESoulAbilityActivationPolicy::OnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}

bool USoulGameplayAbility::HasEnoughStamina() const
{
	const USoulAttributeSet* AttrSet = GetSoulAttributeSet();
	if (!AttrSet)
	{
		return true; // 没有属性集时默认允许
	}
	return AttrSet->GetStamina() >= StaminaCost;
}

bool USoulGameplayAbility::HasEnoughMana() const
{
	const USoulAttributeSet* AttrSet = GetSoulAttributeSet();
	if (!AttrSet)
	{
		return true;
	}
	return AttrSet->GetMana() >= ManaCost;
}

void USoulGameplayAbility::ConsumeStamina(float Amount)
{
	float ActualCost = (Amount < 0.0f) ? StaminaCost : Amount;
	if (ActualCost <= 0.0f) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	const USoulAttributeSet* AttrSet = GetSoulAttributeSet();
	if (!AttrSet) return;

	// 直接修改属性值（简单方式，后续可改为 GameplayEffect）
	float NewStamina = FMath::Max(AttrSet->GetStamina() - ActualCost, 0.0f);
	ASC->SetNumericAttributeBase(USoulAttributeSet::GetStaminaAttribute(), NewStamina);

	UE_LOG(LogTemp, Log, TEXT("SoulGameplayAbility::ConsumeStamina - 消耗体力: %.1f, 剩余: %.1f"), ActualCost, NewStamina);
}

void USoulGameplayAbility::ConsumeMana(float Amount)
{
	float ActualCost = (Amount < 0.0f) ? ManaCost : Amount;
	if (ActualCost <= 0.0f) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	const USoulAttributeSet* AttrSet = GetSoulAttributeSet();
	if (!AttrSet) return;

	float NewMana = FMath::Max(AttrSet->GetMana() - ActualCost, 0.0f);
	ASC->SetNumericAttributeBase(USoulAttributeSet::GetManaAttribute(), NewMana);

	UE_LOG(LogTemp, Log, TEXT("SoulGameplayAbility::ConsumeMana - 消耗法力: %.1f, 剩余: %.1f"), ActualCost, NewMana);
}

UAnimMontage* USoulGameplayAbility::PlayRandomMontage()
{
	if (AbilityMontages.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SoulGameplayAbility::PlayRandomMontage - 没有配置动画蒙太奇"));
		return nullptr;
	}

	// 随机选择一个不同于上次的蒙太奇
	int32 NewIndex;
	if (AbilityMontages.Num() == 1)
	{
		NewIndex = 0;
	}
	else
	{
		do
		{
			NewIndex = FMath::RandRange(0, AbilityMontages.Num() - 1);
		} while (NewIndex == LastMontageIndex && AbilityMontages.Num() > 1);
	}

	LastMontageIndex = NewIndex;
	return PlayMontageByIndex(NewIndex);
}

UAnimMontage* USoulGameplayAbility::PlayMontageByIndex(int32 Index)
{
	if (!AbilityMontages.IsValidIndex(Index) || !AbilityMontages[Index])
	{
		UE_LOG(LogTemp, Warning, TEXT("SoulGameplayAbility::PlayMontageByIndex - 无效索引: %d"), Index);
		return nullptr;
	}

	UAnimMontage* MontageToPlay = AbilityMontages[Index];

	// 使用 GAS 内置的蒙太奇播放（自动处理中断和结束）
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return nullptr;
	}

	// 获取角色的动画实例
	ACharacter* Character = GetOwnerCharacter();
	if (!Character || !Character->GetMesh() || !Character->GetMesh()->GetAnimInstance())
	{
		return nullptr;
	}

	// 播放蒙太奇
	float Duration = Character->GetMesh()->GetAnimInstance()->Montage_Play(MontageToPlay, MontagePlayRate);
	if (Duration <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("SoulGameplayAbility::PlayMontageByIndex - 蒙太奇播放失败: %s"), *MontageToPlay->GetName());
		return nullptr;
	}

	UE_LOG(LogTemp, Log, TEXT("SoulGameplayAbility::PlayMontageByIndex - 播放蒙太奇: %s (索引: %d, 时长: %.2f)"),
		*MontageToPlay->GetName(), Index, Duration);

	return MontageToPlay;
}

USoulAbilitySystemComponent* USoulGameplayAbility::GetSoulAbilitySystemComponent() const
{
	return Cast<USoulAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
}

const USoulAttributeSet* USoulGameplayAbility::GetSoulAttributeSet() const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return nullptr;

	return ASC->GetSet<USoulAttributeSet>();
}

ACharacter* USoulGameplayAbility::GetOwnerCharacter() const
{
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		return Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	}
	return nullptr;
}

void USoulGameplayAbility::OnMontageCompleted()
{
	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void USoulGameplayAbility::OnMontageBlendOut()
{
	// 混合结束时也结束技能
	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void USoulGameplayAbility::OnMontageCancelled()
{
	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
