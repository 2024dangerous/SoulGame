// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "SoulGameData/SoulEnumType.h"
#include "Delegates/DelegateCombinations.h"
#include "SoulGameTags/SoulGameplayTagInterface.h"
#include "SoulGameTags/SoulStateMachineComponent.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "SoulBaseCharacter.generated.h"

//绑定输入的宏定义（判断是否有Input）
#define BIND_ACTION_SAFE(Component, Action, EventType, Object, Func) \
    if (Action) { Component->BindAction(Action, EventType, Object, Func); }


class UInputAction;
class UUI_FightMainUI;
class APickupItem;
class ASoulBaseEnemy;
class USoulAbilitySystemComponent;
class USoulAttributeSet;
class USoulGameplayAbility;
class USoulPickupComponent;
class USoulPerceptionComponent;

UCLASS()
class SOULGAME_API ASoulBaseCharacter : public ACharacter, public ISoulGameplayTagInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

#pragma region "Base"
public:
	// Sets default values for this character's properties
	ASoulBaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
    //第三人称摄像机组件
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = Camera , meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;
	


	//弹簧臂组件
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = Camera , meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    //摄像机最小臂长
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	float CameraMinArmLength = 200.f;

	//摄像机最大臂长
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	float CameraMaxArmLength = 600.f;

	//摄像机默认臂长
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	float CameraDefaultArmLength = 350.f;

	//摄像机缩放步长
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	float CameraZoomStep = 10.f;

    //圆形碰撞体组件（用于感知敌人）——已迁移到 USoulPerceptionComponent
	// 敌人感知组件
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Perception")
	USoulPerceptionComponent* PerceptionComponent;

	// 获取感知组件
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Perception")
	USoulPerceptionComponent* GetPerceptionComponent() const { return PerceptionComponent; }
#pragma endregion "Base"
#pragma region "Input"
    //输入系统
public:
    //增强输入绑定系统
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    //输入映射上下文
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | BaseInput")
    class UInputMappingContext* PlayerMappingContext;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | BaseInput")
    UInputAction* MoveAction;    //移动——WASD
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | BaseInput")
    UInputAction* LookAction;    //视角——鼠标移动
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | BaseInput")
    UInputAction* AttackAction;  //攻击——鼠标左键
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category ="Input | BaseInput" )
	UInputAction* RollingAction; //翻滚——空格键
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | BaseInput")
    UInputAction* WalkAction;    //静步——左Ctrl
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | BaseInput")
    UInputAction* RunAction;     //冲刺——左shift
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category ="Input | BaseInput" )
	UInputAction* VisibilityAction;//摄像头远近——鼠标滚轮
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category ="Input | BaseInput" )
	UInputAction* ShowMouseAction; //显示鼠标——Alt键
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category ="Input | BaseInput")
    UInputAction* WeaponsAction;   //切换武器——Tab键
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category ="Input | BaseInput")
    UInputAction* DefenseAction;   //防御——鼠标右键
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category ="Input | BaseInput")
    UInputAction* InteractionAction;//交互——F键
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category ="Input | BaseInput")
    UInputAction* FocusAction;     //注视——Q键
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category ="Input | BaseInput")
    UInputAction* ExitAction;     //退出——Esc键
    
    void Move(const FInputActionValue& Value);      //移动
    virtual void Look(const FInputActionValue& Value);      //视角
	virtual void Attack() {};                       //攻击
    virtual void Rolling() {};                      //翻滚
    virtual void Weapons() {};                      //切换武器
    virtual void Focus() {};                        //注视
	virtual void Exit(const FInputActionValue& Value) {};        //设置菜单
	virtual void Defense(const FInputActionValue& Value) {};  //防御
	void Walk(const FInputActionValue& Value);      //静步
	void Run(const FInputActionValue& Value);       //奔跑
	void Visibility(const FInputActionValue& Value);//摄像头远近
	void ShowMouse(const FInputActionValue& Value); //显示鼠标
	void Interaction(const FInputActionValue& Value);//交互

	//拳法攻击检测
	UFUNCTION(BlueprintCallable)
	void EnableMeleeCollision();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Melee")
	float MeleeCollisionRadius;

	//拳法攻击伤害值（可配置）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
	float MeleeCollisionDamage = 10.f;

	//默认行走速度
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float DefaultWalkSpeed = 500.f;

	//静步速度
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float SlowWalkSpeed = 200.f;

	//冲刺速度
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 900.f;

	//获取增强输入的值（float）
	UFUNCTION(BlueprintCallable)
	float GetEnhancedInputFloat(UInputAction* InputAction) const;
	//获取增强输入的值（Vector2D）
	UFUNCTION(BlueprintCallable)
	FVector2D GetEnhancedInputVector2D(UInputAction* InputAction) const;
	//是否能奔跑
	bool CanRun();
	bool bIsFocus = false;
	
