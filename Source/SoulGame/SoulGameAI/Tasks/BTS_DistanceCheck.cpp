// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameAI/Tasks/BTS_DistanceCheck.h"
#include "SoulGameAI/SoulAIController.h"
#include "SoulGameAI/SoulBaseEnemy.h"
#include "SoulGameAI/Config/SoulAIConfig.h"
#include "SoulGameCharacter/SoulBaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

void UBTS_DistanceCheck::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    ASoulAIController* AIController = Cast<ASoulAIController>(OwnerComp.GetAIOwner());
    if (!AIController) return;

    ASoulBaseEnemy* AICharacter = Cast<ASoulBaseEnemy>(AIController->GetPawn());
    if (!AICharacter) return;

    UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
    if (!BlackboardComp) return;

    // 获取攻击目标
    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("AttackTarget"));
    if (!TargetActor) return;

    float Distance = AICharacter->GetDistanceTo(TargetActor);

    // 获取配置（数据驱动）
    USoulAIConfig* Config = AIController->GetAIConfig();

    // 计算距离阈值
    float MaxChaseDistance = Config ? Config->RangeConfig.MaxChaseDistance : 4000.0f;
    float ResetDistance = Config ? Config->RangeConfig.ResetDistance : 100.0f;

    // 检查是否需要返回初始位置
    FVector InitLocation = BlackboardComp->GetValueAsVector("InitLocation");
    float DistanceToInit = FVector::Dist(AICharacter->GetActorLocation(), InitLocation);

    if (DistanceToInit < ResetDistance)
    {
        BlackboardComp->SetValueAsBool("IsReSet", false);
    }

    if (DistanceToInit > MaxChaseDistance)
    {
        BlackboardComp->SetValueAsBool("IsReSet", true);
    }
    else
    {
        // 使用数据驱动的距离类型判断
        int32 RangeType = AIController->GetRangeTypeFromConfig(Distance);

        switch (RangeType)
        {
        case 0: // 近距离 - 攻击
            BlackboardComp->SetValueAsBool("IsAttack", true);
            BlackboardComp->SetValueAsBool("IsRunning", false);
            break;
        case 1: // 中距离 - 行走
            AICharacter->RunningMovement(false);
            BlackboardComp->SetValueAsBool("IsAttack", false);
            BlackboardComp->SetValueAsBool("IsRunning", false);
            break;
        case 2: // 远距离 - 奔跑追击
            BlackboardComp->SetValueAsBool("IsRunning", true);
            BlackboardComp->SetValueAsBool("IsAttack", false);
            break;
        }
    }
}