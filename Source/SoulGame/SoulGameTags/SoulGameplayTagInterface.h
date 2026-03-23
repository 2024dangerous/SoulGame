// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTags.h"
#include "SoulGameplayTagInterface.generated.h"

/**
 * GameplayTag 接口
 * 所有需要支持 GameplayTags 的 Actor 需要实现此接口
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
	// 获取标签容器
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FGameplayTagContainer GetGameplayTagContainer() const;

	// 添加标签
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AddGameplayTag(FGameplayTag Tag);

	// 移除标签
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RemoveGameplayTag(FGameplayTag Tag);

	// 检查是否有指定标签（精确匹配）
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool HasGameplayTag(FGameplayTag Tag) const;

	// 模糊匹配标签（匹配父标签）
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool HasMatchingTag(FGameplayTag Tag) const;

	// 获取所有标签名
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<FName> GetGameplayTagNames() const;

	// 获取标签显示名
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FText GetTagDisplayName(FGameplayTag Tag) const;
};
