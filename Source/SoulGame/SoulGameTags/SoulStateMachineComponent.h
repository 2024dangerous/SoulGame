// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTags.h"
#include "SoulStateMachineComponent.generated.h"

class USoulGameTagsManager;

/**
 * 鐘舵€佽浆鎹㈣鍒?
 */
USTRUCT(BlueprintType)
struct FStateTransitionRule
{
	GENERATED_BODY()

	// 鐩爣鐘舵€?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag TargetState;

	// 闇€瑕佺殑鏍囩锛堝彲閫夛級
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer RequiredTags;

	// 闃绘杞崲鐨勬爣绛撅紙鍙€夛級
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer BlockedTags;

	// 鏄惁闇€瑕佹鏌ュ喎鍗?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCheckCooldown = true;

	// 鍐峰嵈鏃堕棿锛堢锛?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CooldownTime = 0.5f;
};

/**
 * 鐘舵€佸眰瀹氫箟
 * 姣忎釜灞傜嫭绔嬭繍琛岃嚜宸辩殑鐘舵€佹満锛屼簰涓嶅共鎵?
 */
USTRUCT(BlueprintType)
struct FSoulStateLayer
{
	GENERATED_BODY()

	// 层名称（用于调试和查找）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LayerName;

	// 层标签（用于标识层）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag LayerTag;

	// 褰撳墠鐘舵€?
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag CurrentState;

	// 鍒濆鐘舵€?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag InitialState;

	// 鍏佽鐨勭姸鎬佸垪琛?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer AllowedStates;

	// 鐘舵€佽浆鎹㈣鍒?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FStateTransitionRule> TransitionRules;

	// 涓婃杞崲鏃堕棿鏄犲皠
	TMap<FGameplayTag, float> LastTransitionTimes;

	// 灞備紭鍏堢骇锛堥珮浼樺厛绾у眰鍙互闃绘浣庝紭鍏堢骇灞傜殑鐘舵€佽浆鎹級
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 0;

	// 鏄惁鍚敤
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnabled = true;
};

/**
 * 鐘舵€佹敼鍙樺鎵?
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStateChanged, FGameplayTag, OldState, FGameplayTag, NewState, AActor*, Owner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStateEnter, FGameplayTag, State, AActor*, Owner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStateExit, FGameplayTag, State, AActor*, Owner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnLayerStateChanged, FName, LayerName, FGameplayTag, OldState, FGameplayTag, NewState, AActor*, Owner);

/**
 * SoulStateMachineComponent - 基于 GameplayTags 的分层状态机组件
 * 
 * 鏀寔澶氬眰骞惰鐘舵€佹満锛?
 * - 行为层（Behavior）：Idle/Attack/Rolling/Defense/Injury/Dead
 * - 战斗层（Combat）：None/InCombat/OutOfCombat
 * - 武器层（Weapon）：Melee/Sword/None
 * 
 * 姣忓眰鐙珛杩愯锛屼簰涓嶅共鎵帮紝浣嗛珮浼樺厛绾у眰鍙互闃绘浣庝紭鍏堢骇灞傜殑杞崲
 */
UCLASS(ClassGroup = (SoulGame), meta = (BlueprintSpawnableComponent))
class SOULGAME_API USoulStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USoulStateMachineComponent();

	// 鍒濆鍖栫姸鎬佹満
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine")
	void InitializeComponent() override;

	// ============ 鍗曞眰鎿嶄綔锛堝吋瀹规棫鎺ュ彛锛?===========

	// 璁剧疆鏂扮姸鎬侊紙榛樿鎿嶄綔琛屼负灞傦級
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine")
	bool SetState(FGameplayTag NewState);

	// 鑾峰彇褰撳墠鐘舵€侊紙榛樿鑾峰彇琛屼负灞傦級
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine")
	FGameplayTag GetCurrentState() const { return CurrentState; }

	// 鑾峰彇鍒濆鐘舵€?
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine")
	FGameplayTag GetInitialState() const { return InitialState; }

	// 璁剧疆鍒濆鐘舵€?
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine")
	void SetInitialState(FGameplayTag InInitialState) { InitialState = InInitialState; }

	// 璁剧疆鍏佽鐨勭姸鎬佸垪琛?
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine")
	void SetAllowedStates(const FGameplayTagContainer& InAllowedStates) { AllowedStates = InAllowedStates; }

	// 妫€鏌ユ槸鍚﹀彲浠ヨ浆鎹㈠埌鎸囧畾鐘舵€?
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine")
	bool CanTransitionTo(FGameplayTag NewState) const;

	// 娣诲姞鐘舵€佽浆鎹㈣鍒?
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine")
	void AddTransitionRule(const FGameplayTag FromState, const FStateTransitionRule& Rule);

	// 绉婚櫎鐘舵€佽浆鎹㈣鍒?
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine")
	void RemoveTransitionRule(FGameplayTag ToState);

	// 妫€鏌ュ喎鍗存槸鍚﹀畬鎴?
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine")
	bool IsCooldownComplete(FGameplayTag TargetState) const;

	// ============ 鍒嗗眰鐘舵€佹満鎿嶄綔锛堟柊鎺ュ彛锛?===========

	/**
	 * 娉ㄥ唽涓€涓柊鐨勭姸鎬佸眰
	 * @param LayerName 灞傚悕绉?
	 * @param LayerTag 灞傛爣绛?
	 * @param InInitialState 鍒濆鐘舵€?
	 * @param InAllowedStates 鍏佽鐨勭姸鎬佸垪琛?
	 * @param Priority 层优先级
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine|Layer")
	void RegisterLayer(FName LayerName, FGameplayTag LayerTag, FGameplayTag InInitialState,
		const FGameplayTagContainer& InAllowedStates, int32 Priority = 0);

	/**
	 * 璁剧疆鎸囧畾灞傜殑鐘舵€?
	 * @param LayerName 灞傚悕绉?
	 * @param NewState 鏂扮姸鎬?
	 * @return 鏄惁鎴愬姛
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine|Layer")
	bool SetLayerState(FName LayerName, FGameplayTag NewState);

	/**
	 * 鑾峰彇鎸囧畾灞傜殑褰撳墠鐘舵€?
	 * @param LayerName 灞傚悕绉?
	 * @return 褰撳墠鐘舵€?
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine|Layer")
	FGameplayTag GetLayerState(FName LayerName) const;

	/**
	 * 妫€鏌ユ寚瀹氬眰鏄惁澶勪簬鏌愪釜鐘舵€?
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine|Layer")
	bool IsLayerInState(FName LayerName, FGameplayTag State) const;

	/**
	 * 鍚戞寚瀹氬眰娣诲姞杞崲瑙勫垯
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine|Layer")
	void AddLayerTransitionRule(FName LayerName, const FStateTransitionRule& Rule);

	/**
	 * 鍚敤/绂佺敤鎸囧畾灞?
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine|Layer")
	void SetLayerEnabled(FName LayerName, bool bEnabled);

	/**
	 * 鑾峰彇鎵€鏈夊眰鐨勭姸鎬佸揩鐓э紙鐢ㄤ簬璋冭瘯锛?
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine|Layer")
	TMap<FName, FGameplayTag> GetAllLayerStates() const;

	/**
	 * 妫€鏌ヤ换鎰忓眰鏄惁澶勪簬鎸囧畾鐘舵€?
	 */
	UFUNCTION(BlueprintCallable, Category = "SoulGame|StateMachine|Layer")
	bool IsAnyLayerInState(FGameplayTag State) const;

