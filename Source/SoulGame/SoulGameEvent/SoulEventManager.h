// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTags.h"
#include "SoulMessage.h"
#include "SoulGameData/SoulEnumType.h"
#include "SoulEventManager.generated.h"

// 前向声明
class USoulGameTagsManager;

// ============ 委托声明 ============

// 状态改变事件委托（Tag-Based）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGameplayTagStateChanged, FGameplayTag, OldState, FGameplayTag, NewState, AActor*, Instigator);

// ============ 传统委托声明（保留兼容）============

// 附近可交互物体变化
DECLARE_DELEGATE_TwoParams(FNearbyInteractables, AActor*, bool);
// 打开/关闭战斗UI
DECLARE_DELEGATE_OneParam(FOpenFightUI, bool);
// 状态值变化多播
DECLARE_MULTICAST_DELEGATE_FourParams(FOnStatusBoxChanged, EStatusBox, float, float, float);
// 打开/关闭设置菜单
DECLARE_DELEGATE_OneParam(FOpenSetMenu, bool);
// 打开/关闭交互UI
DECLARE_DELEGATE_TwoParams(FOpenInteractionUI, bool, FText);
// 打开/关闭敌人血条
DECLARE_DELEGATE_TwoParams(FOpenEnemyHealth, FName, bool);
// 打开战斗结果
DECLARE_DELEGATE(FOpenFightResult);
// 切换敌人血条
DECLARE_DELEGATE_TwoParams(FSwitchEnemyHealth, FName, float);

/**
 * SoulEventManager - 基于 GameplayTags 的消息路由管理器（单例）
 * 
 * 特性：
 * - 消息路由：支持通道、优先级过滤
 * - 安全订阅：使用委托句柄管理订阅生命周期
 * - 层级匹配：支持 GameplayTag 父子标签匹配
 * - 兼容旧API：保留传统委托方式兼容
 * 
 * 使用方式：
 * 1. 通过 Get() 获取单例
 * 2. 使用 SendMessage() 发送消息
 * 3. 使用 Subscribe() 订阅消息，返回句柄用于取消订阅
 */
UCLASS(Blueprintable, BlueprintType)
class SOULGAME_API USoulEventManager : public UObject
{
	GENERATED_BODY()

public:
	USoulEventManager();

	// ============ 单例管理 ============
	
	/**
	 * 获取单例实例
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Events")
	static USoulEventManager* Get();

	/**
	 * 初始化（由 GameInstance 调用）
	 */
	void Initialize();

	/**
	 * 关闭（由 GameInstance 调用）
	 */
	void Deinitialize();

	/**
	 * 获取是否已初始化
	 */
	UFUNCTION(BlueprintPure, Category = "SoulGame|Events")
	bool IsInitialized() const { return bIsInitialized; }

	// ============ 传统委托成员（保留兼容）============

	// 附近可交互物体变化
	FNearbyInteractables NearbyInteractables;

	// 打开/关闭战斗UI
	FOpenFightUI OpenFightUI;

	// 打开/关闭设置菜单
	FOpenSetMenu OpenSetMenu;

	// 打开/关闭交互UI
	FOpenInteractionUI OpenInteractionUI;

	// 状态值变化多播
	FOnStatusBoxChanged OnStatusBoxChanged;

	// 打开/关闭敌人血条
	FOpenEnemyHealth OpenEnemyHealth;

	// 打开战斗结果
	FOpenFightResult OpenFightResult;

	// 切换敌人血条
	FSwitchEnemyHealth SwitchEnemyHealth;

	// ============ 消息发送 API（新版）============

	UFUNCTION(BlueprintCallable, Category = "SoulGame|Events")
	void SendMessage(const FSoulMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "SoulGame|Events")
	void SendGameplayTagEvent(FGameplayTag EventTag, AActor* Instigator = nullptr);

	UFUNCTION(BlueprintCallable, Category = "SoulGame|Events")
	void SendStateChangeEvent(FGameplayTag OldState, FGameplayTag NewState, AActor* Instigator = nullptr);

	UFUNCTION(BlueprintCallable, Category = "SoulGame|Events")
	void SendDamageEvent(AActor* Damager, AActor* Victim, float Damage);

	UFUNCTION(BlueprintCallable, Category = "SoulGame|Events")
	void SendDeathEvent(AActor* Victim);

	UFUNCTION(BlueprintCallable, Category = "SoulGame|Events")
	void SendWeaponChangeEvent(AActor* Actor, FGameplayTag NewWeaponTag);

	// ============ 订阅管理 API（新版）============

	UFUNCTION(BlueprintCallable, Category = "SoulGame|Events")
	int32 Subscribe(const FSoulMessageSubscriber& Subscriber);

	UFUNCTION(BlueprintCallable, Category = "SoulGame|Events")
	int32 SubscribeToGameplayTagEvent(FGameplayTag EventTag, UObject* Target, FName FunctionName);

	UFUNCTION(BlueprintCallable, Category = "SoulGame|Events")
	void Unsubscribe(int32 Handle);

	UFUNCTION(BlueprintCallable, Category = "SoulGame|Events")
	void UnsubscribeFromGameplayTagEvent(FGameplayTag EventTag, UObject* Target);

	UFUNCTION(BlueprintCallable, Category = "SoulGame|Events")
	void UnsubscribeAll(UObject* Target);

	UFUNCTION(BlueprintCallable, Category = "SoulGame|Events")
	int32 SubscribeToStateChange(UObject* Target, FName FunctionName);

	UFUNCTION(BlueprintCallable, Category = "SoulGame|Events")
	void UnsubscribeFromStateChange(int32 Handle);

	// ============ 查询 API ============

	UFUNCTION(BlueprintPure, Category = "SoulGame|Events")
	bool HasSubscribers(FGameplayTag EventTag) const;

	UFUNCTION(BlueprintPure, Category = "SoulGame|Events")
	int32 GetSubscriberCount(FGameplayTag EventTag) const;

protected:
	void BroadcastMessage(const FSoulMessage& Message);
	void FindMatchingSubscribers(const FGameplayTag& EventTag, TArray<FSoulMessageSubscriber*>& OutSubscribers);
	void ExecuteDelegate(UObject* Target, FName FunctionName, const FSoulMessage& Message);

private:
	UPROPERTY()
	bool bIsInitialized;

	UPROPERTY()
	TMap<FGameplayTag, FSoulMessageSubscriptionArray> TagSubscriptions;

	UPROPERTY()
	FOnGameplayTagStateChanged OnGameplayTagStateChanged;

	UPROPERTY()
	TArray<int32> StateChangeHandles;

	UPROPERTY()
	int32 CurrentHandleCounter;

	UPROPERTY()
	TMap<int32, FSoulMessageSubscription> HandleToSubscription;

	static USoulEventManager* Instance;
};