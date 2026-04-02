// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameCharacter/SoulBaseCharacter.h"
#include "SoulPlayerController.h"
#include "SoulGameHUD/SoulGameHUD.h"
#include "SoulGameHUD/UI_FightMainUI.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "SoulGameTags/SoulGameTagsManager.h"
#include "SoulGameTags/SoulStateMachineComponent.h"
#include "SoulGameEvent/SoulEventManager.h"
#include "SoulGameItem/PickupItem.h"
#include "SoulGameAI/SoulBaseEnemy.h"
#include "SoulGameInstance.h"
#include "SoulGameDebug/DebugTools.h"
#include "SoulGameGAS/SoulAbilitySystemComponent.h"
#include "SoulGameGAS/SoulAttributeSet.h"
#include "SoulGameGAS/SoulGameplayAbility.h"
#include "SoulGameCharacter/SoulPickupComponent.h"
#include "SoulGameCharacter/SoulPerceptionComponent.h"


// Sets default values
ASoulBaseCharacter::ASoulBaseCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    //不使用控制器旋转
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;

    GetCharacterMovement()->bOrientRotationToMovement = true; //角色朝向移动方向

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = CameraDefaultArmLength;                     //弹簧臂长度
    CameraBoom->bUsePawnControlRotation = true;               //弹簧臂使用pawn控制旋转
    CameraBoom->bEnableCameraLag = true;                      //启用摄像机延迟
    CameraBoom->bEnableCameraRotationLag = true;              //启用摄像机旋转延迟
    CameraBoom->CameraLagSpeed = 8.f;                         //摄像机延迟速度
    CameraBoom->CameraRotationLagSpeed = 8.f;                 //摄像机旋转延迟速度

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom);

    // ============ 敌人感知组件 ============
    PerceptionComponent = CreateDefaultSubobject<USoulPerceptionComponent>(TEXT("PerceptionComponent"));
    // 创建感知球并赋值给组件
    USphereComponent* PerceptionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PerceptionSphere"));
    PerceptionSphere->SetupAttachment(RootComponent);
    PerceptionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PerceptionSphere->SetCollisionObjectType(ECC_WorldDynamic);
    PerceptionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    PerceptionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    PerceptionComponent->PerceptionSphere = PerceptionSphere;

    // ============ GAS 初始化 ============
    AbilitySystemComponent = CreateDefaultSubobject<USoulAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    // ============ 拾取交互组件 ============
    PickupComponent = CreateDefaultSubobject<USoulPickupComponent>(TEXT("PickupComponent"));


    //玩家状态
    WeaponType = EWeaponType::MELEE;
    PlayerStates = EPlayerStates::COMMON;
    PlayerBehavior = EPlayerBehavior::IDLE;

    //玩家属性
    CurrentHealth = 100.f;
    MaxHealth = 100.f;

    CurrentMana = 100.f;
    MaxMana = 100.f;

    MaxStamina = 100.f;
    CurrentStamina = 100.f;

    // 体力恢复速率和消耗阈值
    StaminaRestoreRate = 10.f;
    LastStaminaCostThreshold = 0.f;

    DesiredRotation = FRotator(0, 0, 0);
    RollingForwordValue = 0;
    RollingRightValue = 0;
    RollingAnimSpeed = 1.f;

    MeleeCollisionRadius = 50.f;
}

// Called when the game starts or when spawned
void ASoulBaseCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->SetInputMode(FInputModeGameOnly());
    }
    // 拾取组件会在自己的 BeginPlay 中自动订阅 NearbyInteractables 委托
    // 感知组件会在自己的 BeginPlay 中自动绑定碰撞球事件
}

// Called every frame
void ASoulBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // 计算角色的旋转
    CalculatedRotation();
}

#pragma region "Input"