protected:
	// ============ 委托 ============

	// 鐘舵€佹敼鍙樹簨浠讹紙鍏煎鏃ф帴鍙ｏ級
	UPROPERTY(BlueprintAssignable, Category = "SoulGame|StateMachine")
	FOnStateChanged OnStateChanged;

	// 鐘舵€佽繘鍏ヤ簨浠?
	UPROPERTY(BlueprintAssignable, Category = "SoulGame|StateMachine")
	FOnStateEnter OnStateEnter;

	// 鐘舵€侀€€鍑轰簨浠?
	UPROPERTY(BlueprintAssignable, Category = "SoulGame|StateMachine")
	FOnStateExit OnStateExit;

	// 鍒嗗眰鐘舵€佹敼鍙樹簨浠?
	UPROPERTY(BlueprintAssignable, Category = "SoulGame|StateMachine|Layer")
	FOnLayerStateChanged OnLayerStateChanged;

	// ============ 鏃ф帴鍙ｆ暟鎹紙鍏煎锛?===========

	// 鍒濆鐘舵€?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateMachine")
	FGameplayTag InitialState;

	// 褰撳墠鐘舵€?
	UPROPERTY(BlueprintReadOnly, Category = "StateMachine")
	FGameplayTag CurrentState;

	// 鍏佽鐨勭姸鎬佸垪琛?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateMachine")
	FGameplayTagContainer AllowedStates;

	// 鐘舵€佽浆鎹㈣鍒?
	UPROPERTY(BlueprintReadOnly, Category = "StateMachine")
	TArray<FStateTransitionRule> TransitionRules;

	// ============ 分层数据 ============

	// 鎵€鏈夌姸鎬佸眰
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateMachine|Layers")
	TArray<FSoulStateLayer> StateLayers;

private:
	// 涓婃杞崲鏃堕棿鏄犲皠
	UPROPERTY()
	TMap<FGameplayTag, float> LastTransitionTimes;

	// 娣诲姞鏍囩鍒版墍鏈夎€?
	void AddTagToOwner(FGameplayTag Tag);

	// 浠庢墍鏈夎€呯Щ闄ゆ爣绛?
	void RemoveTagFromOwner(FGameplayTag Tag);

	// 妫€鏌ョ姸鎬佹槸鍚︽湁鏁?
	bool IsValidState(FGameplayTag State) const;

	// 鏌ユ壘灞傦紙鍐呴儴鏂规硶锛?
	FSoulStateLayer* FindLayer(FName LayerName);
	const FSoulStateLayer* FindLayer(FName LayerName) const;

	// 妫€鏌ュ眰鍐呯姸鎬佹槸鍚︽湁鏁?
	bool IsValidLayerState(const FSoulStateLayer& Layer, FGameplayTag State) const;

	// 妫€鏌ュ眰鍐呭喎鍗存槸鍚﹀畬鎴?
	bool IsLayerCooldownComplete(const FSoulStateLayer& Layer, FGameplayTag TargetState) const;

	// 妫€鏌ュ眰鍐呮槸鍚﹀彲浠ヨ浆鎹?
	bool CanLayerTransitionTo(const FSoulStateLayer& Layer, FGameplayTag NewState) const;
};
