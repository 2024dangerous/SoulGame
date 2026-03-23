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


// Sets default values
ASoulBaseCharacter::ASoulBaseCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    //使用控制器旋转
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;

    GetCharacterMovement()->bOrientRotationToMovement = true; //角色朝向移动方向

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 350.0f;                     //弹簧臂长度
    CameraBoom->bUsePawnControlRotation = true;               //弹簧臂使用pawn控制旋转
    CameraBoom->bEnableCameraLag = true;                      //启用摄像机延迟
    CameraBoom->bEnableCameraRotationLag = true;              //启用摄像机旋转延迟
    CameraBoom->CameraLagSpeed = 8.f;                         //摄像机延迟速度
    CameraBoom->CameraRotationLagSpeed = 8.f;                 //摄像机旋转延迟速度

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom);

    PerceptionEnemy = CreateDefaultSubobject<USphereComponent>(TEXT("PerceptionEnemy"));
    PerceptionEnemy->SetupAttachment(RootComponent);
    PerceptionEnemy->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PerceptionEnemy->SetCollisionObjectType(ECC_WorldDynamic);
    PerceptionEnemy->SetCollisionResponseToAllChannels(ECR_Ignore);
    PerceptionEnemy->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);


    //玩家状态
    WeaponType = EWeaponType::MELEE;
    PlayerStates = EPlayerStates::COMMON;
    PlayerBehavior = EPlayerBehavior::IDLE;

    //玩家属性
    CurrentHealth = 100.f;
    MaxHealth = 100.f;
    NewHealth = 100.f;
    SubHealth = 10.f;
    AddHealth = 5.f;

    CurrentMana = 100.f;
    MaxMana = 100.f;
    NewMana = 100.f;
    SubMana = 10.f;
    AddMana = 5.f;

    MaxStamina = 100.f;
    CurrentStamina = 100.f;
    NewStamina = 100.f;
    SubStamina = 10.f;
    AddStamina = 5.f;

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
    USoulEventManager::Get()->NearbyInteractables.BindUObject(this, &ASoulBaseCharacter::OnNearbyInteractablesChanged);

    PerceptionEnemy->OnComponentBeginOverlap.AddUniqueDynamic(this, &ASoulBaseCharacter::AddPerceptionEnemy);
    PerceptionEnemy->OnComponentEndOverlap.AddUniqueDynamic(this, &ASoulBaseCharacter::SubPerceptionEnemy);
}

// Called every frame
void ASoulBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // 计算角色的旋转
    CalculatedRotation();
}

void ASoulBaseCharacter::AddPerceptionEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ASoulBaseEnemy* SoulEnemy = Cast<ASoulBaseEnemy>(OtherActor))
    {
        // 检查敌人是否已经在数组中
        if (!EnemyArray.Contains(SoulEnemy))
        {
            EnemyArray.Add(SoulEnemy);
            USoulEventManager::Get()->OpenEnemyHealth.ExecuteIfBound(SoulEnemy->EnemyName,true);
            USoulEventManager::Get()->SwitchEnemyHealth.ExecuteIfBound(SoulEnemy->EnemyName, SoulEnemy->CurrentHealth / SoulEnemy->MaxHealth);
            ZhouXiaoPeng_PRINT(FString::Printf(TEXT("添加敌人到敌人数组: %d"), EnemyArray.Num()));
        }
    }
}

