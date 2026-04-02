// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_DistanceCheck.generated.h"

/**
 * BTS_DistanceCheck - 距离检测服务（数据驱动版）
 * 
 * 从 SoulAIConfig 读取距离阈值
 * 自动设置 Blackboard 的 IsAttack/IsRunning/IsReSet 值
 */
UCLASS()
class SOULGAME_API UBTS_DistanceCheck : public UBTService
{
	GENERATED_BODY()
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
