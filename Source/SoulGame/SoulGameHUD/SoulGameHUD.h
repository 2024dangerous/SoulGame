// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SoulGameHUD.generated.h"

class UUI_FightMainUI;
class UUI_SetMenu;
UCLASS()
class SOULGAME_API ASoulGameHUD : public AHUD
{
    GENERATED_BODY()

    ASoulGameHUD();

    virtual void BeginPlay() override;
private:
    TSubclassOf<UUI_FightMainUI> UI_FightMainUIClass;
    UUI_FightMainUI* UI_FightMainUI;

    TSubclassOf<UUI_SetMenu> UI_SetMenuClass;
    UUI_SetMenu* UI_SetMenu;

    UPROPERTY()
    TArray<UUserWidget*> CachedUIStack;
    UPROPERTY()
    TArray<UUserWidget*> CurrentlyActiveUI;

public:
    //获取当前的UI_FightMainUI
    UFUNCTION(BlueprintCallable, BlueprintPure)
    UUI_FightMainUI* GetUI_FightMainUI() const { return UI_FightMainUI; };


public:
    void OpenFightUI(bool bOpen);
    void OpenSetMenu(bool bIsOpen);
    void OpenInteractionUI(bool bOpen, FText InteractionText);
};
