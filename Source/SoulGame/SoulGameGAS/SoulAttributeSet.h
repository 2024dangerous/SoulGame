// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "SoulAttributeSet.generated.h"

// 使用 GAS 宏简化属性访问器定义
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * SoulAttributeSet - 角色属性集
 * 
 * 使用 GAS AttributeSet 统一管理角色的所有属性
 * 替代 SoulBaseCharacter 中散落的 Health/Mana/Stamina 变量
 * 
 * 特性：
 * - 属性变化自动通知（通过 GAS 的 AttributeChange 委托）
 * - 支持 GameplayEffect 修改属性
 * - 属性值自动 Clamp
 * - 与 UI 系统自动同步
 */
UCLASS()
class SOULGAME_API USoulAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	USoulAttributeSet();

	// ============ 生命值属性 ============

	/** 当前生命值 */
	UPROPERTY(BlueprintReadOnly, Category = "SoulGame|Attributes|Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(USoulAttributeSet, Health)

	/** 最大生命值 */
	UPROPERTY(BlueprintReadOnly, Category = "SoulGame|Attributes|Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(USoulAttributeSet, MaxHealth)

	/** 生命恢复速率（每秒） */
	UPROPERTY(BlueprintReadOnly, Category = "SoulGame|Attributes|Health")
	FGameplayAttributeData HealthRegenRate;
	ATTRIBUTE_ACCESSORS(USoulAttributeSet, HealthRegenRate)

	// ============ 法力值属性 ============

	/** 当前法力值 */
	UPROPERTY(BlueprintReadOnly, Category = "SoulGame|Attributes|Mana")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(USoulAttributeSet, Mana)

	/** 最大法力值 */
	UPROPERTY(BlueprintReadOnly, Category = "SoulGame|Attributes|Mana")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(USoulAttributeSet, MaxMana)

	// ============ 体力值属性 ============

	/** 当前体力值 */
	UPROPERTY(BlueprintReadOnly, Category = "SoulGame|Attributes|Stamina")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(USoulAttributeSet, Stamina)

	/** 最大体力值 */
	UPROPERTY(BlueprintReadOnly, Category = "SoulGame|Attributes|Stamina")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(USoulAttributeSet, MaxStamina)

	/** 体力恢复速率（每秒） */
	UPROPERTY(BlueprintReadOnly, Category = "SoulGame|Attributes|Stamina")
	FGameplayAttributeData StaminaRegenRate;
	ATTRIBUTE_ACCESSORS(USoulAttributeSet, StaminaRegenRate)

	// ============ 战斗属性 ============

	/** 攻击力 */
	UPROPERTY(BlueprintReadOnly, Category = "SoulGame|Attributes|Combat")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(USoulAttributeSet, AttackPower)

	/** 防御力 */
	UPROPERTY(BlueprintReadOnly, Category = "SoulGame|Attributes|Combat")
	FGameplayAttributeData DefensePower;
	ATTRIBUTE_ACCESSORS(USoulAttributeSet, DefensePower)

	/**
	 * 伤害元属性（Meta Attribute）
	 * 不持久化，仅用于伤害计算流程
	 * GameplayEffect 设置此值 → PreAttributeChange 中计算最终伤害 → 应用到 Health
	 */
	UPROPERTY(BlueprintReadOnly, Category = "SoulGame|Attributes|Combat")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(USoulAttributeSet, Damage)

	// ============ GAS 回调 ============

	/**
	 * 属性值变化前的预处理
	 * 用于 Clamp 属性值到合理范围
	 */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/**
	 * GameplayEffect 执行后的回调
	 * 用于处理伤害计算、死亡判定等
	 */
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	/**
	 * 获取需要复制的属性列表（网络同步用）
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	/**
	 * 将属性值 Clamp 到 [0, MaxValue] 范围
	 */
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
};
