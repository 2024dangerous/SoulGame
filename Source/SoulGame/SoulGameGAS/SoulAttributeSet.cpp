// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameGAS/SoulAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffect.h"
#include "Net/UnrealNetwork.h"

USoulAttributeSet::USoulAttributeSet()
{
	// 初始化默认属性值
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	InitHealthRegenRate(1.0f);

	InitMana(100.0f);
	InitMaxMana(100.0f);

	InitStamina(100.0f);
	InitMaxStamina(100.0f);
	InitStaminaRegenRate(10.0f);

	InitAttackPower(10.0f);
	InitDefensePower(5.0f);
	InitDamage(0.0f);
}

void USoulAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Clamp 属性值到合理范围
	ClampAttribute(Attribute, NewValue);
}

void USoulAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// 获取上下文信息
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* SourceASC = EffectContext.GetOriginalInstigatorAbilitySystemComponent();
	AActor* SourceActor = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetActor = nullptr;

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
	}

	// ============ 处理伤害元属性 ============
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// 获取伤害值
		float LocalDamage = GetDamage();
		SetDamage(0.0f); // 重置元属性

		if (LocalDamage > 0.0f)
		{
			// 考虑防御力减伤
			float FinalDamage = FMath::Max(LocalDamage - GetDefensePower(), 0.0f);

			// 应用伤害到生命值
			float NewHealth = FMath::Max(GetHealth() - FinalDamage, 0.0f);
			SetHealth(NewHealth);

			UE_LOG(LogTemp, Log, TEXT("SoulAttributeSet::PostGameplayEffectExecute - 伤害: %.1f (原始: %.1f, 防御: %.1f), 剩余生命: %.1f"),
				FinalDamage, LocalDamage, GetDefensePower(), NewHealth);

			// 死亡判定
			if (NewHealth <= 0.0f)
			{
				UE_LOG(LogTemp, Log, TEXT("SoulAttributeSet::PostGameplayEffectExecute - 目标死亡: %s"),
					TargetActor ? *TargetActor->GetName() : TEXT("Unknown"));
				// 死亡逻辑由 Ability 或 Character 处理
			}
		}
	}

	// ============ Clamp 生命值 ============
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}

	// ============ Clamp 法力值 ============
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
	}

	// ============ Clamp 体力值 ============
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
	}
}

void USoulAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 注意：当前为单机游戏，暂不需要网络复制
	// 如果未来需要联机，在此添加 DOREPLIFETIME 宏
}

void USoulAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}
