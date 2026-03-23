// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "SoulMessage.generated.h"

/**
 * 消息优先级枚举
 */
UENUM(BlueprintType)
enum class ESoulMessagePriority : uint8
{
	Low,      // 低优先级
	Normal,   // 普通优先级
	High,     // 高优先级
	Critical  // 关键优先级（最高）
};

/**
 * 消息通道枚举 - 用于分类和过滤消息
 */
UENUM(BlueprintType)
enum class ESoulMessageChannel : uint8
{
	Global,      // 全局广播
	Combat,      // 战斗相关
	Inventory,   // 物品/拾取相关
	UI,          // UI相关
	AI,          // AI相关
	Player,      // 玩家相关
	System       // 系统相关
};

/**
 * 消息结构体 - SoulEventManager 消息路由的核心数据结构
 * 携带消息的所有相关信息
 */
USTRUCT(BlueprintType)
struct SOULGAME_API FSoulMessage
{
	GENERATED_BODY()

public:
	// 默认构造函数
	FSoulMessage()
		: EventTag(FGameplayTag::EmptyTag)
		, Instigator(nullptr)
		, Channel(ESoulMessageChannel::Global)
		, Priority(ESoulMessagePriority::Normal)
		, bHandled(false)
		, Timestamp(0.0)
	{}

	// 主要构造函数
	FSoulMessage(FGameplayTag InEventTag, AActor* InInstigator = nullptr,
		ESoulMessageChannel InChannel = ESoulMessageChannel::Global,
		ESoulMessagePriority InPriority = ESoulMessagePriority::Normal)
		: EventTag(InEventTag)
		, Instigator(InInstigator)
		, Channel(InChannel)
		, Priority(InPriority)
		, bHandled(false)
		, Timestamp(FPlatformTime::Seconds())
	{}

	// 事件标签
	UPROPERTY(BlueprintReadWrite, Category = "SoulGame|Message")
	FGameplayTag EventTag;

	// 事件发起者
	UPROPERTY(BlueprintReadWrite, Category = "SoulGame|Message")
	TWeakObjectPtr<AActor> Instigator;

	// 消息通道
	UPROPERTY(BlueprintReadWrite, Category = "SoulGame|Message")
	ESoulMessageChannel Channel;

	// 消息优先级
	UPROPERTY(BlueprintReadWrite, Category = "SoulGame|Message")
	ESoulMessagePriority Priority;

	// 消息是否已被处理（用于短路后续处理）
	UPROPERTY(BlueprintReadWrite, Category = "SoulGame|Message")
	bool bHandled;

	// 消息时间戳
	UPROPERTY(BlueprintReadWrite, Category = "SoulGame|Message")
	double Timestamp;

	// ============ 扩展数据接口 ============
	
	// 获取扩展数据（模板方法）
	template<typename T>
	T GetData() const
	{
		if (auto* Found = ExtensionData.Find(T::StructKey))
		{
			return T(Found);
		}
		return T();
	}

	// 设置扩展数据
	template<typename T>
	void SetData(const T& Data)
	{
		ExtensionData.Add(T::StructKey, Data);
	}

	// 检查是否有扩展数据
	template<typename T>
	bool HasData() const
	{
		return ExtensionData.Contains(T::StructKey);
	}

protected:
	// 扩展数据存储（使用 FStructProperty 兼容的任何结构体）
	UPROPERTY()
	TMap<FName, uint8> ExtensionData;
};

/**
 * 消息订阅者信息
 */
USTRUCT(BlueprintType)
struct SOULGAME_API FSoulMessageSubscriber
{
	GENERATED_BODY()

public:
	FSoulMessageSubscriber()
		: Target(nullptr)
		, FunctionName(NAME_None)
		, Channel(ESoulMessageChannel::Global)
		, Priority(ESoulMessagePriority::Normal)
	{}

	FSoulMessageSubscriber(UObject* InTarget, FName InFunctionName,
		ESoulMessageChannel InChannel = ESoulMessageChannel::Global,
		ESoulMessagePriority InPriority = ESoulMessagePriority::Normal)
		: Target(InTarget)
		, FunctionName(InFunctionName)
		, Channel(InChannel)
		, Priority(InPriority)
	{}

	// 订阅目标对象
	UPROPERTY()
	TWeakObjectPtr<UObject> Target;

	// 回调函数名
	UPROPERTY()
	FName FunctionName;

	// 订阅的通道
	UPROPERTY()
	ESoulMessageChannel Channel;

	// 订阅优先级
	UPROPERTY()
	ESoulMessagePriority Priority;

	// 是否有效
	bool IsValid() const
	{
		return Target.IsValid() && !FunctionName.IsNone();
	}
};

/**
 * 带句柄的订阅信息（用于安全移除订阅）
 */
USTRUCT(BlueprintType)
struct SOULGAME_API FSoulMessageSubscription
{
	GENERATED_BODY()

public:
	FSoulMessageSubscription() = default;

	FSoulMessageSubscription(const FGameplayTag& InTag, const FSoulMessageSubscriber& InSubscriber, int32 InHandle)
		: Tag(InTag)
		, Subscriber(InSubscriber)
		, Handle(InHandle)
	{}

	// 订阅的事件标签
	UPROPERTY()
	FGameplayTag Tag;

	// 订阅者信息
	UPROPERTY()
	FSoulMessageSubscriber Subscriber;

	// 订阅句柄（唯一标识）
	UPROPERTY()
	int32 Handle;
};

/**
 * 订阅列表包装器（用于解决 TMap 中 TArray 作为 value 类型的限制）
 */
USTRUCT(BlueprintType)
struct SOULGAME_API FSoulMessageSubscriptionArray
{
	GENERATED_BODY()

public:
	FSoulMessageSubscriptionArray() = default;

	UPROPERTY()
	TArray<FSoulMessageSubscription> Subscriptions;
};
