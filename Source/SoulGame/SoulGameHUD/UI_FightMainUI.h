// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoulGameHUD/SoulGameWidgetRule.h"
#include "UI_FightMainUI.generated.h"

class UTextBlock;
class UOverlay;
class UUI_StateBar;
class UUI_Interaction;
class UUI_EnemyHealth;
class UUI_FightResult;
class ASoulPlayerCharacter; 
UCLASS()
class SOULGAME_API UUI_FightMainUI : public USoulGameWidgetRule
{
	GENERATED_BODY()

public:
	UUI_FightMainUI();

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
#pragma region "WidgetComponent"
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	UUI_StateBar* WBP_Health;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,meta=(BindWidget))
	UUI_StateBar* WBP_Mana;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	UUI_StateBar* WBP_Stamine;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	UUI_Interaction* WBP_Interaction;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	UUI_EnemyHealth* WBP_EnemyHealth;

	TSubclassOf<UUI_FightResult> UI_FightResultClass;
	UUI_FightResult* UI_FightResult;


	//资源不足文字
	UPROPERTY(BlueprintReadWrite,meta= (BindWidget))
	UTextBlock* TextBlock_ShowInsufficientResource;
#pragma endregion "WidgetComponent"
#pragma region "WidgetAnimation"
	// 绑定动画
    UPROPERTY(BlueprintReadWrite,meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* ShowInsufficientResource;
#pragma endregion "WidgetAnimation"


	//播放资源不足动画（设置资源名称）
	UFUNCTION(BlueprintCallable)
    void PlayShowIRAnimation(FText ShowText);
    //关闭资源不足动画
	UFUNCTION(BlueprintCallable)
    void CloseShowIR();
    //设置资源不足文字颜色
	UFUNCTION(BlueprintCallable)
	void SetIRColor(FSlateColor IRColor);

	UFUNCTION()
	void SetUI_EnemyHealth(FName EnemyName,bool bIsOpen);

	UFUNCTION()
	void SwitchUI_EnemyHealth(FName EnemyName,float CurrentPercent);

	UFUNCTION()
	void OpenUI_FightResult();
protected:
    //体力变化事件
    UFUNCTION()
    void HandleStatusValueChanged(EStatusBox StatusBox, float CurrentValue,float NewValue,float MaxValue);

    UPROPERTY()
    ASoulPlayerCharacter* SoulPlayerCharacter;


};
