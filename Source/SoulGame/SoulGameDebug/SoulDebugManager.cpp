// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameDebug/SoulDebugManager.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

// 定义日志类别
DEFINE_LOG_CATEGORY(LogSoulGame);
DEFINE_LOG_CATEGORY(LogSoulCombat);
DEFINE_LOG_CATEGORY(LogSoulAI);
DEFINE_LOG_CATEGORY(LogSoulPool);
DEFINE_LOG_CATEGORY(LogSoulPerf);

USoulDebugManager::USoulDebugManager()
{
	bGlobalDebugEnabled = true;

	// 默认所有类别启用
	CategoryEnabled.Add(ESoulDebugCategory::General, true);
	CategoryEnabled.Add(ESoulDebugCategory::Combat, true);
	CategoryEnabled.Add(ESoulDebugCategory::AI, true);
	CategoryEnabled.Add(ESoulDebugCategory::Movement, true);
	CategoryEnabled.Add(ESoulDebugCategory::Pool, true);
	CategoryEnabled.Add(ESoulDebugCategory::Loading, true);
	CategoryEnabled.Add(ESoulDebugCategory::Animation, true);
	CategoryEnabled.Add(ESoulDebugCategory::UI, true);
	CategoryEnabled.Add(ESoulDebugCategory::Performance, true);
}

// ============ 日志接口 ============

void USoulDebugManager::SoulLog(ESoulDebugCategory Category, ESoulDebugLevel Level, const FString& Message)
{
	FString Prefix;
	switch (Category)
	{
	case ESoulDebugCategory::General:		Prefix = TEXT("[General]"); break;
	case ESoulDebugCategory::Combat:		Prefix = TEXT("[Combat]"); break;
	case ESoulDebugCategory::AI:			Prefix = TEXT("[AI]"); break;
	case ESoulDebugCategory::Movement:		Prefix = TEXT("[Movement]"); break;
	case ESoulDebugCategory::Pool:			Prefix = TEXT("[Pool]"); break;
	case ESoulDebugCategory::Loading:		Prefix = TEXT("[Loading]"); break;
	case ESoulDebugCategory::Animation:		Prefix = TEXT("[Animation]"); break;
	case ESoulDebugCategory::UI:			Prefix = TEXT("[UI]"); break;
	case ESoulDebugCategory::Performance:	Prefix = TEXT("[Perf]"); break;
	}

	FString FullMessage = FString::Printf(TEXT("%s %s"), *Prefix, *Message);

	FLogCategoryBase& LogCategory = GetLogCategory(Category);

	switch (Level)
	{
	case ESoulDebugLevel::Verbose:
		UE_LOG(LogSoulGame, Verbose, TEXT("%s"), *FullMessage);
		break;
	case ESoulDebugLevel::Log:
		UE_LOG(LogSoulGame, Log, TEXT("%s"), *FullMessage);
		break;
	case ESoulDebugLevel::Warning:
		UE_LOG(LogSoulGame, Warning, TEXT("%s"), *FullMessage);
		break;
	case ESoulDebugLevel::Error:
		UE_LOG(LogSoulGame, Error, TEXT("%s"), *FullMessage);
		break;
	}
}

void USoulDebugManager::Log(const FString& Message)
{
	UE_LOG(LogSoulGame, Log, TEXT("%s"), *Message);
}

void USoulDebugManager::LogWarning(const FString& Message)
{
	UE_LOG(LogSoulGame, Warning, TEXT("%s"), *Message);
}

void USoulDebugManager::LogError(const FString& Message)
{
	UE_LOG(LogSoulGame, Error, TEXT("%s"), *Message);
}

// ============ 屏幕调试信息 ============

void USoulDebugManager::PrintScreen(const FString& Message, FColor Color, float Duration)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}

void USoulDebugManager::PrintScreenKeyed(int32 Key, const FString& Message, FColor Color, float Duration)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(Key, Duration, Color, Message);
	}
}

// ============ 调试绘制 ============

void USoulDebugManager::DrawDebugLineWorld(UObject* WorldContext, FVector Start, FVector End, FColor Color, float Duration, float Thickness)
{
	if (!WorldContext) return;
	UWorld* World = WorldContext->GetWorld();
	if (!World) return;

	DrawDebugLine(World, Start, End, Color, false, Duration, 0, Thickness);
}

void USoulDebugManager::DrawDebugSphereWorld(UObject* WorldContext, FVector Center, float Radius, FColor Color, float Duration)
{
	if (!WorldContext) return;
	UWorld* World = WorldContext->GetWorld();
	if (!World) return;

	DrawDebugSphere(World, Center, Radius, 16, Color, false, Duration);
}

void USoulDebugManager::DrawDebugBoxWorld(UObject* WorldContext, FVector Center, FVector Extent, FColor Color, float Duration)
{
	if (!WorldContext) return;
	UWorld* World = WorldContext->GetWorld();
	if (!World) return;

	DrawDebugBox(World, Center, Extent, Color, false, Duration);
}

void USoulDebugManager::DrawDebugStringWorld(UObject* WorldContext, FVector Location, const FString& Text, FColor Color, float Duration)
{
	if (!WorldContext) return;
	UWorld* World = WorldContext->GetWorld();
	if (!World) return;

	DrawDebugString(World, Location, Text, nullptr, Color, Duration);
}

// ============ 性能计时器 ============

void USoulDebugManager::StartTimer(FName TimerName)
{
	TimerStartTimes.Add(TimerName, FPlatformTime::Seconds());
}

float USoulDebugManager::StopTimer(FName TimerName, bool bLogResult)
{
	double* StartTime = TimerStartTimes.Find(TimerName);
	if (!StartTime)
	{
		UE_LOG(LogSoulPerf, Warning, TEXT("Timer '%s' was not started."), *TimerName.ToString());
		return -1.0f;
	}

	double Elapsed = (FPlatformTime::Seconds() - *StartTime) * 1000.0; // 转换为毫秒
	float ElapsedMs = static_cast<float>(Elapsed);

	TimerResults.Add(TimerName, ElapsedMs);
	TimerStartTimes.Remove(TimerName);

	if (bLogResult)
	{
		UE_LOG(LogSoulPerf, Log, TEXT("[Timer] %s: %.3f ms"), *TimerName.ToString(), ElapsedMs);
	}

	return ElapsedMs;
}

TMap<FName, float> USoulDebugManager::GetAllTimerResults() const
{
	return TimerResults;
}

// ============ 开关控制 ============

void USoulDebugManager::SetCategoryEnabled(ESoulDebugCategory Category, bool bEnabled)
{
	CategoryEnabled.Add(Category, bEnabled);
}

bool USoulDebugManager::IsCategoryEnabled(ESoulDebugCategory Category) const
{
	if (const bool* bEnabled = CategoryEnabled.Find(Category))
	{
		return *bEnabled;
	}
	return true;
}

void USoulDebugManager::SetDebugEnabled(bool bEnabled)
{
	bGlobalDebugEnabled = bEnabled;
	UE_LOG(LogSoulGame, Log, TEXT("USoulDebugManager - 全局调试 %s"), bEnabled ? TEXT("启用") : TEXT("禁用"));
}

FLogCategoryBase& USoulDebugManager::GetLogCategory(ESoulDebugCategory Category)
{
	switch (Category)
	{
	case ESoulDebugCategory::Combat:		return LogSoulCombat;
	case ESoulDebugCategory::AI:			return LogSoulAI;
	case ESoulDebugCategory::Pool:			return LogSoulPool;
	case ESoulDebugCategory::Performance:	return LogSoulPerf;
	default:								return LogSoulGame;
	}
}
