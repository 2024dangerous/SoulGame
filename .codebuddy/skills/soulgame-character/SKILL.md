---
name: soulgame-character
description: |
  This skill provides detailed documentation for the SoulGame Character system, including player character and enemy base classes, combat mechanics, movement, and animation handling.
  Use this skill when working with character-related code, combat system, input handling, or animation.
when: |
  When user wants to understand character implementation, combat mechanics (attack/roll/defense), weapon switching, camera system, or character animation.
---

# SoulGame Character System

## 模块路径
`d:/Unreal Projects/SoulGame/Source/SoulGame/SoulGameCharacter/`

## 核心文件

| 文件 | 类 | 功能 |
|------|-----|------|
| `SoulBaseCharacter.h/cpp` | `ASoulBaseCharacter` | 角色基类，所有角色的父类 |
| `SoulPlayerCharacter.h/cpp` | `ASoulPlayerCharacter` | 玩家角色实现 |

## SoulBaseCharacter（角色基类）

### 组件
- `UCameraComponent* FollowCamera` - 第三人称摄像机
- `USpringArmComponent* CameraBoom` - 弹簧臂组件
- `USphereComponent* PerceptionEnemy` - 敌人感知球
- `USoulStateMachineComponent* StateMachineComponent` - 状态机组件

### GameplayTags 支持
实现 `ISoulGameplayTagInterface` 接口，支持 GameplayTags：

| 属性 | 类型 | 说明 |
|------|------|------|
| `GameplayTagContainer` | FGameplayTagContainer | 角色拥有的所有标签 |
| `StateMachineComponent` | USoulStateMachineComponent* | 状态机组件 |

### 关键功能
- **增强输入系统** - Move、Look、Attack、Rolling、Walk、Run、Focus 等
- **属性系统** - Health（生命）、Mana（法力）、Stamina（体力）
- **状态枚举** - EPlayerStates（COMMON/PREPARWAR）
- **行为枚举** - EPlayerBehavior（IDLE/ATTACK/ROLLING/DEFENSE/INJURY）
- **武器类型** - EWeaponType（MELEE/SWORD）
- **GameplayTags** - 标签系统支持层级状态管理
- **摄像机控制、翻滚朝向计算**

## SoulPlayerCharacter（玩家角色）

### 组件
- `UStaticMeshComponent* Sword` - 剑模型
- `UStaticMeshComponent* SwordSheath` - 剑鞘模型
- `UNiagaraComponent* SwordNiagara` - 剑特效
- `UBoxComponent* SwordAttackBox` - 攻击碰撞盒

### 关键功能

| 方法 | 功能 |
|------|------|
| `Attack()` | 根据武器类型调用近战或剑攻击 |
| `Rolling()` | 翻滚动作，根据方向播放不同动画 |
| `Defense()` | 防御状态切换 |
| `Focus()` | 锁定/解锁目标敌人 |
| `Weapons()` | 切换武器，播放Niagara特效 |
| `Injury()` | 受伤处理，根据武器类型播放受击动画 |
| `Die()` | 死亡处理，关闭输入、播放死亡动画 |

## 数据定义

**路径：** `d:/Unreal Projects/SoulGame/Source/SoulGame/SoulGameData/SoulEnumType.h`

```cpp
// 武器类型
UENUM()
enum class EWeaponType : uint8 {
    MELEE,
    SWORD
};

// 玩家状态
UENUM()
enum class EPlayerStates : uint8 {
    COMMON,
    PREPARWAR
};

// 玩家行为
UENUM()
enum class EPlayerBehavior : uint8 {
    IDLE,
    ATTACK,
    ROLLING,
    DEFENSE,
    INJURY
};
```

## 相关模块

- `SoulGameAI/` - 敌人AI（共享 EWeaponType 等枚举）
- `SoulGameEvent/` - 事件系统（状态变化通知）
- `SoulGameHUD/` - UI（显示生命/体力等状态）