// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameCharacter/SoulPlayerCharacter.h"
#include "SoulPlayerController.h"
#include "Engine/DataTable.h"
#include "SoulGameData/SoulActionType.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/SlateCore/Public/Styling/SlateColor.h"
#include "../../../../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "../SoulGameDebug/DebugTools.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/BoxComponent.h"
#include "../../../../../../../Source/Runtime/Core/Public/Math/MathFwd.h"
#include "../SoulGameAI/SoulBaseEnemy.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "../SoulGameEvent/SoulEventManager.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/CapsuleComponent.h"

ASoulPlayerCharacter::ASoulPlayerCharacter()
{

    Sword = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerSword"));
    Sword->SetupAttachment(GetMesh(), "Sword");
    SwordNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PlayerSwordNiagara"));
    SwordNiagara->SetupAttachment(Sword);
    SwordSheath = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerSwordSheath"));
    SwordSheath->SetupAttachment(GetMesh(), "SwordSheath");
    SwordAttackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SwordAttackBox"));
    SwordAttackBox->SetupAttachment(Sword);
    SwordAttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SwordAttackBox->SetCollisionObjectType(ECC_WorldDynamic);
    SwordAttackBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    SwordAttackBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    CurrentWeaponMaterial = Sword->GetMaterial(0);
    CurrentSwordSheathMaterial = SwordSheath->GetMaterial(0);
    ChangeWeaponMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/_MySoulGame/Characters/_MoYing/Materials/MI_Sword_Niagara.MI_Sword_Niagara"));
    bIsChangingWeapons = false;
    bIsWeapons = false;


    LastMeleeAttackIndex = -1;
    LastSwordAttackIndex = -1;

    //玩家属性
    CurrentHealth = 100.f;
    MaxHealth = 100.f;
    MaxStamina = 100.f;
    CurrentStamina = 100.f;
    SubStamina = 0.f;
    AddStamina = 10.f;

    bIsDie = false;
    //RollingAnimSpeed = 0.5f;
}
void ASoulPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (ASoulPlayerController* PC = Cast<ASoulPlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(PlayerMappingContext, 0);
        }
    }
    InitAnimMontage();
    SwordAttackBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ASoulPlayerCharacter::SwordAttackEnemy);
}
void ASoulPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    Focusing();
}
void ASoulPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        BIND_ACTION_SAFE(EnhancedInputComponent, AttackAction, ETriggerEvent::Started, this, &ASoulPlayerCharacter::Attack);
        BIND_ACTION_SAFE(EnhancedInputComponent, RollingAction, ETriggerEvent::Started, this, &ASoulPlayerCharacter::Rolling);
        BIND_ACTION_SAFE(EnhancedInputComponent, WeaponsAction, ETriggerEvent::Started, this, &ASoulPlayerCharacter::Weapons);
        BIND_ACTION_SAFE(EnhancedInputComponent, DefenseAction, ETriggerEvent::Started, this, &ASoulPlayerCharacter::Defense);
        BIND_ACTION_SAFE(EnhancedInputComponent, DefenseAction, ETriggerEvent::Completed, this, &ASoulPlayerCharacter::Defense);
        BIND_ACTION_SAFE(EnhancedInputComponent, FocusAction, ETriggerEvent::Started, this, &ASoulPlayerCharacter::Focus);
    }
}