private:

	bool bIsShowMouseActive = false;  //是否在显示鼠标状态
    bool bIsRunActive = false;        //是否在奔跑状态

	// 记录每个 InputAction 对应的输入值，比如轴值（float/Vector）、按键状态等
	TMap<const UInputAction*, FInputActionValue> InputActionValueMap;

#pragma endregion "Input"
#pragma region "GameplayTags"
public:
	// ============ GameplayTags 接口实现 ============
	// 角色拥有的 GameplayTags
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|GameplayTags")
	FGameplayTagContainer GameplayTagContainer;

	// 状态机组件
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|GameplayTags")
	class USoulStateMachineComponent* StateMachineComponent;

	// 获取标签容器
	virtual FGameplayTagContainer GetGameplayTagContainer_Implementation() const override;

	// 添加标签
	virtual void AddGameplayTag_Implementation(const FGameplayTag Tag) override;

	// 移除标签
	virtual void RemoveGameplayTag_Implementation(const FGameplayTag Tag) override;

	// 检查标签
	virtual bool HasGameplayTag_Implementation(const FGameplayTag Tag) const override;

	// 模糊匹配标签
	virtual bool HasMatchingTag_Implementation(const FGameplayTag Tag) const override;

	// 获取所有标签名
	virtual TArray<FName> GetGameplayTagNames_Implementation() const override;

	// 获取标签显示名
	virtual FText GetTagDisplayName_Implementation(const FGameplayTag Tag) const override;

	// 初始化状态机
	UFUNCTION(BlueprintCallable, Category = "Player|GameplayTags")
	void InitializeStateMachine();

#pragma endregion "GameplayTags"
#pragma region "GAS"
	// ============ GAS (GameplayAbilitySystem) ============
public:
	// IAbilitySystemInterface 接口实现
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// 获取自定义 ASC
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GAS")
	USoulAbilitySystemComponent* GetSoulAbilitySystemComponent() const;

	// 获取属性集
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GAS")
	const USoulAttributeSet* GetSoulAttributeSet() const;

	// 初始化 GAS（在 BeginPlay 或 PossessedBy 中调用）
	UFUNCTION(BlueprintCallable, Category = "SoulGame|GAS")
	void InitializeGAS();

	// 将旧属性值同步到 GAS AttributeSet
	void SyncAttributeToGAS(const FGameplayAttribute& Attribute, float NewValue);

protected:
	// 技能系统组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SoulGame|GAS")
	USoulAbilitySystemComponent* AbilitySystemComponent;

	// 属性集
	UPROPERTY()
	const USoulAttributeSet* AttributeSet;

	// 默认授予的技能列表（在编辑器中配置）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SoulGame|GAS")
	TArray<TSubclassOf<USoulGameplayAbility>> DefaultAbilities;

	// GAS 是否已初始化
	bool bGASInitialized = false;

#pragma endregion "GAS"
#pragma region "Attribute"
//玩家属性
public:
#pragma region "Health"
   //生命值（作为 GAS 的本地缓存，优先通过 GAS 读写）
   UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Player|Attribute|Health")
   float MaxHealth;

   UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Player|Attribute|Health")
   float CurrentHealth;
