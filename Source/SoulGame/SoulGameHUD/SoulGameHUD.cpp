// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameHUD/SoulGameHUD.h"
#include "UI_FightMainUI.h"
#include "../SoulGameEvent/SoulEventManager.h"
#include "UI_Interaction.h"
#include "UI_SetMenu.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "../SoulGameDebug/DebugTools.h"
#include "UI_FightResult.h"

ASoulGameHUD::ASoulGameHUD()
{
    static ConstructorHelpers::FClassFinder<UUI_FightMainUI> UI_FightMainUIClassBP(TEXT("/Game/_MySoulGame/UI/Fight/WBP_FightMain"));
    UI_FightMainUIClass = UI_FightMainUIClassBP.Class;

    static ConstructorHelpers::FClassFinder<UUI_SetMenu> UI_SetMenuClassBP(TEXT("/Game/_MySoulGame/UI/Settings/WBP_SettingsMenu"));
    UI_SetMenuClass = UI_SetMenuClassBP.Class;


    CachedUIStack = {};
    CurrentlyActiveUI = {};

}

void ASoulGameHUD::BeginPlay()
{
    Super::BeginPlay();
    UI_FightMainUI = CreateWidget<UUI_FightMainUI>(GetWorld(), UI_FightMainUIClass);
    UI_FightMainUI->AddToViewport(0);


    UI_SetMenu = CreateWidget<UUI_SetMenu>(GetWorld(), UI_SetMenuClass);
    UI_SetMenu->AddToViewport(0);
    UI_SetMenu->SetVisibility(ESlateVisibility::Collapsed);

    USoulEventManager::Get()->OpenFightUI.BindUObject(this, &ASoulGameHUD::OpenFightUI);
    USoulEventManager::Get()->OpenSetMenu.BindUObject(this, &ASoulGameHUD::OpenSetMenu);
    USoulEventManager::Get()->OpenInteractionUI.BindUObject(this, &ASoulGameHUD::OpenInteractionUI);

}

void ASoulGameHUD::OpenFightUI(bool bOpen)
{
    if (bOpen && UI_FightMainUIClass)
    {
        UI_FightMainUI = CreateWidget<UUI_FightMainUI>(GetWorld(), UI_FightMainUIClass);
        UI_FightMainUI->AddToViewport(0);
    }
    else
    {
        if (UI_FightMainUI)
        {
            UI_FightMainUI->RemoveFromParent();
            UI_FightMainUI = nullptr;
        }
    }
}

void ASoulGameHUD::OpenSetMenu(bool bIsOpen)
{
    if (bIsOpen && UI_SetMenu)
    {
        UI_SetMenu->SetVisibility(ESlateVisibility::Visible);
        UGameplayStatics::SetGamePaused(GetWorld(), true);  // 暂停

        APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
        if (PC)
        {
            //UI_SetMenu->bIsFocusable = true;
            //FInputModeUIOnly InputMode;
            //InputMode.SetWidgetToFocus(UI_SetMenu->TakeWidget());  // 关键！！！
            //InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

            //PC->SetInputMode(InputMode);
            UI_SetMenu->SetKeyboardFocus();
            PC->SetInputMode(FInputModeUIOnly());
            PC->bShowMouseCursor = true; // 显示鼠标
        }
    }
}

void ASoulGameHUD::OpenInteractionUI(bool bOpen, FText InteractionText)
{
    if (!UI_FightMainUI || !UI_FightMainUI->WBP_Interaction) return;
    UUI_Interaction* CurInteractionUI = UI_FightMainUI->WBP_Interaction;


    if (bOpen && CurInteractionUI->GetVisibility() != ESlateVisibility::Visible)
    {
        CurInteractionUI->SetInteractionText(InteractionText);
        CurInteractionUI->SetVisibility(ESlateVisibility::Visible);
        CurInteractionUI->PlayInteractionAnimation();
    }
    else if(!bOpen && CurInteractionUI->GetVisibility() == ESlateVisibility::Visible)
    {
        CurInteractionUI->SetVisibility(ESlateVisibility::Collapsed);
    }
}
