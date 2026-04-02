// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameHUD/UI_FightResult.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Internationalization/Text.h"
#include "SoulPlayerController.h"

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
    // 鍒濆鍖栨枃鏈樉绀?
    CountdownText = FString::Printf(TEXT("閫€鍑烘父鎴忓€掕鏃讹細%d"), Float_CountDown);
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
            CountdownText = FString::Printf(TEXT("閫€鍑烘父鎴忓€掕鏃讹細%d"), DisplayTime);
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