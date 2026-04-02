// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/IConsoleManager.h"

/**
 * SoulGameCVars - 控制台变量集合
 * 
 * 所有 SoulGame 的控制台变量统一在此定义
 * 可在运行时通过控制台命令修改
 * 
 * 使用方法：
 * - 控制台输入: soul.debug.ShowCombatLog 1
 * - C++ 代码: FSoulGameCVars::CVarShowCombatLog->GetBool()
 */
struct SOULGAME_API FSoulGameCVars
{
	// ============ 调试相关 ============

	// 是否显示战斗日志
	static TAutoConsoleVariable<int32> CVarShowCombatLog;

	// 是否显示 AI 调试信息
	static TAutoConsoleVariable<int32> CVarShowAIDebug;

	// 是否显示对象池统计
	static TAutoConsoleVariable<int32> CVarShowPoolStats;

	// 是否显示性能统计
	static TAutoConsoleVariable<int32> CVarShowPerfStats;

	// 是否启用调试绘制
	static TAutoConsoleVariable<int32> CVarEnableDebugDraw;

	// 调试绘制持续时间
	static TAutoConsoleVariable<float> CVarDebugDrawDuration;

	// ============ 战斗相关 ============

	// 伤害倍率（调试用）
	static TAutoConsoleVariable<float> CVarDamageMultiplier;

	// 是否启用无敌模式
	static TAutoConsoleVariable<int32> CVarGodMode;

	// 是否启用一击必杀
	static TAutoConsoleVariable<int32> CVarOneHitKill;

	// ============ AI 相关 ============

	// AI 检测距离倍率
	static TAutoConsoleVariable<float> CVarAIDetectionMultiplier;

	// 是否禁用 AI
	static TAutoConsoleVariable<int32> CVarDisableAI;

	// AI 更新频率（秒）
	static TAutoConsoleVariable<float> CVarAIUpdateRate;

	// ============ 性能相关 ============

	// LOD 检查间隔
	static TAutoConsoleVariable<float> CVarLODCheckInterval;

	// 最大同屏敌人数量
	static TAutoConsoleVariable<int32> CVarMaxVisibleEnemies;

	// 对象池自动缩容间隔
	static TAutoConsoleVariable<float> CVarPoolShrinkInterval;

	// ============ 便捷访问方法 ============

	static bool ShowCombatLog() { return CVarShowCombatLog.GetValueOnGameThread() != 0; }
	static bool ShowAIDebug() { return CVarShowAIDebug.GetValueOnGameThread() != 0; }
	static bool ShowPoolStats() { return CVarShowPoolStats.GetValueOnGameThread() != 0; }
	static bool ShowPerfStats() { return CVarShowPerfStats.GetValueOnGameThread() != 0; }
	static bool EnableDebugDraw() { return CVarEnableDebugDraw.GetValueOnGameThread() != 0; }
	static float DebugDrawDuration() { return CVarDebugDrawDuration.GetValueOnGameThread(); }

	static float DamageMultiplier() { return CVarDamageMultiplier.GetValueOnGameThread(); }
	static bool GodMode() { return CVarGodMode.GetValueOnGameThread() != 0; }
	static bool OneHitKill() { return CVarOneHitKill.GetValueOnGameThread() != 0; }

	static float AIDetectionMultiplier() { return CVarAIDetectionMultiplier.GetValueOnGameThread(); }
	static bool DisableAI() { return CVarDisableAI.GetValueOnGameThread() != 0; }
	static float AIUpdateRate() { return CVarAIUpdateRate.GetValueOnGameThread(); }

	static float LODCheckInterval() { return CVarLODCheckInterval.GetValueOnGameThread(); }
	static int32 MaxVisibleEnemies() { return CVarMaxVisibleEnemies.GetValueOnGameThread(); }
	static float PoolShrinkInterval() { return CVarPoolShrinkInterval.GetValueOnGameThread(); }
};
