// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameAI/Tasks/BT_Attack.h"
#include "SoulGameAI/SoulAIController.h"
#include "SoulGameAI/SoulBaseEnemy.h"
#include "SoulGameAI/Config/SoulAIConfig.h"
#include "SoulGameGAS/SoulAbilitySystemComponent.h"
#include "SoulGameGAS/SoulGameplayAbility.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SoulGameData/SoulEnumType.h"

EBTNodeResult::Type UBT_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ASoulAIController* AIController = Cast<ASoulAIController>(OwnerComp.GetAIOwner());
    if (!AIController) return EBTNodeResult::Failed;

    ASoulBaseEnemy* AICharacter = Cast<ASoulBaseEnemy>(AIController->GetPawn());
    if (!AICharacter) return EBTNodeResult::Failed;

    UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
    if (!BlackboardComp) return EBTNodeResult::Failed;

    // 获取与目标的距离
    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("AttackTarget"));
    float Distance = TargetActor ? AICharacter->GetDistanceTo(TargetActor) : 0.0f;

    // 判断是否应该观察（数据驱动）
    if (AIController->ShouldObserveFromConfig())
    {
        // 进入观察模式
        BlackboardComp->SetValueAsBool("IsObserver", true);
        return EBTNodeResult::Succeeded;
    }

    // 尝试使用 GAS 技能攻击
    USoulAbilitySystemComponent* ASC = AICharacter->GetSoulAbilitySystemComponent();
    USoulAIConfig* Config = AIController->GetAIConfig();

    if (Config && ASC)
    {
        // 从配置中选择攻击
        int32 AttackIndex = AIController->SelectAttackFromConfig(Distance);
        if (AttackIndex >= 0 && AttackIndex < Config->AttackEntries.Num())
        {
            const FSoulAIAttackEntry& AttackEntry = Config->AttackEntries[AttackIndex];

            // 优先使用 GAS 技能
            if (AttackEntry.AbilityClass)
            {
                if (ASC->TryActivateAbilityByClassSoul(AttackEntry.AbilityClass))
                {
                    BlackboardComp->SetValueAsBool("IsAttacking", true);
                    BlackboardComp->SetValueAsBool("IsAttack", false);
                    return EBTNodeResult::Succeeded;
                }
            }
        }
    }

    // 回退到旧逻辑（兼容）
    AICharacter->EnemyBehavior = EEnemyBehavior::MELEEATTACK;
    AICharacter->Attack();
    return EBTNodeResult::Succeeded;
}