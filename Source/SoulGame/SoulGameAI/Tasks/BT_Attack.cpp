// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameAI/Tasks/BT_Attack.h"
#include "../SoulAIController.h"
#include "SoulGameAI/ShadowSpecter.h"
#include "../../../../../../../Source/Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "../../SoulGameData/SoulEnumType.h"

EBTNodeResult::Type UBT_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (ASoulAIController* AIController = Cast<ASoulAIController>(OwnerComp.GetAIOwner()))
    {
        if (AShadowSpecter* AICharacter = Cast<AShadowSpecter>(AIController->GetPawn()))
        {
            AICharacter->EnemyBehavior = EEnemyBehavior::MELEEATTACK;
            AICharacter->Attack();
            return EBTNodeResult::Succeeded;
        }
    }

    return EBTNodeResult::Failed;
}
