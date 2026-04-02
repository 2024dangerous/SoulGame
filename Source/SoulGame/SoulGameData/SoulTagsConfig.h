// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SoulGameConfigBase.h"
#include "GameplayTags.h"
#include "SoulTagsConfig.generated.h"

/**
 * 鏍囩閰嶇疆缁撴瀯浣?
 * 鐢ㄤ簬鍦?DataAsset 涓厤缃崟涓爣绛?
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

	// 鏍囩鍚嶇О锛堜笉鍚墠缂€锛屽 "Idle", "Attack"锛?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
	FName TagName;

	// 显示名称
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
	FText DisplayName;

	// 描述
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
	FText Description;

	// 鏄惁鍚敤
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
	bool bEnabled = true;
};

/**
 * 鏍囩鍒嗙被閰嶇疆
 */
USTRUCT(BlueprintType)
struct SOULGAME_API FTagCategoryConfig
{
	GENERATED_BODY()

public:
	// 鍒嗙被鍚嶇О锛堝 "Behavior", "Weapon", "Combat"锛?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
	FName CategoryName;

	// 鏍囩鍓嶇紑锛堝 "State.Behavior", "State.Weapon"锛?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
	FString TagPrefix;

	// 璇ュ垎绫讳笅鐨勬墍鏈夋爣绛?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
	TArray<FTagConfigEntry> Tags;

	// 鏄惁鍚敤璇ュ垎绫?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
	bool bEnabled = true;
};

/**
 * SoulTagsConfig - 鏍囩閰嶇疆 DataAsset
 * 
 * 替代 SoulGameTagsManager 涓殑纭紪鐮佹爣绛鹃厤缃?
 * 鏀寔閫氳繃 DataAsset 缂栬緫鍣ㄨ繘琛岄厤缃?
 * 鏀寔鐑洿鏂?
 */
UCLASS(BlueprintType, meta = (DisplayName = "Soul Tags Config"))
class SOULGAME_API USoulTagsConfig : public USoulGameConfigBase
{
	GENERATED_BODY()

public:
	USoulTagsConfig();

	// ============ 閰嶇疆鍐呭 ============

	// 琛屼负鐘舵€佹爣绛惧垎绫?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags|Behavior")
	FTagCategoryConfig BehaviorTags;

	// 姝﹀櫒鐘舵€佹爣绛惧垎绫?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags|Weapon")
	FTagCategoryConfig WeaponTags;

	// 鎴樻枟鐘舵€佹爣绛惧垎绫?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags|Combat")
	FTagCategoryConfig CombatTags;

	// 浜嬩欢鏍囩鍒嗙被
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags|Event")
	FTagCategoryConfig EventTags;

	// 鎶€鑳芥爣绛惧垎绫?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags|Skill")
	FTagCategoryConfig SkillTags;

	// 鏁堟灉鏍囩鍒嗙被
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags|Effect")
	FTagCategoryConfig EffectTags;

	// ============ 便捷访问方法 ============

	/**
	 * 鑾峰彇琛屼负鏍囩
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Tags")
	FGameplayTag GetBehaviorTag(FName TagName) const;

	/**
	 * 鑾峰彇姝﹀櫒鏍囩
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Tags")
	FGameplayTag GetWeaponTag(FName TagName) const;

	/**
	 * 鑾峰彇鎴樻枟鏍囩
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Tags")
	FGameplayTag GetCombatTag(FName TagName) const;

	/**
	 * 鑾峰彇浜嬩欢鏍囩
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Tags")
	FGameplayTag GetEventTag(FName TagName) const;

	/**
	 * 鑾峰彇鎶€鑳芥爣绛?
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Tags")
	FGameplayTag GetSkillTag(FName TagName) const;

	/**
	 * 鑾峰彇鏁堟灉鏍囩
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Tags")
	FGameplayTag GetEffectTag(FName TagName) const;

	/**
	 * 鑾峰彇鎵€鏈夐厤缃爣绛?
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Tags")
	FGameplayTagContainer GetAllTags() const;

	/**
	 * 鏍规嵁鏍囩鍚嶈幏鍙栨爣绛?
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