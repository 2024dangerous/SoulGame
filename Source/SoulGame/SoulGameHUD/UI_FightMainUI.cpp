// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameHUD/UI_FightMainUI.h"
#include "Kismet/GameplayStatics.h"
#include "UI_StateBar.h"
#include "Components/TextBlock.h"
#include "SoulGame/SoulPlayerController.h"
#include "SoulGameCharacter/SoulPlayerCharacter.h"
#include "UI_EnemyHealth.h"
#include "../SoulGameEvent/SoulEventManager.h"
#include "UI_FightResult.h"
#include "UObject/ConstructorHelpers.h"

UUI_FightMainUI::UUI_FightMainUI()
{
    static ConstructorHelpers::FClassFinder<UUI_FightResult> UI_FightResultClassBP(TEXT("/Game/_MySoulGame/UI/Fight/WBP_FightResult"));
    UI_FightResultClass = UI_FightResultClassBP.Class;
}

void UUI_FightMainUI::NativeConstruct()
{
    Super::NativeConstruct();
    WBP_EnemyHealth->SetVisibility(ESlateVisibility::Collapsed);

    USoulEventManager* EventMgr = USoulEventManager::Get();
    if (EventMgr && EventMgr->IsInitialized())
    {
        EventMgr->OnStatusBoxChanged.AddUObject(this, &UUI_FightMainUI::HandleStatusValueChanged);
        EventMgr->OpenEnemyHealth.BindUObject(this, &UUI_FightMainUI::SetUI_EnemyHealth);
        EventMgr->OpenFightResult.BindUObject(this, &UUI_FightMainUI::OpenUI_FightResult);
        EventMgr->SwitchEnemyHealth.BindUObject(this, &UUI_FightMainUI::SwitchUI_EnemyHealth);
    }

    ASoulPlayerController* PC = Cast<ASoulPlayerController>(GetOwningPlayer());
    if (PC)
    {
        SoulPlayerCharacter = Cast<ASoulPlayerCharacter>(PC->GetPawn());
        if (SoulPlayerCharacter)
        {
            WBP_Health->InitValue(SoulPlayerCharacter->CurrentHealth, SoulPlayerCharacter->MaxHealth);
            WBP_Mana->InitValue(SoulPlayerCharacter->CurrentMana, SoulPlayerCharacter->MaxMana);
            WBP_Stamine->InitValue(SoulPlayerCharacter->CurrentStamina, SoulPlayerCharacter->MaxStamina);
        }
    }
}

void UUI_FightMainUI::NativeDestruct()
{
    Super::NativeDestruct();

    USoulEventManager* EventMgr = USoulEventManager::Get();
    if (EventMgr && EventMgr->IsInitialized())
    {
        EventMgr->OnStatusBoxChanged.RemoveAll(this);
    }
}

//播放资源不足动画（设置资源名称）
void UUI_FightMainUI::PlayShowIRAnimation(FText ShowText)
{
    TextBlock_ShowInsufficientResource->SetText(ShowText);
    PlayAnimation(ShowInsufficientResource, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f, false);
}

//关闭资源不足动画
void UUI_FightMainUI::CloseShowIR()
{
    TextBlock_ShowInsufficientResource->SetVisibility(ESlateVisibility::Collapsed);
}

//设置资源不足文字颜色
void UUI_FightMainUI::SetIRColor(FSlateColor IRColor)
{
    TextBlock_ShowInsufficientResource->SetColorAndOpacity(IRColor);
}

void UUI_FightMainUI::SetUI_EnemyHealth(FName EnemyName, bool bIsOpen)
{
    if (bIsOpen)
    {
        if (WBP_EnemyHealth->GetVisibility() != ESlateVisibility::Visible)
        {
            WBP_EnemyHealth->SetEnemyNameText(FText::FromName(EnemyName));
            WBP_EnemyHealth->SetVisibility(ESlateVisibility::Visible);
        }
    }
    else
    {
        WBP_EnemyHealth->SetVisibility(ESlateVisibility::Collapsed);
    }
}


void UUI_FightMainUI::SwitchUI_EnemyHealth(FName EnemyName, float CurrentPercent)
{
    if (WBP_EnemyHealth->GetVisibility() == ESlateVisibility::Visible)
    {
        WBP_EnemyHealth->SetEnemyNameText(FText::FromName(EnemyName));
        WBP_EnemyHealth->SetEnemyHealth(CurrentPercent);
    }
}

void UUI_FightMainUI::OpenUI_FightResult()
{
    UI_FightResult = CreateWidget<UUI_FightResult>(GetWorld(), UI_FightResultClass);
    if (!UI_FightResult) return;
    
	UI_FightResult->AddToViewport(0);
	if (ASoulPlayerController* PC = Cast<ASoulPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
	}
}


//体力变化事件
void UUI_FightMainUI::HandleStatusValueChanged(EStatusBox StatusBox, float CurrentValue, float NewValue,float MaxValue)
{
    switch (StatusBox)
    {
    case EStatusBox::Health:
        WBP_Health->ValueChanged(CurrentValue, NewValue, MaxValue);
        break;
    case EStatusBox::Mana:
        WBP_Mana->ValueChanged(CurrentValue, NewValue, MaxValue);
        break;
    case EStatusBox::Stamina:
        WBP_Stamine->ValueChanged(CurrentValue, NewValue, MaxValue);
        break;
    }
}