void ASoulPlayerCharacter::InitAnimMontage()
{
    if (StaminaCost)
    {
        MeleeAttackAnim = StaminaCost->FindRow<FSoulActionType>(FName(TEXT("MeleeAttackAnim")), TEXT("InitActionAnimations"));
        MeleeRollingAnim = StaminaCost->FindRow<FSoulActionType>(FName(TEXT("MeleeRollingAnim")), TEXT("InitActionAnimations"));
        SwordAttackAnim = StaminaCost->FindRow<FSoulActionType>(FName(TEXT("SwordAttackAnim")), TEXT("InitActionAnimations"));
        SwordRollingAnim = StaminaCost->FindRow<FSoulActionType>(FName(TEXT("SwordRollingAnim")), TEXT("InitActionAnimations"));

        MeleeInjuryAnim = StaminaCost->FindRow<FSoulActionType>(FName(TEXT("MeleeInjuryAnim")), TEXT("InitActionAnimations"));
        MeleePreparwarInjuryAnim = StaminaCost->FindRow<FSoulActionType>(FName(TEXT("MeleePreparwarInjuryAnim")), TEXT("InitActionAnimations"));
        SwordInjuryAnim = StaminaCost->FindRow<FSoulActionType>(FName(TEXT("SwordInjuryAnim")), TEXT("InitActionAnimations"));
        SwordDefenseInjuryAnim = StaminaCost->FindRow<FSoulActionType>(FName(TEXT("SwordDefenseInjuryAnim")), TEXT("InitActionAnimations"));
    }
    else
    {
        ZhouXiaoPeng_LOG(TEXT("StaminaCost!!!!"));
    }

    //安全处理，如果没有找到对应的行，则使用默认值
    static FSoulActionType DefaultAction;
    if (!MeleeAttackAnim) MeleeAttackAnim = &DefaultAction;
    if (!MeleeRollingAnim) MeleeRollingAnim = &DefaultAction;
    if (!SwordAttackAnim) SwordAttackAnim = &DefaultAction;
    if (!SwordRollingAnim) SwordRollingAnim = &DefaultAction;
    if (!MeleeInjuryAnim) MeleeInjuryAnim = &DefaultAction;
    if (!MeleePreparwarInjuryAnim) MeleePreparwarInjuryAnim = &DefaultAction;
    if (!SwordInjuryAnim) SwordInjuryAnim = &DefaultAction;
    if (!SwordDefenseInjuryAnim) SwordDefenseInjuryAnim = &DefaultAction;
}

void ASoulPlayerCharacter::SetSwordAttackBoxCollisionEnabled(bool bIsEnabled)
{
    if (bIsEnabled)
    {
        SwordAttackBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
    else
    {
        SwordAttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}

void ASoulPlayerCharacter::SwordAttackEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ASoulBaseEnemy* Enemy = Cast<ASoulBaseEnemy>(OtherActor))
    {
        Enemy->Injure(30);
    }
}

void ASoulPlayerCharacter::Look(const FInputActionValue& Value)
{
    Super::Look(Value);
    FVector2D LookVector = Value.Get<FVector2D>();
    if (bIsFocus)
    {
        AccumulatedMouseX += LookVector.X;
        ZhouXiaoPeng_PRINT(FString::Printf(TEXT("AccumulatedMouseX: %f"), AccumulatedMouseX));
        if (AccumulatedMouseX >= SwitchThreshold)
        {
            SwitchTargetEnemy(true); // 向右切换目标
            AccumulatedMouseX = 0.0f; // 重置累计
        }
        else if (AccumulatedMouseX <= -SwitchThreshold)
        {
            SwitchTargetEnemy(false); // 向左切换目标
            AccumulatedMouseX = 0.0f; // 重置累计
        }

        // 在锁定模式下，可能不允许自由旋转相机，你可以选择跳过下面的相机控制
    }
}

ASoulBaseEnemy* ASoulPlayerCharacter::GetNextTargetEnemy(bool bIsRight)
{
    if (EnemyArray.Num() == 0 || !Controller) return nullptr;

    FVector CameraForward = Controller->GetControlRotation().Vector(); // 摄像机前向
    FVector MyLocation = GetActorLocation();

    TArray<TPair<float, ASoulBaseEnemy*>> SortedEnemies;

    for (ASoulBaseEnemy* Enemy : EnemyArray)
    {
        if (!IsValid(Enemy)) continue;

        FVector DirToEnemy = (Enemy->GetActorLocation() - MyLocation).GetSafeNormal();
        float Angle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(CameraForward, DirToEnemy)));

        // 判断敌人是否在右边（叉乘看方向）
        FVector Cross = FVector::CrossProduct(CameraForward, DirToEnemy);
        float SignedAngle = Cross.Z > 0 ? Angle : -Angle;

        SortedEnemies.Add(TPair<float, ASoulBaseEnemy*>(SignedAngle, Enemy));
    }

    // 从左到右排序（角度从负到正）
    SortedEnemies.Sort([](const auto& A, const auto& B)
        {
            return A.Key < B.Key;
        });

    // 找到当前锁定目标位置
    int32 CurrentIndex = SortedEnemies.IndexOfByPredicate([this](const TPair<float, ASoulBaseEnemy*>& Pair)
        {
            return Pair.Value == FocusedTarget;
        });

    // 计算下一个目标索引
    int32 NextIndex = INDEX_NONE;
    if (CurrentIndex != INDEX_NONE)
    {
        if (bIsRight)
        {
            NextIndex = (CurrentIndex + 1) % SortedEnemies.Num();
        }
        else
        {
            NextIndex = (CurrentIndex - 1 + SortedEnemies.Num()) % SortedEnemies.Num();
        }
    }
    else
    {
        // 当前未锁定，选最正前方的（夹角最小）
        NextIndex = 0;
    }

    return SortedEnemies.IsValidIndex(NextIndex) ? SortedEnemies[NextIndex].Value : nullptr;

}

