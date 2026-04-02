// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulObjectPool/SoulObjectPoolConfig.h"

TArray<FSoulPoolEntry> USoulObjectPoolConfig::GetPoolsForLevel(FName LevelName) const
{
	TArray<FSoulPoolEntry> Result;

	for (const FSoulPoolEntry& Entry : PoolEntries)
	{
		// 如果没有指定适用关卡，则适用于所有关卡
		if (Entry.ApplicableLevels.Num() == 0)
		{
			Result.Add(Entry);
			continue;
		}

		// 检查是否适用于当前关卡
		if (Entry.ApplicableLevels.Contains(LevelName))
		{
			Result.Add(Entry);
		}
	}

	return Result;
}
