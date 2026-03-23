// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SoulGameConfigBase.h"
#include "GameplayTags.h"
#include "SoulTagsConfig.generated.h"

/**
 * 标签配置结构体
 * 用于在 DataAsset 中配置单个标签
 */
USTRUCT(BlueprintType)
struct SOULGAME_API FTagConfigEntry
{
	GENERATED_BODY()

public:
	FTagConfigEntry() = default;

	FTagConfigEntry(FName InTagName, FText InDisplayName, FText InDescription = FText())
		: TagName(InTagName)
		, DisplayName(InDisplayName)
		, Description(InDescription)
	{}

	// 标签名称（不含前缀，如 "Idle", "Attack"）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
	FName TagName;

	// 显示名称
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
	FText DisplayName;

	// 描述
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
	FText Description;

	// 是否启用
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
	bool bEnabled = true;
};

/**
 * 标签分类配置
 */
USTRUCT(BlueprintType)
struct SOULGAME_API FTagCategoryConfig
{
	GENERATED_BODY()

public:
	// 分类名称（如 "Behavior", "Weapon", "Combat"）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
	FName CategoryName;

	// 标签前缀（如 "State.Behavior", "State.Weapon"）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
	FString TagPrefix;

	// 该分类下的所有标签
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
	TArray<FTagConfigEntry> Tags;

	// 是否启用该分类
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
	bool bEnabled = true;
};

/**
 * SoulTagsConfig - 标签配置 DataAsset
 * 
 * 替代 SoulGameTagsManager 中的硬编码标签配置
 * 支持通过 DataAsset 编辑器进行配置
 * 支持热更新
 */
UCLASS(BlueprintType, meta = (DisplayName = "Soul Tags Config"))
class SOULGAME_API USoulTagsConfig : public USoulGameConfigBase
{
	GENERATED_BODY()

public:
	USoulTagsConfig();

	// ============ 配置内容 ============

	// 行为状态标签分类
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags|Behavior")
	FTagCategoryConfig BehaviorTags;

	// 武器状态标签分类
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags|Weapon")
	FTagCategoryConfig WeaponTags;

	// 战斗状态标签分类
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags|Combat")
	FTagCategoryConfig CombatTags;

	// 事件标签分类
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags|Event")
	FTagCategoryConfig EventTags;

	// 技能标签分类
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags|Skill")
	FTagCategoryConfig SkillTags;

	// 效果标签分类
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags|Effect")
	FTagCategoryConfig EffectTags;

	// ============ 便捷访问方法 ============

	/**
	 * 获取行为标签
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Tags")
	FGameplayTag GetBehaviorTag(FName TagName) const;

	/**
	 * 获取武器标签
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Tags")
	FGameplayTag GetWeaponTag(FName TagName) const;

	/**
	 * 获取战斗标签
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Tags")
	FGameplayTag GetCombatTag(FName TagName) const;

	/**
	 * 获取事件标签
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Tags")
	FGameplayTag GetEventTag(FName TagName) const;

	/**
	 * 获取技能标签
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Tags")
	FGameplayTag GetSkillTag(FName TagName) const;

	/**
	 * 获取效果标签
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Tags")
	FGameplayTag GetEffectTag(FName TagName) const;

	/**
	 * 获取所有配置标签
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Tags")
	FGameplayTagContainer GetAllTags() const;

	/**
	 * 根据标签名获取标签
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Tags")
	FGameplayTag GetTag(FName TagName) const;

protected:
	// ============ USoulGameConfigBase ============

	virtual bool ValidateConfig(FText& OutError) ;

private:
	// 内部：获取标签的辅助方法
	FGameplayTag GetTagInternal(const FTagCategoryConfig& Category, FName TagName) const;
};