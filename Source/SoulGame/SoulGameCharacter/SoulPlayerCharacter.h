// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoulGameCharacter/SoulBaseCharacter.h"
#include "SoulPlayerCharacter.generated.h"

class UStaticMeshComponent;
class UNiagaraComponent;
class UDataTable;
class UBoxComponent;
class ASoulBaseEnemy;
struct FSoulActionType;
UCLASS()
class SOULGAME_API ASoulPlayerCharacter : public ASoulBaseCharacter
{
    GENERATED_BODY()

public:
    ASoulPlayerCharacter();
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
#pragma region "Component"
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* Sword;   //剑
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    UNiagaraComponent* SwordNiagara;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* SwordSheath; //剑鞘
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
    UBoxComponent* SwordAttackBox;

    UFUNCTION(BlueprintCallable, Category = "SwordAttackBox")
    void SetSwordAttackBoxCollisionEnabled(bool bIsEnabled);


    UFUNCTION()
    void SwordAttackEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

#pragma endregion "Component"
public:
    virtual void Look(const FInputActionValue& Value) override;

    ASoulBaseEnemy* GetNextTargetEnemy(bool bIsRight);     //获取下一个目标敌人

    void SwitchTargetEnemy(bool bIsRight);       //切换目标敌人

    virtual void Exit(const FInputActionValue& Value) override;
#pragma region "Attack"
    //攻击逻辑
    virtual void Attack() override;
    //近战攻击逻辑
    void MeleeAttack();
    //刀剑攻击逻辑
    void SwordAttack();
    //退出攻击状态
    void ExitAttackState();
#pragma endregion "Attack"
#pragma region "Rolling"
    //翻滚逻辑
    virtual void Rolling() override;
    //近战翻滚逻辑
    void MeleeRolling();
    //刀剑翻滚逻辑
    void SwordRolling();
#pragma endregion "Rolling"
#pragma region "Defense"
    //翻滚逻辑
    virtual void Defense(const FInputActionValue& Value) override;
    //是否能进行刀剑翻滚
    bool CanDefense();
    //是否在防御状态 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defense")
    bool bIsDefense;
#pragma endregion "Defense"
#pragma region "Weapons"
    //切换武器
    virtual void Weapons() override;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void ChangeSwordSlot(bool bIsSword);

    void ChangeSwordWeaponType();
    void CloseSwordNiagara();

    //是否能进行切换武器
    bool CanWeapons();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")

    bool bIsWeapons; //是否有武器

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    bool bIsChangingWeapons; //是否在切换武器

    //当前武器材质
    UPROPERTY(Transient)
    UMaterialInterface* CurrentWeaponMaterial;
    //当前剑鞘材质
    UPROPERTY(Transient)
    UMaterialInterface* CurrentSwordSheathMaterial;
    //切换武器时的特效材质（编辑器可配置）
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Material")
    TSoftObjectPtr<UMaterialInterface> ChangeWeaponMaterialAsset;
    //切换武器时的材质（运行时缓存）
    UPROPERTY(Transient)
    UMaterialInterface* ChangeWeaponMaterial;
#pragma endregion "Weapons"
#pragma region "Focus"
    //注视敌人
    virtual void Focus() override;

    ASoulBaseEnemy* FocusedTarget = nullptr;
    float AccumulatedMouseX = 0.0f; // 累计鼠标横向移动值
    const float SwitchThreshold = 100.0f; // 设定一个合适的切换阈值（可调）

    //持续注视
    void Focusing();

    //平滑开始注视
    void SetStartFocus();

    // 所有敌人离开感知范围时的回调（取消锁定）
    UFUNCTION()
    void OnAllEnemiesLost();
#pragma endregion "Focus"
#pragma region "Injury"
    void Injury(FVector HitLocation, float Health_Sub);

    UFUNCTION(BlueprintCallable)
    void MeleeInjury(FVector HitLocation,float Health_Sub);

    UFUNCTION(BlueprintCallable)
    void SwordInjury(FVector HitLocation, float Health_Sub);

    void HealthChange(float Health_Sub);

    bool CanInjury();
#pragma endregion "Injury"
private:
#pragma region "Animation"
    //动作数据表
    UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = AnimMontage, meta = (AllowPrivateAccess = "true"))
    UDataTable* StaminaCost;

    //初始化动画蒙太奇，读取数据表
    void InitAnimMontage();
    FSoulActionType* MeleeAttackAnim;      //近战攻击动画
    FSoulActionType* MeleeRollingAnim;     //近战翻滚动画
    FSoulActionType* SwordAttackAnim;      //刀剑攻击动画
    FSoulActionType* SwordRollingAnim;     //刀剑翻滚动画
    FSoulActionType* MeleeInjuryAnim;          //近战受击动画
    FSoulActionType* MeleePreparwarInjuryAnim; //近战战斗受击动画
    FSoulActionType* SwordInjuryAnim;          //持剑受击动画
    FSoulActionType* SwordDefenseInjuryAnim;          //持剑防御受击动画

    int32 LastMeleeAttackIndex = -1;       //上次拳脚攻击的索引
    int32 LastSwordAttackIndex = -1;       //上次单手剑攻击的索引

#pragma endregion "Animation"

#pragma region "Combat"
    //通用攻击执行方法（消除 MeleeAttack/SwordAttack 重复代码）
    void ExecuteAttack(FSoulActionType* ActionAnim, int32& LastIndex, bool bClearExitTimer = false);
    //通用翻滚执行方法（消除 MeleeRolling/SwordRolling 重复代码）
    void ExecuteRolling(FSoulActionType* RollingAnim);
    //通用动作检查方法（消除 CanMeleeAttack/CanSwordAttack/CanMeleeRolling/CanSwordRolling 重复代码）
    bool CanExecuteAction(FSoulActionType* ActionAnim, bool bRequireWeapon = false, bool bRequireSwordType = false);

    //剑攻击伤害值（可配置）
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
    float SwordDamage = 30.f;
#pragma endregion "Combat"

    //定时器句柄
    FTimerHandle ExitAttackStateTimerHandle; //退出攻击状态定时器句柄
    FTimerHandle StaminaRestoreTimer;        //恢复体力定时器句柄
    FTimerHandle WeaponSwitchTimer;
    FTimerHandle CloseNiagaraWeapons;

#pragma region "Function"
    //恢复体力
    void UpdateStaminaRestore();

    // 播放随机动画
    bool PlayRandomAnimMontage(FSoulActionType* CurrentAnimMontage, int32& LastAnimMontageIndex);
    //获取翻滚动画索引
    int32 GetRollingAnimIndex(float ForwardValue, float RightValue);
    //播放翻滚动画
    bool PlayRollingAnimMontage(FSoulActionType* CurrentAnimMontage);
    bool PlayInjuryAnimMontage(FSoulActionType* CurrentAnimMontage, FVector HitLocation);

    //获取最近敌人
    ASoulBaseEnemy* GetNearestEnemy();

    int32 GetHitLocationAnimIndex(FVector HitLocation);

    void Die();
    bool bIsDie;

    void RemovePlayerInput();

#pragma endregion "Function"
};
