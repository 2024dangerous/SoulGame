// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "SoulCheatManager.generated.h"

/**
 * SoulCheatManager - 自定义作弊管理器
 * 
 * 提供运行时调试命令，可在控制台中使用
 * 
 * 使用方法：
 * 1. 在 PlayerController 中设置 CheatClass = USoulCheatManager::StaticClass()
 * 2. 在控制台中输入命令名即可执行
 * 
 * 命令列表：
 * - SoulGodMode: 切换无敌模式
 * - SoulOneHitKill: 切换一击必杀
 * - SoulKillAll: 杀死所有敌人
 * - SoulHealFull: 满血恢复
 * - SoulShowStats: 显示性能统计
 * - SoulShowPoolStats: 显示对象池统计
 * - SoulSetDamage <倍率>: 设置伤害倍率
 * - SoulDisableAI: 切换AI开关
 * - SoulTeleport <X> <Y> <Z>: 传送到指定位置
 */
UCLASS()
class SOULGAME_API USoulCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	// ============ 战斗作弊 ============

	/** 切换无敌模式 */
	UFUNCTION(Exec)
	void SoulGodMode();

	/** 切换一击必杀 */
	UFUNCTION(Exec)
	void SoulOneHitKill();

	/** 杀死所有敌人 */
	UFUNCTION(Exec)
	void SoulKillAll();

	/** 满血恢复 */
	UFUNCTION(Exec)
	void SoulHealFull();

	/** 设置伤害倍率 */
	UFUNCTION(Exec)
	void SoulSetDamage(float Multiplier);

	// ============ AI 作弊 ============

	/** 切换AI开关 */
	UFUNCTION(Exec)
	void SoulDisableAI();

	/** 设置AI检测距离倍率 */
	UFUNCTION(Exec)
	void SoulSetAIDetection(float Multiplier);

	// ============ 调试显示 ============

	/** 显示/隐藏性能统计 */
	UFUNCTION(Exec)
	void SoulShowStats();

	/** 显示/隐藏对象池统计 */
	UFUNCTION(Exec)
	void SoulShowPoolStats();

	/** 显示/隐藏AI调试信息 */
	UFUNCTION(Exec)
	void SoulShowAIDebug();

	/** 显示/隐藏战斗日志 */
	UFUNCTION(Exec)
	void SoulShowCombatLog();

	/** 切换调试绘制 */
	UFUNCTION(Exec)
	void SoulToggleDebugDraw();

	// ============ 工具命令 ============

	/** 传送到指定位置 */
	UFUNCTION(Exec)
	void SoulTeleport(float X, float Y, float Z);

	/** 设置游戏速度 */
	UFUNCTION(Exec)
	void SoulSetTimeScale(float Scale);

	/** 打印当前角色属性 */
	UFUNCTION(Exec)
	void SoulPrintAttributes();
};