void ASoulPlayerCharacter::SwitchTargetEnemy(bool bIsRight)
{
    ASoulBaseEnemy* NewTarget = GetNextTargetEnemy(bIsRight);
    if (NewTarget && NewTarget != FocusedTarget)
    {
        FocusedTarget->SetEnemyTipVisibility(false);
        FocusedTarget = NewTarget;
        FocusedTarget->SetEnemyTipVisibility(true);
        USoulEventManager::Get()->SwitchEnemyHealth.ExecuteIfBound(FocusedTarget->EnemyName, FocusedTarget->CurrentHealth / FocusedTarget->MaxHealth);
        // 你可以在这里播放UI提示或特效
    }
}

void ASoulPlayerCharacter::Exit(const FInputActionValue& Value)
{
    USoulEventManager::Get()->OpenSetMenu.ExecuteIfBound(true);
}


//FLinearColor(0.25f,0.75f,1.0f,1.f)法力值颜色
#pragma region "Attack"

//攻击逻辑
void ASoulPlayerCharacter::Attack()
{
    switch (WeaponType)
    {
    case EWeaponType::MELEE:
        MeleeAttack();
        break;
    case EWeaponType::SWORD:
        SwordAttack();
        break;
    }
}
void ASoulPlayerCharacter::MeleeAttack()
{
    //是否能攻击
    if (!CanMeleeAttack()) return;

    //播放随机动画
    if (
        !PlayRandomAnimMontage(MeleeAttackAnim, LastMeleeAttackIndex)) return;


    GetWorldTimerManager().ClearTimer(ExitAttackStateTimerHandle); // 清除之前的退出攻击状态的定时器
    GetWorldTimerManager().ClearTimer(StaminaRestoreTimer); // 清除之前的恢复体力的定时器
    //改变状态（战斗状态）
    PlayerBehavior = EPlayerBehavior::ATTACK;
    PlayerStates = EPlayerStates::PREPARWAR;


    // 设置体力值，并广播（CurrentStamina最低为0）
    SetStamina(CurrentStamina, FMath::Max(CurrentStamina - MeleeAttackAnim->StaminaAnimCost, 0.f),MaxStamina);
    //摄像头晃动
    CameraShakeFeedBack(false);

    //设置退出攻击状态的定时器
    GetWorldTimerManager().SetTimer(ExitAttackStateTimerHandle, this, &ASoulPlayerCharacter::ExitAttackState, 10.0f, false);
    //设置恢复体力的定时器
    GetWorldTimerManager().SetTimer(StaminaRestoreTimer, this, &ASoulPlayerCharacter::UpdateStaminaRestore, 0.05f, true);

}
void ASoulPlayerCharacter::SwordAttack()
{
    //是否能攻击
    if (!CanSwordAttack()) return;

    if (!PlayRandomAnimMontage(SwordAttackAnim, LastSwordAttackIndex)) return;

    //改变行为（攻击行为）
    PlayerBehavior = EPlayerBehavior::ATTACK;

    GetWorldTimerManager().ClearTimer(StaminaRestoreTimer); // 清除之前的恢复体力的定时器
    // 设置体力值，并广播（CurrentStamina最低为0）
    SetStamina(CurrentStamina, FMath::Max(CurrentStamina - SwordAttackAnim->StaminaAnimCost, 0.f), MaxStamina);
    //摄像头晃动
    CameraShakeFeedBack(false);
    //设置恢复体力的定时器
    GetWorldTimerManager().SetTimer(StaminaRestoreTimer, this, &ASoulPlayerCharacter::UpdateStaminaRestore, 0.05f, true);
}
bool ASoulPlayerCharacter::CanMeleeAttack()
{
    if (!MeleeAttackAnim || MeleeAttackAnim->AnimMontage.Num() == 0) return false;

    //玩家为待机状态和体力值大于等于攻击消耗值，才能攻击
    if (PlayerBehavior == EPlayerBehavior::IDLE && CurrentStamina >= MeleeAttackAnim->StaminaAnimCost)
    {
        return true;
    }

    //体力不足播放动画
    if ((CurrentStamina < MeleeAttackAnim->StaminaAnimCost) && !bIsIRShow)
    {
        ShowIR(FText::FromString(TEXT("体力不足")), FSlateColor(FLinearColor::Yellow));
        SubStamina = MeleeAttackAnim->StaminaAnimCost;
    }
    return false;
}
bool ASoulPlayerCharacter::CanSwordAttack()
{
    //是否有动作
    if (!SwordAttackAnim || SwordAttackAnim->AnimMontage.Num() == 0) return false;

    //是否在武器状态
    if (!bIsWeapons) return false;

    //玩家为待机状态和体力值大于等于攻击消耗值，才能攻击
    if (PlayerBehavior == EPlayerBehavior::IDLE && CurrentStamina >= SwordAttackAnim->StaminaAnimCost)
    {
        return true;
    }

    //体力不足播放动画
    if ((CurrentStamina < SwordAttackAnim->StaminaAnimCost) && !bIsIRShow)
    {
        ShowIR(FText::FromString(TEXT("体力不足")), FSlateColor(FLinearColor::Yellow));
        SubStamina = SwordAttackAnim->StaminaAnimCost;
    }
    return false;
}
//是否能攻击

