---
name: soulgame
description: |
  SoulGame is a third-person action RPG game built with Unreal Engine C++. This skill should be used when exploring, understanding, or modifying the SoulGame codebase. It provides a comprehensive overview of the project's architecture, modules, and key features.
  Use this skill as the entry point for any SoulGame-related tasks.
when: |
  When user asks about SoulGame project structure, wants to understand the game architecture, or needs to explore any module of the SoulGame codebase.
---

# SoulGame - 第三人称动作RPG游戏

## 项目概览

SoulGame 是一款基于 Unreal Engine C++ 开发第三人称动作RPG游戏，具有魂类游戏的锁定系统、双武器切换、体力管理等核心玩法。

## 项目路径
`d:/Unreal Projects/SoulGame/Source/SoulGame/`

## 核心模块

| 模块 | 路径 | 功能 |
|------|------|------|
| **Character** | `SoulGameCharacter/` | 角色系统：玩家/敌人基类、战斗/翻滚/防御 |
| **AI** | `SoulGameAI/` | AI系统：行为树、敌人控制器、攻击任务 |
| **Item** | `SoulGameItem/` | 物品系统：可拾取物品、物品类型枚举 |
| **HUD** | `SoulGameHUD/` | UI系统：战斗界面、状态条、敌人血条 |
| **Event** | `SoulGameEvent/` | 事件系统：单例事件管理器、Tag-Based事件 |
| **ObjectPool** | `SoulObjectPool/` | 对象池：物品池化管理、对象复用 |
| **Data** | `SoulGameData/` | 数据定义：枚举类型、动作数据结构 |
| **Tags** | `SoulGameTags/` | GameplayTags系统：标签管理、状态机、接口 |
| **Debug** | `SoulGameDebug/` | 调试工具：统一日志输出 |

## 核心玩法

1. **双武器系统** - 近战拳法和剑术可切换战斗
2. **锁定目标** - 类似魂游戏的锁定敌人视角
3. **体力管理** - 攻击/翻滚/防御消耗体力
4. **AI行为树** - 敌人智能战斗（攻击/奔跑/闪避）
5. **对象池优化** - 物品系统减少GC开销

## 设计模式

- 单例模式：`SoulEventManager`
- 对象池模式：`ObjectPoolManager`
- 观察者模式：事件委托广播
- 组件模式：Actor模块化组件

## 子Skills

本项目包含以下子Skills，用于深入了解各模块：

- `soulgame-character` - 角色系统详细文档
- `soulgame-ai` - AI系统详细文档
- `soulgame-item` - 物品系统详细文档
- `soulgame-hud` - UI系统详细文档
- `soulgame-event` - 事件系统详细文档
- `soulgame-tags` - GameplayTags系统详细文档
- `soulgame-objectpool` - 对象池系统详细文档

## 快速参考

**关键类：**
- `APickupItem` - 可拾取物品（对象池）
- `ASoulPlayerCharacter` - 玩家角色
- `ASoulBaseEnemy` / `AShadowSpecter` - 敌人
- `USoulEventManager` - 事件管理器
- `UObjectPoolManager` - 对象池管理器

**关键枚举：**
- `EWeaponType` - 武器类型（MELEE/SWORD）
- `EPlayerStates` - 玩家状态（COMMON/PREPARWAR）
- `EPlayerBehavior` - 玩家行为（IDLE/ATTACK/ROLLING/DEFENSE/INJURY）
- `ESoulGameItemType` - 物品类型（None/Money/Armor/Sword/Bow/Shield/Materials/Food/Prop）