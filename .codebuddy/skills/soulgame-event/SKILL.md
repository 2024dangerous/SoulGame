---
name: soulgame-event
description: |
  This skill provides detailed documentation for the SoulGame Event system, including the event manager singleton, message routing, tag-based events, and DataAsset configuration support.
  Use this skill when working with event-driven communication, module decoupling, or UI reactivity.
when: |
  When user wants to understand event-driven architecture, module communication, message routing, or how to add new event notifications.
---

# SoulGame Event System

## 模块路径
`d:/Unreal Projects/SoulGame/Source/SoulGame/SoulGameEvent/`

## 核心文件

| 文件 | 类 | 功能 |
|------|-----|------|
| `SoulMessage.h` | `FSoulMessage`, `FSoulMessageSubscriber` | 消息数据结构 |
| `SoulEventManager.h/cpp` | `USoulEventManager` | 消息路由管理器（单例） |

## SoulEventManager（消息路由管理器）

单例模式的消息路由中心，提供基于 GameplayTags 的事件分发功能。

### 核心特性

- **消息路由**：支持通道（Channel）和优先级（Priority）过滤
- **安全订阅**：使用委托句柄管理订阅生命周期
- **层级匹配**：支持 GameplayTag 父子标签匹配
- **兼容旧API**：保留传统委托方式兼容
- **GameInstance 生命周期**：正确的生命周期管理

### 设计模式
- **单例模式** - 全局唯一实例
- **观察者模式** - 委托广播状态变化
- **Tag-Based 事件** - 基于 GameplayTags 的层级式事件
- **消息路由** - 支持通道和优先级

---

## 消息结构体 (FSoulMessage)

消息路由的核心数据结构：

```cpp
struct FSoulMessage
{
    FGameplayTag EventTag;           // 事件标签
    AActor* Instigator;              // 事件发起者
    ESoulMessageChannel Channel;     // 消息通道
    ESoulMessagePriority Priority;    // 消息优先级
    bool bHandled;                   // 是否已处理（用于中断广播）
    double Timestamp;                // 时间戳
};
```

### 消息通道 (ESoulMessageChannel)

```cpp
enum class ESoulMessageChannel : uint8
{
    Global,      // 全局广播
    Combat,      // 战斗相关
    Inventory,   // 物品/拾取相关
    UI,          // UI相关
    AI,          // AI相关
    Player,      // 玩家相关
    System       // 系统相关
};
```

### 消息优先级 (ESoulMessagePriority)

```cpp
enum class ESoulMessagePriority : uint8
{
    Low,      // 低优先级
    Normal,   // 普通优先级
    High,     // 高优先级
    Critical  // 关键优先级（最高）
};
```

---

## 新版 API（推荐）

### 发送消息

```cpp
// 创建并发送消息
FSoulMessage Message;
Message.EventTag = USoulGameTagsManager::Get()->GetEventTag(TEXT("Damage"));
Message.Instigator = this;
Message.Channel = ESoulMessageChannel::Combat;
Message.Priority = ESoulMessagePriority::High;

USoulEventManager::Get()->SendMessage(Message);
```

### 订阅消息（返回句柄）

```cpp
// 订阅消息
FSoulMessageSubscriber Subscriber;
Subscriber.Target = this;
Subscriber.FunctionName = TEXT("OnDamageReceived");
Subscriber.Channel = ESoulMessageChannel::Combat;
Subscriber.Priority = ESoulMessagePriority::Normal;

int32 Handle = USoulEventManager::Get()->Subscribe(Subscriber);

// 通过句柄取消订阅
USoulEventManager::Get()->Unsubscribe(Handle);

// 取消目标的所有订阅
USoulEventManager::Get()->UnsubscribeAll(this);
```

---

## 兼容旧 API

### 传统委托方式（保留兼容）

```cpp
// 附近可交互物体变化（拾取物品）
DECLARE_DELEGATE_TwoParams(FNearbyInteractables, AActor*, bool);
FNearbyInteractables NearbyInteractables;

// 打开/关闭战斗UI
DECLARE_DELEGATE_OneParam(FOpenFightUI, bool);
FOpenFightUI OpenFightUI;

// 状态值变化多播
DECLARE_MULTICAST_DELEGATE_FourParams(FOnStatusBoxChanged, EStatusBox, float, float, float);
FOnStatusBoxChanged OnStatusBoxChanged;
```

### Tag-Based 事件系统

#### 核心方法

