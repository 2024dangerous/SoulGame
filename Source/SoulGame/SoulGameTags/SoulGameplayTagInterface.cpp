// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulGameTags/SoulGameplayTagInterface.h"

// Default implementations for ISoulGameplayTagInterface
// These are used when a class doesn't override the function
//
//FGameplayTagContainer ISoulGameplayTagInterface::GetGameplayTagContainer_Implementation() const
//{
//	return FGameplayTagContainer();
//}

//void ISoulGameplayTagInterface::AddGameplayTag_Implementation(FGameplayTag Tag)
//{
//	UE_LOG(LogTemp, Warning, TEXT("ISoulGameplayTagInterface::AddGameplayTag_Implementation - Tag: %s"), *Tag.ToString());
//}
//
//void ISoulGameplayTagInterface::RemoveGameplayTag_Implementation(FGameplayTag Tag)
//{
//	UE_LOG(LogTemp, Warning, TEXT("ISoulGameplayTagInterface::RemoveGameplayTag_Implementation - Tag: %s"), *Tag.ToString());
//}
//
//bool ISoulGameplayTagInterface::HasGameplayTag_Implementation(FGameplayTag Tag) const
//{
//	return false;
//}
//
//bool ISoulGameplayTagInterface::HasMatchingTag_Implementation(FGameplayTag Tag) const
//{
//	return false;
//}
//
//TArray<FName> ISoulGameplayTagInterface::GetGameplayTagNames_Implementation() const
//{
//	return TArray<FName>();
//}
//
//FText ISoulGameplayTagInterface::GetTagDisplayName_Implementation(FGameplayTag Tag) const
//{
//	return FText::FromName(Tag.GetTagName());
//}
