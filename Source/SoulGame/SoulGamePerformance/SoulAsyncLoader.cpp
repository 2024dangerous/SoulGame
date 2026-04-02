// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGamePerformance/SoulAsyncLoader.h"
#include "Engine/StreamableManager.h"

USoulAsyncLoader::USoulAsyncLoader()
{
	NextRequestID = 1;
}

// ============ 单个资源加载 ============

int32 USoulAsyncLoader::LoadAssetAsync(TSoftObjectPtr<UObject> AssetPath, ESoulLoadPriority Priority, FOnAssetLoaded OnLoaded)
{
	FSoftObjectPath SoftPath = AssetPath.ToSoftObjectPath();

	if (!SoftPath.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulAsyncLoader::LoadAssetAsync - 无效的资源路径"));
		return -1;
	}

	// 检查缓存
	if (TWeakObjectPtr<UObject>* CachedPtr = AssetCache.Find(SoftPath))
	{
		if (CachedPtr->IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("USoulAsyncLoader::LoadAssetAsync - 从缓存获取: %s"), *SoftPath.ToString());
			int32 RequestID = NextRequestID++;
			RequestStatuses.Add(RequestID, ESoulLoadStatus::Completed);
			OnLoaded.ExecuteIfBound(RequestID, CachedPtr->Get());
			return RequestID;
		}
		else
		{
			AssetCache.Remove(SoftPath);
		}
	}

	int32 RequestID = NextRequestID++;

	// 保存回调
	LoadCallbacks.Add(RequestID, OnLoaded);
	RequestStatuses.Add(RequestID, ESoulLoadStatus::Loading);

	// 发起异步加载
	TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestAsyncLoad(
		SoftPath,
		FStreamableDelegate::CreateUObject(this, &USoulAsyncLoader::OnSingleAssetLoaded, RequestID, SoftPath),
		GetStreamablePriority(Priority)
	);

	if (Handle.IsValid())
	{
		ActiveHandles.Add(RequestID, Handle);
		UE_LOG(LogTemp, Log, TEXT("USoulAsyncLoader::LoadAssetAsync - 开始加载: %s (ID: %d)"), *SoftPath.ToString(), RequestID);
	}
	else
	{
		RequestStatuses[RequestID] = ESoulLoadStatus::Failed;
		UE_LOG(LogTemp, Error, TEXT("USoulAsyncLoader::LoadAssetAsync - 加载失败: %s"), *SoftPath.ToString());
	}

	return RequestID;
}

int32 USoulAsyncLoader::LoadAssetByPath(const FString& AssetPath, FOnAssetLoaded OnLoaded)
{
	FSoftObjectPath SoftPath(AssetPath);
	TSoftObjectPtr<UObject> SoftPtr(SoftPath);
	return LoadAssetAsync(SoftPtr, ESoulLoadPriority::Normal, OnLoaded);
}

// ============ 批量加载 ============

int32 USoulAsyncLoader::LoadBatchAsync(const TArray<FSoftObjectPath>& AssetPaths, ESoulLoadPriority Priority,
	FOnBatchLoaded OnBatchLoaded, FOnLoadProgress OnProgress)
{
	if (AssetPaths.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("USoulAsyncLoader::LoadBatchAsync - 空的资源路径列表"));
		return -1;
	}

	int32 BatchID = NextRequestID++;

	// 保存回调
	BatchCallbacks.Add(BatchID, OnBatchLoaded);
	if (OnProgress.IsBound())
	{
		ProgressCallbacks.Add(BatchID, OnProgress);
	}
	RequestStatuses.Add(BatchID, ESoulLoadStatus::Loading);

	// 发起批量异步加载
	TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestAsyncLoad(
		AssetPaths,
		FStreamableDelegate::CreateUObject(this, &USoulAsyncLoader::OnBatchAssetsLoaded, BatchID, AssetPaths),
		GetStreamablePriority(Priority)
	);

	if (Handle.IsValid())
	{
		ActiveHandles.Add(BatchID, Handle);
		UE_LOG(LogTemp, Log, TEXT("USoulAsyncLoader::LoadBatchAsync - 开始批量加载 %d 个资源 (BatchID: %d)"),
			AssetPaths.Num(), BatchID);
	}
	else
	{
		RequestStatuses[BatchID] = ESoulLoadStatus::Failed;
		UE_LOG(LogTemp, Error, TEXT("USoulAsyncLoader::LoadBatchAsync - 批量加载失败"));
	}

	return BatchID;
}

// ============ 管理接口 ============

void USoulAsyncLoader::CancelRequest(int32 RequestID)
{
	if (TSharedPtr<FStreamableHandle>* Handle = ActiveHandles.Find(RequestID))
	{
		if (Handle->IsValid())
		{
			(*Handle)->CancelHandle();
		}
		ActiveHandles.Remove(RequestID);
	}

	LoadCallbacks.Remove(RequestID);
	BatchCallbacks.Remove(RequestID);
	ProgressCallbacks.Remove(RequestID);
	RequestStatuses.Add(RequestID, ESoulLoadStatus::Cancelled);

	UE_LOG(LogTemp, Log, TEXT("USoulAsyncLoader::CancelRequest - 取消请求: %d"), RequestID);
}

