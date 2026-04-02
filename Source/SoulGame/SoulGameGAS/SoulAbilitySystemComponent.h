// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SoulAbilitySystemComponent.generated.h"

class USoulGameplayAbility;

/**
 * SoulAbilitySystemComponent - 自定义技能系统组件
 * 
 * 基于 UE5 GAS (GameplayAbilitySystem) 的封装
 * 提供：
 * - 初始化默认技能
 * - 技能授予/移除便捷接口
 * - 与 SoulEventManager 的集成
 */
UCLASS(ClassGroup = (SoulGame), meta = (BlueprintSpawnableComponent))
class SOULGAME_API USoulAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	USoulAbilitySystemComponent();

	// ============ 初始化 ============

	/**
	 * 初始化默认技能
	 * 在角色 BeginPlay 或 PossessedBy 中调用
	 * @param DefaultAbilities 默认授予的技能类列表
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GAS")
	void InitializeDefaultAbilities(const TArray<TSubclassOf<USoulGameplayAbility>>& DefaultAbilities);

	/**
	 * 授予单个技能
	 * @param AbilityClass 技能类
	 * @param Level 技能等级（默认1）
	 * @return 技能句柄
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GAS")
	FGameplayAbilitySpecHandle GrantAbility(TSubclassOf<USoulGameplayAbility> AbilityClass, int32 Level = 1);

	/**
	 * 移除技能
	 * @param AbilityClass 要移除的技能类
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GAS")
	void RemoveAbilityByClass(TSubclassOf<USoulGameplayAbility> AbilityClass);

	/**
	 * 尝试激活指定类的技能
	 * @param AbilityClass 技能类
	 * @return 是否成功激活
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GAS")
	bool TryActivateAbilityByClassSoul(TSubclassOf<USoulGameplayAbility> AbilityClass);

	/**
	 * 通过 GameplayTag 激活技能
	 * @param AbilityTag 技能标签
	 * @return 是否成功激活
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GAS")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTag);

	/**
	 * 取消所有带有指定标签的技能
	 * @param CancelTag 取消标签
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GAS")
	void CancelAbilitiesByTag(FGameplayTagContainer CancelTags);

	/**
	 * 获取当前激活的技能数量
	 */
	UFUNCTION(BlueprintPure, Category = "SoulGame|GAS")
	int32 GetActiveAbilityCount() const;

	/**
	 * 检查是否有指定标签的技能正在激活
	 */
	UFUNCTION(BlueprintPure, Category = "SoulGame|GAS")
	bool IsAbilityActiveByTag(FGameplayTag AbilityTag) const;

protected:
	// 已授予的技能句柄缓存（用于快速查找和移除）
	UPROPERTY()
	TMap<TSubclassOf<UGameplayAbility>, FGameplayAbilitySpecHandle> GrantedAbilityHandles;

	// 是否已初始化默认技能
	bool bDefaultAbilitiesInitialized = false;
};
