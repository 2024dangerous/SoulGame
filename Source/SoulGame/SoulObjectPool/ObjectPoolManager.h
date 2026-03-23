#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ObjectPoolManager.generated.h"

class UObjectPoolBase;
UCLASS()
class SOULGAME_API UObjectPoolManager : public UGameInstance
{
    GENERATED_BODY()

public:
    // 初始化池子（可在GameInstance Init中调用）
    virtual void Init() override;

    // 创建一个新的对象池
    UFUNCTION(BlueprintCallable, Category = "ObjectPool")
    void CreatePool(FName PoolName, TSubclassOf<UObjectPoolBase> PoolClass, TSubclassOf<AActor> ActorClass, int32 InitialSize, UWorld* World);


    // 获取池中对象
    UFUNCTION(BlueprintCallable, Category = "ObjectPool")
    UObject* Acquire(FName PoolName, UObject* WorldContextObject);

    // 归还对象到池
    UFUNCTION(BlueprintCallable, Category = "ObjectPool")
    void Release(FName PoolName, UObject* Object);

private:
    // 储存所有对象池
    UPROPERTY()
    TMap<FName, UObjectPoolBase*> Pools;

  
};