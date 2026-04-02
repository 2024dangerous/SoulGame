// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameDebug/SoulGameCVars.h"

// ============ 调试相关 ============

TAutoConsoleVariable<int32> FSoulGameCVars::CVarShowCombatLog(
	TEXT("soul.debug.ShowCombatLog"),
	0,
	TEXT("是否显示战斗日志 (0=关闭, 1=开启)"),
	ECVF_Default
);

TAutoConsoleVariable<int32> FSoulGameCVars::CVarShowAIDebug(
	TEXT("soul.debug.ShowAIDebug"),
	0,
	TEXT("是否显示AI调试信息 (0=关闭, 1=开启)"),
	ECVF_Default
);

TAutoConsoleVariable<int32> FSoulGameCVars::CVarShowPoolStats(
	TEXT("soul.debug.ShowPoolStats"),
	0,
	TEXT("是否显示对象池统计 (0=关闭, 1=开启)"),
	ECVF_Default
);

TAutoConsoleVariable<int32> FSoulGameCVars::CVarShowPerfStats(
	TEXT("soul.debug.ShowPerfStats"),
	0,
	TEXT("是否显示性能统计 (0=关闭, 1=开启)"),
	ECVF_Default
);

TAutoConsoleVariable<int32> FSoulGameCVars::CVarEnableDebugDraw(
	TEXT("soul.debug.EnableDebugDraw"),
	0,
	TEXT("是否启用调试绘制 (0=关闭, 1=开启)"),
	ECVF_Default
);

TAutoConsoleVariable<float> FSoulGameCVars::CVarDebugDrawDuration(
	TEXT("soul.debug.DrawDuration"),
	3.0f,
	TEXT("调试绘制持续时间（秒）"),
	ECVF_Default
);

// ============ 战斗相关 ============

TAutoConsoleVariable<float> FSoulGameCVars::CVarDamageMultiplier(
	TEXT("soul.combat.DamageMultiplier"),
	1.0f,
	TEXT("伤害倍率（调试用，默认1.0）"),
	ECVF_Cheat
);

TAutoConsoleVariable<int32> FSoulGameCVars::CVarGodMode(
	TEXT("soul.combat.GodMode"),
	0,
	TEXT("无敌模式 (0=关闭, 1=开启)"),
	ECVF_Cheat
);

TAutoConsoleVariable<int32> FSoulGameCVars::CVarOneHitKill(
	TEXT("soul.combat.OneHitKill"),
	0,
	TEXT("一击必杀 (0=关闭, 1=开启)"),
	ECVF_Cheat
);

// ============ AI 相关 ============

TAutoConsoleVariable<float> FSoulGameCVars::CVarAIDetectionMultiplier(
	TEXT("soul.ai.DetectionMultiplier"),
	1.0f,
	TEXT("AI检测距离倍率（默认1.0）"),
	ECVF_Default
);

TAutoConsoleVariable<int32> FSoulGameCVars::CVarDisableAI(
	TEXT("soul.ai.DisableAI"),
	0,
	TEXT("禁用AI (0=正常, 1=禁用)"),
	ECVF_Cheat
);

TAutoConsoleVariable<float> FSoulGameCVars::CVarAIUpdateRate(
	TEXT("soul.ai.UpdateRate"),
	0.1f,
	TEXT("AI更新频率（秒，默认0.1）"),
	ECVF_Default
);

// ============ 性能相关 ============

TAutoConsoleVariable<float> FSoulGameCVars::CVarLODCheckInterval(
	TEXT("soul.perf.LODCheckInterval"),
	0.5f,
	TEXT("LOD检查间隔（秒，默认0.5）"),
	ECVF_Default
);

TAutoConsoleVariable<int32> FSoulGameCVars::CVarMaxVisibleEnemies(
	TEXT("soul.perf.MaxVisibleEnemies"),
	20,
	TEXT("最大同屏敌人数量（默认20）"),
	ECVF_Default
);

TAutoConsoleVariable<float> FSoulGameCVars::CVarPoolShrinkInterval(
	TEXT("soul.perf.PoolShrinkInterval"),
	60.0f,
	TEXT("对象池自动缩容间隔（秒，默认60）"),
	ECVF_Default
);
