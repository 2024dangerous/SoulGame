// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoulGameHUD/SoulGameWidgetRule.h"
#include "UI_FightResult.generated.h"

/**
 * 
 */
class UTextBlock;
class UButton;
class FText;
UCLASS()
class SOULGAME_API UUI_FightResult : public USoulGameWidgetRule
{
	GENERATED_BODY()
	
	

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextBlock_CountDown;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* Button_Restart;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* Button_Exit;

	UFUNCTION()
	void OnButton_RestartClicked();

	UFUNCTION()
	void OnButton_ExitClicked();

	UFUNCTION()
	void GetCurrentWorld();


	FString CountdownText;
	float Float_CountDown = 30.f;

	UObject* WorldContextObject;

};