//退出攻击状态
void ASoulPlayerCharacter::ExitAttackState()
{
    PlayerStates = EPlayerStates::COMMON;
}
#pragma endregion "Attack"
#pragma region "Rolling"
//翻滚逻辑
void ASoulPlayerCharacter::Rolling()
{
    switch (WeaponType)
    {
    case EWeaponType::MELEE:
        MeleeRolling();
        break;
    case EWeaponType::SWORD:
        SwordRolling();
        break;
    }
}
void ASoulPlayerCharacter::MeleeRolling()
{
    if (!CanMeleeRolling()) return;
    if (!PlayRollingAnimMontage(MeleeRollingAnim)) return;

    //切换状态（翻滚状态）
    PlayerBehavior = EPlayerBehavior::ROLLING;

    //开始恢复体力
    GetWorldTimerManager().ClearTimer(StaminaRestoreTimer); // 清除之前的

    // 设置体力值，并广播（CurrentStamina最低为0）
    SetStamina(CurrentStamina, FMath::Max(CurrentStamina - MeleeRollingAnim->StaminaAnimCost, 0.f), MaxStamina);

    //摄像头晃动
    CameraShakeFeedBack(false);

    //设置恢复体力的定时器
    GetWorldTimerManager().SetTimer(StaminaRestoreTimer, this, &ASoulPlayerCharacter::UpdateStaminaRestore, 0.05f, true);

}
void ASoulPlayerCharacter::SwordRolling()
{
    if (!CanSwordRolling()) return;
    if (!PlayRollingAnimMontage(SwordRollingAnim)) return;

    //切换状态（翻滚状态）
    PlayerBehavior = EPlayerBehavior::ROLLING;

    //开始恢复体力
    GetWorldTimerManager().ClearTimer(StaminaRestoreTimer); // 清除之前的

    // 设置体力值，并广播（CurrentStamina最低为0）
    SetStamina(CurrentStamina, FMath::Max(CurrentStamina - SwordRollingAnim->StaminaAnimCost, 0.f), MaxStamina);

    //摄像头晃动
    CameraShakeFeedBack(false);

    //设置恢复体力的定时器
    GetWorldTimerManager().SetTimer(StaminaRestoreTimer, this, &ASoulPlayerCharacter::UpdateStaminaRestore, 0.05f, true);
}
//是否能翻滚
bool ASoulPlayerCharacter::CanMeleeRolling()
{
    if (!MeleeRollingAnim || MeleeRollingAnim->AnimMontage.Num() == 0) return false;

    //玩家为待机状态和体力值大于等于攻击消耗值，才能翻滚
    if (PlayerBehavior == EPlayerBehavior::IDLE && CurrentStamina >= MeleeRollingAnim->StaminaAnimCost)
    {
        return true;
    }

    //体力不足播放动画
    if ((CurrentStamina < MeleeRollingAnim->StaminaAnimCost) && !bIsIRShow)
    {
        ShowIR(FText::FromString(TEXT("体力不足")), FSlateColor(FLinearColor::Yellow));
        SubStamina = MeleeRollingAnim->StaminaAnimCost;
    }

    return false;
}
bool ASoulPlayerCharacter::CanSwordRolling()
{
    if (!SwordRollingAnim || SwordRollingAnim->AnimMontage.Num() == 0) return false;

    //玩家为待机状态和体力值大于等于攻击消耗值，才能翻滚
    if (PlayerBehavior == EPlayerBehavior::IDLE && CurrentStamina >= SwordRollingAnim->StaminaAnimCost && WeaponType == EWeaponType::SWORD)
    {
        return true;
    }

    //体力不足播放动画
    if ((CurrentStamina < SwordRollingAnim->StaminaAnimCost) && !bIsIRShow)
    {
        ShowIR(FText::FromString(TEXT("体力不足")), FSlateColor(FLinearColor::Yellow));
        SubStamina = SwordRollingAnim->StaminaAnimCost;
    }

    return false;
}

