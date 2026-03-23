# SoulGame Tags System

SoulGame GameplayTags 系统，用于状态管理、事件标记和技能判断。

## 核心组件

### 1. SoulGameplayTagInterface
游戏标签接口，所有需要支持 GameplayTags 的 Actor 必须实现此接口。

**文件位置**: `Source/SoulGame/SoulGameTags/SoulGameplayTagInterface.h`

**接口方法**:
| 方法 | 说明 |
|------|------|
| `GetGameplayTagNames()` | 获取当前 Actor 的所有 GameplayTags |
| `GetGameplayTagContainer()` | 获取当前 Actor 的 GameplayTag 容器 |
| `AddGameplayTag(Tag)` | 添加一个 GameplayTag |
| `RemoveGameplayTag(Tag)` | 移除一个 GameplayTag |
| `HasGameplayTag(Tag)` | 检查是否拥有指定 Tag |
| `HasMatchingTag(Tag)` | 检查是否拥有指定 Tag（模糊匹配，支持父标签） |
| `GetTagDisplayName(Tag)` | 获取标签对应的显示名称 |

**使用方式**:
```cpp
#include "SoulGameTags/SoulGameplayTagInterface.h"

class AMyCharacter : public AActor, public ISoulGameplayTagInterface
{
    GENERATED_BODY()
public:
    // 实现接口方法...
};
```

### 2. SoulGameTagsManager
GameplayTags 管理器单例，提供标签的获取、查询、验证等功能。

**文件位置**: `Source/SoulGame/SoulGameTags/SoulGameTagsManager.h`

**获取方式**:
```cpp
USoulGameTagsManager* TagsManager = USoulGameTagsManager::Get();
```

## 标签分类

### 行为状态标签 (State.Behavior.*)
| 标签名 | 显示名称 | 说明 |
|--------|----------|------|
| `State.Behavior.Idle` | 待机 | 角色处于待机状态，可以进行移动和观察 |
| `State.Behavior.Attack` | 攻击 | 角色正在执行攻击动作 |
| `State.Behavior.Rolling` | 翻滚 | 角色正在执行翻滚动作 |
| `State.Behavior.Defense` | 防御 | 角色处于防御姿态 |
| `State.Behavior.Injury` | 受伤 | 角色正在受击动画中 |
| `State.Behavior.Dead` | 死亡 | 角色已死亡 |

### 武器状态标签 (State.Weapon.*)
| 标签名 | 显示名称 | 说明 |
|--------|----------|------|
| `State.Weapon.None` | 无武器 | 未装备武器 |
| `State.Weapon.Melee` | 拳法 | 近战拳法武器 |
| `State.Weapon.Sword` | 剑术 | 剑类武器 |

### 战斗状态标签 (State.Combat.*)
| 标签名 | 显示名称 | 说明 |
|--------|----------|------|
| `State.Combat.None` | 无战斗 | 非战斗状态 |
| `State.Combat.Normal` | 普通战斗 | 普通敌人战斗 |
| `State.Combat.Boss` | Boss战 | Boss 战斗 |

### 玩家状态标签 (State.Player.*)
| 标签名 | 显示名称 |
|--------|----------|
| `State.Player.Common` | 普通 |
| `State.Player.PrepareWar` | 备战 |

### 事件标签 (Event.*)
| 标签名 | 说明 |
|--------|------|
| `Event.Damage` | 伤害事件 |
| `Event.Dead` | 死亡事件 |
| `Event.Respawn` | 重生事件 |
| `Event.WeaponChange` | 武器切换事件 |
| `Event.StateChange` | 状态切换事件 |
| `Event.LockOn` | 锁定目标事件 |
| `Event.Dodge` | 闪避事件 |

### 技能标签 (Skill.*)
**近战技能**:
| 标签名 | 说明 |
|--------|------|
| `Skill.Melee.Combo` | 近战连击 |
| `Skill.Melee.Heavy` | 近战重击 |

**剑术技能**:
| 标签名 | 说明 |
|--------|------|
| `Skill.Sword.Combo` | 剑术连击 |
| `Skill.Sword.Heavy` | 剑术重击 |

**闪避技能**:
| 标签名 | 说明 |
|--------|------|
| `Skill.Dodge.Roll` | 翻滚闪避 |
| `Skill.Dodge.Backstep` | 后撤闪避 |

**防御技能**:
| 标签名 | 说明 |
|--------|------|
| `Skill.Defense.Block` | 格挡 |
| `Skill.Defense.Parry` | 弹反 |

### 效果标签 (Effect.*)
| 标签名 | 说明 |
|--------|------|
| `Effect.Bleeding` | 出血 |
| `Effect.Burning` | 燃烧 |
| `Effect.Frozen` | 冰冻 |
| `Effect.Stunned` | 眩晕 |
| `Effect.Poisoned` | 中毒 |
| `Effect.Slowed` | 减速 |

## 常用操作

### 获取标签
```cpp
// 通过管理器获取
FGameplayTag AttackTag = USoulGameTagsManager::Get()->GetTag(TEXT("State.Behavior.Attack"));

// 通过便捷方法获取
FGameplayTag AttackTag = USoulGameTagsManager::Get()->GetBehaviorTag(TEXT("Attack"));
FGameplayTag SwordTag = USoulGameTagsManager::Get()->GetWeaponTag(TEXT("Sword"));
FGameplayTag DamageTag = USoulGameTagsManager::Get()->GetEventTag(TEXT("Damage"));
```

### 检查标签
```cpp
// 精确匹配
if (ActorTags.HasTag(AttackTag))
{
    // ...
}

### 模糊匹配（支持父标签）— 注意：EGameplayTagMatchType 已在 UE 5.4 中废弃
```cpp
// 使用 MatchesTag 替代废弃的 HasTag + EGameplayTagMatchType
if (ActorTags.MatchesTag(Tag))
{
    // State.Behavior.Attack 会匹配 State.Behavior
}
```
```

### 标签匹配
```cpp
// 检查标签是否匹配（用于状态机判断）
bool bMatches = USoulGameTagsManager::Get()->TagMatches(CurrentTag, BaseTag);

// 获取子标签
FGameplayTagContainer ChildTags = USoulGameTagsManager::Get()->GetChildTags(TEXT("State.Behavior"));
```

### 获取显示名称和描述
```cpp
FText DisplayName = USoulGameTagsManager::Get()->GetTagDisplayName(Tag);
FString Description = USoulGameTagsManager::Get()->GetTagDescription(Tag);
```

## 与 Event 系统联动

Tags 系统与 Event 系统配合使用，通过 `SoulEventManager` 进行事件分发：

```cpp
// 发送带标签的事件
USoulEventManager::Get()->SendGameplayEvent(Event.Damage, TagContainer, params);

// 监听带标签的事件
USoulEventManager::Get()->RegisterListener(Event.Damage, this, &MyClass::OnDamage);
```
