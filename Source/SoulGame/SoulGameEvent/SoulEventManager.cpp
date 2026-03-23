// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameEvent/SoulEventManager.h"
#include "SoulGameTags/SoulGameTagsManager.h"
#include "Kismet/GameplayStatics.h"

USoulEventManager* USoulEventManager::Instance = nullptr;

USoulEventManager::USoulEventManager()
	: bIsInitialized(false)
	, CurrentHandleCounter(0)
{
}

USoulEventManager* USoulEventManager::Get()
{
	if (!Instance)
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulEventManager::Get - Instance not initialized. Call Initialize() first."));
	}
	return Instance;
}

void USoulEventManager::Initialize()
{
	if (bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulEventManager::Initialize - Already initialized."));
		return;
	}

	if (Instance && Instance != this)
	{
		// 清理旧实例
		Instance->Deinitialize();
	}

	Instance = this;
	Instance->AddToRoot();
	Instance->bIsInitialized = true;

	UE_LOG(LogTemp, Log, TEXT("USoulEventManager::Initialize - Initialized successfully."));
}

void USoulEventManager::Deinitialize()
{
	if (!bIsInitialized)
	{
		return;
	}

	// 清理所有订阅
	TagSubscriptions.Empty();
	HandleToSubscription.Empty();
	StateChangeHandles.Empty();

	// 从Root移除
	RemoveFromRoot();
	bIsInitialized = false;

	UE_LOG(LogTemp, Log, TEXT("USoulEventManager::Deinitialize - Deinitialized."));
}

void USoulEventManager::SendMessage(const FSoulMessage& Message)
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulEventManager::SendMessage - Not initialized."));
		return;
	}

	if (!Message.EventTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulEventManager::SendMessage - Invalid EventTag."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("USoulEventManager::SendMessage - Event: %s, Channel: %d, Priority: %d, Instigator: %s"),
		*Message.EventTag.ToString(),
		static_cast<int32>(Message.Channel),
		static_cast<int32>(Message.Priority),
		Message.Instigator.IsValid() ? *Message.Instigator->GetName() : TEXT("None"));

	// 广播消息
	BroadcastMessage(Message);
}

void USoulEventManager::SendGameplayTagEvent(FGameplayTag EventTag, AActor* Instigator)
{
	FSoulMessage Message(EventTag, Instigator, ESoulMessageChannel::Global, ESoulMessagePriority::Normal);
	SendMessage(Message);
}

void USoulEventManager::SendStateChangeEvent(FGameplayTag OldState, FGameplayTag NewState, AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("USoulEventManager::SendStateChangeEvent - Old: %s, New: %s"),
		*OldState.ToString(), *NewState.ToString());

	if (OnGameplayTagStateChanged.IsBound())
	{
		OnGameplayTagStateChanged.Broadcast(OldState, NewState, Instigator);
	}

	// 同时发送标签事件
	FSoulMessage Message(USoulGameTagsManager::Get()->GetEventTag(TEXT("StateChange")), Instigator,
		ESoulMessageChannel::Player, ESoulMessagePriority::Normal);
	SendMessage(Message);
}

void USoulEventManager::SendDamageEvent(AActor* Damager, AActor* Victim, float Damage)
{
	FSoulMessage Message(USoulGameTagsManager::Get()->GetEventTag(TEXT("Damage")), Victim,
		ESoulMessageChannel::Combat, ESoulMessagePriority::High);
	SendMessage(Message);
}

void USoulEventManager::SendDeathEvent(AActor* Victim)
{
	FSoulMessage Message(USoulGameTagsManager::Get()->GetEventTag(TEXT("Dead")), Victim,
		ESoulMessageChannel::Combat, ESoulMessagePriority::Critical);
	SendMessage(Message);
}

void USoulEventManager::SendWeaponChangeEvent(AActor* Actor, FGameplayTag NewWeaponTag)
{
	FSoulMessage Message(USoulGameTagsManager::Get()->GetEventTag(TEXT("WeaponChange")), Actor,
		ESoulMessageChannel::Player, ESoulMessagePriority::Normal);
	SendMessage(Message);
}

