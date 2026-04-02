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
 * 鏀寔涓ょ妯″紡锛?
 * 1. DataAsset 模式（推荐）- 浠?USoulTagsConfig 加载
 * 2. 纭紪鐮佹ā寮忥紙鍏煎锛? 浣跨敤鍐呯疆榛樿鍊?
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
	 * 鍒濆鍖栨爣绛剧郴缁?
	 * @param bUseDataAsset 鏄惁浣跨敤 DataAsset 閰嶇疆锛堟帹鑽愬紑鍚級
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	void Initialize(bool bUseDataAsset = true);

	/**
	 * 璁剧疆鏍囩閰嶇疆 DataAsset
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	void SetTagsConfig(USoulTagsConfig* InConfig);

	/**
	 * 鑾峰彇褰撳墠鏍囩閰嶇疆
	 */
	UFUNCTION(BlueprintPure, Category = "SoulGame|GameplayTags")
	USoulTagsConfig* GetTagsConfig() const { return TagsConfig; }

	// ============ 鏍囩鑾峰彇鎺ュ彛 ============

	// 鑾峰彇鎵€鏈夋敞鍐岀殑鏍囩
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	FGameplayTagContainer GetAllTags() const;

	// 鏍规嵁鏍囩鍚嶈幏鍙栨爣绛撅紙浼氬皾璇曞尮閰嶅凡娉ㄥ唽鐨勬爣绛撅級
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	FGameplayTag GetTag(const FName& TagName) const;

	// 鑾峰彇琛屼负鐘舵€佹爣绛?
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	FGameplayTag GetBehaviorTag(const FName& BehaviorName) const;

	// 鑾峰彇姝﹀櫒鐘舵€佹爣绛?
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	FGameplayTag GetWeaponTag(const FName& WeaponName) const;

	// 鑾峰彇鎴樻枟鐘舵€佹爣绛?
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	FGameplayTag GetCombatTag(const FName& CombatName) const;

	// 鑾峰彇浜嬩欢鏍囩
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	FGameplayTag GetEventTag(const FName& EventName) const;

	// 鑾峰彇鎶€鑳芥爣绛?
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	FGameplayTag GetSkillTag(const FName& SkillName) const;

	// 鑾峰彇鏁堟灉鏍囩
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	FGameplayTag GetEffectTag(const FName& EffectName) const;

	// 鑾峰彇鏍囩鏄剧ず鍚?
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GameplayTags")
	FText GetTagDisplayName(const FGameplayTag& Tag) const;

	// ============ 鏍囩缂撳瓨 ============

protected:
	// 鏍囩閰嶇疆 DataAsset（可选）
	UPROPERTY()
	USoulTagsConfig* TagsConfig;

	// 鏄惁浣跨敤 DataAsset 模式
	UPROPERTY()
	bool bUseDataAssetMode;

	// 琛屼负鐘舵€佹爣绛剧紦瀛?
	UPROPERTY()
	TMap<FName, FGameplayTag> BehaviorTags;

	// 姝﹀櫒鐘舵€佹爣绛剧紦瀛?
	UPROPERTY()
	TMap<FName, FGameplayTag> WeaponTags;

	// 鎴樻枟鐘舵€佹爣绛剧紦瀛?
	UPROPERTY()
	TMap<FName, FGameplayTag> CombatTags;

	// 浜嬩欢鏍囩缂撳瓨
	UPROPERTY()
	TMap<FName, FGameplayTag> EventTags;

	// 鎶€鑳芥爣绛剧紦瀛?
	UPROPERTY()
	TMap<FName, FGameplayTag> SkillTags;

	// 鏁堟灉鏍囩缂撳瓨
	UPROPERTY()
	TMap<FName, FGameplayTag> EffectTags;

private:
	static USoulGameTagsManager* Singleton;
};