// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameAI/Tasks/BT_Observer.h"
#include "SoulGameAI/SoulAIController.h"
#include "SoulGameAI/SoulBaseEnemy.h"
#include "SoulGameAI/Config/SoulAIConfig.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBT_Observer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AIController = Cast<ASoulAIController>(OwnerComp.GetAIOwner());
    if (!AIController) return EBTNodeResult::Failed;

    UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
    if (!BlackboardComp) return EBTNodeResult::Failed;

    bool IsOb = BlackboardComp->GetValueAsBool("ObserverDoOnce");

    AICharacter = Cast<ASoulBaseEnemy>(AIController->GetPawn());
    if (AICharacter && IsOb)
    {
        BlackboardComp->SetValueAsBool("ObserverDoOnce", false);
    }

    return EBTNodeResult::Type();
}

void UBT_Observer::EndObserver()
{
    if (AIController)
    {
        UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
        if (BlackboardComp)
        {
            BlackboardComp->SetValueAsBool("IsObserver", false);
            BlackboardComp->SetValueAsBool("ObserverDoOnce", false);
        }
    }
}