int32 USoulEventManager::Subscribe(const FSoulMessageSubscriber& Subscriber)
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulEventManager::Subscribe - Not initialized."));
		return -1;
	}

	if (!Subscriber.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulEventManager::Subscribe - Invalid subscriber."));
		return -1;
	}

	// 生成唯一句柄
	int32 Handle = ++CurrentHandleCounter;

	// 创建订阅信息
	FSoulMessageSubscription Sub(FGameplayTag(), Subscriber, Handle);

	// 添加到映射
	HandleToSubscription.Add(Handle, Sub);

	// 按标签订阅（使用空标签作为通用订阅）
	FSoulMessageSubscriptionArray& SubsArray = TagSubscriptions.FindOrAdd(FGameplayTag());
	SubsArray.Subscriptions.Add(Sub);

	UE_LOG(LogTemp, Log, TEXT("USoulEventManager::Subscribe - Handle: %d, Function: %s, Target: %s"),
		Handle, *Subscriber.FunctionName.ToString(), *Subscriber.Target->GetName());

	return Handle;
}

int32 USoulEventManager::SubscribeToGameplayTagEvent(FGameplayTag EventTag, UObject* Target, FName FunctionName)
{
	if (!EventTag.IsValid() || !Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulEventManager::SubscribeToGameplayTagEvent - Invalid parameters."));
		return -1;
	}

	// 生成唯一句柄
	int32 Handle = ++CurrentHandleCounter;

	// 创建订阅者信息
	FSoulMessageSubscriber Subscriber;
	Subscriber.Target = Target;
	Subscriber.FunctionName = FunctionName;
	Subscriber.Channel = ESoulMessageChannel::Global;
	Subscriber.Priority = ESoulMessagePriority::Normal;

	// 创建订阅信息
	FSoulMessageSubscription Sub(EventTag, Subscriber, Handle);

	// 添加到句柄映射
	HandleToSubscription.Add(Handle, Sub);

	// 添加到标签映射
	FSoulMessageSubscriptionArray& SubsArray = TagSubscriptions.FindOrAdd(EventTag);
	SubsArray.Subscriptions.Add(Sub);

	UE_LOG(LogTemp, Log, TEXT("USoulEventManager::SubscribeToGameplayTagEvent - Tag: %s, Handle: %d, Function: %s"),
		*EventTag.ToString(), Handle, *FunctionName.ToString());

	return Handle;
}

void USoulEventManager::Unsubscribe(int32 Handle)
{
	if (Handle < 0)
	{
		return;
	}

	// 从句柄映射中移除
	FSoulMessageSubscription Sub;
	if (HandleToSubscription.RemoveAndCopyValue(Handle, Sub))
	{
		// 从标签映射中移除
		FSoulMessageSubscriptionArray* SubsArray = TagSubscriptions.Find(Sub.Tag);
		if (SubsArray)
		{
			SubsArray->Subscriptions.RemoveAll([Handle](const FSoulMessageSubscription& S) {
				return S.Handle == Handle;
			});
		}

		UE_LOG(LogTemp, Log, TEXT("USoulEventManager::Unsubscribe - Handle: %d removed."), Handle);
	}
}

void USoulEventManager::UnsubscribeFromGameplayTagEvent(FGameplayTag EventTag, UObject* Target)
{
	if (!EventTag.IsValid() || !Target)
	{
		return;
	}

	// 查找该目标的所有订阅句柄
	TArray<int32> HandlesToRemove;
	for (auto& Pair : HandleToSubscription)
	{
		if (Pair.Value.Tag == EventTag && Pair.Value.Subscriber.Target == Target)
		{
			HandlesToRemove.Add(Pair.Key);
		}
	}

	// 移除所有匹配的订阅
	for (int32 Handle : HandlesToRemove)
	{
		Unsubscribe(Handle);
	}
}

void USoulEventManager::UnsubscribeAll(UObject* Target)
{
	if (!Target)
	{
		return;
	}

	// 查找该目标的所有订阅句柄
	TArray<int32> HandlesToRemove;
	for (auto& Pair : HandleToSubscription)
	{
		if (Pair.Value.Subscriber.Target == Target)
		{
			HandlesToRemove.Add(Pair.Key);
		}
	}

	// 移除所有订阅
	for (int32 Handle : HandlesToRemove)
	{
		Unsubscribe(Handle);
	}

	UE_LOG(LogTemp, Log, TEXT("USoulEventManager::UnsubscribeAll - Target: %s, Removed: %d subscriptions."),
		*Target->GetName(), HandlesToRemove.Num());
}

