// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTags.h"
#include "SoulMessage.h"
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
 * SoulEventManager - 基于 GameplayTags 的事件管理器
 * 
 * 功能：
 * - 信息传递：发送和接收信息
 * - 事件订阅：订阅和取消订阅事件
 * - 状态改变：处理状态改变事件
 * - 损伤事件：处理伤害事件
 * - 死亡事件：处理死亡事件
 * - 武器改变：处理武器改变事件
 * 
 * 使用方法：
 * 1. 通过 Get() 获取单例
 * 2. 使用 SendMessage() 发送信息
 * 3. 使用 Subscribe() 订阅信息
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
	 * 初始化，通常在 GameInstance 中使用
	 */
	void Initialize();

	/**
	 * 关闭，通常在 GameInstance 中使用
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

	// 打开/关闭敌人血条
	FOpenEnemyHealth OpenEnemyHealth;

	// 打开战斗结果
	FOpenFightResult OpenFightResult;

	// 切换敌人血条
	FSwitchEnemyHealth SwitchEnemyHealth;

	// ============ 信息传递 API（新版）============

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