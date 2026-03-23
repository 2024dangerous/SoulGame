// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_Observer.generated.h"

/**
 * 
 */
class ASoulAIController;
class AShadowSpecter;
UCLASS()
class SOULGAME_API UBT_Observer : public UBTTaskNode
{
	GENERATED_BODY()

	ASoulAIController* AIController;

	AShadowSpecter* AICharacter;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	void EndObserver();
};
