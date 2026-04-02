// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoulPickupComponent.generated.h"

class APickupItem;

/**
 * USoulPickupComponent - 拾取交互组件
 * 
 * 负责管理角色附近的可拾取物品列表，以及交互UI的显示/隐藏。
 * 从 SoulBaseCharacter 中拆分出来，实现职责单一化。
 * 
 * 使用方式：
 * 1. 在角色蓝图中添加此组件
 * 2. 组件会自动订阅 SoulEventManager::NearbyInteractables 委托
 * 3. 按交互键时调用 PickupAllItems() 拾取所有物品
 */
UCLASS(ClassGroup=(SoulGame), meta=(BlueprintSpawnableComponent))
class SOULGAME_API USoulPickupComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USoulPickupComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// ============ 拾取物品管理 ============

	/** 获取当前附近的可拾取物品数组 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SoulGame|Pickup")
	const TArray<APickupItem*>& GetPickupItems() const { return PickupItemArray; }

	/** 附近是否有可拾取物品 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SoulGame|Pickup")
	bool HasPickupItems() const { return PickupItemArray.Num() > 0; }

	/** 获取附近可拾取物品数量 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SoulGame|Pickup")
	int32 GetPickupItemCount() const { return PickupItemArray.Num(); }

	/** 拾取所有附近的物品（销毁并从列表移除） */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Pickup")
	void PickupAllItems();

	/** 添加拾取物品到列表 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Pickup")
	void AddPickupItem(APickupItem* Item);

	/** 从列表移除拾取物品 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Pickup")
	void RemovePickupItem(APickupItem* Item);

private:
	/** 附近的可拾取物品数组 */
	UPROPERTY()
	TArray<APickupItem*> PickupItemArray;

	/** 事件回调：附近可交互物体变化（适配 SoulEventManager 委托签名） */
	void OnNearbyInteractablesChanged(AActor* InteractableActor, bool bIsAdd);

	/** 更新交互UI的显示状态 */
	void UpdateInteractionUI();
};