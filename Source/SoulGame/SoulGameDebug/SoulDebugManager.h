// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoulDebugManager.generated.h"

// 自定义日志类别
DECLARE_LOG_CATEGORY_EXTERN(LogSoulGame, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSoulCombat, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSoulAI, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSoulPool, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSoulPerf, Log, All);

/**
 * 调试类别
 */
UENUM(BlueprintType)
enum class ESoulDebugCategory : uint8
{
	General		UMETA(DisplayName = "通用"),
	Combat		UMETA(DisplayName = "战斗"),
	AI			UMETA(DisplayName = "AI"),
	Movement	UMETA(DisplayName = "移动"),
	Pool		UMETA(DisplayName = "对象池"),
	Loading		UMETA(DisplayName = "资源加载"),
	Animation	UMETA(DisplayName = "动画"),
	UI			UMETA(DisplayName = "UI"),
	Performance	UMETA(DisplayName = "性能")
};

/**
 * 调试日志级别
 */
UENUM(BlueprintType)
enum class ESoulDebugLevel : uint8
{
	Verbose		UMETA(DisplayName = "详细"),
	Log			UMETA(DisplayName = "日志"),
	Warning		UMETA(DisplayName = "警告"),
	Error		UMETA(DisplayName = "错误")
};

/**
 * 屏幕调试信息条目
 */
USTRUCT(BlueprintType)
struct FSoulDebugScreenEntry
{
	GENERATED_BODY()

	UPROPERTY()
	FString Message;

	UPROPERTY()
	FColor Color = FColor::White;

	UPROPERTY()
	float Duration = 5.0f;

	UPROPERTY()
	float TimeAdded = 0.0f;
};

/**
 * SoulDebugManager - 统一调试管理器
 * 
 * 功能：
 * - 分类日志系统（战斗、AI、对象池等）
 * - 屏幕调试信息管理
 * - 调试绘制（线条、球体、盒子等）
 * - 性能计时器
 * - 运行时开关控制
 * 
 * 使用方法：
 * - 通过静态方法直接调用
 * - 或通过 GameInstance 获取实例
 */
UCLASS(BlueprintType)
class SOULGAME_API USoulDebugManager : public UObject
{
	GENERATED_BODY()

public:
	USoulDebugManager();

	// ============ 日志接口 ============

	/**
	 * 输出分类日志
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Debug")
	static void SoulLog(ESoulDebugCategory Category, ESoulDebugLevel Level, const FString& Message);

	/**
	 * 输出通用日志（快捷方法）
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Debug")
	static void Log(const FString& Message);

	/**
	 * 输出警告日志
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Debug")
	static void LogWarning(const FString& Message);

	/**
	 * 输出错误日志
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Debug")
	static void LogError(const FString& Message);

	// ============ 屏幕调试信息 ============

	/**
	 * 在屏幕上显示调试信息
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Debug")
	static void PrintScreen(const FString& Message, FColor Color = FColor::Cyan, float Duration = 5.0f);

	/**
	 * 在屏幕上显示带键值的调试信息（同一 Key 会覆盖）
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Debug")
	static void PrintScreenKeyed(int32 Key, const FString& Message, FColor Color = FColor::Cyan, float Duration = 5.0f);

	// ============ 调试绘制 ============

	/**
	 * 绘制调试线条
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Debug|Draw")
	static void DrawDebugLineWorld(UObject* WorldContext, FVector Start, FVector End, FColor Color = FColor::Green, float Duration = 3.0f, float Thickness = 1.0f);

	/**
	 * 绘制调试球体
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Debug|Draw")
	static void DrawDebugSphereWorld(UObject* WorldContext, FVector Center, float Radius = 50.0f, FColor Color = FColor::Red, float Duration = 3.0f);

	/**
	 * 绘制调试盒子
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Debug|Draw")
	static void DrawDebugBoxWorld(UObject* WorldContext, FVector Center, FVector Extent, FColor Color = FColor::Blue, float Duration = 3.0f);

	/**
	 * 在世界中绘制调试文字
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Debug|Draw")
	static void DrawDebugStringWorld(UObject* WorldContext, FVector Location, const FString& Text, FColor Color = FColor::White, float Duration = 3.0f);

	// ============ 性能计时器 ============

	/**
	 * 开始计时
	 * @param TimerName 计时器名称
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Debug|Perf")
	void StartTimer(FName TimerName);

	/**
	 * 停止计时并返回耗时（毫秒）
	 * @param TimerName 计时器名称
	 * @param bLogResult 是否输出到日志
	 * @return 耗时（毫秒）
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Debug|Perf")
	float StopTimer(FName TimerName, bool bLogResult = true);

	/**
	 * 获取所有计时器结果
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Debug|Perf")
	TMap<FName, float> GetAllTimerResults() const;

	// ============ 开关控制 ============

	/**
	 * 设置调试类别是否启用
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Debug")
	void SetCategoryEnabled(ESoulDebugCategory Category, bool bEnabled);

	/**
	 * 检查调试类别是否启用
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Debug")
	bool IsCategoryEnabled(ESoulDebugCategory Category) const;

	/**
	 * 启用/禁用所有调试输出
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Debug")
	void SetDebugEnabled(bool bEnabled);

	/**
	 * 全局调试是否启用
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Debug")
	bool IsDebugEnabled() const { return bGlobalDebugEnabled; }

private:
	// 全局调试开关
	bool bGlobalDebugEnabled;

	// 各类别开关
	TMap<ESoulDebugCategory, bool> CategoryEnabled;

	// 计时器起始时间
	TMap<FName, double> TimerStartTimes;

	// 计时器结果
	TMap<FName, float> TimerResults;

	// 获取类别对应的日志类别
	static FLogCategoryBase& GetLogCategory(ESoulDebugCategory Category);
};

// ============ 便捷宏定义 ============

// 通用日志
#define SOUL_LOG(Format, ...) \
	USoulDebugManager::Log(FString::Printf(TEXT(Format), ##__VA_ARGS__))

// 警告日志
#define SOUL_WARNING(Format, ...) \
	USoulDebugManager::LogWarning(FString::Printf(TEXT(Format), ##__VA_ARGS__))

// 错误日志
#define SOUL_ERROR(Format, ...) \
	USoulDebugManager::LogError(FString::Printf(TEXT(Format), ##__VA_ARGS__))

// 分类日志
#define SOUL_LOG_CATEGORY(Category, Format, ...) \
	USoulDebugManager::SoulLog(ESoulDebugCategory::Category, ESoulDebugLevel::Log, FString::Printf(TEXT(Format), ##__VA_ARGS__))

// 屏幕打印
#define SOUL_PRINT(Format, ...) \
	USoulDebugManager::PrintScreen(FString::Printf(TEXT(Format), ##__VA_ARGS__))

// 性能计时（作用域）
#define SOUL_SCOPED_TIMER(Name) \
	FSoulScopedTimer ScopedTimer_##Name(TEXT(#Name))

/**
 * 作用域计时器辅助类
 */
struct FSoulScopedTimer
{
	FString TimerName;
	double StartTime;

	FSoulScopedTimer(const FString& InName)
		: TimerName(InName)
	{
		StartTime = FPlatformTime::Seconds();
	}

	~FSoulScopedTimer()
	{
		double Elapsed = (FPlatformTime::Seconds() - StartTime) * 1000.0;
		UE_LOG(LogSoulPerf, Log, TEXT("[ScopedTimer] %s: %.3f ms"), *TimerName, Elapsed);
	}
};
