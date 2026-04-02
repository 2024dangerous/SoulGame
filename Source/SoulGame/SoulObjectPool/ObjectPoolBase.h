#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ObjectPoolBase.generated.h"

/**
 * 泛用的对象池基类
 */
UCLASS(Abstract, Blueprintable)
class SOULGAME_API UObjectPoolBase : public UObject
{
    GENERATED_BODY()

public:
    // 初始化对象池（派生类必须实现）
    virtual void InitializePool(int32 InitialSize) PURE_VIRTUAL(UObjectPoolBase::InitializePool, );

    // 获取一个可用对象（派生类必须实现）
    virtual UObject* AcquireObject(UWorld* World) PURE_VIRTUAL(UObjectPoolBase::AcquireObject, return nullptr;);

    // 回收对象（派生类必须实现）
    virtual void ReleaseObject(UObject* Object) PURE_VIRTUAL(UObjectPoolBase::ReleaseObject, );

    virtual void SetupPool(TSubclassOf<AActor> ActorClass, UWorld* World) {}
};