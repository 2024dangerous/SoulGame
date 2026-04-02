// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SoulGameWidgetRule.generated.h"

/**
 * 
 */
UCLASS()
class SOULGAME_API USoulGameWidgetRule : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
public:
	// 自动同步变量到界面
	virtual void SynchronizeProperties() override;
};
