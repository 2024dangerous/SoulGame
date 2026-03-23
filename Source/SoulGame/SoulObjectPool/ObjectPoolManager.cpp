#include "SoulObjectPool/ObjectPoolManager.h"
#include "SoulObjectPool/ObjectPoolBase.h"
#include "SoulObjectPool/PickupItemPool.h"  // 如需支持其他类型的池，记得引入
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "../SoulGameItem/PickupItem.h"

void UObjectPoolManager::Init()
{
    Super::Init();
}

void UObjectPoolManager::CreatePool(FName PoolName, TSubclassOf<UObjectPoolBase> PoolClass, TSubclassOf<AActor> ActorClass, int32 InitialSize, UWorld* World)
{
    if (Pools.Contains(PoolName))
    {
        UE_LOG(LogTemp, Warning, TEXT("Pool '%s' already exists."), *PoolName.ToString());
        return;
    }

    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("World is null when creating pool '%s'."), *PoolName.ToString());
        return;
    }

    UObjectPoolBase* NewPool = NewObject<UObjectPoolBase>(this, PoolClass);
    if (!NewPool)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create pool '%s'."), *PoolName.ToString());
        return;
    }

    NewPool->SetupPool(ActorClass, World);
    NewPool->InitializePool(InitialSize);

    Pools.Add(PoolName, NewPool);

    UE_LOG(LogTemp, Log, TEXT("Created pool '%s' with %d objects."), *PoolName.ToString(), InitialSize);
}

UObject* UObjectPoolManager::Acquire(FName PoolName, UObject* WorldContextObject)
{
    if (!WorldContextObject)
    {
        UE_LOG(LogTemp, Error, TEXT("WorldContextObject is null in Acquire."));
        return nullptr;
    }

    UWorld* World = WorldContextObject->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("World is null in Acquire for context object: %s"), *WorldContextObject->GetName());
        return nullptr;
    }

    if (UObjectPoolBase** FoundPool = Pools.Find(PoolName))
    {
        return (*FoundPool)->AcquireObject(World); 
    }

    UE_LOG(LogTemp, Warning, TEXT("No pool found with name: %s"), *PoolName.ToString());
    return nullptr;
}

void UObjectPoolManager::Release(FName PoolName, UObject* Object)
{
    if (UObjectPoolBase** FoundPool = Pools.Find(PoolName))
    {
        (*FoundPool)->ReleaseObject(Object);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No pool found with name: %s"), *PoolName.ToString());
    }
}