#pragma endregion "Rolling"
#pragma region "Defense"
void ASoulPlayerCharacter::Defense(const FInputActionValue& Value)
{
    if (Value.Get<bool>() && CanDefense())
    {
        PlayerBehavior = EPlayerBehavior::DENFENSE;
        bIsDefense = true;
    }
    else if (!Value.Get<bool>())
    {
        PlayerBehavior = EPlayerBehavior::IDLE;
        bIsDefense = false;
    }
}

bool ASoulPlayerCharacter::CanDefense()
{
    if (PlayerBehavior == EPlayerBehavior::IDLE && WeaponType == EWeaponType::SWORD)
    {
        return true;
    }
    return false;
}
#pragma endregion "Defense"
#pragma region "Weapons"
void ASoulPlayerCharacter::Weapons()
{
    //是否能切换武器
    if (!CanWeapons()) return;

    //开始切换武器特效
    SwordNiagara->Activate(true);
    Sword->SetMaterial(0, ChangeWeaponMaterial);
    SwordSheath->SetMaterial(0, ChangeWeaponMaterial);

    bIsChangingWeapons = true;
    //一秒后切换状态
    GetWorldTimerManager().SetTimer(QieHuanWeapons, this, &ASoulPlayerCharacter::ChangeSwordWeaponType, 1.f, false);
}

//切换武器插槽（动画通知切换）
void ASoulPlayerCharacter::ChangeSwordSlot(bool bIsSword)
{
    if (bIsSword)
    {
        Sword->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "Weapon_r");
        SwordSheath->SetVisibility(false);
        GetWorldTimerManager().SetTimer(CloseNiagaraWeapons, this, &ASoulPlayerCharacter::CloseSwordNiagara, 1.f, false);
    }
    else
    {
        Sword->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "Sword");
        SwordSheath->SetVisibility(true);
        GetWorldTimerManager().SetTimer(CloseNiagaraWeapons, this, &ASoulPlayerCharacter::CloseSwordNiagara, 1.f, false);
    }
}


void ASoulPlayerCharacter::ChangeSwordWeaponType()
{
    if (WeaponType == EWeaponType::MELEE)
    {
        WeaponType = EWeaponType::SWORD;
        bIsWeapons = true;
    }
    else if (WeaponType == EWeaponType::SWORD)
    {
        WeaponType = EWeaponType::MELEE;
        ChangeSwordSlot(false);
        bIsWeapons = false;
    }
}


void ASoulPlayerCharacter::CloseSwordNiagara()
{
    SwordNiagara->Deactivate();
    Sword->SetMaterial(0, CurrentWeaponMaterial);
    SwordSheath->SetMaterial(0, CurrentSwordSheathMaterial);
    bIsChangingWeapons = false;
}


bool ASoulPlayerCharacter::CanWeapons()
{
    if (PlayerBehavior == EPlayerBehavior::IDLE && !bIsChangingWeapons)
    {
        return true;
    }
    return false;
}


