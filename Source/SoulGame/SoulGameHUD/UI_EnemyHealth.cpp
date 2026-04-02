// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameHUD/UI_EnemyHealth.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UUI_EnemyHealth::NativeConstruct()
{
    Super::NativeConstruct();
    EnemyHealth->SetPercent(0.f);
    NowHealthPercent = 0;
    TargetHealthPercent = 0;
    bIsAddOrSub = false;
    bIsEnemyHealthChaneing = false;
}

void UUI_EnemyHealth::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    if (bIsEnemyHealthChaneing)
    {
        NowHealthPercent = FMath::FInterpTo(NowHealthPercent, TargetHealthPercent, InDeltaTime, 2.0f);
        EnemyHealth->SetPercent(NowHealthPercent);
        // 目标接近时浮点精度判断
        if (FMath::IsNearlyEqual(NowHealthPercent, TargetHealthPercent, 0.001f))
        {
            NowHealthPercent = TargetHealthPercent; // 保证最终一致
            EnemyHealth->SetPercent(NowHealthPercent);
            bIsEnemyHealthChaneing = false;
        }
    }
}

void UUI_EnemyHealth::SetEnemyHealth(float CurrentPercent)
{
    if (EnemyHealth)
    {
        bIsEnemyHealthChaneing = true;
        TargetHealthPercent = CurrentPercent;
        SetAddOrSub();
    }
}



void UUI_EnemyHealth::SetEnemyNameText(FText Name)
{
    if (EnemyName)
    {
        EnemyName->SetText(Name);
    }
}

void UUI_EnemyHealth::SetAddOrSub()
{
    if (NowHealthPercent <= TargetHealthPercent )
    {
        bIsAddOrSub = true;
    }
    else
    {
        bIsAddOrSub = false;
    }
}
