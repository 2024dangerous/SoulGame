// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoulGameHUD/SoulGameWidgetRule.h"
#include "UI_Interaction.generated.h"

/**
 * 
 */
class UTextBlock;
UCLASS()
class SOULGAME_API UUI_Interaction : public USoulGameWidgetRule
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
    UTextBlock* TextBlock_Interaction;

	// °ó¶¨¶¯»­
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* Interaction_Visable;

public:
    UFUNCTION()
    void SetInteractionText(FText InteractionText);

    UFUNCTION(BlueprintCallable)
    void PlayInteractionAnimation();
};
