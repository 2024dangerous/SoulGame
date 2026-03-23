---
name: soulgame-ai
description: |
  This skill provides detailed documentation for the SoulGame AI system, including enemy base class, behavior tree tasks, and AI controller implementation.
  Use this skill when working with enemy AI, behavior trees, combat AI logic, or monster implementation.
when: |
  When user wants to understand AI implementation, behavior tree nodes, enemy attack patterns, or AI controller logic.
---

# SoulGame AI System

## 模块路径
`d:/Unreal Projects/SoulGame/Source/SoulGame/SoulGameAI/`

## 核心文件

| 文件 | 类 | 功能 |
|------|-----|------|
| `SoulBaseEnemy.h/cpp` | `ASoulBaseEnemy` | 敌人基类 |
| `ShadowSpecter.h/cpp` | `AShadowSpecter` | 影魍敌人实现 |
| `SoulAIController.h/cpp` | `ASoulAIController` | AI控制器 |
| `Tasks/BTS_DistanceCheck.h/cpp` | `UBTS_DistanceCheck` | 距离检测服务 |
| `Tasks/BT_Attack.h/cpp` | `UBT_Attack` | 攻击任务 |
| `Tasks/BT_Running.h/cpp` | `UBT_Running` | 奔跑任务 |
| `Tasks/BT_UnEquip.h/cpp` | `UBT_UnEquip` | 收剑任务 |
| `Tasks/BT_Observer.h/cpp` | `UBT_Observer` | 观察者任务（闪避） |

## SoulBaseEnemy（敌人基类）

### 组件
- `USphereComponent* PandoraBox` - 战斗感知球
- `UWidgetComponent* EnemyTip` - 头顶提示组件
- `ASoulAIController* AIController` - AI控制器

## ShadowSpecter（影魍敌人）

具体敌人实现，使用行为树进行战斗AI。

### 关键功能
- **武器切换** - 拔剑/收剑状态
- **攻击系统** - MeleeAttack、SwordAttack、RushAttack
- **观察者系统** - 随机闪避移动
- **动画Montage管理**

## 行为树节点

### BTS_DistanceCheck（Service - 每帧执行）
检测与玩家距离，设置攻击/奔跑/停止状态

### BT_Attack（Task）
执行近战攻击

### BT_Running（Task）
设置敌人奔跑移动

### BT_UnEquip（Task）
收剑并锁定目标

### BT_Observer（Task）
闪避观察行为

## 枚举定义

**路径：** `d:/Unreal Projects/SoulGame/Source/SoulGame/SoulGameData/SoulEnumType.h`

```cpp
// 敌人武器类型
UENUM()
enum class EEnemyWeaponType : uint8 {
    MELEE,
    SWORD
};

// 敌人行为
UENUM()
enum class EEnemyBehavior : uint8 {
    IDLE,
    ATTACK,
    RUNNING,
    UNARMED
};
```

## 相关模块

- `SoulGameCharacter/` - 玩家角色（AI的攻击目标）
- `SoulGameEvent/` - 事件系统（受伤通知）
- `SoulGameHUD/` - UI（敌人血条显示）