| 方法 | 说明 |
|------|------|
| `SendMessage(FSoulMessage)` | 发送消息（新版） |
| `SendGameplayTagEvent(Tag, Instigator)` | 发送标签事件（兼容） |
| `SendStateChangeEvent(Old, New, Instigator)` | 发送状态改变事件 |
| `Subscribe(FSoulMessageSubscriber)` | 订阅消息（新版，返回句柄） |
| `SubscribeToGameplayTagEvent(Tag, Object, Func)` | 订阅标签事件（兼容） |
| `Unsubscribe(Handle)` | 取消订阅（新版） |
| `UnsubscribeAll(Object)` | 取消目标所有订阅 |

#### 便捷方法

```cpp
// 发送伤害事件
void SendDamageEvent(AActor* Damager, AActor* Victim, float Damage);

// 发送死亡事件
void SendDeathEvent(AActor* Victim);

// 发送武器切换事件
void SendWeaponChangeEvent(AActor* Actor, FGameplayTag NewWeaponTag);
```

### 使用示例

```cpp
#include "SoulGameEvent/SoulEventManager.h"
#include "SoulGameTags/SoulGameTagsManager.h"

// 1. 发送消息（新版）
void AMyCharacter::TakeDamage(float Damage, AActor* Attacker)
{
    FSoulMessage Message(
        USoulGameTagsManager::Get()->GetEventTag(TEXT("Damage")),
        Attacker,
        ESoulMessageChannel::Combat,
        ESoulMessagePriority::High
    );
    USoulEventManager::Get()->SendMessage(Message);
}

// 2. 订阅消息（新版）
void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();

    FSoulMessageSubscriber Subscriber;
    Subscriber.Target = this;
    Subscriber.FunctionName = TEXT("OnDamageReceived");
    Subscriber.Channel = ESoulMessageChannel::Combat;
    Subscriber.Priority = ESoulMessagePriority::Normal;

    // 保存句柄用于取消订阅
    DamageEventHandle = USoulEventManager::Get()->Subscribe(Subscriber);
}

// 3. 回调函数签名
UFUNCTION()
void OnDamageReceived(FGameplayTag EventTag, AActor* Instigator);

// 4. 取消订阅
void AMyCharacter::EndPlay(const EEndPlayReason& Reason)
{
    USoulEventManager::Get()->Unsubscribe(DamageEventHandle);
}
```

---

## 与 Tags 系统联动

Tag-Based 事件系统深度依赖 `SoulGameTagsManager` 管理器，所有事件标签通过 `USoulGameTagsManager` 进行注册和管理。

### 联动架构

```
SoulEventManager                    SoulGameTagsManager
┌─────────────────────┐            ┌─────────────────────┐
│ SendMessage()       │ ───────► │ GetEventTag()       │
│ Subscribe()         │ ───────► │ TagMatches()        │
└─────────────────────┘            └─────────────────────┘
```

### 常用事件标签

通过 `USoulGameTagsManager::Get()->GetEventTag()` 获取：

| 事件标签 | 获取方式 | 触发时机 |
|----------|----------|----------|
| `Event.Damage` | `GetEventTag("Damage")` | 造成伤害时 |
| `Event.Dead` | `GetEventTag("Dead")` | 角色死亡时 |
| `Event.Respawn` | `GetEventTag("Respawn")` | 复活时 |
| `Event.WeaponChange` | `GetEventTag("WeaponChange")` | 切换武器时 |
| `Event.StateChange` | `GetEventTag("StateChange")` | 状态变化时 |
| `Event.LockOn` | `GetEventTag("LockOn")` | 锁定/取消锁定目标 |
| `Event.Dodge` | `GetEventTag("Dodge")` | 闪避时 |

---

## DataAsset 配置体系

事件系统支持通过 DataAsset 进行配置：

### SoulTagsConfig

```cpp
// 标签配置 DataAsset
UCLASS(BlueprintType, meta = (DisplayName = "Soul Tags Config"))
class USoulTagsConfig : public USoulGameConfigBase
{
    FTagCategoryConfig BehaviorTags;   // 行为标签
    FTagCategoryConfig WeaponTags;     // 武器标签
    FTagCategoryConfig EventTags;      // 事件标签
    // ...
};
```

---

## 相关模块

- `SoulGameCharacter/` - 订阅状态变化事件
- `SoulGameItem/` - 发送拾取范围事件
- `SoulGameHUD/` - 订阅UI控制事件
- `SoulGameAI/` - 敌人受伤通知
- `SoulGameTags/` - Tag-Based 事件依赖 GameplayTags 系统
- `SoulGameData/` - DataAsset 配置体系