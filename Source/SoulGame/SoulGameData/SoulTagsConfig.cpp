// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameData/SoulTagsConfig.h"
#include "GameplayTagsManager.h"

USoulTagsConfig::USoulTagsConfig()
{
	ConfigId = FName(TEXT("Tags"));
	ConfigVersion = 1;
	ConfigDescription = FText::FromString(TEXT("Gameplay Tags Configuration"));

	// 初始化默认标签
	BehaviorTags.CategoryName = TEXT("Behavior");
	BehaviorTags.TagPrefix = TEXT("State.Behavior");
	BehaviorTags.Tags = {
		FTagConfigEntry(TEXT("Idle"), FText::FromString(TEXT("待机"))),
		FTagConfigEntry(TEXT("Attack"), FText::FromString(TEXT("攻击"))),
		FTagConfigEntry(TEXT("Rolling"), FText::FromString(TEXT("翻滚"))),
		FTagConfigEntry(TEXT("Defense"), FText::FromString(TEXT("防御"))),
		FTagConfigEntry(TEXT("Injury"), FText::FromString(TEXT("受伤"))),
		FTagConfigEntry(TEXT("Dead"), FText::FromString(TEXT("死亡")))
	};

	WeaponTags.CategoryName = TEXT("Weapon");
	WeaponTags.TagPrefix = TEXT("State.Weapon");
	WeaponTags.Tags = {
		FTagConfigEntry(TEXT("None"), FText::FromString(TEXT("无武器"))),
		FTagConfigEntry(TEXT("Melee"), FText::FromString(TEXT("拳法"))),
		FTagConfigEntry(TEXT("Sword"), FText::FromString(TEXT("剑")))
	};

	CombatTags.CategoryName = TEXT("Combat");
	CombatTags.TagPrefix = TEXT("State.Combat");
	CombatTags.Tags = {
		FTagConfigEntry(TEXT("None"), FText::FromString(TEXT("无战斗"))),
		FTagConfigEntry(TEXT("Normal"), FText::FromString(TEXT("普通战斗"))),
		FTagConfigEntry(TEXT("Boss"), FText::FromString(TEXT("Boss战")))
	};

	EventTags.CategoryName = TEXT("Event");
	EventTags.TagPrefix = TEXT("Event");
	EventTags.Tags = {
		FTagConfigEntry(TEXT("Damage"), FText::FromString(TEXT("伤害事件"))),
		FTagConfigEntry(TEXT("Dead"), FText::FromString(TEXT("死亡事件"))),
		FTagConfigEntry(TEXT("Respawn"), FText::FromString(TEXT("重生事件"))),
		FTagConfigEntry(TEXT("WeaponChange"), FText::FromString(TEXT("武器切换事件"))),
		FTagConfigEntry(TEXT("StateChange"), FText::FromString(TEXT("状态改变事件"))),
		FTagConfigEntry(TEXT("LockOn"), FText::FromString(TEXT("锁定目标事件"))),
		FTagConfigEntry(TEXT("Dodge"), FText::FromString(TEXT("闪避事件")))
	};

	SkillTags.CategoryName = TEXT("Skill");
	SkillTags.TagPrefix = TEXT("Skill");
	SkillTags.Tags = {
		FTagConfigEntry(TEXT("MeleeCombo"), FText::FromString(TEXT("拳法连击"))),
		FTagConfigEntry(TEXT("MeleeHeavy"), FText::FromString(TEXT("拳法重击"))),
		FTagConfigEntry(TEXT("SwordCombo"), FText::FromString(TEXT("剑术连击"))),
		FTagConfigEntry(TEXT("SwordHeavy"), FText::FromString(TEXT("剑术重击"))),
		FTagConfigEntry(TEXT("DodgeRoll"), FText::FromString(TEXT("翻滚闪避"))),
		FTagConfigEntry(TEXT("DodgeBackstep"), FText::FromString(TEXT("后撤闪避"))),
		FTagConfigEntry(TEXT("DefenseBlock"), FText::FromString(TEXT("格挡"))),
		FTagConfigEntry(TEXT("DefenseParry"), FText::FromString(TEXT("弹反")))
	};

	EffectTags.CategoryName = TEXT("Effect");
	EffectTags.TagPrefix = TEXT("Effect");
	EffectTags.Tags = {
		FTagConfigEntry(TEXT("Bleeding"), FText::FromString(TEXT("流血"))),
		FTagConfigEntry(TEXT("Burning"), FText::FromString(TEXT("灼烧"))),
		FTagConfigEntry(TEXT("Frozen"), FText::FromString(TEXT("冰冻"))),
		FTagConfigEntry(TEXT("Stunned"), FText::FromString(TEXT("眩晕"))),
		FTagConfigEntry(TEXT("Poisoned"), FText::FromString(TEXT("中毒"))),
		FTagConfigEntry(TEXT("Slowed"), FText::FromString(TEXT("减速")))
	};

	bIsLoaded = true;
}

