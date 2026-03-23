// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameAI/Tasks/BT_Observer.h"
#include "../SoulAIController.h"
#include "../SoulBaseEnemy.h"
#include "../../../../../../../Source/Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "../ShadowSpecter.h"

EBTNodeResult::Type UBT_Observer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AIController = Cast<ASoulAIController>(OwnerComp.GetAIOwner());

    bool Isob =  AIController->GetBlackboardComponent()->GetValueAsBool("ObserverDoOnce");
    if (AIController)
    {
        AICharacter = Cast<AShadowSpecter>(AIController->GetPawn());
        if (AICharacter && Isob)
        {
            AIController->GetBlackboardComponent()->SetValueAsBool("ObserverDoOnce", false);
        }
    }
    return EBTNodeResult::Type();
}

void UBT_Observer::EndObserver()
{
    AIController->GetBlackboardComponent()->SetValueAsBool("IsObserver", false);
    AIController->GetBlackboardComponent()->SetValueAsBool("ObserverDoOnce", false);
}
