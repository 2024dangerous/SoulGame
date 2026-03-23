// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameAI/ShadowSpecter.h"
#include "../SoulGameData/SoulActionType.h"
#include "../../../../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SphereComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/BoxComponent.h"
#include "../SoulGameCharacter/SoulBaseCharacter.h"
#include "../SoulGameDebug/DebugTools.h"
#include "SoulAIController.h"
#include "../../../../../../../Source/Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "../SoulGameEvent/SoulEventManager.h"
#include "../../../../../../../Source/Runtime/Core/Public/Math/UnrealMathUtility.h"

AShadowSpecter::AShadowSpecter()
{
    Sword = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerSword"));
    Sword->SetupAttachment(GetMesh(), "Sword");
    SwordNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PlayerSwordNiagara"));
    SwordNiagara->SetupAttachment(Sword);

    ChangeWeaponMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/_MySoulGame/Characters/_MoYing/Materials/MI_Sword_Niagara.MI_Sword_Niagara"));

    SwordSheath = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerSwordSheath"));
    SwordSheath->SetupAttachment(GetMesh(), "SwordSheath");
    SwordAttackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SwordAttackBox"));
    SwordAttackBox->SetupAttachment(Sword);
    SwordAttackBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SwordAttackBox->SetCollisionObjectType(ECC_WorldDynamic);
    SwordAttackBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    SwordAttackBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    EnemyName = FName(TEXT("影魍"));
    AttackTarget = nullptr;

    bIsAttacking = false;
    InitLocation = {0,0,0};
    RandeomStrafeValue = -1.f;
}

void AShadowSpecter::BeginPlay()
{
    Super::BeginPlay();

    InitLocation = GetActorLocation(); 


    InitAnimMontage();
    GetAIController();

    if (AIController != nullptr)
    {
        AIController->GetBlackboardComponent()->SetValueAsVector("InitLocation", InitLocation);
    }

    PandoraBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AShadowSpecter::StartBattle);
    PandoraBox->OnComponentEndOverlap.AddUniqueDynamic(this, &AShadowSpecter::EndBattle);
}

void AShadowSpecter::Tick(float DeltaTime)
{

}

void AShadowSpecter::StartBattle(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ASoulBaseCharacter* PlayerCharacter = Cast<ASoulBaseCharacter>(OtherActor))
    {
        //设置攻击目标
        AttackTarget = PlayerCharacter;
        if (!AIController) return;
        UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
        if (!BlackboardComp) return;

        BlackboardComp->SetValueAsObject("AttackTarget", AttackTarget);
    }
}

void AShadowSpecter::EndBattle(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (ASoulBaseCharacter* PlayerCharacter = Cast<ASoulBaseCharacter>(OtherActor))
    {
        if (!AIController) return;
        UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
        if (!BlackboardComp) return;

        BlackboardComp->SetValueAsObject("AttackTarget", nullptr);
    }
}

void AShadowSpecter::InitAnimMontage()
{
    if (AnimMontageTable)
    {
        EquipAnims = AnimMontageTable->FindRow<FSoulActionType>(FName(TEXT("EquipAnims")), TEXT("InitEnemyActionAnimations"));
        MeleeAttackAnims = AnimMontageTable->FindRow<FSoulActionType>(FName(TEXT("MeleeAttackAnims")), TEXT("InitEnemyActionAnimations"));
        MeleeRollingAnims = AnimMontageTable->FindRow<FSoulActionType>(FName(TEXT("MeleeRollingAnims")), TEXT("InitEnemyActionAnimations"));
        SwordAttackAnims = AnimMontageTable->FindRow<FSoulActionType>(FName(TEXT("SwordAttackAnims")), TEXT("InitEnemyActionAnimations"));
        SwordRollingAnims = AnimMontageTable->FindRow<FSoulActionType>(FName(TEXT("SwordRollingAnims")), TEXT("InitEnemyActionAnimations"));
        RushAttackAnims = AnimMontageTable->FindRow<FSoulActionType>(FName(TEXT("RushAttackAnims")), TEXT("InitEnemyActionAnimations"));
        InjuryAnims = AnimMontageTable->FindRow<FSoulActionType>(FName(TEXT("InjuryAnims")), TEXT("InitEnemyActionAnimations"));
        DenfenseAnims = AnimMontageTable->FindRow<FSoulActionType>(FName(TEXT("DenfenseAnims")), TEXT("InitEnemyActionAnimations"));
    }

    //安全处理，如果没有找到对应的行，则使用默认值
    static FSoulActionType DefaultAction;
    if (!EquipAnims) EquipAnims = &DefaultAction;
    if (!MeleeAttackAnims) MeleeAttackAnims = &DefaultAction;
    if (!MeleeRollingAnims) MeleeRollingAnims = &DefaultAction;
    if (!SwordAttackAnims) SwordAttackAnims = &DefaultAction;
    if (!SwordRollingAnims) SwordRollingAnims = &DefaultAction;
    if (!RushAttackAnims) RushAttackAnims = &DefaultAction;
    if (!InjuryAnims) InjuryAnims = &DefaultAction;
    if (!DenfenseAnims) DenfenseAnims = &DefaultAction;
}

