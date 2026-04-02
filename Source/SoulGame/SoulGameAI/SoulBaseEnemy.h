// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SoulGameTags/SoulGameplayTagInterface.h"
#include "AbilitySystemInterface.h"
#include "SoulBaseEnemy.generated.h"

class USphereComponent;
class ASoulAIController;
class UWidgetComponent;
class USoulAIConfig;
class USoulAbilitySystemComponent;
class USoulAttributeSet;
class USoulGameplayAbility;
struct FSoulActionType;
enum class EEnemyBehavior :uint8;
enum class EEnemyWeaponType :uint8;

UCLASS()
class SOULGAME_API ASoulBaseEnemy : public ACharacter, public ISoulGameplayTagInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ASoulBaseEnemy();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    // ============ 基础属性 ============

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Enemy|Attribute")
    FName EnemyName;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Enemy|Attribute")
    float CurrentHealth;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Enemy|Attribute")
    float MaxHealth;

    // ============ 组件 ============

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sphere")
	USphereComponent* PandoraBox;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tip")
	UWidgetComponent* EnemyTip;

    // ============ 行为状态（旧接口保留兼容）============

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnemyBehavior")
    EEnemyBehavior EnemyBehavior;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnemyBehavior")
    EEnemyWeaponType EnemyWeaponType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    bool bIsWeapons;

    ASoulAIController* AIController;
   
	// 动作数据表
    UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "AnimMontage", meta = (AllowPrivateAccess = "true"))
    UDataTable* AnimMontageTable;

	// 初始化动画蒙太奇，读取数据表
	virtual void InitAnimMontage() {};

    void GetAIController();
   
    void SetEnemyTipVisibility(bool bIsVisibility);

    virtual void Injure(float SubHealth);
    virtual void Attack() {};

    void RunningMovement(bool IsRun);

    // ============ AI 配置（数据驱动）============

    /** AI 配置数据资产 - 策划在编辑器中配置 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|AI")
    USoulAIConfig* AIConfig;

    // ============ GAS (GameplayAbilitySystem) ============

    // IAbilitySystemInterface 接口实现
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    // 获取自定义 ASC
    UFUNCTION(BlueprintCallable, Category = "SoulGame|GAS")
    USoulAbilitySystemComponent* GetSoulAbilitySystemComponent() const;

    // 获取属性集
    UFUNCTION(BlueprintCallable, Category = "SoulGame|GAS")
    const USoulAttributeSet* GetSoulAttributeSet() const;

    // 初始化 GAS
    UFUNCTION(BlueprintCallable, Category = "SoulGame|GAS")
    void InitializeGAS();

    // 获取当前生命值（优先从 GAS 读取）
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SoulGame|GAS")
    float GetHealth() const;

    // ============ GameplayTags 接口 ============

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|GameplayTags")
    FGameplayTagContainer GameplayTagContainer;

    virtual FGameplayTagContainer GetGameplayTagContainer_Implementation() const override;
    virtual void AddGameplayTag_Implementation(const FGameplayTag Tag) override;
    virtual void RemoveGameplayTag_Implementation(const FGameplayTag Tag) override;
    virtual bool HasGameplayTag_Implementation(const FGameplayTag Tag) const override;

protected:
    // 技能系统组件
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SoulGame|GAS")
    USoulAbilitySystemComponent* AbilitySystemComponent;

    // 属性集
    UPROPERTY()
    const USoulAttributeSet* AttributeSet;

    // 默认授予的技能列表
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SoulGame|GAS")
    TArray<TSubclassOf<USoulGameplayAbility>> DefaultAbilities;

    // GAS 是否已初始化
    bool bGASInitialized = false;
};