int32 USoulEventManager::SubscribeToStateChange(UObject* Target, FName FunctionName)
{
	if (!Target)
	{
		return -1;
	}

	FScriptDelegate Delegate;
	Delegate.BindUFunction(Target, FunctionName);
	OnGameplayTagStateChanged.Add(Delegate);
	StateChangeHandles.Add(CurrentHandleCounter++);

	UE_LOG(LogTemp, Log, TEXT("USoulEventManager::SubscribeToStateChange - Target: %s, Function: %s"),
		*Target->GetName(), *FunctionName.ToString());

	return StateChangeHandles.Num() - 1;
}

void USoulEventManager::UnsubscribeFromStateChange(int32 HandleIndex)
{
	if (HandleIndex < 0 || HandleIndex >= StateChangeHandles.Num())
	{
		return;
	}

	// 状态改变事件使用动态委托，无法通过索引移除
	// 这里仅移除索引记录，实际委托绑定由UObject生命周期管理
	StateChangeHandles.RemoveAt(HandleIndex);
}

bool USoulEventManager::HasSubscribers(FGameplayTag EventTag) const
{
	if (const FSoulMessageSubscriptionArray* SubsArray = TagSubscriptions.Find(EventTag))
	{
		return SubsArray->Subscriptions.Num() > 0;
	}
	return false;
}

int32 USoulEventManager::GetSubscriberCount(FGameplayTag EventTag) const
{
	if (const FSoulMessageSubscriptionArray* SubsArray = TagSubscriptions.Find(EventTag))
	{
		return SubsArray->Subscriptions.Num();
	}
	return 0;
}

void USoulEventManager::BroadcastMessage(const FSoulMessage& Message)
{
	// 1. 收集所有匹配的订阅者（支持标签层级匹配）
	TArray<FSoulMessageSubscriber*> MatchingSubscribers;

	// 精确匹配
	if (const FSoulMessageSubscriptionArray* ExactSubsArray = TagSubscriptions.Find(Message.EventTag))
	{
		for (const FSoulMessageSubscription& Sub : ExactSubsArray->Subscriptions)
		{
			// 通道过滤
			if (Sub.Subscriber.Channel != ESoulMessageChannel::Global &&
				Sub.Subscriber.Channel != Message.Channel)
			{
				continue;
			}
			MatchingSubscribers.Add(const_cast<FSoulMessageSubscriber*>(&Sub.Subscriber));
		}
	}

	// 标签层级匹配
	for (const auto& Pair : TagSubscriptions)
	{
		if (Pair.Key.IsValid() && Pair.Key.MatchesTag(Message.EventTag) && Pair.Key != Message.EventTag)
		{
			for (const FSoulMessageSubscription& Sub : Pair.Value.Subscriptions)
			{
				// 通道过滤
				if (Sub.Subscriber.Channel != ESoulMessageChannel::Global &&
					Sub.Subscriber.Channel != Message.Channel)
				{
					continue;
				}
				MatchingSubscribers.Add(const_cast<FSoulMessageSubscriber*>(&Sub.Subscriber));
			}
		}
	}

	// 2. 按优先级排序（Critical > High > Normal > Low）
	for (int32 i = 1; i < MatchingSubscribers.Num(); ++i)
	{
		FSoulMessageSubscriber* Current = MatchingSubscribers[i];
		int32 j = i - 1;
		while (j >= 0 && static_cast<int32>(MatchingSubscribers[j]->Priority) < static_cast<int32>(Current->Priority))
		{
			MatchingSubscribers[j + 1] = MatchingSubscribers[j];
			--j;
		}
		MatchingSubscribers[j + 1] = Current;
	}

	// 3. 执行回调
	for (FSoulMessageSubscriber* Subscriber : MatchingSubscribers)
	{
		if (Subscriber->IsValid())
		{
			ExecuteDelegate(Subscriber->Target.Get(), Subscriber->FunctionName, Message);
		}

		// 如果消息已标记为处理，中断后续广播
		if (Message.bHandled)
		{
			break;
		}
	}
}

void USoulEventManager::ExecuteDelegate(UObject* Target, FName FunctionName, const FSoulMessage& Message)
{
	if (!Target || FunctionName.IsNone())
	{
		return;
	}

	// 使用 UFunction 查找
	if (UFunction* Function = Target->FindFunction(FunctionName))
	{
		// 准备参数
		struct
		{
			FGameplayTag EventTag;
			AActor* Instigator;
		} Params;

		Params.EventTag = Message.EventTag;
		Params.Instigator = Message.Instigator.Get();

		// 调用函数
		Target->ProcessEvent(Function, &Params);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulEventManager::ExecuteDelegate - Function %s not found on %s"),
			*FunctionName.ToString(), *Target->GetName());
	}
}