void AShadowSpecter::Injure(float SubHealth)
{
    CurrentHealth = FMath::Clamp(CurrentHealth - SubHealth,0.f,MaxHealth);
    USoulEventManager::Get()->SwitchEnemyHealth.ExecuteIfBound(EnemyName, CurrentHealth / MaxHealth);
    ZhouXiaoPeng_PRINT(TEXT("daijdaiwfw"));
}

void AShadowSpecter::Attack()
{
    if (!CanAttack()) return;

    RunningMovement(false);
    int TotalWeight = 70 + 30;
    // 生成 0 ~ TotalWeight-1 的随机数
    int RandValue = FMath::RandRange(0, TotalWeight - 1);
    if (RandValue<70)
    {
        if (EnemyBehavior == EEnemyBehavior::MELEEATTACK)
        {
            bIsAttacking = true;
            PlayRandomAnimMontage(MeleeAttackAnims, LastMeleeAttackIndex);
            AIController->GetBlackboardComponent()->SetValueAsBool("IsAttacking", true);
            AIController->GetBlackboardComponent()->SetValueAsBool("IsAttack", false);
        }
        else if (EnemyBehavior == EEnemyBehavior::SWORDATTACK)
        {
            bIsAttacking = true;
            PlayRandomAnimMontage(SwordAttackAnims, LastSwordAttackIndex);
            AIController->GetBlackboardComponent()->SetValueAsBool("IsAttacking", true);
            AIController->GetBlackboardComponent()->SetValueAsBool("IsAttack", false);
        }
    } 
    else
    {
        EnableObServer();
        GetWorldTimerManager().SetTimer(CloseObserverHandle, this, &AShadowSpecter::EndObserver, 2.f, false);
    }
}

bool AShadowSpecter::CanAttack()
{
    if (bIsAttacking) return false;

    if (EnemyBehavior == EEnemyBehavior::MELEEATTACK)
    {
        if (MeleeAttackAnims && MeleeAttackAnims->AnimMontage.Num() == 0) return false;

        return true;
    }
    
    if (EnemyBehavior == EEnemyBehavior::SWORDATTACK)
    {
        if (SwordAttackAnims && SwordAttackAnims->AnimMontage.Num() == 0) return false;

        return true;
    }
    return false;
}

void AShadowSpecter::ResetAttack()
{
    AIController->GetBlackboardComponent()->SetValueAsBool("IsAttacking", false);
}



// 播放随机动画
bool AShadowSpecter::PlayRandomAnimMontage(FSoulActionType* CurrentAnimMontage, int32& LastAnimMontageIndex)
{
    //获取当前动画实例
    UAnimInstance* CurAnimIns = GetMesh()->GetAnimInstance();
    if (!CurAnimIns) return false;

    if (!CurrentAnimMontage || CurrentAnimMontage->AnimMontage.Num() == 0) return false;

    //随机一个攻击动作
    int32 AttackAnimIndex = FMath::RandRange(0, CurrentAnimMontage->AnimMontage.Num() - 1);
    if (AttackAnimIndex == LastAnimMontageIndex)
    {
        // 确保不重复播放上一个动画
        AttackAnimIndex = (AttackAnimIndex + 1) % CurrentAnimMontage->AnimMontage.Num();
    }
    LastAnimMontageIndex = AttackAnimIndex;
    CurAnimIns->Montage_Play(CurrentAnimMontage->AnimMontage[AttackAnimIndex], 1.0f);

    return true;
}

void AShadowSpecter::EnableObServer()
{
    if (UKismetMathLibrary::RandomBool())
    {
        RandeomStrafeValue = 1.f;
    }
    else
    {
        RandeomStrafeValue = -1.f;
    }
    GetWorldTimerManager().SetTimer(ObserverHandle, this,&AShadowSpecter::ObserverMove, 0.001f,true);
    ZhouXiaoPeng_PRINT(TEXT("dfafafawf"));
}


void AShadowSpecter::ObserverMove()
{
    const FRotator LocalRotation = GetActorRotation();
    const FVector LocalVector = FRotationMatrix(FRotator(0,LocalRotation.Yaw,0)).GetUnitAxis(EAxis::Y);
    AddMovementInput(LocalVector, RandeomStrafeValue);
}

void AShadowSpecter::EndObserver()
{
    GetWorldTimerManager().ClearTimer(ObserverHandle);
    ResetAttack();
}

void AShadowSpecter::Running(bool IsRun)
{
    if (IsRun)
    {
        GetCharacterMovement()->MaxWalkSpeed = 600.f; // 设置跑步速度
    }
    else
    {
        GetCharacterMovement()->MaxWalkSpeed = 200.f; // 设置行走速度
    }
}

