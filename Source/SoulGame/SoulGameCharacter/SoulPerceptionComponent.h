// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoulPerceptionComponent.generated.h"

class ASoulBaseEnemy;
class USphereComponent;

/**
 * USoulPerceptionComponent - 敌人感知组件
 * 
 * 负责管理角色周围的敌人感知，包括：
 * - 维护附近敌人列表
 * - 通过碰撞球检测敌人进入/离开
 * - 通知 UI 显示/隐藏敌人血条
 * 
 * 从 SoulBaseCharacter 中拆分出来，实现职责单一化。
 */
UCLASS(ClassGroup=(SoulGame), meta=(BlueprintSpawnableComponent))
class SOULGAME_API USoulPerceptionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USoulPerceptionComponent();

protected:
	virtual void BeginPlay() override;

public:
	// ============ 敌人感知管理 ============

	/** 获取当前感知到的敌人数组 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SoulGame|Perception")
	const TArray<ASoulBaseEnemy*>& GetEnemyArray() const { return EnemyArray; }

	/** 附近是否有敌人 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SoulGame|Perception")
	bool HasEnemies() const { return EnemyArray.Num() > 0; }

	/** 获取附近敌人数量 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SoulGame|Perception")
	int32 GetEnemyCount() const { return EnemyArray.Num(); }

	/** 获取最近的敌人 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SoulGame|Perception")
	ASoulBaseEnemy* GetNearestEnemy() const;

	/** 获取感知球组件 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SoulGame|Perception")
	USphereComponent* GetPerceptionSphere() const { return PerceptionSphere; }

	// ============ 委托：敌人列表变化时通知外部 ============

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyPerceptionChanged, ASoulBaseEnemy*, Enemy, bool, bIsAdded);

	/** 敌人进入/离开感知范围时广播 */
	UPROPERTY(BlueprintAssignable, Category = "SoulGame|Perception")
	FOnEnemyPerceptionChanged OnEnemyPerceptionChanged;

	/** 所有敌人离开感知范围时广播 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllEnemiesLost);

	UPROPERTY(BlueprintAssignable, Category = "SoulGame|Perception")
	FOnAllEnemiesLost OnAllEnemiesLost;

public:
	/** 感知碰撞球组件（由 Owner 创建并赋值） */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SoulGame|Perception")
	USphereComponent* PerceptionSphere;

	/** 当前感知到的敌人数组 */
	UPROPERTY()
	TArray<ASoulBaseEnemy*> EnemyArray;

private:
	/** 碰撞球重叠回调：敌人进入 */
	UFUNCTION()
	void OnPerceptionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** 碰撞球重叠回调：敌人离开 */
	UFUNCTION()
	void OnPerceptionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
