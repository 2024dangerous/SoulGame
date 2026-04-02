// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoulGameAI/SoulBaseEnemy.h"
#include "ShadowSpecter.generated.h"

/**
 * 影魍
 */
class UNiagaraComponent;
class UBoxComponent;
class ASoulBaseCharacter;
UCLASS()
class SOULGAME_API AShadowSpecter : public ASoulBaseEnemy
{
	GENERATED_BODY()
	
public: 
	AShadowSpecter();

    virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	UStaticMeshComponent* Sword;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    UNiagaraComponent* SwordNiagara;
    UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* SwordSheath;  //剑鞘
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	UBoxComponent* SwordAttackBox;

	//切换武器时的特效材质（编辑器可配置）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Material")
	TSoftObjectPtr<UMaterialInterface> ChangeWeaponMaterialAsset;
	//切换武器时的材质（运行时缓存）
	UPROPERTY(Transient)
	UMaterialInterface* ChangeWeaponMaterial;

	UFUNCTION()
	void StartBattle(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndBattle(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//初始化动画蒙太奇，读取数据表
	virtual void InitAnimMontage() override;

	virtual void Injure(float SubHealth) override;

	
    UFUNCTION(BlueprintCallable)
	virtual void Attack() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShadowSpecter")
    bool bIsAttacking;
	bool CanAttack();

	UFUNCTION(BlueprintCallable)
	void ResetAttack();


	FVector InitLocation; //初始位置  
private:

	FSoulActionType* EquipAnims;          //拔剑动画
    FSoulActionType* MeleeAttackAnims;    //近战攻击动画
    FSoulActionType* MeleeRollingAnims;   //近战翻滚动画
    FSoulActionType* SwordAttackAnims;    //单手剑攻击动画
    FSoulActionType* SwordRollingAnims;   //单手剑翻滚动画
    FSoulActionType* RushAttackAnims;     //冲刺攻击动画
    FSoulActionType* InjuryAnims;         //受伤动画
    FSoulActionType* DefenseAnims;        //防御动画
	
	int32 LastMeleeAttackIndex = -1;       //上次拳脚攻击的索引
	int32 LastSwordAttackIndex = -1;       //上次单手剑攻击的索引
	int32 LastAnimationIndex = -1;

	// 播放随机动画
	bool PlayRandomAnimMontage(FSoulActionType* CurrentAnimMontage, int32& LastAnimMontageIndex);


    
public:
	UPROPERTY(BlueprintReadOnly,Category = "AttackTarget")
    ASoulBaseCharacter* AttackTarget; //攻击目标

	UPROPERTY(BlueprintReadOnly,Category = "AttackTarget")
    TSubclassOf<ASoulBaseCharacter> InsPlayer; //攻击目标

    FTimerHandle ObserverHandle; //观察者计时器句柄
    FTimerHandle CloseObserverHandle; //关闭观察者计时器句柄

	void EnableObServer();

	void ObserverMove();

	void EndObserver();

	float RandomStrafeValue;

	void Running(bool IsRun);

	//设置攻击目标
};
