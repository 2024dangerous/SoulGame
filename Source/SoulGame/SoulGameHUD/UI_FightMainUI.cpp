// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameHUD/UI_FightMainUI.h"
#include "Kismet/GameplayStatics.h"
#include "UI_StateBar.h"
#include "Components/TextBlock.h"
#include "SoulGame/SoulPlayerController.h"
#include "SoulGameCharacter/SoulPlayerCharacter.h"
#include "UI_EnemyHealth.h"
#include "SoulGameEvent/SoulEventManager.h"
#include "UI_FightResult.h"
#include "UObject/ConstructorHelpers.h"
#include "AbilitySystemComponent.h"
#include "SoulGameGAS/SoulAttributeSet.h"

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

            // 缓存旧值
            CachedOldHealth = SoulPlayerCharacter->CurrentHealth;
            CachedOldMana = SoulPlayerCharacter->CurrentMana;
            CachedOldStamina = SoulPlayerCharacter->CurrentStamina;

            // 绑定 GAS 属性变化委托
            BindGASAttributeDelegates();
        }
    }
}

void UUI_FightMainUI::NativeDestruct()
{
    Super::NativeDestruct();

    // 解绑 GAS 属性变化委托
    UnbindGASAttributeDelegates();
}

//鎾斁璧勬簮涓嶈冻鍔ㄧ敾锛堣缃祫婧愬悕绉帮級
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


//体力变化事件 - 通过 GAS 属性变化委托直接监听
void UUI_FightMainUI::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
    float MaxValue = SoulPlayerCharacter ? SoulPlayerCharacter->MaxHealth : 100.f;
    WBP_Health->ValueChanged(CachedOldHealth, Data.NewValue, MaxValue);
    CachedOldHealth = Data.NewValue;
}

void UUI_FightMainUI::HandleManaChanged(const FOnAttributeChangeData& Data)
{
    float MaxValue = SoulPlayerCharacter ? SoulPlayerCharacter->MaxMana : 100.f;
    WBP_Mana->ValueChanged(CachedOldMana, Data.NewValue, MaxValue);
    CachedOldMana = Data.NewValue;
}

void UUI_FightMainUI::HandleStaminaChanged(const FOnAttributeChangeData& Data)
{
    float MaxValue = SoulPlayerCharacter ? SoulPlayerCharacter->MaxStamina : 100.f;
    WBP_Stamine->ValueChanged(CachedOldStamina, Data.NewValue, MaxValue);
    CachedOldStamina = Data.NewValue;
}

void UUI_FightMainUI::BindGASAttributeDelegates()
{
    if (!SoulPlayerCharacter) return;

    CachedASC = SoulPlayerCharacter->GetAbilitySystemComponent();
    if (!CachedASC) return;

    CachedASC->GetGameplayAttributeValueChangeDelegate(USoulAttributeSet::GetHealthAttribute())
        .AddUObject(this, &UUI_FightMainUI::HandleHealthChanged);
    CachedASC->GetGameplayAttributeValueChangeDelegate(USoulAttributeSet::GetManaAttribute())
        .AddUObject(this, &UUI_FightMainUI::HandleManaChanged);
    CachedASC->GetGameplayAttributeValueChangeDelegate(USoulAttributeSet::GetStaminaAttribute())
        .AddUObject(this, &UUI_FightMainUI::HandleStaminaChanged);
}

void UUI_FightMainUI::UnbindGASAttributeDelegates()
{
    if (!CachedASC) return;

    CachedASC->GetGameplayAttributeValueChangeDelegate(USoulAttributeSet::GetHealthAttribute())
        .RemoveAll(this);
    CachedASC->GetGameplayAttributeValueChangeDelegate(USoulAttributeSet::GetManaAttribute())
        .RemoveAll(this);
    CachedASC->GetGameplayAttributeValueChangeDelegate(USoulAttributeSet::GetStaminaAttribute())
        .RemoveAll(this);
    CachedASC = nullptr;
}
