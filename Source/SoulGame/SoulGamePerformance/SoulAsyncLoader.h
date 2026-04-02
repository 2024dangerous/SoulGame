// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/StreamableManager.h"
#include "SoulAsyncLoader.generated.h"

/**
 * 加载请求状态
 */
UENUM(BlueprintType)
enum class ESoulLoadStatus : uint8
{
	Pending		UMETA(DisplayName = "等待中"),
	Loading		UMETA(DisplayName = "加载中"),
	Completed	UMETA(DisplayName = "已完成"),
	Failed		UMETA(DisplayName = "失败"),
	Cancelled	UMETA(DisplayName = "已取消")
};

/**
 * 加载请求优先级
 */
UENUM(BlueprintType)
enum class ESoulLoadPriority : uint8
{
	Low			UMETA(DisplayName = "低"),
	Normal		UMETA(DisplayName = "普通"),
	High		UMETA(DisplayName = "高"),
	Critical	UMETA(DisplayName = "关键")
};

/**
 * 单个加载请求
 */
USTRUCT(BlueprintType)
struct FSoulLoadRequest
{
	GENERATED_BODY()

	// 请求 ID
	UPROPERTY(BlueprintReadOnly)
	int32 RequestID = -1;

	// 资源路径
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UObject> AssetPath;

	// 优先级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESoulLoadPriority Priority = ESoulLoadPriority::Normal;

	// 当前状态
	UPROPERTY(BlueprintReadOnly)
	ESoulLoadStatus Status = ESoulLoadStatus::Pending;

	// 加载进度（0-1）
	UPROPERTY(BlueprintReadOnly)
	float Progress = 0.0f;

	// 加载完成的资源
	UPROPERTY(BlueprintReadOnly)
	UObject* LoadedAsset = nullptr;
};

/**
 * 批量加载请求
 */
USTRUCT(BlueprintType)
struct FSoulBatchLoadRequest
{
	GENERATED_BODY()

	// 批次 ID
	UPROPERTY(BlueprintReadOnly)
	int32 BatchID = -1;

	// 资源路径列表
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSoftObjectPath> AssetPaths;

	// 优先级
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESoulLoadPriority Priority = ESoulLoadPriority::Normal;

	// 当前状态
	UPROPERTY(BlueprintReadOnly)
	ESoulLoadStatus Status = ESoulLoadStatus::Pending;

	// 总体进度（0-1）
	UPROPERTY(BlueprintReadOnly)
	float Progress = 0.0f;

	// 已加载的资源
	UPROPERTY(BlueprintReadOnly)
	TArray<UObject*> LoadedAssets;
};

// 委托声明
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnAssetLoaded, int32, RequestID, UObject*, LoadedAsset);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnBatchLoaded, int32, BatchID, const TArray<UObject*>&, LoadedAssets);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnLoadProgress, int32, RequestID, float, Progress);

/**
 * SoulAsyncLoader - 异步资源加载管理器
 * 
 * 功能：
 * - 单个资源异步加载
 * - 批量资源异步加载
 * - 加载优先级队列
 * - 加载进度回调
 * - 资源缓存管理
 * - 取消加载请求
 * 
 * 使用方法：
 * 1. 通过 GameInstance 获取 SoulAsyncLoader
 * 2. 调用 LoadAssetAsync / LoadBatchAsync 发起加载
 * 3. 绑定回调获取加载结果
 */
UCLASS(BlueprintType)
class SOULGAME_API USoulAsyncLoader : public UObject
{
	GENERATED_BODY()

public:
	USoulAsyncLoader();

	// ============ 单个资源加载 ============

	/**
	 * 异步加载单个资源
	 * @param AssetPath 资源软引用路径
	 * @param Priority 加载优先级
	 * @param OnLoaded 加载完成回调
	 * @return 请求 ID
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|AsyncLoader")
	int32 LoadAssetAsync(TSoftObjectPtr<UObject> AssetPath, ESoulLoadPriority Priority, FOnAssetLoaded OnLoaded);

	/**
	 * 异步加载指定类型的资源
	 * @param AssetPath 资源路径字符串
	 * @param OnLoaded 加载完成回调
	 * @return 请求 ID
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|AsyncLoader")
	int32 LoadAssetByPath(const FString& AssetPath, FOnAssetLoaded OnLoaded);

	// ============ 批量加载 ============

	/**
	 * 异步批量加载资源
	 * @param AssetPaths 资源路径列表
	 * @param Priority 加载优先级
	 * @param OnBatchLoaded 批量加载完成回调
	 * @param OnProgress 进度回调（可选）
	 * @return 批次 ID
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|AsyncLoader")
	int32 LoadBatchAsync(const TArray<FSoftObjectPath>& AssetPaths, ESoulLoadPriority Priority,
		FOnBatchLoaded OnBatchLoaded, FOnLoadProgress OnProgress);

	// ============ 管理接口 ============

	/**
	 * 取消加载请求
	 * @param RequestID 请求 ID
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|AsyncLoader")
	void CancelRequest(int32 RequestID);

	/**
	 * 取消所有加载请求
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|AsyncLoader")
	void CancelAllRequests();

	/**
	 * 获取加载请求状态
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|AsyncLoader")
	ESoulLoadStatus GetRequestStatus(int32 RequestID) const;

	/**
	 * 获取当前活跃的加载请求数量
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|AsyncLoader")
	int32 GetActiveRequestCount() const;

	// ============ 缓存管理 ============

	/**
	 * 检查资源是否已缓存
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|AsyncLoader|Cache")
	bool IsAssetCached(const FSoftObjectPath& AssetPath) const;

	/**
	 * 从缓存获取资源
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|AsyncLoader|Cache")
	UObject* GetCachedAsset(const FSoftObjectPath& AssetPath) const;

	/**
	 * 清除所有缓存
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|AsyncLoader|Cache")
	void ClearCache();

	/**
	 * 获取缓存大小
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|AsyncLoader|Cache")
	int32 GetCacheSize() const;

private:
	// 流式加载管理器
	FStreamableManager StreamableManager;

	// 请求 ID 计数器
	int32 NextRequestID;

	// 活跃的单个加载请求
	TMap<int32, TSharedPtr<FStreamableHandle>> ActiveHandles;

	// 加载完成回调映射
	TMap<int32, FOnAssetLoaded> LoadCallbacks;

	// 批量加载回调映射
	TMap<int32, FOnBatchLoaded> BatchCallbacks;

	// 进度回调映射
	TMap<int32, FOnLoadProgress> ProgressCallbacks;

	// 请求状态映射
	TMap<int32, ESoulLoadStatus> RequestStatuses;

	// 资源缓存
	TMap<FSoftObjectPath, TWeakObjectPtr<UObject>> AssetCache;

	// 内部回调
	void OnSingleAssetLoaded(int32 RequestID, FSoftObjectPath AssetPath);
	void OnBatchAssetsLoaded(int32 BatchID, TArray<FSoftObjectPath> AssetPaths);

	// 获取流式加载优先级
	int32 GetStreamablePriority(ESoulLoadPriority Priority) const;
};
