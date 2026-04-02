// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameHUD/UI_SetMenu.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "SoulPlayerController.h"

void UUI_SetMenu::NativeConstruct()
{
    Super::NativeConstruct();
    if (Button_Restart)
    {
        Button_Restart->OnClicked.AddDynamic(this, &UUI_SetMenu::OnButton_RestartClicked);
    }
    if (Button_Exit)
    {
        Button_Exit->OnClicked.AddDynamic(this, &UUI_SetMenu::OnButton_ExitClicked);
    }
}

void UUI_SetMenu::NativeDestruct()
{
    Super::NativeDestruct();
}

// 监听Esc按键指令
FReply UUI_SetMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::Escape)
    {
        SetVisibility(ESlateVisibility::Collapsed);
        ASoulPlayerController* PC = Cast<ASoulPlayerController>(UGameplayStatics::GetPlayerController(this, 0)) ;
        if (PC)
        {
            PC->SetInputMode(FInputModeGameOnly());
            PC->bShowMouseCursor = false; // 隐藏鼠标
            UGameplayStatics::SetGamePaused(GetWorld(), false);
        }
        return FReply::Handled();
    }

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UUI_SetMenu::OnButton_RestartClicked()
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

void UUI_SetMenu::OnButton_ExitClicked()
{
    GetCurrentWorld();
    if (!WorldContextObject) return;
   
    APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
    UKismetSystemLibrary::QuitGame(WorldContextObject, PC, EQuitPreference::Quit, false);
}

void UUI_SetMenu::GetCurrentWorld()
{
    WorldContextObject = GetWorld();
}
