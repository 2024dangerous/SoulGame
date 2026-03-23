// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameAI/Tasks/BTS_DistanceCheck.h"
#include "../SoulAIController.h"
#include "../SoulBaseEnemy.h"
#include "../../SoulGameCharacter/SoulBaseCharacter.h"
#include "SoulGameAI/ShadowSpecter.h"
#include "../../../../../../../Source/Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"

enum class ERangeType
{
    Close,
    Medium,
    Far
};


ERangeType GetRangeType(float Distance)
{
    if (Distance <= 180.f) return ERangeType::Close;
    if (Distance > 180.f  && Distance < 500.f) return ERangeType::Medium;
    return ERangeType::Far;
}



void UBTS_DistanceCheck::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
    if (ASoulAIController* AIController = Cast<ASoulAIController>(OwnerComp.GetAIOwner()))
    {
        if (AShadowSpecter* AICharacter = Cast<AShadowSpecter>(AIController->GetPawn()))
        {
            if (ASoulBaseCharacter* Player = Cast<ASoulBaseCharacter>(AICharacter->AttackTarget))
            {
                float Distance = AICharacter->GetDistanceTo(Player);
                FVector CurrentLocation = AICharacter->GetActorLocation();
                float DistanceToInit = FVector::Dist(CurrentLocation, AICharacter->InitLocation);
                if (DistanceToInit < 100.f)
                {

                    AIController->GetBlackboardComponent()->SetValueAsBool("IsReSet", false);
                }
                if (DistanceToInit > 4000.f)
                {
                    AIController->GetBlackboardComponent()->SetValueAsBool("IsReSet", true);
                }
                else
                {
                    switch(GetRangeType(Distance))
                    {
                    case ERangeType::Close:
                        AIController->GetBlackboardComponent()->SetValueAsBool("IsAttack", true);
                        AIController->GetBlackboardComponent()->SetValueAsBool("IsRunning", false);
                        break;
                    case ERangeType::Medium:
                        AICharacter->RunningMovement(false);
                        AIController->GetBlackboardComponent()->SetValueAsBool("IsAttack", false);
                        AIController->GetBlackboardComponent()->SetValueAsBool("IsRunning", false);
                        break;
                    case ERangeType::Far:
                        AIController->GetBlackboardComponent()->SetValueAsBool("IsRunning", true);
                        AIController->GetBlackboardComponent()->SetValueAsBool("IsAttack", false);
                        break;
                    }
                }
            }
        }
    }
}
