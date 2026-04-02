---
name: soulgame-item
description: |
  This skill provides detailed documentation for the SoulGame Item system, including pickup items, item types, and item pooling.
  Use this skill when working with items, pickups, inventory, or object pooling for game objects.
when: |
  When user wants to understand item implementation, pickup mechanics, item type system, or object pooling for items.
---

# SoulGame Item System

## 模块路径
`d:/Unreal Projects/SoulGame/Source/SoulGame/SoulGameItem/`

## 核心文件

| 文件 | 类 | 功能 |
|------|-----|------|
| `ItemBase.h/cpp` | `AItemBase` | 物品基类 |
| `PickupItem.h/cpp` | `APickupItem` | 可拾取物品 |
| `SoulGameItemType/SoulGameItemType.h` | `ESoulGameItemType` | 物品类型枚举 |

## ItemBase（物品基类）

所有物品的基类，提供物品的基础结构。

## PickupItem（可拾取物品）

可被玩家拾取的物品，集成对象池接口。

### 组件
- `UStaticMeshComponent* ItemMesh` - 物品静态网格
- `UNiagaraComponent* ItemNiagara` - 物品粒子特效
- `USphereComponent* ItemSphereComponent` - 拾取范围检测球

### 关键功能

| 方法 | 功能 |
|------|------|
| `OnPooledObjectSpawn()` | 对象池取出时调用，显示物品 |
| `OnPooledObjectDespawn()` | 对象池回收时调用，隐藏物品 |
| `OnPlayerEnterPickupRange()` | 玩家进入拾取范围，通知UI |
| `OnPlayerExitPickupRange()` | 玩家离开拾取范围，隐藏UI |

### 事件触发
玩家进入/离开拾取范围时，通过 `SoulEventManager::NearbyInteractables` 委托通知 `USoulPickupComponent`（拾取交互组件）管理物品列表和交互UI。

### 与 USoulPickupComponent 的交互流程
```
PickupItem 进入范围 → SoulEventManager::NearbyInteractables 委托
→ USoulPickupComponent::OnNearbyInteractablesChanged()
→ AddPickupItem() → UpdateInteractionUI()
→ 按 F 键 → SoulBaseCharacter::Interaction() → PickupComponent->PickupAllItems()
```

## 物品类型枚举

**路径：** `d:/Unreal Projects/SoulGame/Source/SoulGame/SoulGameItem/SoulGameItemType/SoulGameItemType.h`

```cpp
UENUM()
enum class ESoulGameItemType : uint8 {
    None     = 0,
    Money    = 1,
    Armor    = 2,
    Sword    = 3,
    Bow      = 4,
    Shield   = 5,
    Materials= 6,
    Food     = 7,
    Prop     = 8
};
```

## 相关模块

- `SoulObjectPool/` - PickupItemPool 管理物品对象池
- `SoulGameEvent/` - NearbyInteractables 事件通知
- `SoulGameHUD/` - UI_Interaction 显示拾取提示