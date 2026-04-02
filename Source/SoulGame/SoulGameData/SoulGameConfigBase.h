// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SoulGameConfigBase.generated.h"

/**
 * SoulGameConfigBase - 配置 DataAsset 基类
 * 
 * 鎵€鏈夋父鎴忛厤缃?DataAsset 都应继承此类
 * 鎻愪緵閰嶇疆鍔犺浇銆侀噸杞姐€侀獙璇佺瓑閫氱敤鍔熻兘
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class SOULGAME_API USoulGameConfigBase : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	USoulGameConfigBase();

	// ============ 配置标识 ============
	
	// 閰嶇疆鍞竴鏍囪瘑
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
	 * @param OutError 閿欒淇℃伅
	 * @return 鏄惁鍔犺浇鎴愬姛
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Config")
	virtual bool LoadConfig(FText& OutError);

	/**
	 * 閲嶈浇閰嶇疆锛堟敮鎸佺儹鏇存柊锛?
	 * @param OutError 閿欒淇℃伅
	 * @return 鏄惁閲嶈浇鎴愬姛
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|Config")
	virtual bool ReloadConfig(FText& OutError);

	/**
	 * 验证配置
	 * @param OutError 閿欒淇℃伅
	 * @return 鏄惁楠岃瘉閫氳繃
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "SoulGame|Config")
	bool ValidateConfig(FText& OutError);

	/**
	 * 鑾峰彇閰嶇疆鏄惁鏈夋晥
	 */
	UFUNCTION(BlueprintPure, Category = "SoulGame|Config")
	bool IsConfigValid() const { return bIsLoaded && !bHasError; }

	/**
	 * 鑾峰彇閰嶇疆鏄惁宸插姞杞?
	 */
	UFUNCTION(BlueprintPure, Category = "SoulGame|Config")
	bool IsLoaded() const { return bIsLoaded; }

protected:
	// 閰嶇疆鏄惁宸插姞杞?
	UPROPERTY()
	bool bIsLoaded;

	// 閰嶇疆鏄惁鏈夐敊璇?
	UPROPERTY()
	bool bHasError;

	// 鏈€鍚庨敊璇俊鎭?
	UPROPERTY()
	FText LastError;
};
