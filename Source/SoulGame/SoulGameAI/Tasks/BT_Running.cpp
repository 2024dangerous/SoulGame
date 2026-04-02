// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameAI/Tasks/BT_Running.h"
#include "SoulGameAI/SoulAIController.h"
#include "SoulGameAI/SoulBaseEnemy.h"
#include "SoulGameCharacter/SoulBaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SoulGameData/SoulEnumType.h"

EBTNodeResult::Type UBT_Running::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ASoulAIController* AIController = Cast<ASoulAIController>(OwnerComp.GetAIOwner());
    if (!AIController) return EBTNodeResult::Failed;

    ASoulBaseEnemy* AICharacter = Cast<ASoulBaseEnemy>(AIController->GetPawn());
    if (!AICharacter) return EBTNodeResult::Failed;

    AICharacter->EnemyBehavior = EEnemyBehavior::IDLE;
    AICharacter->RunningMovement(true); // 使用数据驱动的速度

    return EBTNodeResult::Succeeded;
}