// Called to bind functionality to input
void ASoulBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        BIND_ACTION_SAFE(EnhancedInputComponent, MoveAction, ETriggerEvent::Triggered, this, &ASoulBaseCharacter::Move);
        BIND_ACTION_SAFE(EnhancedInputComponent, LookAction, ETriggerEvent::Triggered, this, &ASoulBaseCharacter::Look);
        BIND_ACTION_SAFE(EnhancedInputComponent, WalkAction, ETriggerEvent::Started, this, &ASoulBaseCharacter::Walk);
        BIND_ACTION_SAFE(EnhancedInputComponent, WalkAction, ETriggerEvent::Completed, this, &ASoulBaseCharacter::Walk);
        BIND_ACTION_SAFE(EnhancedInputComponent, RunAction, ETriggerEvent::Started, this, &ASoulBaseCharacter::Run);
        BIND_ACTION_SAFE(EnhancedInputComponent, RunAction, ETriggerEvent::Completed, this, &ASoulBaseCharacter::Run);
        BIND_ACTION_SAFE(EnhancedInputComponent, VisibilityAction, ETriggerEvent::Triggered, this, &ASoulBaseCharacter::Visibility);
        BIND_ACTION_SAFE(EnhancedInputComponent, ShowMouseAction, ETriggerEvent::Started, this, &ASoulBaseCharacter::ShowMouse);
        BIND_ACTION_SAFE(EnhancedInputComponent, ShowMouseAction, ETriggerEvent::Completed, this, &ASoulBaseCharacter::ShowMouse);
        BIND_ACTION_SAFE(EnhancedInputComponent, InteractionAction, ETriggerEvent::Started, this, &ASoulBaseCharacter::Interaction);
        BIND_ACTION_SAFE(EnhancedInputComponent, ExitAction, ETriggerEvent::Started, this, &ASoulBaseCharacter::Exit);
        BIND_ACTION_SAFE(EnhancedInputComponent, ExitAction, ETriggerEvent::Completed, this, &ASoulBaseCharacter::Exit);
    }
}
//移动
void ASoulBaseCharacter::Move(const FInputActionValue& Value)
{
    // 记录当前输入值
    InputActionValueMap.Add(MoveAction, Value);

    FVector2D MovementInput = Value.Get<FVector2D>();
    RollingForwordValue = MovementInput.Y;
    RollingRightValue = MovementInput.X;

    if (ASoulPlayerController* PC = Cast<ASoulPlayerController>(Controller))
    {
        const FRotator Rotation = Controller->GetControlRotation();

        const FVector ForwardDirection = FRotationMatrix(FRotator(0, Rotation.Yaw, 0)).GetUnitAxis(EAxis::X);
        const FVector RightDirection = FRotationMatrix(FRotator(0, Rotation.Yaw, 0)).GetUnitAxis(EAxis::Y);

        AddMovementInput(ForwardDirection, MovementInput.Y);
        AddMovementInput(RightDirection, MovementInput.X);
    }
}
//视角
void ASoulBaseCharacter::Look(const FInputActionValue& Value)
{
    InputActionValueMap.Add(LookAction, Value);

    FVector2D LookVector = Value.Get<FVector2D>();
    // 普通自由视角下才执行相机旋转
    if (Controller && !bIsFocus)
    {
        AddControllerYawInput(LookVector.X);
        AddControllerPitchInput(LookVector.Y);
    }
}
//静步
void ASoulBaseCharacter::Walk(const FInputActionValue& Value)
{
    InputActionValueMap.Add(WalkAction, Value);

    UCharacterMovementComponent* MovementComp = GetCharacterMovement();
    if (MovementComp)
    {
        // Value.Get<bool>() 返回 true 表示 "Started"，false 表示 "Completed"
        bool bIsAccelerating = Value.Get<bool>();

        MovementComp->MaxWalkSpeed = bIsAccelerating ? SlowWalkSpeed : DefaultWalkSpeed;
    }
}
//奔跑
void ASoulBaseCharacter::Run(const FInputActionValue& Value)
{
    InputActionValueMap.Add(RunAction, Value);

    UCharacterMovementComponent* MovementComp = GetCharacterMovement();
    if (MovementComp && CanRun())
    {
        // Value.Get<bool>() 返回 true 表示 "Started"，false 表示 "Completed"
        bIsRunActive = Value.Get<bool>();

        MovementComp->MaxWalkSpeed = bIsRunActive ? SprintSpeed : DefaultWalkSpeed;
    }
}
//摄像机远近
void ASoulBaseCharacter::Visibility(const FInputActionValue& Value)
{
    InputActionValueMap.Add(VisibilityAction, Value);

    float ArmLength = CameraBoom->TargetArmLength;

    if (Value.Get<float>() > 0)
    {
        if (CameraBoom->TargetArmLength >= CameraMinArmLength)
        {
            CameraBoom->TargetArmLength = ArmLength - CameraZoomStep;
        }
    }
    else
    {
        if (CameraBoom->TargetArmLength <= CameraMaxArmLength)
        {
            CameraBoom->TargetArmLength = ArmLength + CameraZoomStep;
        }
    }
}
//显示鼠标
void ASoulBaseCharacter::ShowMouse(const FInputActionValue& Value)
{
    InputActionValueMap.Add(ShowMouseAction, Value);

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        // Value.Get<bool>() 返回 true 表示 "Started"，false 表示 "Completed"
        bIsShowMouseActive = Value.Get<bool>();
        PlayerController->bShowMouseCursor = bIsShowMouseActive;
    }
}
void ASoulBaseCharacter::Interaction(const FInputActionValue& Value)
{
    // 委托给拾取组件处理
    if (PickupComponent)
    {
        PickupComponent->PickupAllItems();
    }
}

