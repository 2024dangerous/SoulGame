---
name: soulgame-hud
description: |
  This skill provides detailed documentation for the SoulGame HUD system, including combat UI, state bars, enemy health bars, and menus.
  Use this skill when working with UI implementation, widget blueprints, HUD elements, or user interface logic.
when: |
  When user wants to understand UI implementation, HUD widgets, state bars, enemy health display, or menu systems.
---

# SoulGame HUD System

## 模块路径
`d:/Unreal Projects/SoulGame/Source/SoulGame/SoulGameHUD/`

## 核心文件

| 文件 | 类 | 功能 |
|------|-----|------|
| `SoulGameHUD.h/cpp` | `ASoulGameHUD` | HUD主控类 |
| `UI_FightMainUI.h/cpp` | `UUI_FightMainUI` | 战斗主界面 |
| `UI_StateBar.h/cpp` | `UUI_StateBar` | 状态条（血/蓝/体力） |
| `UI_EnemyHealth.h/cpp` | `UUI_EnemyHealth` | 敌人血条 |
| `UI_Interaction.h/cpp` | `UUI_Interaction` | 交互提示 |
| `UI_SetMenu.h/cpp` | `UUI_SetMenu` | 设置菜单 |
| `UI_FightResult.h/cpp` | `UUI_FightResult` | 战斗结果界面 |

## SoulGameHUD（HUD主控）

管理所有游戏UI的创建和显示。

### 管理的UI
- `UUI_FightMainUI*` - 战斗主界面
- `UUI_SetMenu*` - 设置菜单

## UI_FightMainUI（战斗主界面）

整合所有战斗相关UI元素。

### 子组件
- `UUI_StateBar* WBP_Health` - 生命条
- `UUI_StateBar* WBP_Mana` - 法力条
- `UUI_StateBar* WBP_Stamine` - 体力条
- `UUI_Interaction* WBP_Interaction` - 交互提示
- `UUI_EnemyHealth* WBP_EnemyHealth` - 敌人血条
- `UUI_FightResult*` - 战斗结果

## UI_StateBar（状态条）

可复用的血条/蓝条组件，支持动态材质动画效果。

### 特性
- 动态材质动画效果
- 受伤/治疗动画
- Material Parameter控制视觉效果
- 通过 GAS 属性变化委托直接监听属性变化（不再依赖 OnStatusBoxChanged）

## UI_EnemyHealth（敌人血条）

显示锁定敌人的血量。

### 特性
- 平滑血量过渡动画（NativeTick插值）
- 跟随目标头顶显示

## UI_SetMenu（设置菜单）

游戏暂停菜单。

### 功能
- 重启游戏
- 退出游戏
- ESC键响应

## UI_FightResult（战斗结果）

死亡后显示的结算界面。

### 功能
- 30秒倒计时自动退出
- 重新开始按钮

## 相关模块

- `SoulGameEvent/` - 事件订阅（状态变化、血条显示）
- `SoulGameCharacter/` - 玩家状态数据
- `SoulGameAI/` - 敌人血量数据