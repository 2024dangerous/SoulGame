#pragma once

#include "CoreMinimal.h"
#include "ObjectPoolBase.h"

#include "PickupItemPool.generated.h"

class APickupItem;
UCLASS(Blueprintable)
class SOULGAME_API UPickupItemPool : public UObjectPoolBase
{
    GENERATED_BODY()

public:
    // 初始化池子
    virtual void InitializePool(int32 InitialSize) override;

    // 从池中获取一个 APickupItem 实例
    virtual UObject* AcquireObject(UWorld* World) override;

    // 将使用完的对象归还池中
    virtual void ReleaseObject(UObject* Object) override;

protected:
    // 实际用于存储可用对象的容器
    UPROPERTY()
    TArray<APickupItem*> PooledItems;

    // 用于指定生成的Actor类型（可换成子类）
    TSubclassOf<APickupItem> ItemClass;

    // 当前所在的World指针，实例化Actor时用
    UWorld* WorldContext;

public:
    // 设置生成的Actor类型和World（你可以在创建时设置）
    virtual void SetupPool(TSubclassOf<AActor> ActorClass, UWorld* World) override;
};