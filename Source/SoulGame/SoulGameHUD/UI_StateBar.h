// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoulGameHUD/SoulGameWidgetRule.h"
#include "UI_StateBar.generated.h"


/**
 * 
 */
class UTextBlock;
class UOverlay;
class UImage;
class UWidgetAnimation;
class UMaterialInstanceDynamic;
UCLASS()
class SOULGAME_API UUI_StateBar : public USoulGameWidgetRule
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

public:
	// 自动同步变量到界面
	virtual void SynchronizeProperties() override;
#pragma region "WidgetComponent"
	//状态栏文字
	UPROPERTY(BlueprintReadWrite,meta= (BindWidget))
	UTextBlock* Text_StateBar;

	//状态栏血条
    UPROPERTY(BlueprintReadWrite,meta= (BindWidget))
	UOverlay* Overlay_StateBar;

    //状态栏血条背景
	 UPROPERTY(BlueprintReadWrite,meta= (BindWidget))
	 UImage* BarBorder;

	 UPROPERTY(BlueprintReadWrite,meta= (BindWidget))
	 UImage* BarFill;

	 UPROPERTY(BlueprintReadWrite,meta= (BindWidget))
	 UImage* BarGlow;

     UMaterialInstanceDynamic* BarBorderMID;
	 UMaterialInstanceDynamic* BarFillMID;
	 UMaterialInstanceDynamic* BarGlowMID;
#pragma endregion "WidgetComponent"
#pragma region "WidgetAnimation"
	// 绑定动画
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* OnDamaged;

    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* OnHealed;


    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* OnEliminated;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* OnSpawned;
#pragma endregion "WidgetAnimation"

	//暴露给父控件，实时更改文本
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Attribute")
	FString UpdateNameText = TEXT("Text Black");

	//判断是否为生命值,使用不同逻辑
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Attribute")
	bool bIsHealth = false;

	//是否在动画中
	bool bIsAnimating = false;

public:
    //初始化材质，设置动态材质
	UFUNCTION(BlueprintCallable)
	 void SetDynamicMaterials();
	//重置动画状态
	UFUNCTION(BlueprintCallable)
	 void ResetAnimatedState();
     //初始化血条视觉效果
	UFUNCTION(BlueprintCallable)
	 void InitializeBarVisuals();
     //死亡动画
	UFUNCTION(BlueprintCallable)
	 void EventOnEliminated();


     //设置血条的当前值
	UFUNCTION(BlueprintCallable)
	void ValueChanged(float OldValue,float NewValue, float MaxValue);

	UFUNCTION(BlueprintCallable)
	void InitValue(float InitValue,float MaxValue);

};
