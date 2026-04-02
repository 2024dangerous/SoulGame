// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTags.h"
#include "SoulGameplayTagInterface.generated.h"

/**
 * GameplayTag 接口
 * 鎵€鏈夐渶瑕佹敮鎸?GameplayTags 鐨?Actor 闇€瑕佸疄鐜版鎺ュ彛
 */
UINTERFACE(Blueprintable, BlueprintType)
class SOULGAME_API USoulGameplayTagInterface : public UInterface
{
	GENERATED_BODY()
};

class SOULGAME_API ISoulGameplayTagInterface
{
	GENERATED_BODY()

public:
	// 鑾峰彇鏍囩瀹瑰櫒
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FGameplayTagContainer GetGameplayTagContainer() const;

	// 娣诲姞鏍囩
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AddGameplayTag(FGameplayTag Tag);

	// 绉婚櫎鏍囩
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RemoveGameplayTag(FGameplayTag Tag);

	// 妫€鏌ユ槸鍚︽湁鎸囧畾鏍囩锛堢簿纭尮閰嶏級
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool HasGameplayTag(FGameplayTag Tag) const;

	// 妯＄硦鍖归厤鏍囩锛堝尮閰嶇埗鏍囩锛?
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool HasMatchingTag(FGameplayTag Tag) const;

	// 鑾峰彇鎵€鏈夋爣绛惧悕
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<FName> GetGameplayTagNames() const;

	// 鑾峰彇鏍囩鏄剧ず鍚?
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FText GetTagDisplayName(FGameplayTag Tag) const;
};
