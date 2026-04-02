// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoulGameHUD/SoulGameWidgetRule.h"
#include "UI_EnemyHealth.generated.h"

/**
 * 
 */
class UTextBlock;
class UProgressBar;
UCLASS()
class SOULGAME_API UUI_EnemyHealth : public USoulGameWidgetRule
{
	GENERATED_BODY()

public: 
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	 //状态栏文字
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta= (BindWidget),Category = "Enemy|Attribute")
	UTextBlock* EnemyName;

	 //状态栏血条
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta= (BindWidget), Category = "Enemy|Attribute")
	UProgressBar* EnemyHealth;
	
    float NowHealthPercent; //当前血量百分比

	float TargetHealthPercent; 

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void SetEnemyHealth(float CurrentPercent);

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void SetEnemyNameText(FText Name);

	void SetAddOrSub();
	bool bIsAddOrSub;

	bool bIsEnemyHealthChaneing;

	

};