void ASoulBaseCharacter::SubPerceptionEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (ASoulBaseEnemy* SoulEnemy = Cast<ASoulBaseEnemy>(OtherActor))
    {
        // 检查敌人是否已经在数组中
        if (EnemyArray.Contains(SoulEnemy))
        {
            EnemyArray.Remove(SoulEnemy);
            if (EnemyArray.Num() == 0)
            {
                USoulEventManager::Get()->OpenEnemyHealth.ExecuteIfBound(SoulEnemy->EnemyName, false);
                Focus();
            }
            ZhouXiaoPeng_PRINT(FString::Printf(TEXT("删除敌人到敌人数组: %d"), EnemyArray.Num()));
        }
    }
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

        MovementComp->MaxWalkSpeed = bIsAccelerating ? 200 : 500;
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

        MovementComp->MaxWalkSpeed = bIsRunActive ? 900 : 500;
    }
}
//摄像机远近
void ASoulBaseCharacter::Visibility(const FInputActionValue& Value)
{
    InputActionValueMap.Add(VisibilityAction, Value);

    float ArmLength = CameraBoom->TargetArmLength;

    if (Value.Get<float>() > 0)
    {
        if (CameraBoom->TargetArmLength >= 200)
        {
            CameraBoom->TargetArmLength = ArmLength - 10;
        }
    }
    else
    {
        if (CameraBoom->TargetArmLength <= 600)
        {
            CameraBoom->TargetArmLength = ArmLength + 10;
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
    if (PickupItemArray.Num() == 0) return;

    TArray<APickupItem*> ItemsCopy = PickupItemArray;
    for (APickupItem* Item : ItemsCopy)
    {
        if (IsValid(Item))  // 确保不是空指针
        {
            Item->Destroy();
            PickupItemArray.Remove(Item);
        }
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
            Enemy->Injure(10);
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
void ASoulBaseCharacter::SetPickupItemArray(APickupItem* PickupItem, bool bIsAddOrSub)
{
    if (!PickupItem) return;

    if (bIsAddOrSub)
    {
        if (!PickupItemArray.Contains(PickupItem))
        {
            PickupItemArray.Add(PickupItem);
        }
    }
    else
    {
        PickupItemArray.Remove(PickupItem);
    }
    SetInteractionUIVisibility();
}
void ASoulBaseCharacter::OnNearbyInteractablesChanged(AActor* PickupItem, bool bIsAddOrSub)
{
    if (APickupItem* TypedItem = Cast<APickupItem>(PickupItem))
    {
        SetPickupItemArray(TypedItem, bIsAddOrSub);
    }
}
void ASoulBaseCharacter::SetInteractionUIVisibility()
{
    if (!PickupItemArray.IsEmpty())
    {
        USoulEventManager::Get()->OpenInteractionUI.ExecuteIfBound(true, FText::FromString(TEXT("拾取")));
    }
    else
    {
        USoulEventManager::Get()->OpenInteractionUI.ExecuteIfBound(false, FText::FromString(TEXT("")));
    }
}
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


void ASoulBaseCharacter::SetHealth(float InCurrentHealth, float InNewHealth,float InMaxHealth)
{
    USoulEventManager::Get()->OnStatusBoxChanged.Broadcast(EStatusBox::Health, InCurrentHealth, InNewHealth, InMaxHealth);
    CurrentStamina = InNewHealth;
}

void ASoulBaseCharacter::SetMana(float InCurrentMana, float InNewMana,float InMaxMana)
{
    USoulEventManager::Get()->OnStatusBoxChanged.Broadcast(EStatusBox::Mana, InCurrentMana, InNewMana, InMaxMana);
    CurrentStamina = InNewMana;
}

//体力值变化时广播
void ASoulBaseCharacter::SetStamina(float InCurrentStamina, float InNewStamina,float InMaxStamina)
{
    USoulEventManager::Get()->OnStatusBoxChanged.Broadcast(EStatusBox::Stamina,InCurrentStamina, InNewStamina, InMaxStamina);
    CurrentStamina = InNewStamina;
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
void ASoulBaseCharacter::fhnaof()
{
    ////PickupItemPool
    //USoulGameInstance* GameInstance = Cast<USoulGameInstance>(GetGameInstance());


    //if (GameInstance)
    //{
    //    UObject* Obj = GameInstance->PoolManager->Acquire("PickupItemPool", GetWorld());
    //    if (Obj)
    //    {
    //        GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, TEXT("fwfwfwf"));

    //    }
    //    else {
    //        GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, TEXT("fwfwffwafawfaff"));
    //    }
    //}
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
