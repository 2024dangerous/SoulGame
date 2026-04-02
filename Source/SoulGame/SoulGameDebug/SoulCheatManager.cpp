// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameDebug/SoulCheatManager.h"
#include "SoulGameDebug/SoulGameCVars.h"
#include "SoulGameDebug/SoulDebugManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

// ============ 战斗作弊 ============

void USoulCheatManager::SoulGodMode()
{
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("soul.combat.GodMode"));
	if (CVar)
	{
		int32 Current = CVar->GetInt();
		CVar->Set(Current == 0 ? 1 : 0);
		USoulDebugManager::PrintScreen(
			FString::Printf(TEXT("无敌模式: %s"), Current == 0 ? TEXT("开启") : TEXT("关闭")),
			Current == 0 ? FColor::Green : FColor::Red
		);
	}
}

void USoulCheatManager::SoulOneHitKill()
{
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("soul.combat.OneHitKill"));
	if (CVar)
	{
		int32 Current = CVar->GetInt();
		CVar->Set(Current == 0 ? 1 : 0);
		USoulDebugManager::PrintScreen(
			FString::Printf(TEXT("一击必杀: %s"), Current == 0 ? TEXT("开启") : TEXT("关闭")),
			Current == 0 ? FColor::Green : FColor::Red
		);
	}
}

void USoulCheatManager::SoulKillAll()
{
	UWorld* World = GetWorld();
	if (!World) return;

	int32 KillCount = 0;

	// 遍历所有 Pawn，杀死非玩家的
	for (TActorIterator<APawn> It(World); It; ++It)
	{
		APawn* Pawn = *It;
		if (Pawn && !Pawn->IsPlayerControlled())
		{
			Pawn->Destroy();
			KillCount++;
		}
	}

	USoulDebugManager::PrintScreen(
		FString::Printf(TEXT("杀死了 %d 个敌人"), KillCount),
		FColor::Red
	);
}

void USoulCheatManager::SoulHealFull()
{
	APlayerController* PC = GetOuterAPlayerController();
	if (!PC) return;

	ACharacter* Character = PC->GetCharacter();
	if (!Character) return;

	// 通过 GAS 恢复属性值（如果有的话）
	// 这里先打印提示，具体恢复逻辑需要依赖 AttributeSet 实现
	USoulDebugManager::PrintScreen(TEXT("满血恢复"), FColor::Green);

	UE_LOG(LogSoulGame, Log, TEXT("SoulHealFull - 角色满血恢复"));
}

void USoulCheatManager::SoulSetDamage(float Multiplier)
{
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("soul.combat.DamageMultiplier"));
	if (CVar)
	{
		CVar->Set(Multiplier);
		USoulDebugManager::PrintScreen(
			FString::Printf(TEXT("伤害倍率设置为: %.2f"), Multiplier),
			FColor::Yellow
		);
	}
}

// ============ AI 调试 ============

void USoulCheatManager::SoulDisableAI()
{
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("soul.ai.DisableAI"));
	if (CVar)
	{
		int32 Current = CVar->GetInt();
		CVar->Set(Current == 0 ? 1 : 0);
		USoulDebugManager::PrintScreen(
			FString::Printf(TEXT("AI: %s"), Current == 0 ? TEXT("禁用") : TEXT("启用")),
			Current == 0 ? FColor::Red : FColor::Green
		);
	}
}

void USoulCheatManager::SoulSetAIDetection(float Multiplier)
{
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("soul.ai.DetectionMultiplier"));
	if (CVar)
	{
		CVar->Set(Multiplier);
		USoulDebugManager::PrintScreen(
			FString::Printf(TEXT("AI感知距离倍率: %.2f"), Multiplier),
			FColor::Yellow
		);
	}
}

// ============ 调试显示 ============

void USoulCheatManager::SoulShowStats()
{
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("soul.debug.ShowPerfStats"));
	if (CVar)
	{
		int32 Current = CVar->GetInt();
		CVar->Set(Current == 0 ? 1 : 0);
		USoulDebugManager::PrintScreen(
			FString::Printf(TEXT("性能统计: %s"), Current == 0 ? TEXT("显示") : TEXT("隐藏")),
			FColor::Cyan
		);
	}
}