FGameplayTag USoulTagsConfig::GetBehaviorTag(FName TagName) const
{
	return GetTagInternal(BehaviorTags, TagName);
}

FGameplayTag USoulTagsConfig::GetWeaponTag(FName TagName) const
{
	return GetTagInternal(WeaponTags, TagName);
}

FGameplayTag USoulTagsConfig::GetCombatTag(FName TagName) const
{
	return GetTagInternal(CombatTags, TagName);
}

FGameplayTag USoulTagsConfig::GetEventTag(FName TagName) const
{
	return GetTagInternal(EventTags, TagName);
}

FGameplayTag USoulTagsConfig::GetSkillTag(FName TagName) const
{
	return GetTagInternal(SkillTags, TagName);
}

FGameplayTag USoulTagsConfig::GetEffectTag(FName TagName) const
{
	return GetTagInternal(EffectTags, TagName);
}

FGameplayTag USoulTagsConfig::GetTag(FName TagName) const
{
	// 依次在所有分类中查找
	if (FGameplayTag Tag = GetBehaviorTag(TagName); Tag.IsValid())
		return Tag;
	if (FGameplayTag Tag = GetWeaponTag(TagName); Tag.IsValid())
		return Tag;
	if (FGameplayTag Tag = GetCombatTag(TagName); Tag.IsValid())
		return Tag;
	if (FGameplayTag Tag = GetEventTag(TagName); Tag.IsValid())
		return Tag;
	if (FGameplayTag Tag = GetSkillTag(TagName); Tag.IsValid())
		return Tag;
	if (FGameplayTag Tag = GetEffectTag(TagName); Tag.IsValid())
		return Tag;

	return FGameplayTag::EmptyTag;
}

FGameplayTagContainer USoulTagsConfig::GetAllTags() const
{
	FGameplayTagContainer Container;

	// 娣诲姞鎵€鏈夊垎绫荤殑鏍囩
	TArray<const FTagCategoryConfig*> Categories = {
		&BehaviorTags, &WeaponTags, &CombatTags,
		&EventTags, &SkillTags, &EffectTags
	};

	for (const FTagCategoryConfig* Category : Categories)
	{
		if (!Category->bEnabled)
			continue;

		for (const FTagConfigEntry& Entry : Category->Tags)
		{
			if (!Entry.bEnabled)
				continue;

			FString FullTagName = Category->TagPrefix + TEXT(".") + Entry.TagName.ToString();
			FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*FullTagName));
			if (Tag.IsValid())
			{
				Container.AddTag(Tag);
			}
		}
	}

	return Container;
}

bool USoulTagsConfig::ValidateConfig(FText& OutError)
{
	// 检查是否有重复的标签名
	TSet<FName> SeenTags;

	TArray<FTagCategoryConfig*> Categories = {
		&BehaviorTags, &WeaponTags, &CombatTags,
		&EventTags, &SkillTags, &EffectTags
	};

	for (FTagCategoryConfig* Category : Categories)
	{
		if (!Category->bEnabled)
			continue;

		for (const FTagConfigEntry& Entry : Category->Tags)
		{
			if (!Entry.bEnabled)
				continue;

			if (SeenTags.Contains(Entry.TagName))
			{
				OutError = FText::Format(
					//TEXT("Duplicate tag name: {0} in category {1}"),
					FText::FromName(Entry.TagName),
					FText::FromName(Category->CategoryName)
				);
				return false;
			}
			SeenTags.Add(Entry.TagName);
		}
	}

	return true;
}

FGameplayTag USoulTagsConfig::GetTagInternal(const FTagCategoryConfig& Category, FName TagName) const
{
	if (!Category.bEnabled)
		return FGameplayTag::EmptyTag;

	for (const FTagConfigEntry& Entry : Category.Tags)
	{
		if (Entry.bEnabled && Entry.TagName == TagName)
		{
			FString FullTagName = Category.TagPrefix + TEXT(".") + TagName.ToString();
			return FGameplayTag::RequestGameplayTag(FName(*FullTagName));
		}
	}

	return FGameplayTag::EmptyTag;
}