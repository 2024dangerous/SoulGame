// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameHUD/UI_FightResult.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/Button.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/TextBlock.h"
#include "../../../../../../../Source/Runtime/Core/Public/Internationalization/Text.h"
#include "../../../../../../../Source/Runtime/Experimental/ChaosCore/Public/Chaos/Vector.h"
#include "../SoulPlayerController.h"

void UUI_FightResult::NativeConstruct()
{
    Super::NativeConstruct();
    if (Button_Restart)
    {
        Button_Restart->OnClicked.AddDynamic(this, &UUI_FightResult::OnButton_RestartClicked);
    }
    if (Button_Exit)
    {
        Button_Exit->OnClicked.AddDynamic(this, &UUI_FightResult::OnButton_ExitClicked);
    }
    if (ASoulPlayerController* PC = Cast<ASoulPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
    {
        PC->bShowMouseCursor = true;
        this->SetFocus();
    }
    // 初始化文本显示
    CountdownText = FString::Printf(TEXT("退出游戏倒计时：%d"), Float_CountDown);
    if (TextBlock_CountDown)
    {
        TextBlock_CountDown->SetText(FText::FromString(CountdownText));
    }
    
}

void UUI_FightResult::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    if (Float_CountDown > 0.0f)
    {
        Float_CountDown -= InDeltaTime;

        int32 DisplayTime = FMath::CeilToInt(Float_CountDown);
        if (TextBlock_CountDown)
        {
            CountdownText = FString::Printf(TEXT("退出游戏倒计时：%d"), DisplayTime);
            TextBlock_CountDown->SetText(FText::FromString(CountdownText));
        }
    }
    else
    {
        OnButton_ExitClicked();
    }
   
}


void UUI_FightResult::OnButton_RestartClicked()
{
     GetCurrentWorld();
     if (!WorldContextObject) return;
 
     if (UWorld* World = GetWorld())
     {
         FString LevelName = World->GetMapName();
         LevelName.RemoveFromStart(World->StreamingLevelsPrefix);
         UGameplayStatics::OpenLevel(WorldContextObject, FName(*LevelName));
     }
}

void UUI_FightResult::OnButton_ExitClicked()
{
     GetCurrentWorld();
     if (!WorldContextObject) return;
 
     APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
     UKismetSystemLibrary::QuitGame(WorldContextObject, PC, EQuitPreference::Quit, false);
}

void UUI_FightResult::GetCurrentWorld()
{
    WorldContextObject = GetWorld();
}