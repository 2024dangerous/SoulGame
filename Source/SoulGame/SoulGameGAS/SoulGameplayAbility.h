// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "SoulGameplayAbility.generated.h"

class USoulAbilitySystemComponent;
class USoulAttributeSet;

/**
 * 技能激活策略枚举
 */
UENUM(BlueprintType)
enum class ESoulAbilityActivationPolicy : uint8
{
	// 输入触发（按键按下时激活）
	OnInputTriggered,
	// 授予时立即激活（被动技能）
	OnGranted,
	// 由事件触发（GameplayEvent）
	OnEventReceived
};

/**
 * SoulGameplayAbility - 技能基类
 * 
 * 所有 SoulGame 的技能都应继承此类
 * 提供：
 * - 统一的技能激活策略
 * - 体力消耗检查
 * - 动画蒙太奇播放
 * - 与 SoulEventManager 集成
 * - 冷却时间管理
 */
UCLASS(Abstract)
class SOULGAME_API USoulGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	USoulGameplayAbility();

	// ============ 技能配置 ============

	/** 技能激活策略 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SoulGame|Ability")
	ESoulAbilityActivationPolicy ActivationPolicy = ESoulAbilityActivationPolicy::OnInputTriggered;

	/** 技能消耗的体力值（0表示不消耗） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SoulGame|Ability|Cost")
	float StaminaCost = 0.0f;

	/** 技能消耗的法力值（0表示不消耗） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SoulGame|Ability|Cost")
	float ManaCost = 0.0f;

	/** 技能播放的动画蒙太奇列表（支持随机播放） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SoulGame|Ability|Animation")
	TArray<UAnimMontage*> AbilityMontages;

	/** 动画播放速率 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SoulGame|Ability|Animation")
	float MontagePlayRate = 1.0f;

	/** 体力不足时的提示文本 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SoulGame|Ability|UI")
	FText InsufficientStaminaText;

	/** 法力不足时的提示文本 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SoulGame|Ability|UI")
	FText InsufficientManaText;

	// ============ GAS 回调重写 ============

	/** 技能是否可以激活 */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	/** 技能激活 */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** 技能结束 */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	/** 技能被授予时调用 */
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	// ============ 便捷方法 ============

protected:
	/**
	 * 检查体力是否足够
	 * @return 体力是否足够执行此技能
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Ability")
	bool HasEnoughStamina() const;

	/**
	 * 检查法力是否足够
	 * @return 法力是否足够执行此技能
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Ability")
	bool HasEnoughMana() const;

	/**
	 * 消耗体力
	 * @param Amount 消耗量（默认使用 StaminaCost）
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Ability")
	void ConsumeStamina(float Amount = -1.0f);

	/**
	 * 消耗法力
	 * @param Amount 消耗量（默认使用 ManaCost）
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Ability")
	void ConsumeMana(float Amount = -1.0f);

	/**
	 * 播放随机动画蒙太奇
	 * @return 播放的蒙太奇（nullptr 表示失败）
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Ability")
	UAnimMontage* PlayRandomMontage();

	/**
	 * 播放指定索引的动画蒙太奇
	 * @param Index 蒙太奇索引
	 * @return 播放的蒙太奇（nullptr 表示失败）
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Ability")
	UAnimMontage* PlayMontageByIndex(int32 Index);

	/**
	 * 获取 SoulAbilitySystemComponent
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Ability")
	USoulAbilitySystemComponent* GetSoulAbilitySystemComponent() const;

	/**
	 * 获取 SoulAttributeSet
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Ability")
	const USoulAttributeSet* GetSoulAttributeSet() const;

	/**
	 * 获取拥有者角色
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Ability")
	ACharacter* GetOwnerCharacter() const;

	// 上次播放的蒙太奇索引（用于避免连续重复）
	int32 LastMontageIndex = -1;

	// 蒙太奇结束回调
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageBlendOut();

	UFUNCTION()
	void OnMontageCancelled();
};
