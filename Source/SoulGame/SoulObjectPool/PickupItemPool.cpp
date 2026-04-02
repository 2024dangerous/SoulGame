#include "PickupItemPool.h"
#include "SoulGameItem/PickupItem.h"
#include "Engine/World.h"

void UPickupItemPool::InitializePool(int32 InitialSize)
{
    if (!ItemClass || !WorldContext)
    {
        UE_LOG(LogTemp, Warning, TEXT("PickupItemPool: ItemClass or WorldContext is not set."));
        return;
    }

    for (int32 i = 0; i < InitialSize; ++i)
    {
        FActorSpawnParameters SpawnParams; 
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        APickupItem* NewItem = WorldContext->SpawnActor<APickupItem>(ItemClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
        if (NewItem)
        {
            NewItem->SetActorEnableCollision(false);
            NewItem->SetActorHiddenInGame(true);
            NewItem->SetActorTickEnabled(false);

            PooledItems.Add(NewItem);
        }
    }
}

UObject* UPickupItemPool::AcquireObject(UWorld* World)
{
    if (PooledItems.Num() > 0)
    {
        APickupItem* Item = PooledItems.Pop();
        if (IsValid(Item))
        {
            Item->SetActorEnableCollision(true);
            Item->SetActorHiddenInGame(false);
            Item->SetActorTickEnabled(true);
            return Item;
        }
    }

    // 没有可用对象，尝试创建
    if (ItemClass && World)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        APickupItem* NewItem = World->SpawnActor<APickupItem>(ItemClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
        if (NewItem)
        {
            return NewItem;
        }
    }

    return nullptr;
}

void UPickupItemPool::ReleaseObject(UObject* Object)
{
    APickupItem* Item = Cast<APickupItem>(Object);
    if (IsValid(Item))
    {
        Item->SetActorEnableCollision(false);
        Item->SetActorHiddenInGame(true);
        Item->SetActorTickEnabled(false);

        // 可选：重置位置
        Item->SetActorLocation(FVector::ZeroVector);

        PooledItems.Add(Item);
    }
}

void UPickupItemPool::SetupPool(TSubclassOf<AActor> ActorClass, UWorld* InWorld)
{
    if (ActorClass->IsChildOf(APickupItem::StaticClass()))
    {
        ItemClass = ActorClass; // 隐式转换安全
        WorldContext = InWorld;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ActorClass is not a subclass of APickupItem"));
    }
}