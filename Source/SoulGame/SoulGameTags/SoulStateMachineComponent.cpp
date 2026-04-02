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

	// 初始化默认层（兼容旧接口）
	if (!CurrentState.IsValid())
	{
		if (InitialState.IsValid())
		{
			CurrentState = InitialState;
		}
		else
		{
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

	// 初始化所有注册的层
	for (FSoulStateLayer& Layer : StateLayers)
	{
		if (!Layer.CurrentState.IsValid() && Layer.InitialState.IsValid())
		{
			Layer.CurrentState = Layer.InitialState;
			AddTagToOwner(Layer.CurrentState);
		}
	}
}

// ============ 单层操作（兼容旧接口）===========

bool USoulStateMachineComponent::SetState(FGameplayTag NewState)
{
	if (!NewState.IsValid())
	{
        UE_LOG(LogTemp, Warning, TEXT("USoulStateMachineComponent::SetState - 无效状态"));
		return false;
	}

	if (!IsValidState(NewState))
	{
        UE_LOG(LogTemp, Warning, TEXT("USoulStateMachineComponent::SetState - 状态不在允许列表中: %s"), *NewState.ToString());
		return false;
	}

	if (!CanTransitionTo(NewState))
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulStateMachineComponent::SetState - 无法转换到: %s"), *NewState.ToString());
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

    UE_LOG(LogTemp, Log, TEXT("USoulStateMachineComponent::SetState - 状态从 %s 变为 %s"),
		*OldState.ToString(), *NewState.ToString());

	return true;
}

bool USoulStateMachineComponent::CanTransitionTo(FGameplayTag NewState) const
{
	if (!NewState.IsValid() || !IsValidState(NewState))
	{
		return false;
	}

	if (!IsCooldownComplete(NewState))
	{
		return false;
	}

	if (TransitionRules.Num() == 0)
	{
		return true;
	}

	for (const FStateTransitionRule& Rule : TransitionRules)
	{
		if (Rule.TargetState == NewState)
		{
            // 检查阻止标签
            if (Rule.BlockedTags.Num() > 0)
            {
                AActor* Owner = GetOwner();
                if (Owner)
                {
                    TScriptInterface<ISoulGameplayTagInterface> TagInterface(Owner);
                    if (TagInterface)
                    {
                        FGameplayTagContainer OwnerTags = TagInterface->GetGameplayTagContainer();
                        if (OwnerTags.HasAny(Rule.BlockedTags))
                        {
                            return false;
                        }
                    }
                }
            }

            // 检查必需标签
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

	return (CurrentState == NewState);
}

void USoulStateMachineComponent::AddTransitionRule(const FGameplayTag FromState, const FStateTransitionRule& Rule)
{
	for (FStateTransitionRule& ExistingRule : TransitionRules)
	{
		if (ExistingRule.TargetState == Rule.TargetState)
		{
			ExistingRule = Rule;
			return;
		}
	}
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
		float CooldownTime = 0.5f;
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

// ============ 鍒嗗眰鐘舵€佹満鎿嶄綔 ============

void USoulStateMachineComponent::RegisterLayer(FName LayerName, FGameplayTag LayerTag,
	FGameplayTag InInitialState, const FGameplayTagContainer& InAllowedStates, int32 Priority)
{
    // 检查是否已存在同名层
	if (FindLayer(LayerName))
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulStateMachineComponent::RegisterLayer - 层已存在: %s"), *LayerName.ToString());
		return;
	}

	FSoulStateLayer NewLayer;
	NewLayer.LayerName = LayerName;
	NewLayer.LayerTag = LayerTag;
	NewLayer.InitialState = InInitialState;
	NewLayer.CurrentState = InInitialState;
	NewLayer.AllowedStates = InAllowedStates;
	NewLayer.Priority = Priority;
	NewLayer.bEnabled = true;

	StateLayers.Add(NewLayer);

	// 添加初始状态标签
	if (InInitialState.IsValid())
	{
		AddTagToOwner(InInitialState);
	}

	UE_LOG(LogTemp, Log, TEXT("USoulStateMachineComponent::RegisterLayer - 注册层: %s (初始状态: %s, 优先级: %d)"),
		*LayerName.ToString(), *InInitialState.ToString(), Priority);
}

bool USoulStateMachineComponent::SetLayerState(FName LayerName, FGameplayTag NewState)
{
	FSoulStateLayer* Layer = FindLayer(LayerName);
	if (!Layer)
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulStateMachineComponent::SetLayerState - 层不存在: %s"), *LayerName.ToString());
		return false;
	}

	if (!Layer->bEnabled)
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulStateMachineComponent::SetLayerState - 层已禁用: %s"), *LayerName.ToString());
		return false;
	}

	if (!NewState.IsValid())
	{
		return false;
	}

	if (!IsValidLayerState(*Layer, NewState))
	{
        UE_LOG(LogTemp, Warning, TEXT("USoulStateMachineComponent::SetLayerState - 状态不在层 %s 的允许列表中: %s"),
			*LayerName.ToString(), *NewState.ToString());
		return false;
	}

	if (!CanLayerTransitionTo(*Layer, NewState))
	{
		return false;
	}

	FGameplayTag OldState = Layer->CurrentState;

    // 移除旧状态标签
    if (OldState.IsValid())
    {
        RemoveTagFromOwner(OldState);
        OnStateExit.Broadcast(OldState, GetOwner());
    }

    // 更新状态
    Layer->CurrentState = NewState;

    // 添加新状态标签
	AddTagToOwner(NewState);
	OnStateEnter.Broadcast(NewState, GetOwner());

	// 记录转换时间
	Layer->LastTransitionTimes.Add(NewState, GetWorld()->GetTimeSeconds());

	// 广播事件
	OnLayerStateChanged.Broadcast(LayerName, OldState, NewState, GetOwner());
	OnStateChanged.Broadcast(OldState, NewState, GetOwner());

    UE_LOG(LogTemp, Log, TEXT("USoulStateMachineComponent::SetLayerState - 层 %s: %s -> %s"),
		*LayerName.ToString(), *OldState.ToString(), *NewState.ToString());

	return true;
}

FGameplayTag USoulStateMachineComponent::GetLayerState(FName LayerName) const
{
	const FSoulStateLayer* Layer = FindLayer(LayerName);
	if (Layer)
	{
		return Layer->CurrentState;
	}
	return FGameplayTag();
}

bool USoulStateMachineComponent::IsLayerInState(FName LayerName, FGameplayTag State) const
{
	const FSoulStateLayer* Layer = FindLayer(LayerName);
	if (Layer)
	{
		return Layer->CurrentState == State;
	}
	return false;
}

void USoulStateMachineComponent::AddLayerTransitionRule(FName LayerName, const FStateTransitionRule& Rule)
{
	FSoulStateLayer* Layer = FindLayer(LayerName);
	if (!Layer)
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulStateMachineComponent::AddLayerTransitionRule - 层不存在: %s"), *LayerName.ToString());
		return;
	}

	// 检查是否已存在相同目标状态的规则
	for (FStateTransitionRule& ExistingRule : Layer->TransitionRules)
	{
		if (ExistingRule.TargetState == Rule.TargetState)
		{
			ExistingRule = Rule;
			return;
		}
	}
	Layer->TransitionRules.Add(Rule);
}

