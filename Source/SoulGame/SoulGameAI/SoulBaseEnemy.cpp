// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameAI/SoulBaseEnemy.h"
#include "Components/SphereComponent.h"
#include "SoulGameData/SoulEnumType.h"
#include "SoulAIController.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SoulGameGAS/SoulAbilitySystemComponent.h"
#include "SoulGameGAS/SoulAttributeSet.h"
#include "SoulGameGAS/SoulGameplayAbility.h"
#include "SoulGameAI/Config/SoulAIConfig.h"

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

    // GAS 初始化
    AbilitySystemComponent = CreateDefaultSubobject<USoulAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    AnimMontageTable = nullptr;
    AIConfig = nullptr;
    EnemyBehavior = EEnemyBehavior::IDLE;
    EnemyName = "";
    CurrentHealth = 100.f;
    MaxHealth = 100.f;
}

void ASoulBaseEnemy::BeginPlay()
{
    Super::BeginPlay();

    // 初始化 GAS
    InitializeGAS();
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
    EnemyTip->SetVisibility(bIsVisibility);
}

void ASoulBaseEnemy::Injure(float SubHealth)
{
    CurrentHealth = FMath::Clamp(CurrentHealth - SubHealth, 0.f, MaxHealth);
    // 同步到 GAS
    if (AbilitySystemComponent && bGASInitialized)
    {
        AbilitySystemComponent->SetNumericAttributeBase(USoulAttributeSet::GetHealthAttribute(), CurrentHealth);
    }
}

void ASoulBaseEnemy::RunningMovement(bool IsRun)
{
    if (AIController)
    {
        AIController->SetMovementSpeed(IsRun);
    }
    else
    {
        // 回退到默认逻辑
        if (IsRun)
        {
            GetCharacterMovement()->MaxWalkSpeed = 600.f;
        }
        else
        {
            GetCharacterMovement()->MaxWalkSpeed = 200.f;
        }
    }
}

// ============ GAS ============

UAbilitySystemComponent* ASoulBaseEnemy::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

USoulAbilitySystemComponent* ASoulBaseEnemy::GetSoulAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

const USoulAttributeSet* ASoulBaseEnemy::GetSoulAttributeSet() const
{
    return AttributeSet;
}

void ASoulBaseEnemy::InitializeGAS()
{
    if (bGASInitialized) return;

    if (!AbilitySystemComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("ASoulBaseEnemy::InitializeGAS - AbilitySystemComponent is null"));
        return;
    }

    // 初始化 ASC
    AbilitySystemComponent->InitAbilityActorInfo(this, this);

    // 获取属性集
    AttributeSet = AbilitySystemComponent->GetSet<USoulAttributeSet>();
    if (!AttributeSet)
    {
        USoulAttributeSet* NewAttributeSet = NewObject<USoulAttributeSet>(this);
        AbilitySystemComponent->AddAttributeSetSubobject(NewAttributeSet);
        AttributeSet = NewAttributeSet;
    }

    // 同步旧属性到 GAS
    AbilitySystemComponent->SetNumericAttributeBase(USoulAttributeSet::GetHealthAttribute(), CurrentHealth);
    AbilitySystemComponent->SetNumericAttributeBase(USoulAttributeSet::GetMaxHealthAttribute(), MaxHealth);

    // 从 AIConfig 授予默认技能
    if (AIConfig && AIConfig->DefaultAbilities.Num() > 0)
    {
        AbilitySystemComponent->InitializeDefaultAbilities(AIConfig->DefaultAbilities);
    }
    // 或从 DefaultAbilities 列表授予
    else if (DefaultAbilities.Num() > 0)
    {
        AbilitySystemComponent->InitializeDefaultAbilities(DefaultAbilities);
    }

    bGASInitialized = true;
    UE_LOG(LogTemp, Log, TEXT("ASoulBaseEnemy::InitializeGAS - %s GAS 初始化完成"), *EnemyName.ToString());
}

float ASoulBaseEnemy::GetHealth() const
{
    if (AttributeSet)
    {
        return AttributeSet->GetHealth();
    }
    return CurrentHealth;
}

// ============ GameplayTags 接口 ============

FGameplayTagContainer ASoulBaseEnemy::GetGameplayTagContainer_Implementation() const
{
    return GameplayTagContainer;
}

void ASoulBaseEnemy::AddGameplayTag_Implementation(const FGameplayTag Tag)
{
    if (Tag.IsValid())
    {
        GameplayTagContainer.AddTag(Tag);
    }
}

void ASoulBaseEnemy::RemoveGameplayTag_Implementation(const FGameplayTag Tag)
{
    if (Tag.IsValid())
    {
        GameplayTagContainer.RemoveTag(Tag);
    }
}

bool ASoulBaseEnemy::HasGameplayTag_Implementation(const FGameplayTag Tag) const
{
    return GameplayTagContainer.HasTag(Tag);
}