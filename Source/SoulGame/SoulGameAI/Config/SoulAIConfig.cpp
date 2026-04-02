// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameAI/Config/SoulAIConfig.h"

int32 USoulAIConfig::SelectAttack(float Distance, const FGameplayTagContainer& OwnerTags) const
{
	// 收集所有可用的攻击
	TArray<int32> ValidIndices;
	TArray<int32> ValidWeights;
	int32 TotalWeight = 0;

	for (int32 i = 0; i < AttackEntries.Num(); i++)
	{
		const FSoulAIAttackEntry& Entry = AttackEntries[i];

		// 检查距离
		if (Distance < Entry.MinRange || Distance > Entry.MaxRange)
		{
			continue;
		}

		// 检查必需标签
		if (Entry.RequiredTags.Num() > 0 && !OwnerTags.HasAll(Entry.RequiredTags))
		{
			continue;
		}

		// 检查阻止标签
		if (Entry.BlockedTags.Num() > 0 && OwnerTags.HasAny(Entry.BlockedTags))
		{
			continue;
		}

		ValidIndices.Add(i);
		ValidWeights.Add(Entry.Weight);
		TotalWeight += Entry.Weight;
	}

	if (ValidIndices.Num() == 0 || TotalWeight <= 0)
	{
		return -1;
	}

	// 加权随机选择
	int32 RandValue = FMath::RandRange(0, TotalWeight - 1);
	int32 AccumulatedWeight = 0;

	for (int32 i = 0; i < ValidIndices.Num(); i++)
	{
		AccumulatedWeight += ValidWeights[i];
		if (RandValue < AccumulatedWeight)
		{
			return ValidIndices[i];
		}
	}

	return ValidIndices.Last();
}

bool USoulAIConfig::ShouldObserve() const
{
	int32 AttackWeight = 100 - ObserveWeight;
	int32 RandValue = FMath::RandRange(0, 99);
	return RandValue < ObserveWeight;
}

int32 USoulAIConfig::GetRangeType(float Distance) const
{
	if (Distance <= RangeConfig.CloseRange)
	{
		return 0; // 近
	}
	if (Distance <= RangeConfig.MediumRange)
	{
		return 1; // 中
	}
	return 2; // 远
}