void USoulStateMachineComponent::SetLayerEnabled(FName LayerName, bool bEnabled)
{
	FSoulStateLayer* Layer = FindLayer(LayerName);
	if (Layer)
	{
		Layer->bEnabled = bEnabled;
		UE_LOG(LogTemp, Log, TEXT("USoulStateMachineComponent::SetLayerEnabled - 层 %s: %s"),
			*LayerName.ToString(), bEnabled ? TEXT("启用") : TEXT("禁用"));
	}
}

TMap<FName, FGameplayTag> USoulStateMachineComponent::GetAllLayerStates() const
{
	TMap<FName, FGameplayTag> Result;
	for (const FSoulStateLayer& Layer : StateLayers)
	{
		Result.Add(Layer.LayerName, Layer.CurrentState);
	}
	return Result;
}

bool USoulStateMachineComponent::IsAnyLayerInState(FGameplayTag State) const
{
	for (const FSoulStateLayer& Layer : StateLayers)
	{
		if (Layer.CurrentState == State)
		{
			return true;
		}
	}
	// 也检查默认层
	return CurrentState == State;
}

// ============ 内部方法 ============

bool USoulStateMachineComponent::IsValidState(FGameplayTag State) const
{
	if (AllowedStates.Num() == 0)
	{
		return true;
	}

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

FSoulStateLayer* USoulStateMachineComponent::FindLayer(FName LayerName)
{
	for (FSoulStateLayer& Layer : StateLayers)
	{
		if (Layer.LayerName == LayerName)
		{
			return &Layer;
		}
	}
	return nullptr;
}

const FSoulStateLayer* USoulStateMachineComponent::FindLayer(FName LayerName) const
{
	for (const FSoulStateLayer& Layer : StateLayers)
	{
		if (Layer.LayerName == LayerName)
		{
			return &Layer;
		}
	}
	return nullptr;
}

bool USoulStateMachineComponent::IsValidLayerState(const FSoulStateLayer& Layer, FGameplayTag State) const
{
	if (Layer.AllowedStates.Num() == 0)
	{
		return true;
	}

	for (const FGameplayTag& AllowedTag : Layer.AllowedStates)
	{
		if (AllowedTag.MatchesTag(State) || State.MatchesTag(AllowedTag))
		{
			return true;
		}
	}
	return false;
}

bool USoulStateMachineComponent::IsLayerCooldownComplete(const FSoulStateLayer& Layer, FGameplayTag TargetState) const
{
	if (const float* LastTime = Layer.LastTransitionTimes.Find(TargetState))
	{
		float CooldownTime = 0.5f;
		for (const FStateTransitionRule& Rule : Layer.TransitionRules)
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

bool USoulStateMachineComponent::CanLayerTransitionTo(const FSoulStateLayer& Layer, FGameplayTag NewState) const
{
	if (!NewState.IsValid() || !IsValidLayerState(Layer, NewState))
	{
		return false;
	}

	if (!IsLayerCooldownComplete(Layer, NewState))
	{
		return false;
	}

	if (Layer.TransitionRules.Num() == 0)
	{
		return true;
	}

	for (const FStateTransitionRule& Rule : Layer.TransitionRules)
	{
		if (Rule.TargetState == NewState)
		{
			// 检查阻止标签
			if (Rule.BlockedTags.Num() > 0)
			{
				AActor* Owner = GetOwner();
				if (Owner)
				{
					TScriptInterface<ISoulGameplayTagInterface> TagInterface(Owner);
					if (TagInterface)
					{
						FGameplayTagContainer OwnerTags = TagInterface->GetGameplayTagContainer();
						if (OwnerTags.HasAny(Rule.BlockedTags))
						{
							return false;
						}
					}
				}
			}

			// 检查必需标签
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

	return (Layer.CurrentState == NewState);
}