void ASoulBaseCharacter::EnableMeleeCollision()
{
    FVector SpherePos = GetActorLocation() + (GetActorForwardVector() * 70.f);
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery3);
    TArray<AActor*> IgnoreActors;
    IgnoreActors.Add(this);
    TArray<AActor*> OverlappedActors;
    UKismetSystemLibrary::SphereOverlapActors(GetWorld(),SpherePos,MeleeCollisionRadius,ObjectTypes,nullptr,IgnoreActors,OverlappedActors);

    for (auto AttackEnemy : OverlappedActors)
    {
        ASoulBaseEnemy* Enemy = Cast<ASoulBaseEnemy>(AttackEnemy);
        if (Enemy)
        {
            Enemy->Injure(MeleeCollisionDamage);
        }
    }

}
//获取增强输入的值（float）
float ASoulBaseCharacter::GetEnhancedInputFloat(UInputAction* InputAction) const
{
    if (const FInputActionValue* Found = InputActionValueMap.Find(InputAction))
    {
        // 把 ActionValue 里的 float 提取出来
        return Found->Get<float>();
    }
    return 0.f;
}
//获取增强输入的值（Vector2D）
FVector2D ASoulBaseCharacter::GetEnhancedInputVector2D(UInputAction* InputAction) const
{
    if (const FInputActionValue* Found = InputActionValueMap.Find(InputAction))
    {
        // 把 ActionValue 里的 Vector2D 提取出来
        return Found->Get<FVector2D>();
    }
    return FVector2D::Zero();
}
//是否能奔跑
bool ASoulBaseCharacter::CanRun()
{
    if (PlayerBehavior == EPlayerBehavior::IDLE)
    {
        return true;
    }
    return false;
}
#pragma endregion "Input"
#pragma region "Function"
//获取战斗UI
UUI_FightMainUI* ASoulBaseCharacter::GetFightMainUI()
{
    if (const ASoulPlayerController* PC = Cast<ASoulPlayerController>(Controller))
    {
        if (const ASoulGameHUD* GameHUD = Cast<ASoulGameHUD>(PC->GetHUD()))
        {
            return GameHUD->GetUI_FightMainUI();
        }
    }
    return nullptr;
}


void ASoulBaseCharacter::SetHealth(float InNewHealth)
{
    CurrentHealth = FMath::Clamp(InNewHealth, 0.f, MaxHealth);
    // 同步到 GAS（GAS 属性变化委托会自动通知 UI）
    SyncAttributeToGAS(USoulAttributeSet::GetHealthAttribute(), CurrentHealth);
}

void ASoulBaseCharacter::SetMana(float InNewMana)
{
    CurrentMana = FMath::Clamp(InNewMana, 0.f, MaxMana);
    // 同步到 GAS（GAS 属性变化委托会自动通知 UI）
    SyncAttributeToGAS(USoulAttributeSet::GetManaAttribute(), CurrentMana);
}

