// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
//#include "UObject.h"
#include "GameplayTags.h"
#include "SoulGameTagsManager.generated.h"

class USoulTagsConfig;

/**
 * SoulGameTagsManager - GameplayTags 管理单例
 * 提供获取和注册游戏标签的接口
 * 
 * 支持两种模式：
 * 1. DataAsset 模式（推荐）- 从 USoulTagsConfig 加载
 * 2. 硬编码模式（兼容）- 使用内置默认值
 */
UCLASS(Blueprintable, BlueprintType)
class SOULGAME_API USoulGameTagsManager : public UObject
{
	GENERATED_BODY()

public:
	USoulGameTagsManager();

	// ============ 单例管理 ============

	// 获取单例实例
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	static USoulGameTagsManager* Get();

	/**
	 * 初始化标签系统
	 * @param bUseDataAsset 是否使用 DataAsset 配置（推荐开启）
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	void Initialize(bool bUseDataAsset = true);

	/**
	 * 设置标签配置 DataAsset
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	void SetTagsConfig(USoulTagsConfig* InConfig);

	/**
	 * 获取当前标签配置
	 */
	UFUNCTION(BlueprintPure, Category = "SoulGame|GameplayTags")
	USoulTagsConfig* GetTagsConfig() const { return TagsConfig; }

	// ============ 标签获取接口 ============

	// 获取所有注册的标签
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	FGameplayTagContainer GetAllTags() const;

	// 根据标签名获取标签（会尝试匹配已注册的标签）
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	FGameplayTag GetTag(const FName& TagName) const;

	// 获取行为状态标签
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	FGameplayTag GetBehaviorTag(const FName& BehaviorName) const;

	// 获取武器状态标签
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	FGameplayTag GetWeaponTag(const FName& WeaponName) const;

	// 获取战斗状态标签
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	FGameplayTag GetCombatTag(const FName& CombatName) const;

	// 获取事件标签
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	FGameplayTag GetEventTag(const FName& EventName) const;

	// 获取技能标签
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	FGameplayTag GetSkillTag(const FName& SkillName) const;

	// 获取效果标签
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	FGameplayTag GetEffectTag(const FName& EffectName) const;

	// 获取标签显示名
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	FText GetTagDisplayName(const FGameplayTag& Tag) const;

	// ============ 标签缓存 ============

protected:
	// 标签配置 DataAsset（可选）
	UPROPERTY()
	USoulTagsConfig* TagsConfig;

	// 是否使用 DataAsset 模式
	UPROPERTY()
	bool bUseDataAssetMode;

	// 行为状态标签缓存
	UPROPERTY()
	TMap<FName, FGameplayTag> BehaviorTags;

	// 武器状态标签缓存
	UPROPERTY()
	TMap<FName, FGameplayTag> WeaponTags;

	// 战斗状态标签缓存
	UPROPERTY()
	TMap<FName, FGameplayTag> CombatTags;

	// 事件标签缓存
	UPROPERTY()
	TMap<FName, FGameplayTag> EventTags;

	// 技能标签缓存
	UPROPERTY()
	TMap<FName, FGameplayTag> SkillTags;

	// 效果标签缓存
	UPROPERTY()
	TMap<FName, FGameplayTag> EffectTags;

private:
	static USoulGameTagsManager* Singleton;
};