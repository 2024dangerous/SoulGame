
#pragma once

#include "CoreMinimal.h"
#include "SoulGameItemType.generated.h"

//物品种类
UENUM(BlueprintType)
enum class ESoulGameItemType :uint8
{
    None,      //无
    Money,     //金钱
    Armor,     //衣服
    Sword,     //剑
    Bow,       //弓
    Shield,    //盾
    Materials, //材料
    Food,      //食物
    Prop       //道具
};