// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGamePerformance/SoulLODManager.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"

USoulLODManager::USoulLODManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	// 降低 Tick 频率，不需要每帧检查
	PrimaryComponentTick.TickInterval = 0.5f;

	CurrentLODLevel = 0;
	CachedDistance = 0.0f;
	bIsInFrustum = true;
	LastCheckTime = 0.0f;
	bOriginalTickEnabled = true;
	bOriginalCollisionEnabled = true;
}

void USoulLODManager::BeginPlay()
{
	Super::BeginPlay();

	// 记录原始状态
	AActor* Owner = GetOwner();
	if (Owner)
	{
		bOriginalTickEnabled = Owner->PrimaryActorTick.bCanEverTick;
	}

	// 按距离排序 LOD 级别
	LODLevels.Sort([](const FSoulLODLevel& A, const FSoulLODLevel& B)
	{
		return A.DistanceThreshold < B.DistanceThreshold;
	});
}

void USoulLODManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastCheckTime < CheckInterval)
	{
		return;
	}
	LastCheckTime = CurrentTime;

	UpdateLOD();
}

void USoulLODManager::ForceUpdate()
{
	UpdateLOD();
}

void USoulLODManager::UpdateLOD()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	// 计算距离
	FVector CameraLocation = GetCameraLocation();
	CachedDistance = FVector::Dist(Owner->GetActorLocation(), CameraLocation);

	// 检查视锥
	if (bEnableFrustumCulling)
	{
		bIsInFrustum = CheckFrustumVisibility();
	}
	else
	{
		bIsInFrustum = true;
	}

	// 超过最大距离，直接隐藏
	if (CachedDistance > MaxDistance)
	{
		Owner->SetActorHiddenInGame(true);
		Owner->SetActorTickEnabled(false);
		return;
	}

	// 不在视锥内，隐藏但保持 Tick（可能需要 AI 逻辑）
	if (!bIsInFrustum)
	{
		Owner->SetActorHiddenInGame(true);
		// 注意：不禁用 Tick，因为 AI 可能需要继续运行
		return;
	}

	// 确定 LOD 级别
	int32 NewLODLevel = 0;
	for (int32 i = LODLevels.Num() - 1; i >= 0; i--)
	{
		if (CachedDistance >= LODLevels[i].DistanceThreshold)
		{
			NewLODLevel = i;
			break;
		}
	}

	// LOD 级别变化时应用新设置
	if (NewLODLevel != CurrentLODLevel || !Owner->IsHidden() == false)
	{
		CurrentLODLevel = NewLODLevel;

		if (LODLevels.IsValidIndex(CurrentLODLevel))
		{
			ApplyLODSettings(LODLevels[CurrentLODLevel]);
		}
		else
		{
			RestoreOriginalSettings();
		}
	}
}

void USoulLODManager::ApplyLODSettings(const FSoulLODLevel& LODConfig)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	// 可见性
	Owner->SetActorHiddenInGame(LODConfig.bHideActor);

	// Tick
	Owner->SetActorTickEnabled(!LODConfig.bDisableTick);

	// 碰撞
	if (LODConfig.bDisableCollision)
	{
		Owner->SetActorEnableCollision(false);
	}
	else
	{
		Owner->SetActorEnableCollision(true);
	}

	// 阴影和动画更新频率
	TArray<UActorComponent*> Components;
	Owner->GetComponents(Components);

	for (UActorComponent* Comp : Components)
	{
		// 阴影
		if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Comp))
		{
			PrimComp->SetCastShadow(!LODConfig.bDisableShadow);
		}

		// 动画更新频率
		if (USkeletalMeshComponent* SkelComp = Cast<USkeletalMeshComponent>(Comp))
		{
			if (LODConfig.AnimUpdateRate > 0.0f)
			{
				// 使用 URO (Update Rate Optimization)
				SkelComp->bEnableUpdateRateOptimizations = true;
			}
			else
			{
				SkelComp->bEnableUpdateRateOptimizations = false;
			}
		}
	}

	UE_LOG(LogTemp, Verbose, TEXT("USoulLODManager::ApplyLODSettings - %s LOD 级别: %s (距离: %.0f)"),
		*Owner->GetName(), *LODConfig.LevelName.ToString(), CachedDistance);
}

void USoulLODManager::RestoreOriginalSettings()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	Owner->SetActorHiddenInGame(false);
	Owner->SetActorTickEnabled(bOriginalTickEnabled);
	Owner->SetActorEnableCollision(true);

	TArray<UActorComponent*> Components;
	Owner->GetComponents(Components);

	for (UActorComponent* Comp : Components)
	{
		if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Comp))
		{
			PrimComp->SetCastShadow(true);
		}

		if (USkeletalMeshComponent* SkelComp = Cast<USkeletalMeshComponent>(Comp))
		{
			SkelComp->bEnableUpdateRateOptimizations = false;
		}
	}
}

bool USoulLODManager::CheckFrustumVisibility() const
{
	AActor* Owner = GetOwner();
	if (!Owner) return true;

	UWorld* World = GetWorld();
	if (!World) return true;

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC || !PC->PlayerCameraManager) return true;

	FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
	FRotator CameraRotation = PC->PlayerCameraManager->GetCameraRotation();
	float CameraFOV = PC->PlayerCameraManager->GetFOVAngle();

	// 计算 Actor 相对于摄像机的方向
	FVector ToActor = (Owner->GetActorLocation() - CameraLocation).GetSafeNormal();
	FVector CameraForward = CameraRotation.Vector();

	// 计算角度
	float DotProduct = FVector::DotProduct(CameraForward, ToActor);
	float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(DotProduct, -1.0f, 1.0f)));

	// 在 FOV + 边距内视为可见
	float HalfFOV = (CameraFOV * 0.5f) + FrustumCullingMargin;

	return AngleDeg <= HalfFOV;
}

FVector USoulLODManager::GetCameraLocation() const
{
	UWorld* World = GetWorld();
	if (!World) return FVector::ZeroVector;

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC || !PC->PlayerCameraManager) return FVector::ZeroVector;

	return PC->PlayerCameraManager->GetCameraLocation();
}
