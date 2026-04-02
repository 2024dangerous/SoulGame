// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_Observer.generated.h"

class ASoulAIController;
class ASoulBaseEnemy;

/**
 * BT_Observer - AI 观察任务（数据驱动版）
 * 
 * AI 进入观察模式，绕行目标
 */
UCLASS()
class SOULGAME_API UBT_Observer : public UBTTaskNode
{
	GENERATED_BODY()

	ASoulAIController* AIController;

	ASoulBaseEnemy* AICharacter;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	void EndObserver();
};
