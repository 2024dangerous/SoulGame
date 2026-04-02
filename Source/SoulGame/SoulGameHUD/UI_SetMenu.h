// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoulGameHUD/SoulGameWidgetRule.h"
#include "UI_SetMenu.generated.h"

/**
 * 
 */
class UButton;
UCLASS()
class SOULGAME_API UUI_SetMenu : public USoulGameWidgetRule
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

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

	UObject* WorldContextObject;
};
