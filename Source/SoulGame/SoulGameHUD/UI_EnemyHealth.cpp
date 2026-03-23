// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameHUD/UI_EnemyHealth.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/TextBlock.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/ProgressBar.h"
#include "../../../../../../../Source/Runtime/Experimental/ChaosCore/Public/Chaos/Vector.h"

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
        // 改进：避免浮点数比较问题
        if (FMath::IsNearlyEqual(NowHealthPercent, TargetHealthPercent, 0.001f))
        {
            NowHealthPercent = TargetHealthPercent; // 保证收尾一致
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