#pragma endregion "Health"

#pragma region "Mana"
   //法力值（作为 GAS 的本地缓存，优先通过 GAS 读写）
   UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Player|Attribute|Mana")
   float MaxMana;

   UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Player|Attribute|Mana")
   float CurrentMana;
#pragma endregion "Mana"
#pragma region "Stamina"
   //最大体力值（作为 GAS 的本地缓存，优先通过 GAS 读写）
   UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Player|Attribute|Stamina") 
   float MaxStamina;

   //当前体力值
   UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Player|Attribute|Stamina")
   float CurrentStamina;

   // 体力恢复速率（每 Tick 恢复 StaminaRestoreRate * DeltaTime）
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|Attribute|Stamina")
   float StaminaRestoreRate;

   // 上次动作的体力消耗阈值（用于判断是否关闭"体力不足"提示）
   float LastStaminaCostThreshold;

   
public:
    // 修改生命值并广播（同时同步到 GAS）
    UFUNCTION(BlueprintCallable)
	virtual void SetHealth(float InNewHealth);
	// 修改法力值并广播（同时同步到 GAS）
    UFUNCTION(BlueprintCallable)
	virtual void SetMana(float InNewMana);

	// 修改体力值并广播（同时同步到 GAS）
    UFUNCTION(BlueprintCallable)
	virtual void SetStamina(float InNewStamina);
#pragma endregion "Stamina" 
#pragma region "SoulEnumType"

   //玩家攻击方式
   UPROPERTY(BlueprintReadWrite,Category = "Player|Attribute")
   EWeaponType WeaponType;

   //玩家状态
   UPROPERTY(BlueprintReadWrite,Category = "Player|Attribute")
   EPlayerStates PlayerStates;

   //玩家行为
   UPROPERTY(BlueprintReadWrite,Category = "Player|Attribute")
   EPlayerBehavior PlayerBehavior;
#pragma endregion "SoulEnumType"
#pragma region "Function"
  //获取玩家状态
   
    //获取玩家生命值（优先从 GAS 读取）
   UFUNCTION(BlueprintCallable,BlueprintPure)
   float GetHealth() const;
     
    //获取玩家当前体力值（优先从 GAS 读取）
   UFUNCTION(BlueprintCallable,BlueprintPure)
   float GetCurrentStamina() const;

    //获取玩家当前法力值（优先从 GAS 读取）
   UFUNCTION(BlueprintCallable,BlueprintPure)
   float GetCurrentMana() const;
#pragma endregion "Function"
#pragma endregion "Attribute" 
#pragma region "PickUp"
   // 拾取交互组件（管理附近可拾取物品列表和交互UI）
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PickUp")
   USoulPickupComponent* PickupComponent;

   // 获取拾取组件
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PickUp")
   USoulPickupComponent* GetPickupComponent() const { return PickupComponent; }
#pragma endregion "PickUp"
public:
	int32 RollingForwordValue;    //翻滚朝向值（前后）
	int32 RollingRightValue;      //翻滚朝向值（左右）
    float RollingAnimSpeed ;      //翻滚动画速度

public:
#pragma region "Function"
	//摄像机晃动
	UFUNCTION(BlueprintImplementableEvent)
	void CameraShakeFeedBack(bool IsOpen);
	//获取战斗UI
	UFUNCTION(BlueprintCallable)
	UUI_FightMainUI* GetFightMainUI();


	//显示资源不足
	UFUNCTION(BlueprintCallable)
    void ShowIR(FText IRText,FSlateColor IRColor);
    //关闭资源不足
	UFUNCTION(BlueprintCallable)
    void CloseIR();
    bool bIsIRShow = false; //是否在显示资源不足的提示


	//旋转的方向
    FRotator DesiredRotation;
	//角色旋转的具体逻辑
    void CalculatedRotation();
	//是否可以旋转（攻击时朝方向旋转，动画通知状态中改变）
	UPROPERTY(BlueprintReadWrite)
	bool bIsShouldRotate = false;
#pragma endregion "Function"

};
