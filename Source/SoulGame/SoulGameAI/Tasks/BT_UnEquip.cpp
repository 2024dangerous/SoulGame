// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameAI/Tasks/BT_UnEquip.h"
#include "../SoulAIController.h"
#include "../../SoulGameCharacter/SoulBaseCharacter.h"
#include "../../../../../../../Source/Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBT_UnEquip::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (ASoulAIController* AIController = Cast<ASoulAIController>(OwnerComp.GetAIOwner()))
    {
        if (ASoulBaseCharacter* Player = Cast<ASoulBaseCharacter>(AIController->GetBlackboardComponent()->GetValueAsObject("AttackTarget")))
        {
            AIController->SetFocus(Player);
            AIController->GetBlackboardComponent()->SetValueAsBool("IsUnEquip", true);
            return EBTNodeResult::Succeeded;
        }
    }
    return EBTNodeResult::Failed;
}