#pragma endregion "Weapons"
#pragma region "Focus"
void ASoulPlayerCharacter::Focus()
{
    if (!bIsFocus)
    {
        FocusedTarget = GetNearestEnemy();
        if (FocusedTarget)
        {
            FocusedTarget->SetEnemyTipVisibility(true);
            SetStartFocus();
            USoulEventManager::Get()->SwitchEnemyHealth.ExecuteIfBound(FocusedTarget->EnemyName, FocusedTarget->CurrentHealth / FocusedTarget->MaxHealth);
        }
        bIsFocus = FocusedTarget != nullptr;
    }
    else
    {
        if (FocusedTarget)
        {
            FocusedTarget->SetEnemyTipVisibility(false);
            CameraBoom->bUsePawnControlRotation = true;
            //FollowCamera->bUsePawnControlRotation = true;
        }
        bIsFocus = false;
        FocusedTarget = nullptr;
    }
}

void ASoulPlayerCharacter::Focusing()
{
    if (bIsFocus && FocusedTarget)
    {
        FVector Direction = FocusedTarget->GetActorLocation() - GetActorLocation();
        FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
        SetActorRotation(LookAtRotation);
        Controller->SetControlRotation(GetActorRotation());
    }
}

void ASoulPlayerCharacter::SetStartFocus()
{
    //如果注视目标存在，则将摄像机朝向目标
    FVector Direction = FocusedTarget->GetActorLocation() - GetActorLocation();
    Direction.Z = 0; // 忽略Z轴抬头低头
    FRotator TargetRotation = Direction.Rotation();

    // 平滑角色转向
    FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, 0.001f, 5.0f);
    SetActorRotation(NewRotation);
    CameraBoom->bUsePawnControlRotation = false;
    //FollowCamera->bUsePawnControlRotation = false;
    // 平滑摄像机控制器转向
    FRotator NewControlRotation = FMath::RInterpTo(Controller->GetControlRotation(), TargetRotation, 0.001f, 5.0f);
    Controller->SetControlRotation(NewControlRotation);
}

#pragma endregion "Focus"
void ASoulPlayerCharacter::Injury(FVector HitLocation, float Health_Sub)
{
    switch (WeaponType)
    {
    case EWeaponType::MELEE:
        MeleeInjury(HitLocation,Health_Sub);
        break;
    case EWeaponType::SWORD:
        SwordInjury(HitLocation,Health_Sub);
        break;
    }
}
void ASoulPlayerCharacter::MeleeInjury(FVector HitLocation, float Health_Sub)
{
    if (!CanInjury()) return;

    if (PlayerStates == EPlayerStates::COMMON)
    {
        PlayInjuryAnimMontage(MeleeInjuryAnim, HitLocation);
    }
    else
    {
        PlayInjuryAnimMontage(MeleePreparwarInjuryAnim, HitLocation);
    }
    CameraShakeFeedBack(true);
    HealthChange(Health_Sub);
}

void ASoulPlayerCharacter::SwordInjury(FVector HitLocation, float Health_Sub)
{
    if (!CanInjury()) return;

    if (PlayerBehavior == EPlayerBehavior::DENFENSE)
    {
        int DefenseIndex = -1;
        PlayRandomAnimMontage(SwordDefenseInjuryAnim, DefenseIndex);
    }
    else
    {
        PlayInjuryAnimMontage(SwordInjuryAnim, HitLocation);
    }
    CameraShakeFeedBack(true);
    HealthChange(Health_Sub);
}

void ASoulPlayerCharacter::HealthChange(float Health_Sub)
{
    float InCurrentHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(CurrentHealth - Health_Sub, 0.f, MaxHealth);
    if (CurrentHealth <= 0.f)
    {
        Die();
    }
    SetHealth(InCurrentHealth, CurrentHealth, MaxHealth);
}

bool ASoulPlayerCharacter::CanInjury()
{
    return true;
}

#pragma region "Function"
void ASoulPlayerCharacter::UpdateStaminaRestore()
{
    if (CurrentStamina >= MaxStamina)
    {
        GetWorldTimerManager().ClearTimer(StaminaRestoreTimer);
        return;
    }

    if (CurrentStamina < MaxStamina && PlayerBehavior == EPlayerBehavior::IDLE)
    {
        NewStamina = FMath::Min(CurrentStamina + AddStamina * 0.05f, 100.f);
        //关闭资源不足显示
        if (NewStamina >= SubStamina && bIsIRShow)
        {
            CloseIR();
        }
        SetStamina(CurrentStamina, NewStamina, MaxStamina);
    }
}

