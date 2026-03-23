// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameAI/Tasks/BT_Running.h"
#include "../SoulAIController.h"
#include "SoulGameAI/ShadowSpecter.h"
#include "../../SoulGameCharacter/SoulBaseCharacter.h"
#include "../../../../../../../Source/Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"



EBTNodeResult::Type UBT_Running::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (ASoulAIController* AIController = Cast<ASoulAIController>(OwnerComp.GetAIOwner()))
    {
        if (AShadowSpecter* AICharacter = Cast<AShadowSpecter>(AIController->GetPawn()))
        {
            AICharacter->EnemyBehavior = EEnemyBehavior::IDLE;
            AICharacter->RunningMovement(true);
            return EBTNodeResult::Succeeded;
        }
    }
    return EBTNodeResult::Failed;
}
