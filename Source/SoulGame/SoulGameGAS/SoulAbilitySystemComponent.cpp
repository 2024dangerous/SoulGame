// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameGAS/SoulAbilitySystemComponent.h"
#include "SoulGameGAS/SoulGameplayAbility.h"

USoulAbilitySystemComponent::USoulAbilitySystemComponent()
{
	// 默认设置：允许复制
	SetIsReplicatedByDefault(true);
}

void USoulAbilitySystemComponent::InitializeDefaultAbilities(const TArray<TSubclassOf<USoulGameplayAbility>>& DefaultAbilities)
{
	if (bDefaultAbilitiesInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulAbilitySystemComponent::InitializeDefaultAbilities - 默认技能已初始化，跳过"));
		return;
	}

	for (const TSubclassOf<USoulGameplayAbility>& AbilityClass : DefaultAbilities)
	{
		if (AbilityClass)
		{
			GrantAbility(AbilityClass);
		}
	}

	bDefaultAbilitiesInitialized = true;
	UE_LOG(LogTemp, Log, TEXT("USoulAbilitySystemComponent::InitializeDefaultAbilities - 已授予 %d 个默认技能"), DefaultAbilities.Num());
}

FGameplayAbilitySpecHandle USoulAbilitySystemComponent::GrantAbility(TSubclassOf<USoulGameplayAbility> AbilityClass, int32 Level)
{
	if (!AbilityClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulAbilitySystemComponent::GrantAbility - AbilityClass 为空"));
		return FGameplayAbilitySpecHandle();
	}

	// 检查是否已授予
	if (GrantedAbilityHandles.Contains(AbilityClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulAbilitySystemComponent::GrantAbility - 技能 %s 已授予"), *AbilityClass->GetName());
		return GrantedAbilityHandles[AbilityClass];
	}

	// 创建技能规格
	FGameplayAbilitySpec AbilitySpec(AbilityClass, Level, INDEX_NONE, GetOwner());
	FGameplayAbilitySpecHandle Handle = GiveAbility(AbilitySpec);

	// 缓存句柄
	GrantedAbilityHandles.Add(AbilityClass, Handle);

	UE_LOG(LogTemp, Log, TEXT("USoulAbilitySystemComponent::GrantAbility - 授予技能: %s (Level: %d)"), *AbilityClass->GetName(), Level);
	return Handle;
}

void USoulAbilitySystemComponent::RemoveAbilityByClass(TSubclassOf<USoulGameplayAbility> AbilityClass)
{
	if (!AbilityClass)
	{
		return;
	}

	FGameplayAbilitySpecHandle Handle;
	if (GrantedAbilityHandles.RemoveAndCopyValue(AbilityClass, Handle))
	{
		ClearAbility(Handle);
		UE_LOG(LogTemp, Log, TEXT("USoulAbilitySystemComponent::RemoveAbilityByClass - 移除技能: %s"), *AbilityClass->GetName());
	}
}

bool USoulAbilitySystemComponent::TryActivateAbilityByClassSoul(TSubclassOf<USoulGameplayAbility> AbilityClass)
{
	if (!AbilityClass)
	{
		return false;
	}

	return TryActivateAbilityByClass(AbilityClass);
}

bool USoulAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTag)
{
	if (!AbilityTag.IsValid())
	{
		return false;
	}

	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(AbilityTag);
	return TryActivateAbilitiesByTag(TagContainer);
}

void USoulAbilitySystemComponent::CancelAbilitiesByTag(FGameplayTagContainer CancelTags)
{
	// 遍历所有激活的技能，取消匹配标签的
	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.IsActive() && Spec.Ability)
		{
			FGameplayTagContainer AbilityTags = Spec.Ability->AbilityTags.Filter(CancelTags);
			if (AbilityTags.Num() > 0)
			{
				CancelAbilityHandle(Spec.Handle);
			}
		}
	}
}

int32 USoulAbilitySystemComponent::GetActiveAbilityCount() const
{
	int32 Count = 0;
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.IsActive())
		{
			Count++;
		}
	}
	return Count;
}

bool USoulAbilitySystemComponent::IsAbilityActiveByTag(FGameplayTag AbilityTag) const
{
	if (!AbilityTag.IsValid())
	{
		return false;
	}

	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(AbilityTag);

	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.IsActive() && Spec.Ability)
		{
			if (Spec.Ability->AbilityTags.HasAny(TagContainer))
			{
				return true;
			}
		}
	}
	return false;
}
