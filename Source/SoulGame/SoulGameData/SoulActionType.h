#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SoulActionType.generated.h"

//动作消耗体力表
USTRUCT(BlueprintType)
struct FSoulActionType : public FTableRowBase
{
    GENERATED_BODY()

public:

  
    // 消耗的体力值
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StaminaAnimCost = 0.f;

    // 动作冷却时间（可选）
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Cooldown= 0.f;

    // 动作播放的Montage（可选）
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<UAnimMontage*> AnimMontage = {};

    FSoulActionType() :
        StaminaAnimCost(5.f),
        Cooldown(0.f),
        AnimMontage()
    {}
};