void USoulAsyncLoader::CancelAllRequests()
{
	for (auto& Pair : ActiveHandles)
	{
		if (Pair.Value.IsValid())
		{
			Pair.Value->CancelHandle();
		}
	}

	ActiveHandles.Empty();
	LoadCallbacks.Empty();
	BatchCallbacks.Empty();
	ProgressCallbacks.Empty();

	// 更新所有活跃请求的状态
	for (auto& Pair : RequestStatuses)
	{
		if (Pair.Value == ESoulLoadStatus::Loading || Pair.Value == ESoulLoadStatus::Pending)
		{
			Pair.Value = ESoulLoadStatus::Cancelled;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("USoulAsyncLoader::CancelAllRequests - 取消所有请求"));
}

ESoulLoadStatus USoulAsyncLoader::GetRequestStatus(int32 RequestID) const
{
	if (const ESoulLoadStatus* Status = RequestStatuses.Find(RequestID))
	{
		return *Status;
	}
	return ESoulLoadStatus::Failed;
}

int32 USoulAsyncLoader::GetActiveRequestCount() const
{
	int32 Count = 0;
	for (const auto& Pair : RequestStatuses)
	{
		if (Pair.Value == ESoulLoadStatus::Loading || Pair.Value == ESoulLoadStatus::Pending)
		{
			Count++;
		}
	}
	return Count;
}

// ============ 缓存管理 ============

bool USoulAsyncLoader::IsAssetCached(const FSoftObjectPath& AssetPath) const
{
	if (const TWeakObjectPtr<UObject>* CachedPtr = AssetCache.Find(AssetPath))
	{
		return CachedPtr->IsValid();
	}
	return false;
}

UObject* USoulAsyncLoader::GetCachedAsset(const FSoftObjectPath& AssetPath) const
{
	if (const TWeakObjectPtr<UObject>* CachedPtr = AssetCache.Find(AssetPath))
	{
		if (CachedPtr->IsValid())
		{
			return CachedPtr->Get();
		}
	}
	return nullptr;
}

void USoulAsyncLoader::ClearCache()
{
	AssetCache.Empty();
	UE_LOG(LogTemp, Log, TEXT("USoulAsyncLoader::ClearCache - 缓存已清除"));
}

int32 USoulAsyncLoader::GetCacheSize() const
{
	int32 ValidCount = 0;
	for (const auto& Pair : AssetCache)
	{
		if (Pair.Value.IsValid())
		{
			ValidCount++;
		}
	}
	return ValidCount;
}

// ============ 内部回调 ============

void USoulAsyncLoader::OnSingleAssetLoaded(int32 RequestID, FSoftObjectPath AssetPath)
{
	UObject* LoadedAsset = AssetPath.ResolveObject();

	if (LoadedAsset)
	{
		// 加入缓存
		AssetCache.Add(AssetPath, LoadedAsset);
		RequestStatuses[RequestID] = ESoulLoadStatus::Completed;

		UE_LOG(LogTemp, Log, TEXT("USoulAsyncLoader::OnSingleAssetLoaded - 加载完成: %s (ID: %d)"),
			*AssetPath.ToString(), RequestID);
	}
	else
	{
		RequestStatuses[RequestID] = ESoulLoadStatus::Failed;
		UE_LOG(LogTemp, Error, TEXT("USoulAsyncLoader::OnSingleAssetLoaded - 加载失败: %s (ID: %d)"),
			*AssetPath.ToString(), RequestID);
	}

	// 执行回调
	if (FOnAssetLoaded* Callback = LoadCallbacks.Find(RequestID))
	{
		Callback->ExecuteIfBound(RequestID, LoadedAsset);
		LoadCallbacks.Remove(RequestID);
	}

	// 清理句柄
	ActiveHandles.Remove(RequestID);
}

void USoulAsyncLoader::OnBatchAssetsLoaded(int32 BatchID, TArray<FSoftObjectPath> AssetPaths)
{
	TArray<UObject*> LoadedAssets;
	bool bAllSuccess = true;

	for (const FSoftObjectPath& Path : AssetPaths)
	{
		UObject* Asset = Path.ResolveObject();
		if (Asset)
		{
			LoadedAssets.Add(Asset);
			AssetCache.Add(Path, Asset);
		}
		else
		{
			LoadedAssets.Add(nullptr);
			bAllSuccess = false;
		}
	}

	RequestStatuses[BatchID] = bAllSuccess ? ESoulLoadStatus::Completed : ESoulLoadStatus::Failed;

	UE_LOG(LogTemp, Log, TEXT("USoulAsyncLoader::OnBatchAssetsLoaded - 批量加载完成: %d/%d 成功 (BatchID: %d)"),
		LoadedAssets.Num(), AssetPaths.Num(), BatchID);

	// 执行回调
	if (FOnBatchLoaded* Callback = BatchCallbacks.Find(BatchID))
	{
		Callback->ExecuteIfBound(BatchID, LoadedAssets);
		BatchCallbacks.Remove(BatchID);
	}

	// 清理
	ProgressCallbacks.Remove(BatchID);
	ActiveHandles.Remove(BatchID);
}

int32 USoulAsyncLoader::GetStreamablePriority(ESoulLoadPriority Priority) const
{
	switch (Priority)
	{
	case ESoulLoadPriority::Low:		return 10;
	case ESoulLoadPriority::Normal:		return 50;
	case ESoulLoadPriority::High:		return 80;
	case ESoulLoadPriority::Critical:	return 100;
	default:							return 50;
	}
}
