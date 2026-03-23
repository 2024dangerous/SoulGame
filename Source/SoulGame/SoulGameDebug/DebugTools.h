// DebugHelper.h
#pragma once

#include "CoreMinimal.h"

// 自定义日志类别（可以在 .cpp 中实现）
DECLARE_LOG_CATEGORY_EXTERN(ZhouXiaoPeng, Log, All);

#define ZhouXiaoPeng_PRINT(MSG) FZhouXiaoPengDebugHelper::Print(MSG)
#define ZhouXiaoPeng_LOG(MSG)   FZhouXiaoPengDebugHelper::Log(MSG)


// Debug 辅助类
class FZhouXiaoPengDebugHelper
{
public:
    // 打印字符串（默认输出到屏幕和日志）
    static void Print(const FString& Msg, FColor Color = FColor::Cyan, float Duration = 15.0f);

    // 输出日志信息（仅输出到日志）
    static void Log(const FString& Msg);


    // 更多自定义 Debug 接口可添加……
};