void USoulCheatManager::SoulShowPoolStats()
{
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("soul.debug.ShowPoolStats"));
	if (CVar)
	{
		int32 Current = CVar->GetInt();
		CVar->Set(Current == 0 ? 1 : 0);
		USoulDebugManager::PrintScreen(
			FString::Printf(TEXT("对象池统计: %s"), Current == 0 ? TEXT("显示") : TEXT("隐藏")),
			FColor::Cyan
		);
	}
}

void USoulCheatManager::SoulShowAIDebug()
{
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("soul.debug.ShowAIDebug"));
	if (CVar)
	{
		int32 Current = CVar->GetInt();
		CVar->Set(Current == 0 ? 1 : 0);
		USoulDebugManager::PrintScreen(
			FString::Printf(TEXT("AI调试: %s"), Current == 0 ? TEXT("显示") : TEXT("隐藏")),
			FColor::Cyan
		);
	}
}

void USoulCheatManager::SoulShowCombatLog()
{
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("soul.debug.ShowCombatLog"));
	if (CVar)
	{
		int32 Current = CVar->GetInt();
		CVar->Set(Current == 0 ? 1 : 0);
		USoulDebugManager::PrintScreen(
			FString::Printf(TEXT("战斗日志: %s"), Current == 0 ? TEXT("显示") : TEXT("隐藏")),
			FColor::Cyan
		);
	}
}

void USoulCheatManager::SoulToggleDebugDraw()
{
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("soul.debug.EnableDebugDraw"));
	if (CVar)
	{
		int32 Current = CVar->GetInt();
		CVar->Set(Current == 0 ? 1 : 0);
		USoulDebugManager::PrintScreen(
			FString::Printf(TEXT("调试绘制: %s"), Current == 0 ? TEXT("开启") : TEXT("关闭")),
			FColor::Cyan
		);
	}
}

// ============ 其他功能 ============

void USoulCheatManager::SoulTeleport(float X, float Y, float Z)
{
	APlayerController* PC = GetOuterAPlayerController();
	if (!PC) return;

	ACharacter* Character = PC->GetCharacter();
	if (!Character) return;

	FVector NewLocation(X, Y, Z);
	Character->SetActorLocation(NewLocation);

	USoulDebugManager::PrintScreen(
		FString::Printf(TEXT("传送到: (%.0f, %.0f, %.0f)"), X, Y, Z),
		FColor::Magenta
	);
}

void USoulCheatManager::SoulSetTimeScale(float Scale)
{
	Scale = FMath::Clamp(Scale, 0.01f, 10.0f);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), Scale);

	USoulDebugManager::PrintScreen(
		FString::Printf(TEXT("游戏速度: %.2fx"), Scale),
		FColor::Yellow
	);
}

void USoulCheatManager::SoulPrintAttributes()
{
	APlayerController* PC = GetOuterAPlayerController();
	if (!PC) return;

	ACharacter* Character = PC->GetCharacter();
	if (!Character) return;

	USoulDebugManager::PrintScreen(TEXT("========== 角色属性 =========="), FColor::White);
	USoulDebugManager::PrintScreen(
		FString::Printf(TEXT("位置: %s"), *Character->GetActorLocation().ToString()),
		FColor::White
	);
	USoulDebugManager::PrintScreen(
		FString::Printf(TEXT("旋转: %s"), *Character->GetActorRotation().ToString()),
		FColor::White
	);

	if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
	{
		USoulDebugManager::PrintScreen(
			FString::Printf(TEXT("速度: %.1f / 最大: %.1f"), MoveComp->Velocity.Size(), MoveComp->MaxWalkSpeed),
			FColor::White
		);
		USoulDebugManager::PrintScreen(
			FString::Printf(TEXT("在地面: %s | 坠落: %s"),
				MoveComp->IsMovingOnGround() ? TEXT("是") : TEXT("否"),
				MoveComp->IsFalling() ? TEXT("是") : TEXT("否")),
			FColor::White
		);
	}

	USoulDebugManager::PrintScreen(TEXT("================================"), FColor::White);

	UE_LOG(LogSoulGame, Log, TEXT("SoulPrintAttributes - 角色属性已打印到屏幕"));
}
