// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameData/SoulGameplayConfig.h"

USoulGameplayConfig::USoulGameplayConfig()
{
	ConfigId = FName(TEXT("Gameplay"));
	ConfigVersion = 1;
	ConfigDescription = FText::FromString(TEXT("Gameplay Configuration"));

	// 玩家属性默认值
	PlayerAttributes.MaxHealth = 100.0f;
	PlayerAttributes.MaxMana = 50.0f;
	PlayerAttributes.MaxStamina = 100.0f;
	PlayerAttributes.HealthRegenRate = 1.0f;
	PlayerAttributes.StaminaRegenRate = 10.0f;

	// 战斗默认值
	Combat.LockOnMaxDistance = 1000.0f;
	Combat.DodgeStaminaCost = 20.0f;
	Combat.DefenseStaminaCost = 5.0f;
	Combat.AttackStaminaCost = 10.0f;
	Combat.HitStunDuration = 0.5f;
	Combat.DeathDuration = 3.0f;

	// 拾取默认值
	Pickup.PickupRadius = 200.0f;
	Pickup.PickupCooldown = 0.5f;
	Pickup.MaxPickupDistance = 300.0f;

	// AI默认值
	AI.PerceptionRadius = 500.0f;
	AI.AttackRange = 200.0f;
	AI.ChaseRange = 1000.0f;
	AI.PatrolRange = 500.0f;

	bIsLoaded = true;
}

bool USoulGameplayConfig::ValidateConfig(FText& OutError)
{
	// 验证玩家属性
	if (PlayerAttributes.MaxHealth <= 0)
	{
		OutError = FText::FromString(TEXT("MaxHealth must be greater than 0"));
		return false;
	}
	if (PlayerAttributes.MaxStamina <= 0)
	{
		OutError = FText::FromString(TEXT("MaxStamina must be greater than 0"));
		return false;
	}

	// 验证战斗配置
	if (Combat.LockOnMaxDistance <= 0)
	{
		OutError = FText::FromString(TEXT("LockOnMaxDistance must be greater than 0"));
		return false;
	}
	if (Combat.DodgeStaminaCost < 0)
	{
		OutError = FText::FromString(TEXT("DodgeStaminaCost cannot be negative"));
		return false;
	}

	// 验证拾取配置
	if (Pickup.PickupRadius <= 0)
	{
		OutError = FText::FromString(TEXT("PickupRadius must be greater than 0"));
		return false;
	}
	if (Pickup.MaxPickupDistance <= 0)
	{
		OutError = FText::FromString(TEXT("MaxPickupDistance must be greater than 0"));
		return false;
	}

	// 验证AI配置
	if (AI.PerceptionRadius <= 0)
	{
		OutError = FText::FromString(TEXT("PerceptionRadius must be greater than 0"));
		return false;
	}
	if (AI.AttackRange <= 0)
	{
		OutError = FText::FromString(TEXT("AttackRange must be greater than 0"));
		return false;
	}

	return true;
}