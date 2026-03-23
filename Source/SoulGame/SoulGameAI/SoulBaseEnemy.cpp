// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameAI/SoulBaseEnemy.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SphereComponent.h"
#include "../SoulGameData/SoulEnumType.h"
#include "SoulAIController.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/WidgetComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"

ASoulBaseEnemy::ASoulBaseEnemy()
{
    EnemyTip = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyTip"));
    EnemyTip->SetupAttachment(GetMesh(), "spine_02");
    PandoraBox = CreateDefaultSubobject<USphereComponent>(TEXT("PandoraBox"));
    PandoraBox->SetupAttachment(RootComponent);
    PandoraBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PandoraBox->SetCollisionObjectType(ECC_WorldDynamic);
    PandoraBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    PandoraBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    AnimMontageTable = nullptr;
    EnemyBehavior = EEnemyBehavior::IDLE;
    EnemyName = "";
    CurrentHealth = 100.f;
    MaxHealth = 100.f;
}

void ASoulBaseEnemy::BeginPlay()
{
    Super::BeginPlay();
}

void ASoulBaseEnemy::Tick(float DeltaTime)
{
}

void ASoulBaseEnemy::GetAIController()
{
    if (AIController == nullptr)
    {
        AIController = Cast<ASoulAIController>(GetController());
    }
}

void ASoulBaseEnemy::SetEnemyTipVisibility(bool bIsVisibility)
{
    if (!EnemyTip) return;
    if (bIsVisibility)
    {
        EnemyTip->SetVisibility(true);
    }
    else
    {
        EnemyTip->SetVisibility(false);
    }
}

void ASoulBaseEnemy::Injure(float SubHealth)
{
}

void ASoulBaseEnemy::RunningMovement(bool IsRun)
{
    if (GetCharacterMovement())
    {
        if (IsRun)
        {
            GetCharacterMovement()->MaxWalkSpeed = 800.f; //设置跑步速度
        }
        else
        {
            GetCharacterMovement()->MaxWalkSpeed = 400.f; //设置走路速度
        }
    }
}
