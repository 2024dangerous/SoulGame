// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameAI/SoulAIController.h"
#include "SoulGameAI/Config/SoulAIConfig.h"
#include "SoulGameAI/SoulBaseEnemy.h"
#include "SoulGameTags/SoulGameplayTagInterface.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ASoulAIController::ASoulAIController()
{
	AIConfig = nullptr;
}

void ASoulAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 从 Pawn 上获取 AIConfig（如果尚未手动配置）
	if (!AIConfig)
	{
		// 尝试从 Enemy 获取
		if (ASoulBaseEnemy* Enemy = Cast<ASoulBaseEnemy>(InPawn))
		{
			AIConfig = Enemy->AIConfig;
		}
	}

	// 初始化行为树
	InitializeBehaviorTree();
}

void ASoulAIController::OnUnPossess()
{
	Super::OnUnPossess();
}

int32 ASoulAIController::SelectAttackFromConfig(float Distance)
{
	if (!AIConfig)
	{
		return -1;
	}

	// 获取拥有者的标签
	FGameplayTagContainer OwnerTags;
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		TScriptInterface<ISoulGameplayTagInterface> TagInterface(ControlledPawn);
		if (TagInterface)
		{
			OwnerTags = TagInterface->GetGameplayTagContainer();
		}
	}

	return AIConfig->SelectAttack(Distance, OwnerTags);
}

int32 ASoulAIController::GetRangeTypeFromConfig(float Distance)
{
	if (!AIConfig)
	{
		// 默认距离判断
		if (Distance <= 180.0f) return 0;
		if (Distance <= 500.0f) return 1;
		return 2;
	}

	return AIConfig->GetRangeType(Distance);
}

bool ASoulAIController::ShouldObserveFromConfig()
{
	if (!AIConfig)
	{
		return FMath::RandRange(0, 99) < 30; // 默认30%概率观察
	}

	return AIConfig->ShouldObserve();
}

void ASoulAIController::SetMovementSpeed(bool bIsRunning)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	ACharacter* OwnerCharacter = Cast<ACharacter>(ControlledPawn);
	if (!OwnerCharacter || !OwnerCharacter->GetCharacterMovement()) return;

	if (AIConfig)
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = bIsRunning ? AIConfig->MoveConfig.RunSpeed : AIConfig->MoveConfig.WalkSpeed;
	}
	else
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = bIsRunning ? 600.0f : 200.0f;
	}
}

void ASoulAIController::InitializeBehaviorTree()
{
	if (AIConfig && AIConfig->BehaviorTree)
	{
		RunBehaviorTree(AIConfig->BehaviorTree);
		UE_LOG(LogTemp, Log, TEXT("ASoulAIController::InitializeBehaviorTree - 成功加载行为树: %s"),
			*AIConfig->BehaviorTree->GetName());
	}
}