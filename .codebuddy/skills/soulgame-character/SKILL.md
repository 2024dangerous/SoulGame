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
`F:/Unreal Engine/SoulGame/Source/SoulGame/SoulGameCharacter/`

## 核心文件

| 文件 | 类 | 功能 |
|------|-----|------|
| `SoulBaseCharacter.h/cpp` | `ASoulBaseCharacter` | 角色基类，所有角色的父类 |
| `SoulPlayerCharacter.h/cpp` | `ASoulPlayerCharacter` | 玩家角色实现 |
| `SoulPickupComponent.h/cpp` | `USoulPickupComponent` | 拾取交互组件（管理附近可拾取物品和交互UI） |
| `SoulPerceptionComponent.h/cpp` | `USoulPerceptionComponent` | 敌人感知组件（管理附近敌人列表和碰撞检测） |

## SoulBaseCharacter（角色基类）

### 组件
- `UCameraComponent* FollowCamera` - 第三人称摄像机
- `USpringArmComponent* CameraBoom` - 弹簧臂组件
- `USoulPerceptionComponent* PerceptionComponent` - 敌人感知组件（替代旧的 USphereComponent* PerceptionEnemy）
- `USoulPickupComponent* PickupComponent` - 拾取交互组件（替代旧的 PickupItemArray + SetPickupItemArray 等方法）
- `USoulStateMachineComponent* StateMachineComponent` - 状态机组件
- `USoulAbilitySystemComponent* AbilitySystemComponent` - GAS 技能系统组件

### 可配置参数（编辑器可调）

| 属性 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `CameraMinArmLength` | float | 200.f | 摄像机最小臂长 |
| `CameraMaxArmLength` | float | 600.f | 摄像机最大臂长 |
| `CameraDefaultArmLength` | float | 350.f | 摄像机默认臂长 |
| `CameraZoomStep` | float | 10.f | 摄像机缩放步长 |
| `DefaultWalkSpeed` | float | 500.f | 默认行走速度 |
| `SlowWalkSpeed` | float | 200.f | 静步速度 |
| `SprintSpeed` | float | 900.f | 冲刺速度 |
| `MeleeCollisionRadius` | float | 50.f | 拳法攻击碰撞半径 |
| `MeleeCollisionDamage` | float | 10.f | 拳法攻击伤害值 |

### GameplayTags 支持
实现 `ISoulGameplayTagInterface` 接口，支持 GameplayTags：

| 属性 | 类型 | 说明 |
|------|------|------|
| `GameplayTagContainer` | FGameplayTagContainer | 角色拥有的所有标签 |
| `StateMachineComponent` | USoulStateMachineComponent* | 状态机组件 |

### 关键功能
- **增强输入系统** - Move、Look、Attack、Rolling、Walk、Run、Focus 等
- **属性系统** - Health（生命）、Mana（法力）、Stamina（体力），统一通过 GAS 管理
- **GAS 集成** - 通过 `SyncAttributeToGAS()` 同步属性到 GAS，UI 通过 GAS 属性变化委托自动更新
- **组件化架构** - 拾取逻辑由 `USoulPickupComponent` 管理，敌人感知由 `USoulPerceptionComponent` 管理
- **状态枚举** - EPlayerStates（COMMON/PREPARWAR）
- **行为枚举** - EPlayerBehavior（IDLE/ATTACK/ROLLING/DEFENSE/INJURY）
- **武器类型** - EWeaponType（MELEE/SWORD）
- **GameplayTags** - 标签系统支持层级状态管理
- **摄像机控制、翻滚朝向计算**

### 属性系统

| 属性 | 类型 | 说明 |
|------|------|------|
| `MaxHealth` / `CurrentHealth` | float | 生命值（GAS 本地缓存） |
| `MaxMana` / `CurrentMana` | float | 法力值（GAS 本地缓存） |
| `MaxStamina` / `CurrentStamina` | float | 体力值（GAS 本地缓存） |
| `StaminaRestoreRate` | float | 体力恢复速率 |
| `LastStaminaCostThreshold` | float | 上次动作的体力消耗阈值 |

属性修改统一通过 `SetHealth()`/`SetMana()`/`SetStamina()` 方法，内部自动同步到 GAS。

## SoulPlayerCharacter（玩家角色）

### 组件
- `UStaticMeshComponent* Sword` - 剑模型
- `UStaticMeshComponent* SwordSheath` - 剑鞘模型
- `UNiagaraComponent* SwordNiagara` - 剑特效
- `UBoxComponent* SwordAttackBox` - 攻击碰撞盒

### 可配置参数（编辑器可调）