void ASoulBaseCharacter::SetStamina(float InNewStamina)
{
    CurrentStamina = FMath::Clamp(InNewStamina, 0.f, MaxStamina);
    // 同步到 GAS（GAS 属性变化委托会自动通知 UI）
    SyncAttributeToGAS(USoulAttributeSet::GetStaminaAttribute(), CurrentStamina);
}
//显示资源不足的通知(IRText为通知文本，IRColor为通知颜色)
void ASoulBaseCharacter::ShowIR(FText IRText, FSlateColor IRColor)
{
    if (UUI_FightMainUI* FightMainUI = GetFightMainUI())
    {
        FightMainUI->SetIRColor(IRColor);
        FightMainUI->PlayShowIRAnimation(IRText);
        bIsIRShow = true;
    }
}
//关闭资源不足的通知
void ASoulBaseCharacter::CloseIR()
{
    if (UUI_FightMainUI* FightMainUI = GetFightMainUI())
    {
        FightMainUI->CloseShowIR();
        bIsIRShow = false;
    }
}


//攻击时角色的朝向
void ASoulBaseCharacter::CalculatedRotation()
{
    if (bIsShouldRotate)
    {
        FVector LastVector = GetCharacterMovement()->GetLastInputVector();
        if (LastVector != FVector(0, 0, 0))
        {
            DesiredRotation = UKismetMathLibrary::MakeRotFromX(LastVector);
            FRotator RInterpRot = UKismetMathLibrary::RInterpTo(GetActorRotation(), DesiredRotation, GetWorld()->GetDeltaSeconds(), 0.f);
            SetActorRotation(FRotator(0, RInterpRot.Yaw, 0));
        }
    }
}

#pragma endregion "Function"

#pragma region "GameplayTags"

FGameplayTagContainer ASoulBaseCharacter::GetGameplayTagContainer_Implementation() const
{
	return GameplayTagContainer;
}

void ASoulBaseCharacter::AddGameplayTag_Implementation(FGameplayTag Tag)
{
	if (!Tag.IsValid()) return;

	if (!GameplayTagContainer.HasTag(Tag))
	{
		GameplayTagContainer.AddTag(Tag);
		UE_LOG(LogTemp, Verbose, TEXT("ASoulBaseCharacter::AddGameplayTag - Added Tag: %s"), *Tag.ToString());
	}
}

void ASoulBaseCharacter::RemoveGameplayTag_Implementation(FGameplayTag Tag)
{
	if (!Tag.IsValid()) return;

	if (GameplayTagContainer.HasTag(Tag))
	{
		GameplayTagContainer.RemoveTag(Tag);
		UE_LOG(LogTemp, Verbose, TEXT("ASoulBaseCharacter::RemoveGameplayTag - Removed Tag: %s"), *Tag.ToString());
	}
}

bool ASoulBaseCharacter::HasGameplayTag_Implementation(FGameplayTag Tag) const
{
	return GameplayTagContainer.HasTag(Tag);
}

bool ASoulBaseCharacter::HasMatchingTag_Implementation(FGameplayTag Tag) const
{
	for (const FGameplayTag& ExistingTag : GameplayTagContainer)
	{
		if (ExistingTag.MatchesTag(Tag) || Tag.MatchesTag(ExistingTag))
		{
			return true;
		}
	}
	return false;
}

TArray<FName> ASoulBaseCharacter::GetGameplayTagNames_Implementation() const
{
	TArray<FName> TagNames;
	for (const FGameplayTag& Tag : GameplayTagContainer)
	{
		TagNames.Add(Tag.GetTagName());
	}
	return TagNames;
}

FText ASoulBaseCharacter::GetTagDisplayName_Implementation(FGameplayTag Tag) const
{
	if (const USoulGameTagsManager* TagsManager = USoulGameTagsManager::Get())
	{
		return TagsManager->GetTagDisplayName(Tag);
	}
	return FText::FromName(Tag.GetTagName());
}