bool ASoulPlayerCharacter::PlayRandomAnimMontage(FSoulActionType* CurrentAnimMontage, int32& LastAnimMontageIndex)
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

int32 ASoulPlayerCharacter::GetRollingAnimIndex(float ForwardValue, float RightValue)
{
    if (ForwardValue == 1) return 0;
    if (ForwardValue == -1) return 1;
    if (RightValue == -1) return 2;
    if (RightValue == 1) return 3;
    return 0;
}
bool ASoulPlayerCharacter::PlayRollingAnimMontage(FSoulActionType* CurrentAnimMontage)
{
    //获取当前动画实例
    UAnimInstance* CurAnimIns = GetMesh()->GetAnimInstance();
    if (!CurAnimIns) return false;

    if (!CurrentAnimMontage || CurrentAnimMontage->AnimMontage.Num() == 0) return false;

    //根据翻滚朝向值，获取翻滚动画索引并播放
    int32 Index = GetRollingAnimIndex(RollingForwordValue, RollingRightValue);
    if (Index > CurrentAnimMontage->AnimMontage.Num() - 1) Index = 0;
    CurAnimIns->Montage_Play(CurrentAnimMontage->AnimMontage[Index], RollingAnimSpeed);

    return true;
}

bool ASoulPlayerCharacter::PlayInjuryAnimMontage(FSoulActionType* CurrentAnimMontage, FVector HitLocation)
{
    //获取当前动画实例
    UAnimInstance* CurAnimIns = GetMesh()->GetAnimInstance();
    if (!CurAnimIns) return false;

    if (!CurrentAnimMontage || CurrentAnimMontage->AnimMontage.Num() == 0) return false;

    //根据受击位置，获取受击动画索引并播放
    int32 Index = GetHitLocationAnimIndex(HitLocation);
    if (Index >= CurrentAnimMontage->AnimMontage.Num()) return false;

    CurAnimIns->Montage_Play(CurrentAnimMontage->AnimMontage[Index], 0.8f);

    
    return true;
}


ASoulBaseEnemy* ASoulPlayerCharacter::GetNearestEnemy()
{
    ASoulBaseEnemy* NearestEnemy = nullptr;
    float MinDistanceSqr = TNumericLimits<float>::Max();

    FVector MyLocation = GetActorLocation();

    for (ASoulBaseEnemy* Enemy : EnemyArray)
    {
        if (!IsValid(Enemy)) continue;

        float DistSqr = FVector::DistSquared(Enemy->GetActorLocation(), MyLocation);
        if (DistSqr < MinDistanceSqr)
        {
            MinDistanceSqr = DistSqr;
            NearestEnemy = Enemy;
        }
    }

    return NearestEnemy;

}

int32 ASoulPlayerCharacter::GetHitLocationAnimIndex(FVector HitLocation)
{
    FVector Direction = HitLocation - GetActorLocation();
    Direction.Normalize();

    FVector Forward = GetActorForwardVector();
    float DotForward = FVector::DotProduct(Forward, Direction);
    float DotRight = FVector::DotProduct(GetActorRightVector(), Direction);

    if (DotForward > 0.707f)
        return 0;
    else if (DotForward < -0.707f)
        return 1;
    else if (DotRight > 0)
        return 3;
    else
        return 2;

}

void ASoulPlayerCharacter::Die()
{
    bIsDie = true;

    RemovePlayerInput();
    if (GetCapsuleComponent())
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (GetMesh())
    {
        GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
        GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        GetMesh()->SetCollisionResponseToAllChannels(ECR_Block);
        GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

        GetMesh()->SetAllBodiesBelowSimulatePhysics("pelvis", true);
    }
    USoulEventManager::Get()->OpenFightResult.ExecuteIfBound();
}

void ASoulPlayerCharacter::RemovePlayerInput()
{
    if (ASoulPlayerController* PC = Cast<ASoulPlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->RemoveMappingContext(PlayerMappingContext);
        }
    }
}

void ASoulPlayerCharacter::fhnaof()
{
    Super::fhnaof();

    Injury(GetActorLocation() + FVector(0, 100, 0),10);
    
}

void ASoulPlayerCharacter::danhfafa()
{
    USoulEventManager::Get()->OpenSetMenu.ExecuteIfBound(true);
   
}

#pragma endregion "Function"
