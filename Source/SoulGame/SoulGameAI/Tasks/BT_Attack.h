// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_Attack.generated.h"

/**
 * BT_Attack - AI 攻击任务（数据驱动版）
 * 
 * 从 SoulAIConfig 读取攻击配置
 * 支持：
 * - 加权随机攻击选择
 * - 观察行为（绕行）
 * - GAS 技能激活
 */
UCLASS()
class SOULGAME_API UBT_Attack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
};
