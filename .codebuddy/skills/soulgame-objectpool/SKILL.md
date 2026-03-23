---
name: soulgame-objectpool
description: |
  This skill provides detailed documentation for the SoulGame Object Pool system, including pool management, poolable interface, and item pool implementation.
  Use this skill when working with object pooling, performance optimization, or item spawning management.
when: |
  When user wants to understand object pooling implementation, improve performance by reducing spawn/destroy, or implement new pooled objects.
---

# SoulGame Object Pool System

## 模块路径
`d:/Unreal Projects/SoulGame/Source/SoulGame/SoulObjectPool/`

## 核心文件

| 文件 | 类 | 功能 |
|------|-----|------|
| `PoolableInterface.h` | `IPoolableInterface` | 可池化对象接口 |
| `ObjectPoolBase.h/cpp` | `UObjectPoolBase` | 对象池基类 |
| `ObjectPoolManager.h/cpp` | `UObjectPoolManager` | 对象池管理器 |
| `PickupItemPool.h/cpp` | `UPickupItemPool` | 物品池实现 |

## 架构图

```
UObjectPoolManager (管理器 - 单例)
    │
    └── UPickupItemPool (物品池)
            │
            └── APickupItem (可拾取物品)
```

## PoolableInterface（池化接口）

定义可池化对象的接口方法。

```cpp
class IPoolableInterface {
public:
    // 对象被取出时调用
    virtual void OnPooledObjectSpawn() = 0;
    
    // 对象被回收时调用
    virtual void OnPooledObjectDespawn() = 0;
};
```

## ObjectPoolBase（对象池基类）

泛型对象池基类。

### 抽象方法
```cpp
virtual void InitializePool() = 0;  // 初始化池
virtual AActor* AcquireObject() = 0; // 获取对象
virtual void ReleaseObject(AActor* Obj) = 0; // 回收对象
virtual void SetupPool() = 0;        // 设置池
```

## ObjectPoolManager（对象池管理器）

管理所有对象池的创建和访问。单例模式。

### 核心功能

| 方法 | 功能 |
|------|------|
| `CreatePool()` | 创建新的对象池 |
| `Acquire()` | 从池中获取对象 |
| `Release()` | 归还对象到池 |

### 存储结构
```cpp
TMap<FName, UObjectPoolBase*> Pools;
```

## PickupItemPool（物品池）

APickupItem专用对象池实现。

### 特性
- 预生成对象（初始10个）
- 隐藏/禁用闲置对象
- 动态扩容

### 初始化
在 `SoulGameInstance::OnWorldInitialized()` 中创建初始池。

## 相关模块

- `SoulGameItem/` - APickupItem 实现池化接口
- `SoulGameInstance/` - 初始化对象池管理器

## 性能优势

使用对象池相比直接 Spawn/Destroy：
- 减少内存分配/释放开销
- 避免频繁GC
- 提升大量物品场景的性能