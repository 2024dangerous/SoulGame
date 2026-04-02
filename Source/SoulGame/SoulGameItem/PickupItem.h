// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoulGameItem/ItemBase.h"
#include "SoulObjectPool/PoolableInterface.h"
#include "PickupItem.generated.h"

/**
 * 
 */
class UStaticMeshComponent;
class UNiagaraComponent;
class USphereComponent;
enum class ESoulGameItemType :uint8;
UCLASS()
class SOULGAME_API APickupItem : public AItemBase , public IPoolableInterface
{
	GENERATED_BODY()
	
public:
    // 添加这两行函数声明
    virtual void OnPooledObjectSpawn() override;
    virtual void OnPooledObjectDespawn() override;


public:
    APickupItem();

    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemType")
    FString ItemName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemType")
    ESoulGameItemType ItemType;

    UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Pickup")
    UStaticMeshComponent* ItemMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup")
    UNiagaraComponent* ItemNiagara;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup")
    USphereComponent* ItemSphereComponent;


    UFUNCTION()
    void OnPlayerEnterPickupRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnPlayerExitPickupRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