void ASoulBaseCharacter::InitializeStateMachine()
{
	// 创建或获取状态机组件
	if (!StateMachineComponent)
	{
		StateMachineComponent = FindComponentByClass<USoulStateMachineComponent>();
		if (!StateMachineComponent)
		{
			StateMachineComponent = NewObject<USoulStateMachineComponent>(this, USoulStateMachineComponent::StaticClass());
			StateMachineComponent->RegisterComponent();
		}
	}

	if (StateMachineComponent)
	{
		// 设置允许的状态
		USoulGameTagsManager* TagsManager = USoulGameTagsManager::Get();

		// 行为状态
		GameplayTagContainer.AddTag(TagsManager->GetBehaviorTag(TEXT("Idle")));
		FGameplayTagContainer BehaviorStates;
		BehaviorStates.AddTag(TagsManager->GetBehaviorTag(TEXT("Idle")));
		BehaviorStates.AddTag(TagsManager->GetBehaviorTag(TEXT("Attack")));
		BehaviorStates.AddTag(TagsManager->GetBehaviorTag(TEXT("Rolling")));
		BehaviorStates.AddTag(TagsManager->GetBehaviorTag(TEXT("Defense")));
		BehaviorStates.AddTag(TagsManager->GetBehaviorTag(TEXT("Injury")));
		BehaviorStates.AddTag(TagsManager->GetBehaviorTag(TEXT("Dead")));
		StateMachineComponent->SetInitialState(TagsManager->GetBehaviorTag(TEXT("Idle")));
		StateMachineComponent->SetAllowedStates(BehaviorStates);

		// 武器状态
		GameplayTagContainer.AddTag(TagsManager->GetWeaponTag(TEXT("Melee")));

		StateMachineComponent->InitializeComponent();
	}
}

#pragma endregion "GameplayTags"

#pragma region "GAS"

UAbilitySystemComponent* ASoulBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

USoulAbilitySystemComponent* ASoulBaseCharacter::GetSoulAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

const USoulAttributeSet* ASoulBaseCharacter::GetSoulAttributeSet() const
{
	return AttributeSet;
}

void ASoulBaseCharacter::InitializeGAS()
{
	if (bGASInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASoulBaseCharacter::InitializeGAS - GAS 已初始化"));
		return;
	}

	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ASoulBaseCharacter::InitializeGAS - AbilitySystemComponent 为空"));
		return;
	}

	// 初始化 ASC
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	// 获取属性集（ASC 会自动创建 AttributeSet 子对象）
	AttributeSet = AbilitySystemComponent->GetSet<USoulAttributeSet>();
	if (!AttributeSet)
	{
		// 手动创建属性集
		USoulAttributeSet* NewAttributeSet = NewObject<USoulAttributeSet>(this);
		AbilitySystemComponent->AddAttributeSetSubobject(NewAttributeSet);
		AttributeSet = NewAttributeSet;
	}

	// 同步旧属性到 GAS（兼容过渡期）
	AbilitySystemComponent->SetNumericAttributeBase(USoulAttributeSet::GetHealthAttribute(), CurrentHealth);
	AbilitySystemComponent->SetNumericAttributeBase(USoulAttributeSet::GetMaxHealthAttribute(), MaxHealth);
	AbilitySystemComponent->SetNumericAttributeBase(USoulAttributeSet::GetManaAttribute(), CurrentMana);
	AbilitySystemComponent->SetNumericAttributeBase(USoulAttributeSet::GetMaxManaAttribute(), MaxMana);
	AbilitySystemComponent->SetNumericAttributeBase(USoulAttributeSet::GetStaminaAttribute(), CurrentStamina);
	AbilitySystemComponent->SetNumericAttributeBase(USoulAttributeSet::GetMaxStaminaAttribute(), MaxStamina);

	// 授予默认技能
	if (DefaultAbilities.Num() > 0)
	{
		AbilitySystemComponent->InitializeDefaultAbilities(DefaultAbilities);
	}

	bGASInitialized = true;
	UE_LOG(LogTemp, Log, TEXT("ASoulBaseCharacter::InitializeGAS - GAS 初始化完成，属性已同步"));
}

float ASoulBaseCharacter::GetHealth() const
{
	// 优先从 GAS 读取
	if (AttributeSet)
	{
		return AttributeSet->GetHealth();
	}
	return CurrentHealth;
}

float ASoulBaseCharacter::GetCurrentStamina() const
{
	// 优先从 GAS 读取
	if (AttributeSet)
	{
		return AttributeSet->GetStamina();
	}
	return CurrentStamina;
}

float ASoulBaseCharacter::GetCurrentMana() const
{
	// 优先从 GAS 读取
	if (AttributeSet)
	{
		return AttributeSet->GetMana();
	}
	return CurrentMana;
}

void ASoulBaseCharacter::SyncAttributeToGAS(const FGameplayAttribute& Attribute, float NewValue)
{
	if (AbilitySystemComponent && bGASInitialized)
	{
		AbilitySystemComponent->SetNumericAttributeBase(Attribute, NewValue);
	}
}

#pragma endregion "GAS"