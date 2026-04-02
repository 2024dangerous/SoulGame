// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulGameCharacter/SoulPerceptionComponent.h"
#include "Components/SphereComponent.h"
#include "SoulGameAI/SoulBaseEnemy.h"
#include "SoulGameEvent/SoulEventManager.h"
#include "SoulGameDebug/DebugTools.h"

USoulPerceptionComponent::USoulPerceptionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// 创建感知碰撞球（作为子组件附加到 Owner）
	PerceptionSphere = nullptr; // 将在 Owner 的构造函数中创建并传入
}

void USoulPerceptionComponent::BeginPlay()
{
	Super::BeginPlay();

	// 绑定碰撞球的重叠事件
	if (PerceptionSphere)
	{
		PerceptionSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &USoulPerceptionComponent::OnPerceptionBeginOverlap);
		PerceptionSphere->OnComponentEndOverlap.AddUniqueDynamic(this, &USoulPerceptionComponent::OnPerceptionEndOverlap);
	}
}

ASoulBaseEnemy* USoulPerceptionComponent::GetNearestEnemy() const
{
	ASoulBaseEnemy* NearestEnemy = nullptr;
	float MinDistanceSqr = TNumericLimits<float>::Max();

	const AActor* Owner = GetOwner();
	if (!Owner) return nullptr;

	const FVector MyLocation = Owner->GetActorLocation();

	for (ASoulBaseEnemy* Enemy : EnemyArray)
	{
		if (!IsValid(Enemy)) continue;

		float DistSqr = FVector::DistSquared(Enemy->GetActorLocation(), MyLocation);
		if (DistSqr < MinDistanceSqr)
		{
			MinDistanceSqr = DistSqr;
			NearestEnemy = Enemy;
		}
	}

	return NearestEnemy;
}

void USoulPerceptionComponent::OnPerceptionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ASoulBaseEnemy* SoulEnemy = Cast<ASoulBaseEnemy>(OtherActor))
	{
		// 检查敌人是否已经在数组中
		if (!EnemyArray.Contains(SoulEnemy))
		{
			EnemyArray.Add(SoulEnemy);

			// 通知 UI 显示敌人血条
			USoulEventManager::Get()->OpenEnemyHealth.ExecuteIfBound(SoulEnemy->EnemyName, true);
			USoulEventManager::Get()->SwitchEnemyHealth.ExecuteIfBound(SoulEnemy->EnemyName, SoulEnemy->CurrentHealth / SoulEnemy->MaxHealth);

			// 广播敌人感知变化
			OnEnemyPerceptionChanged.Broadcast(SoulEnemy, true);

			ZhouXiaoPeng_PRINT(FString::Printf(TEXT("添加敌人到敌人数组: %d"), EnemyArray.Num()));
		}
	}
}

void USoulPerceptionComponent::OnPerceptionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ASoulBaseEnemy* SoulEnemy = Cast<ASoulBaseEnemy>(OtherActor))
	{
		// 检查敌人是否在数组中
		if (EnemyArray.Contains(SoulEnemy))
		{
			EnemyArray.Remove(SoulEnemy);

			// 广播敌人感知变化
			OnEnemyPerceptionChanged.Broadcast(SoulEnemy, false);

			if (EnemyArray.Num() == 0)
			{
				USoulEventManager::Get()->OpenEnemyHealth.ExecuteIfBound(SoulEnemy->EnemyName, false);
				// 广播所有敌人离开
				OnAllEnemiesLost.Broadcast();
			}

			ZhouXiaoPeng_PRINT(FString::Printf(TEXT("删除敌人从敌人数组: %d"), EnemyArray.Num()));
		}
	}
}
