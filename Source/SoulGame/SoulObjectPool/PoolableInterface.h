#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PoolableInterface.generated.h"

// 这个 UINTERFACE 是必须的宏结构，供蓝图识别
UINTERFACE(MinimalAPI)
class UPoolableInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * 对象池接口：让被池化的对象支持自定义行为
 */
class SOULGAME_API IPoolableInterface
{
    GENERATED_BODY()

public:
    // 当对象被从池中取出使用时调用
    virtual void OnPooledObjectSpawn() = 0;

    // 当对象被回收到池中时调用
    virtual void OnPooledObjectDespawn() = 0;
};
