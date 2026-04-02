// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameItem/PickupItem.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "SoulGameCharacter/SoulPlayerCharacter.h"
#include "SoulGameEvent/SoulEventManager.h"
#include "SoulGameItemType/SoulGameItemType.h"

void APickupItem::OnPooledObjectSpawn()
{
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
    GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("PickupItem Spawned!"));
}

void APickupItem::OnPooledObjectDespawn()
{
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("PickupItem Despawn!"));
}

APickupItem::APickupItem()
{
    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    ItemNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemNiagara"));
    ItemSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ItemSphereComponent"));
    ItemMesh->SetupAttachment(ItemSphereComponent);
    ItemNiagara->SetupAttachment(ItemMesh);

    ItemType = ESoulGameItemType::None;
}

void APickupItem::BeginPlay()
{
    Super::BeginPlay();
    //USoulEventManager::Get()->OpenInteractionUI.BindUObject(this, &APickupItem::PickItemToPlayer);
    ItemSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APickupItem::OnPlayerEnterPickupRange);
    ItemSphereComponent->OnComponentEndOverlap.AddDynamic(this, &APickupItem::OnPlayerExitPickupRange);
}

void APickupItem::OnPlayerEnterPickupRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ASoulPlayerCharacter* PlayerCharacter = Cast<ASoulPlayerCharacter>(OtherActor))
    {
        USoulEventManager::Get()->NearbyInteractables.ExecuteIfBound(this,true);
    }
}

void APickupItem::OnPlayerExitPickupRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (ASoulPlayerCharacter* PlayerCharacter = Cast<ASoulPlayerCharacter>(OtherActor))
    {
        USoulEventManager::Get()->NearbyInteractables.ExecuteIfBound(this, false);
    }
}


