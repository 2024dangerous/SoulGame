// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameModeBase.h"
#include "SoulPlayerController.h"
#include "SoulGameHUD/SoulGameHUD.h"
#include "SoulGameCharacter/SoulBaseCharacter.h"
#include "SoulGameDebug/DebugTools.h"

ASoulGameModeBase::ASoulGameModeBase()
{
    static ConstructorHelpers::FClassFinder<ASoulBaseCharacter> PlayerControllerBPClass(TEXT("/Game/_MySoulGame/Characters/BP_Player"));
    if (PlayerControllerBPClass.Class)
    {
        DefaultPawnClass = PlayerControllerBPClass.Class;
    }

    PlayerControllerClass = ASoulPlayerController::StaticClass();

    HUDClass = ASoulGameHUD::StaticClass();
}
