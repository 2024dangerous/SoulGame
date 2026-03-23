
#include "DebugTools.h"
#include "Engine/Engine.h"

// 定义日志类别
DEFINE_LOG_CATEGORY(ZhouXiaoPeng);

void FZhouXiaoPengDebugHelper::Print(const FString& Msg, FColor Color, float Duration)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Msg);
    }

}

void FZhouXiaoPengDebugHelper::Log(const FString& Msg)
{
    UE_LOG(ZhouXiaoPeng, Log, TEXT("%s"), *Msg);
}
