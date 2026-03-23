// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoulEnumType.generated.h"

//玩家攻击方式
UENUM(BlueprintType)
enum class EWeaponType :uint8
{
    NONE,
    MELEE,      //拳法
    SWORD       //剑
};


//玩家近战状态
UENUM(BlueprintType)
enum class EPlayerStates :uint8
{
    NONE,
    COMMON,    //普通状态
    PREPARWAR  //战斗状态
};

//玩家行为
UENUM(BlueprintType)
enum class EPlayerBehavior :uint8
{
    NONE,
    IDLE,      //待机
    ATTACK,    //攻击
    ROLLING,   //翻滚
    DENFENSE,  //防御
    INJURY     //受伤
};

//敌人行为
UENUM(BlueprintType)
enum class EEnemyWeaponType :uint8
{
    NONE,
    MELEE,      //拳法
    SWORD       //剑
};

//敌人行为
UENUM(BlueprintType)
enum class EEnemyBehavior :uint8
{
    NONE,
    IDLE,        //待机
    MELEEATTACK, //拳攻击
    SWORDATTACK, //剑攻击
    ROLLING,     //翻滚
    DENFENSE,    //防御
    INJURY       //受伤
};


//状态条
UENUM(BlueprintType)
enum class EStatusBox :uint8
{
    NONE,
    Health,      //生命值
    Mana,        //法力值
    Stamina      //体力值
};