| 属性 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `SwordDamage` | float | 30.f | 剑攻击伤害值 |
| `ChangeWeaponMaterialAsset` | TSoftObjectPtr<UMaterialInterface> | - | 切换武器特效材质（软引用） |

### 通用战斗方法（重构后）

| 方法 | 功能 |
|------|------|
| `ExecuteAttack(ActionAnim, LastIndex, bClearExitTimer)` | 通用攻击执行（替代重复的MeleeAttack/SwordAttack逻辑） |
| `ExecuteRolling(RollingAnim)` | 通用翻滚执行（替代重复的MeleeRolling/SwordRolling逻辑） |
| `CanExecuteAction(ActionAnim, bRequireWeapon, bRequireSwordType)` | 通用动作检查（替代4个重复的Can*方法） |

### 关键功能

| 方法 | 功能 |
|------|------|
| `Attack()` | 根据武器类型调用近战或剑攻击 |
| `MeleeAttack()` / `SwordAttack()` | 调用通用 ExecuteAttack 方法 |
| `Rolling()` | 翻滚动作，根据方向播放不同动画 |
| `MeleeRolling()` / `SwordRolling()` | 调用通用 ExecuteRolling 方法 |
| `Defense()` | 防御状态切换 |
| `Focus()` | 锁定/解锁目标敌人 |
| `Weapons()` | 切换武器，播放Niagara特效 |
| `Injury()` | 受伤处理，根据武器类型播放受击动画 |
| `Die()` | 死亡处理，关闭输入、播放死亡动画 |
| `OnAllEnemiesLost()` | 所有敌人离开感知范围时取消锁定 |

### 武器材质系统
- `CurrentWeaponMaterial` (UPROPERTY Transient) - 当前武器材质（运行时缓存）
- `CurrentSwordSheathMaterial` (UPROPERTY Transient) - 当前剑鞘材质（运行时缓存）
- `ChangeWeaponMaterialAsset` (TSoftObjectPtr) - 编辑器可配置的切换武器特效材质
- `ChangeWeaponMaterial` (UPROPERTY Transient) - 运行时加载的材质缓存

## 数据定义

**路径：** `F:/Unreal Engine/SoulGame/Source/SoulGame/SoulGameData/SoulEnumType.h`

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
- `SoulGameItem/` - 物品系统（PickupItem 与 SoulPickupComponent 交互）

## USoulPickupComponent（拾取交互组件）

从 `SoulBaseCharacter` 中拆分出来的独立组件，负责管理角色附近的可拾取物品。

### 核心功能

| 方法 | 功能 |
|------|------|
| `GetPickupItems()` | 获取当前附近的可拾取物品数组 |
| `HasPickupItems()` | 附近是否有可拾取物品 |
| `GetPickupItemCount()` | 获取附近可拾取物品数量 |
| `PickupAllItems()` | 拾取所有附近的物品（销毁并从列表移除） |
| `AddPickupItem(Item)` | 添加拾取物品到列表 |
| `RemovePickupItem(Item)` | 从列表移除拾取物品 |

### 工作流程
1. 组件在 `BeginPlay` 中自动订阅 `SoulEventManager::NearbyInteractables` 委托
2. `PickupItem` 进入/离开范围时，通过委托通知组件
3. 组件自动更新交互UI的显示状态
4. 按交互键时，`SoulBaseCharacter::Interaction()` 调用 `PickupComponent->PickupAllItems()`

## USoulPerceptionComponent（敌人感知组件）

从 `SoulBaseCharacter` 中拆分出来的独立组件，负责管理角色周围的敌人感知。

### 核心功能

| 方法 | 功能 |
|------|------|
| `GetEnemyArray()` | 获取当前感知到的敌人数组 |
| `HasEnemies()` | 附近是否有敌人 |
| `GetEnemyCount()` | 获取附近敌人数量 |
| `GetNearestEnemy()` | 获取最近的敌人 |
| `GetPerceptionSphere()` | 获取感知球组件 |

### 委托

| 委托 | 功能 |
|------|------|
| `OnEnemyPerceptionChanged(Enemy, bIsAdded)` | 敌人进入/离开感知范围时广播 |
| `OnAllEnemiesLost()` | 所有敌人离开感知范围时广播 |

### 工作流程
1. 感知球（USphereComponent）在 `SoulBaseCharacter` 构造函数中创建并赋值给组件
2. 组件在 `BeginPlay` 中绑定碰撞球的重叠事件
3. 敌人进入/离开时自动更新敌人数组和UI
4. `SoulPlayerCharacter` 通过 `OnAllEnemiesLost` 委托触发取消锁定
