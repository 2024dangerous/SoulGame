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
	// 鑷姩鍚屾鍙橀噺鍒扮晫闈?
	virtual void SynchronizeProperties() override;
#pragma region "WidgetComponent"
	//鐘舵€佹爮鏂囧瓧
	UPROPERTY(BlueprintReadWrite,meta= (BindWidget))
	UTextBlock* Text_StateBar;

	//鐘舵€佹爮琛€鏉?
    UPROPERTY(BlueprintReadWrite,meta= (BindWidget))
	UOverlay* Overlay_StateBar;

    //鐘舵€佹爮琛€鏉¤儗鏅?
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

	//鏆撮湶缁欑埗鎺т欢锛屽疄鏃舵洿鏀规枃鏈?
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Attribute")
	FString UpdateNameText = TEXT("Text Black");

	//鍒ゆ柇鏄惁涓虹敓鍛藉€?使用不同逻辑
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Attribute")
	bool bIsHealth = false;

	//鏄惁鍦ㄥ姩鐢讳腑
	bool bIsAnimating = false;

public:
    //鍒濆鍖栨潗璐紝璁剧疆鍔ㄦ€佹潗璐?
	UFUNCTION(BlueprintCallable)
	 void SetDynamicMaterials();
	//閲嶇疆鍔ㄧ敾鐘舵€?
	UFUNCTION(BlueprintCallable)
	 void ResetAnimatedState();
     //鍒濆鍖栬鏉¤瑙夋晥鏋?
	UFUNCTION(BlueprintCallable)
	 void InitializeBarVisuals();
     //死亡动画
	UFUNCTION(BlueprintCallable)
	 void EventOnEliminated();


     //璁剧疆琛€鏉＄殑褰撳墠鍊?
	UFUNCTION(BlueprintCallable)
	void ValueChanged(float OldValue,float NewValue, float MaxValue);

	UFUNCTION(BlueprintCallable)
	void InitValue(float InitValue,float MaxValue);

};
