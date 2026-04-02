// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulGameCharacter/SoulPickupComponent.h"
#include "SoulGameItem/PickupItem.h"
#include "SoulGameEvent/SoulEventManager.h"

USoulPickupComponent::USoulPickupComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USoulPickupComponent::BeginPlay()
{
	Super::BeginPlay();

	// 订阅附近可交互物体变化事件
	if (USoulEventManager* EventManager = USoulEventManager::Get())
	{
		EventManager->NearbyInteractables.BindUObject(this, &USoulPickupComponent::OnNearbyInteractablesChanged);
	}
}

void USoulPickupComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 取消订阅事件
	if (USoulEventManager* EventManager = USoulEventManager::Get())
	{
		EventManager->NearbyInteractables.Unbind();
	}

	PickupItemArray.Empty();
	Super::EndPlay(EndPlayReason);
}

void USoulPickupComponent::PickupAllItems()
{
	if (PickupItemArray.Num() == 0) return;

	TArray<APickupItem*> ItemsCopy = PickupItemArray;
	for (APickupItem* Item : ItemsCopy)
	{
		if (IsValid(Item))
		{
			Item->Destroy();
			PickupItemArray.Remove(Item);
		}
	}
	UpdateInteractionUI();
}

void USoulPickupComponent::AddPickupItem(APickupItem* Item)
{
	if (!Item) return;

	if (!PickupItemArray.Contains(Item))
	{
		PickupItemArray.Add(Item);
		UpdateInteractionUI();
	}
}

void USoulPickupComponent::RemovePickupItem(APickupItem* Item)
{
	if (!Item) return;

	if (PickupItemArray.Contains(Item))
	{
		PickupItemArray.Remove(Item);
		UpdateInteractionUI();
	}
}

void USoulPickupComponent::OnNearbyInteractablesChanged(AActor* InteractableActor, bool bIsAdd)
{
	if (APickupItem* TypedItem = Cast<APickupItem>(InteractableActor))
	{
		if (bIsAdd)
		{
			AddPickupItem(TypedItem);
		}
		else
		{
			RemovePickupItem(TypedItem);
		}
	}
}

void USoulPickupComponent::UpdateInteractionUI()
{
	if (USoulEventManager* EventManager = USoulEventManager::Get())
	{
		if (!PickupItemArray.IsEmpty())
		{
			EventManager->OpenInteractionUI.ExecuteIfBound(true, FText::FromString(TEXT("拾取")));
		}
		else
		{
			EventManager->OpenInteractionUI.ExecuteIfBound(false, FText::FromString(TEXT("")));
		}
	}
}
