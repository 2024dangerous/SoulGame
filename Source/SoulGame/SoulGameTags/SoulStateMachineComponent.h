// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTags.h"
#include "SoulStateMachineComponent.generated.h"

class USoulGameTagsManager;

/**
 * 状态转换规则
 */
USTRUCT(BlueprintType)
struct FStateTransitionRule
{
	GENERATED_BODY()

	// 目标状态
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag TargetState;

	// 需要的标签（可选）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer RequiredTags;

	// 是否需要检查冷却
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCheckCooldown = true;

	// 冷却时间（秒）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CooldownTime = 0.5f;
};

/**
 * 状态改变委托
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStateChanged, FGameplayTag, OldState, FGameplayTag, NewState, AActor*, Owner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStateEnter, FGameplayTag, State, AActor*, Owner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStateExit, FGameplayTag, State, AActor*, Owner);

/**
 * SoulStateMachineComponent - 基于 GameplayTags 的状态机组件
 * 提供状态管理和转换功能
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (SoulGame), meta = (BlueprintSpawnableComponent))
class SOULGAME_API USoulStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USoulStateMachineComponent();

	// 初始化状态机
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine")
	void InitializeComponent() override;

	// 设置新状态
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine")
	bool SetState(FGameplayTag NewState);

	// 获取当前状态
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine")
	FGameplayTag GetCurrentState() const { return CurrentState; }

	// 获取初始状态
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine")
	FGameplayTag GetInitialState() const { return InitialState; }

	// 设置初始状态
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine")
	void SetInitialState(FGameplayTag InInitialState) { InitialState = InInitialState; }

	// 设置允许的状态列表
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine")
	void SetAllowedStates(const FGameplayTagContainer& InAllowedStates) { AllowedStates = InAllowedStates; }

	// 检查是否可以转换到指定状态
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine")
	bool CanTransitionTo(FGameplayTag NewState) const;

	// 添加状态转换规则
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine")
	void AddTransitionRule(const FGameplayTag FromState, const FStateTransitionRule& Rule);

	// 移除状态转换规则
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine")
	void RemoveTransitionRule(FGameplayTag ToState);

	// 检查冷却是否完成
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine")
	bool IsCooldownComplete(FGameplayTag TargetState) const;

protected:
	// 状态改变事件
	UPROPERTY(BlueprintAssignable, Category = "SoulGame|StateMachine")
	FOnStateChanged OnStateChanged;

	// 状态进入事件
	UPROPERTY(BlueprintAssignable, Category = "SoulGame|StateMachine")
	FOnStateEnter OnStateEnter;

	// 状态退出事件
	UPROPERTY(BlueprintAssignable, Category = "SoulGame|StateMachine")
	FOnStateExit OnStateExit;

	// 初始状态
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateMachine")
	FGameplayTag InitialState;

	// 当前状态
	UPROPERTY(BlueprintReadOnly, Category = "StateMachine")
	FGameplayTag CurrentState;

	// 允许的状态列表
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateMachine")
	FGameplayTagContainer AllowedStates;

	// 状态转换规则
	UPROPERTY(BlueprintReadOnly, Category = "StateMachine")
	TArray<FStateTransitionRule> TransitionRules;

private:
	// 上次转换时间映射
	UPROPERTY()
	TMap<FGameplayTag, float> LastTransitionTimes;

	// 添加标签到所有者
	void AddTagToOwner(FGameplayTag Tag);

	// 从所有者移除标签
	void RemoveTagFromOwner(FGameplayTag Tag);

	// 检查状态是否有效
	bool IsValidState(FGameplayTag State) const;
};
