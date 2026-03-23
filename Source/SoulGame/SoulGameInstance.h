// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SoulGameInstance.generated.h"

/**
 * 
 */
class UObjectPoolManager;
UCLASS()
class SOULGAME_API USoulGameInstance : public UGameInstance
{
	GENERATED_BODY()
	

	virtual void Init() override;


public:
    UPROPERTY()
    UObjectPoolManager* PoolManager;


	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FName CurrentLevelName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FName TargetLevelName;




	void OnWorldInitialized(UWorld* World, const UWorld::InitializationValues IVS);
};
