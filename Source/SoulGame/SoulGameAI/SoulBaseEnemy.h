// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SoulBaseEnemy.generated.h"

/**
 * 
 */
class USphereComponent;
class ASoulAIController;
class UWidgetComponent;
struct FSoulActionType;
enum class EEnemyBehavior :uint8;
enum class EEnemyWeaponType :uint8;
UCLASS()
class SOULGAME_API ASoulBaseEnemy : public ACharacter
{
	GENERATED_BODY()
	
public:
	ASoulBaseEnemy();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Enemy|Attribute")
    FName EnemyName;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Enemy|Attribute")
    float CurrentHealth;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Enemy|Attribute")
    float MaxHealth;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sphere")
	USphereComponent* PandoraBox;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tip")
	UWidgetComponent* EnemyTip;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnemyBehavior")
    EEnemyBehavior EnemyBehavior; //敌人行为

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnemyBehavior")
    EEnemyWeaponType EnemyWeaponType; //敌人行为

     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")

    bool bIsWeapons; //是否有武器

    ASoulAIController* AIController; //AI控制器
   
	//动作数据表
    UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "AnimMontage", meta = (AllowPrivateAccess = "true"))
    UDataTable* AnimMontageTable;

	//初始化动画蒙太奇，读取数据表
	virtual void InitAnimMontage() {};

    void GetAIController();
   
    void SetEnemyTipVisibility(bool bIsVisibility);

    virtual void Injure(float SubHealth);
    virtual void Attack() {};


    void RunningMovement(bool IsRun);
};
