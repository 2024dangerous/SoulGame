// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SoulGameConfigBase.generated.h"

/**
 * SoulGameConfigBase - 配置 DataAsset 基类
 * 
 * 所有游戏配置 DataAsset 都应继承此类
 * 提供配置加载、重载、验证等通用功能
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class SOULGAME_API USoulGameConfigBase : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	USoulGameConfigBase();

	// ============ 配置标识 ============
	
	// 配置唯一标识
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	FName ConfigId;

	// 配置版本
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	int32 ConfigVersion;

	// 配置描述
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	FText ConfigDescription;

	// ============ 配置生命周期 ============

	/**
	 * 加载配置
	 * @param OutError 错误信息
	 * @return 是否加载成功
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Config")
	virtual bool LoadConfig(FText& OutError);

	/**
	 * 重载配置（支持热更新）
	 * @param OutError 错误信息
	 * @return 是否重载成功
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Config")
	virtual bool ReloadConfig(FText& OutError);

	/**
	 * 验证配置
	 * @param OutError 错误信息
	 * @return 是否验证通过
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "SoulGame|Config")
	bool ValidateConfig(FText& OutError);

	/**
	 * 获取配置是否有效
	 */
	UFUNCTION(BlueprintPure, Category = "SoulGame|Config")
	bool IsConfigValid() const { return bIsLoaded && !bHasError; }

	/**
	 * 获取配置是否已加载
	 */
	UFUNCTION(BlueprintPure, Category = "SoulGame|Config")
	bool IsLoaded() const { return bIsLoaded; }

protected:
	// 配置是否已加载
	UPROPERTY()
	bool bIsLoaded;

	// 配置是否有错误
	UPROPERTY()
	bool bHasError;

	// 最后错误信息
	UPROPERTY()
	FText LastError;
};
