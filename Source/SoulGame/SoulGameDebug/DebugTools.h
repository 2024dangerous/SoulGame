// DebugHelper.h
#pragma once

#include "CoreMinimal.h"
#include "SoulGameDebug/SoulDebugManager.h"

// 自定义日志类别（可以在 .cpp 中实现）
DECLARE_LOG_CATEGORY_EXTERN(ZhouXiaoPeng, Log, All);

// 旧宏保持兼容，内部桥接到新系统
#define ZhouXiaoPeng_PRINT(MSG) FZhouXiaoPengDebugHelper::Print(MSG)
#define ZhouXiaoPeng_LOG(MSG)   FZhouXiaoPengDebugHelper::Log(MSG)

// 推荐使用新宏
// SOUL_LOG("格式化消息 %s", *变量)
// SOUL_WARNING("警告消息")
// SOUL_ERROR("错误消息")
// SOUL_PRINT("屏幕消息")
// SOUL_LOG_CATEGORY(Combat, "战斗消息")
// SOUL_SCOPED_TIMER(TimerName)

// Debug 辅助类（保持旧接口兼容）
class FZhouXiaoPengDebugHelper
{
public:
    // 打印字符串（默认输出到屏幕和日志）
    static void Print(const FString& Msg, FColor Color = FColor::Cyan, float Duration = 15.0f);

    // 输出日志信息（仅输出到日志）
    static void Log(const FString& Msg);
};