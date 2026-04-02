// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameTags/SoulGameTagsManager.h"
#include "SoulGameData/SoulTagsConfig.h"
#include "GameplayTagsManager.h"

USoulGameTagsManager* USoulGameTagsManager::Singleton = nullptr;

USoulGameTagsManager::USoulGameTagsManager()
	: TagsConfig(nullptr)
	, bUseDataAssetMode(false)
{
}

USoulGameTagsManager* USoulGameTagsManager::Get()
{
	if (!Singleton)
	{
		Singleton = NewObject<USoulGameTagsManager>();
		Singleton->AddToRoot();
		Singleton->Initialize(false); // 榛樿浣跨敤纭紪鐮佹ā寮?
	}
	return Singleton;
}

void USoulGameTagsManager::Initialize(bool bInUseDataAsset)
{
	bUseDataAssetMode = bInUseDataAsset;

	if (bUseDataAssetMode && TagsConfig)
	{
		// DataAsset 模式：从配置加载
		UE_LOG(LogTemp, Log, TEXT("USoulGameTagsManager::Initialize - Using DataAsset mode with config: %s"),
			*TagsConfig->GetName());

		// 浠?DataAsset 鍔犺浇鎵€鏈夋爣绛惧埌缂撳瓨
		FGameplayTagContainer AllTags = TagsConfig->GetAllTags();

		// 琛屼负鏍囩
		if (TagsConfig->BehaviorTags.bEnabled)
		{
			for (const auto& Entry : TagsConfig->BehaviorTags.Tags)
			{
				if (Entry.bEnabled)
				{
					BehaviorTags.Add(Entry.TagName, TagsConfig->GetBehaviorTag(Entry.TagName));
				}
			}
		}

		// 姝﹀櫒鏍囩
		if (TagsConfig->WeaponTags.bEnabled)
		{
			for (const auto& Entry : TagsConfig->WeaponTags.Tags)
			{
				if (Entry.bEnabled)
				{
					WeaponTags.Add(Entry.TagName, TagsConfig->GetWeaponTag(Entry.TagName));
				}
			}
		}

		// 鎴樻枟鏍囩
		if (TagsConfig->CombatTags.bEnabled)
		{
			for (const auto& Entry : TagsConfig->CombatTags.Tags)
			{
				if (Entry.bEnabled)
				{
					CombatTags.Add(Entry.TagName, TagsConfig->GetCombatTag(Entry.TagName));
				}
			}
		}

		// 浜嬩欢鏍囩
		if (TagsConfig->EventTags.bEnabled)
		{
			for (const auto& Entry : TagsConfig->EventTags.Tags)
			{
				if (Entry.bEnabled)
				{
					EventTags.Add(Entry.TagName, TagsConfig->GetEventTag(Entry.TagName));
				}
			}
		}

		// 鎶€鑳芥爣绛?
		if (TagsConfig->SkillTags.bEnabled)
		{
			for (const auto& Entry : TagsConfig->SkillTags.Tags)
			{
				if (Entry.bEnabled)
				{
					SkillTags.Add(Entry.TagName, TagsConfig->GetSkillTag(Entry.TagName));
				}
			}
		}

		// 鏁堟灉鏍囩
		if (TagsConfig->EffectTags.bEnabled)
		{
			for (const auto& Entry : TagsConfig->EffectTags.Tags)
			{
				if (Entry.bEnabled)
				{
					EffectTags.Add(Entry.TagName, TagsConfig->GetEffectTag(Entry.TagName));
				}
			}
		}
	}
	else
	{
		// 纭紪鐮佹ā寮忥紙鍏煎锛?
		UE_LOG(LogTemp, Log, TEXT("USoulGameTagsManager::Initialize - Using hardcoded mode"));

		// 琛屼负鐘舵€佹爣绛?
		BehaviorTags.Add(TEXT("Idle"), FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Idle")));
		BehaviorTags.Add(TEXT("Attack"), FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Attack")));
		BehaviorTags.Add(TEXT("Rolling"), FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Rolling")));
		BehaviorTags.Add(TEXT("Defense"), FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Defense")));
		BehaviorTags.Add(TEXT("Injury"), FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Injury")));
		BehaviorTags.Add(TEXT("Dead"), FGameplayTag::RequestGameplayTag(TEXT("State.Behavior.Dead")));

		// 姝﹀櫒鐘舵€佹爣绛?
		WeaponTags.Add(TEXT("None"), FGameplayTag::RequestGameplayTag(TEXT("State.Weapon.None")));
		WeaponTags.Add(TEXT("Melee"), FGameplayTag::RequestGameplayTag(TEXT("State.Weapon.Melee")));
		WeaponTags.Add(TEXT("Sword"), FGameplayTag::RequestGameplayTag(TEXT("State.Weapon.Sword")));

		// 鎴樻枟鐘舵€佹爣绛?
		CombatTags.Add(TEXT("None"), FGameplayTag::RequestGameplayTag(TEXT("State.Combat.None")));
		CombatTags.Add(TEXT("Normal"), FGameplayTag::RequestGameplayTag(TEXT("State.Combat.Normal")));
		CombatTags.Add(TEXT("Boss"), FGameplayTag::RequestGameplayTag(TEXT("State.Combat.Boss")));

		// 浜嬩欢鏍囩
		EventTags.Add(TEXT("Damage"), FGameplayTag::RequestGameplayTag(TEXT("Event.Damage")));
		EventTags.Add(TEXT("Dead"), FGameplayTag::RequestGameplayTag(TEXT("Event.Dead")));
		EventTags.Add(TEXT("Respawn"), FGameplayTag::RequestGameplayTag(TEXT("Event.Respawn")));
		EventTags.Add(TEXT("WeaponChange"), FGameplayTag::RequestGameplayTag(TEXT("Event.WeaponChange")));
		EventTags.Add(TEXT("StateChange"), FGameplayTag::RequestGameplayTag(TEXT("Event.StateChange")));
		EventTags.Add(TEXT("LockOn"), FGameplayTag::RequestGameplayTag(TEXT("Event.LockOn")));
		EventTags.Add(TEXT("Dodge"), FGameplayTag::RequestGameplayTag(TEXT("Event.Dodge")));

		// 鎶€鑳芥爣绛?
		SkillTags.Add(TEXT("MeleeCombo"), FGameplayTag::RequestGameplayTag(TEXT("Skill.Melee.Combo")));
		SkillTags.Add(TEXT("MeleeHeavy"), FGameplayTag::RequestGameplayTag(TEXT("Skill.Melee.Heavy")));
		SkillTags.Add(TEXT("SwordCombo"), FGameplayTag::RequestGameplayTag(TEXT("Skill.Sword.Combo")));
		SkillTags.Add(TEXT("SwordHeavy"), FGameplayTag::RequestGameplayTag(TEXT("Skill.Sword.Heavy")));
		SkillTags.Add(TEXT("DodgeRoll"), FGameplayTag::RequestGameplayTag(TEXT("Skill.Dodge.Roll")));
		SkillTags.Add(TEXT("DodgeBackstep"), FGameplayTag::RequestGameplayTag(TEXT("Skill.Dodge.Backstep")));
		SkillTags.Add(TEXT("DefenseBlock"), FGameplayTag::RequestGameplayTag(TEXT("Skill.Defense.Block")));
		SkillTags.Add(TEXT("DefenseParry"), FGameplayTag::RequestGameplayTag(TEXT("Skill.Defense.Parry")));

		// 鏁堟灉鏍囩
		EffectTags.Add(TEXT("Bleeding"), FGameplayTag::RequestGameplayTag(TEXT("Effect.Bleeding")));
		EffectTags.Add(TEXT("Burning"), FGameplayTag::RequestGameplayTag(TEXT("Effect.Burning")));
		EffectTags.Add(TEXT("Frozen"), FGameplayTag::RequestGameplayTag(TEXT("Effect.Frozen")));
		EffectTags.Add(TEXT("Stunned"), FGameplayTag::RequestGameplayTag(TEXT("Effect.Stunned")));
		EffectTags.Add(TEXT("Poisoned"), FGameplayTag::RequestGameplayTag(TEXT("Effect.Poisoned")));
		EffectTags.Add(TEXT("Slowed"), FGameplayTag::RequestGameplayTag(TEXT("Effect.Slowed")));
	}

	UE_LOG(LogTemp, Log, TEXT("USoulGameTagsManager::Initialize - Tags initialized successfully"));
}

void USoulGameTagsManager::SetTagsConfig(USoulTagsConfig* InConfig)
{
	TagsConfig = InConfig;
	if (TagsConfig && bUseDataAssetMode)
	{
		// 閲嶆柊鍒濆鍖?
		Initialize(true);
	}
}

FGameplayTagContainer USoulGameTagsManager::GetAllTags() const
{
	FGameplayTagContainer Container;

	if (bUseDataAssetMode && TagsConfig)
	{
		return TagsConfig->GetAllTags();
	}

	// 纭紪鐮佹ā寮?
	for (const auto& Pair : BehaviorTags)
	{
		Container.AddTag(Pair.Value);
	}
	for (const auto& Pair : WeaponTags)
	{
		Container.AddTag(Pair.Value);
	}
	for (const auto& Pair : CombatTags)
	{
		Container.AddTag(Pair.Value);
	}
	for (const auto& Pair : EventTags)
	{
		Container.AddTag(Pair.Value);
	}
	for (const auto& Pair : SkillTags)
	{
		Container.AddTag(Pair.Value);
	}
	for (const auto& Pair : EffectTags)
	{
		Container.AddTag(Pair.Value);
	}
	return Container;
}

FGameplayTag USoulGameTagsManager::GetTag(const FName& TagName) const
{
	if (const FGameplayTag* Found = BehaviorTags.Find(TagName))
		return *Found;
	if (const FGameplayTag* Found = WeaponTags.Find(TagName))
		return *Found;
	if (const FGameplayTag* Found = CombatTags.Find(TagName))
		return *Found;
	if (const FGameplayTag* Found = EventTags.Find(TagName))
		return *Found;
	if (const FGameplayTag* Found = SkillTags.Find(TagName))
		return *Found;
	if (const FGameplayTag* Found = EffectTags.Find(TagName))
		return *Found;
	return FGameplayTag::EmptyTag;
}

FGameplayTag USoulGameTagsManager::GetBehaviorTag(const FName& BehaviorName) const
{
	if (const FGameplayTag* Found = BehaviorTags.Find(BehaviorName))
		return *Found;
	return FGameplayTag::EmptyTag;
}

FGameplayTag USoulGameTagsManager::GetWeaponTag(const FName& WeaponName) const
{
	if (const FGameplayTag* Found = WeaponTags.Find(WeaponName))
		return *Found;
	return FGameplayTag::EmptyTag;
}

FGameplayTag USoulGameTagsManager::GetCombatTag(const FName& CombatName) const
{
	if (const FGameplayTag* Found = CombatTags.Find(CombatName))
		return *Found;
	return FGameplayTag::EmptyTag;
}

FGameplayTag USoulGameTagsManager::GetEventTag(const FName& EventName) const
{
	if (const FGameplayTag* Found = EventTags.Find(EventName))
		return *Found;
	return FGameplayTag::EmptyTag;
}

FGameplayTag USoulGameTagsManager::GetSkillTag(const FName& SkillName) const
{
	if (const FGameplayTag* Found = SkillTags.Find(SkillName))
		return *Found;
	return FGameplayTag::EmptyTag;
}

FGameplayTag USoulGameTagsManager::GetEffectTag(const FName& EffectName) const
{
	if (const FGameplayTag* Found = EffectTags.Find(EffectName))
		return *Found;
	return FGameplayTag::EmptyTag;
}

FText USoulGameTagsManager::GetTagDisplayName(const FGameplayTag& Tag) const
{
	// 棣栧厛灏濊瘯浠?DataAsset 获取
	if (bUseDataAssetMode && TagsConfig)
	{
		// 閬嶅巻鏌ユ壘鍖归厤鐨勬爣绛?
		// ...
	}

	// 榛樿杩斿洖鏍囩鍚?
	return FText::FromName(Tag.GetTagName());
}