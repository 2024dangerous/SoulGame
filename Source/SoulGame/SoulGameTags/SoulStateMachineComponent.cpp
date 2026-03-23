// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameTags/SoulStateMachineComponent.h"
#include "SoulGameTags/SoulGameTagsManager.h"
#include "SoulGameTags/SoulGameplayTagInterface.h"
#include "GameplayTagsManager.h"
#include "Engine/World.h"
#include "TimerManager.h"

USoulStateMachineComponent::USoulStateMachineComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void USoulStateMachineComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// 如果当前状态无效，设置初始状态
	if (!CurrentState.IsValid())
	{
		if (InitialState.IsValid())
		{
			CurrentState = InitialState;
		}
		else
		{
			// 默认使用 Idle 状态
			USoulGameTagsManager* TagsManager = USoulGameTagsManager::Get();
			if (TagsManager)
			{
				CurrentState = TagsManager->GetBehaviorTag(TEXT("Idle"));
			}
		}

		if (CurrentState.IsValid())
		{
			AddTagToOwner(CurrentState);
		}
	}
}

bool USoulStateMachineComponent::SetState(FGameplayTag NewState)
{
	if (!NewState.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulStateMachineComponent::SetState - Invalid state"));
		return false;
	}

	if (!IsValidState(NewState))
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulStateMachineComponent::SetState - State not in allowed list: %s"), *NewState.ToString());
		return false;
	}

	if (!CanTransitionTo(NewState))
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulStateMachineComponent::SetState - Cannot transition to: %s"), *NewState.ToString());
		return false;
	}

	FGameplayTag OldState = CurrentState;

	// 移除旧状态标签
	if (OldState.IsValid())
	{
		RemoveTagFromOwner(OldState);
		OnStateExit.Broadcast(OldState, GetOwner());
	}

	// 更新状态
	CurrentState = NewState;

	// 添加新状态标签
	AddTagToOwner(CurrentState);
	OnStateEnter.Broadcast(CurrentState, GetOwner());

	// 记录转换时间
	LastTransitionTimes.Add(NewState, GetWorld()->GetTimeSeconds());

	// 广播状态改变事件
	OnStateChanged.Broadcast(OldState, NewState, GetOwner());

	UE_LOG(LogTemp, Log, TEXT("USoulStateMachineComponent::SetState - State changed from %s to %s"), 
		*OldState.ToString(), *NewState.ToString());

	return true;
}

bool USoulStateMachineComponent::CanTransitionTo(FGameplayTag NewState) const
{
	if (!NewState.IsValid() || !IsValidState(NewState))
	{
		return false;
	}

	// 检查冷却
	if (!IsCooldownComplete(NewState))
	{
		return false;
	}

	// 如果没有转换规则，只要状态有效就可以转换
	if (TransitionRules.Num() == 0)
	{
		return true;
	}

	// 检查是否有转换规则允许这个转换
	for (const FStateTransitionRule& Rule : TransitionRules)
	{
		if (Rule.TargetState == NewState)
		{
			// 检查必需的标签
			if (Rule.RequiredTags.Num() > 0)
			{
				AActor* Owner = GetOwner();
				if (Owner)
				{
					TScriptInterface<ISoulGameplayTagInterface> TagInterface(Owner);
					if (TagInterface)
					{
						FGameplayTagContainer OwnerTags = TagInterface->GetGameplayTagContainer();
						if (OwnerTags.HasAll(Rule.RequiredTags))
						{
							return true;
						}
					}
				}
			}
			else
			{
				return true;
			}
		}
	}

	// 如果没有找到匹配的规则，检查是否同一状态
	return (CurrentState == NewState);
}

void USoulStateMachineComponent::AddTransitionRule(const FGameplayTag FromState, const FStateTransitionRule& Rule)
{
	// 检查是否已存在相同的规则
	for (FStateTransitionRule& ExistingRule : TransitionRules)
	{
		if (ExistingRule.TargetState == Rule.TargetState)
		{
			// 更新现有规则
			ExistingRule = Rule;
			return;
		}
	}

	// 添加新规则
	TransitionRules.Add(Rule);
}

void USoulStateMachineComponent::RemoveTransitionRule(FGameplayTag ToState)
{
	TransitionRules.RemoveAll([&ToState](const FStateTransitionRule& Rule)
	{
		return Rule.TargetState == ToState;
	});
}

bool USoulStateMachineComponent::IsCooldownComplete(FGameplayTag TargetState) const
{
	if (const float* LastTime = LastTransitionTimes.Find(TargetState))
	{
		// 查找对应的转换规则获取冷却时间
		float CooldownTime = 0.5f; // 默认冷却时间
		for (const FStateTransitionRule& Rule : TransitionRules)
		{
			if (Rule.TargetState == TargetState)
			{
				CooldownTime = Rule.CooldownTime;
				break;
			}
		}

		float Elapsed = GetWorld()->GetTimeSeconds() - *LastTime;
		return Elapsed >= CooldownTime;
	}
	return true;
}

bool USoulStateMachineComponent::IsValidState(FGameplayTag State) const
{
	// 如果允许列表为空，允许所有状态
	if (AllowedStates.Num() == 0)
	{
		return true;
	}

	// 检查状态是否在允许列表中，或是否是允许状态的子标签
	for (const FGameplayTag& AllowedTag : AllowedStates)
	{
		if (AllowedTag.MatchesTag(State) || State.MatchesTag(AllowedTag))
		{
			return true;
		}
	}
	return false;
}

void USoulStateMachineComponent::AddTagToOwner(FGameplayTag Tag)
{
	AActor* Owner = GetOwner();
	if (Owner)
	{
		TScriptInterface<ISoulGameplayTagInterface> TagInterface(Owner);
		if (TagInterface)
		{
			TagInterface->AddGameplayTag(Tag);
		}
	}
}

void USoulStateMachineComponent::RemoveTagFromOwner(FGameplayTag Tag)
{
	AActor* Owner = GetOwner();
	if (Owner)
	{
		TScriptInterface<ISoulGameplayTagInterface> TagInterface(Owner);
		if (TagInterface)
		{
			TagInterface->RemoveGameplayTag(Tag);
		}
	}
}
