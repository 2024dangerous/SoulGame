// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameHUD/UI_StateBar.h"
#include "Components/TextBlock.h"
#include "Components/image.h"
#include "Components/Widget.h"
#include "Animation/WidgetAnimation.h"

void UUI_StateBar::NativeConstruct()
{
	Super::NativeConstruct();


	SetDynamicMaterials();
	InitializeBarVisuals();

}
// 鑷姩鍚屾鍙橀噺鍒扮晫闈?
void UUI_StateBar::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (Text_StateBar)
	{
		Text_StateBar->SetText(FText::FromString(UpdateNameText));
	}
}
//鍒濆鍖栨潗璐紝璁剧疆鍔ㄦ€佹潗璐?
void UUI_StateBar::SetDynamicMaterials()
{
	BarBorderMID = BarBorder->GetDynamicMaterial();
    BarFillMID = BarFill->GetDynamicMaterial();
    BarGlowMID = BarGlow->GetDynamicMaterial();
}
//閲嶇疆鍔ㄧ敾鐘舵€?
void UUI_StateBar::ResetAnimatedState()
{
    BarBorderMID->SetScalarParameterValue(FName("Animate_Damage"), 0.0f);
	BarFillMID->SetScalarParameterValue(FName("Animate_Damage"), 0.0f);
	BarGlowMID->SetScalarParameterValue(FName("Animate_Damage"), 0.0f);

	BarBorderMID->SetScalarParameterValue(FName("Animate_DamageFade"), 0.0f);
	BarFillMID->SetScalarParameterValue(FName("Animate_DamageFade"), 0.0f);
	BarGlowMID->SetScalarParameterValue(FName("Animate_DamageFade"), 0.0f);

	BarBorderMID->SetScalarParameterValue(FName("Animate_Glow_AlphaChange"), 0.0f);
	BarGlowMID->SetScalarParameterValue(FName("Animate_Glow_AlphaChange"), 0.0f);

	BarBorderMID->SetScalarParameterValue(FName("Animate_Glow_ColorChange"), 0.0f);
	BarGlowMID->SetScalarParameterValue(FName("Animate_Glow_ColorChange"), 0.0f);
}
//鍒濆鍖栬鏉¤瑙夋晥鏋?
void UUI_StateBar::InitializeBarVisuals()
{
	BarBorderMID->SetScalarParameterValue(FName("Health_Current"), 1.0f);
	BarFillMID->SetScalarParameterValue(FName("Health_Current"), 1.0f);
	BarGlowMID->SetScalarParameterValue(FName("Health_Current"), 1.0f);

	BarBorderMID->SetScalarParameterValue(FName("Health_Updated"), 1.0f);
	BarFillMID->SetScalarParameterValue(FName("Health_Updated"), 1.0f);
	BarGlowMID->SetScalarParameterValue(FName("Health_Updated"), 1.0f);

	ResetAnimatedState();
}
//死亡动画
void UUI_StateBar::EventOnEliminated()
{
	PlayAnimation(OnEliminated, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f, false);
}

//璁剧疆琛€鏉＄殑褰撳墠鍊?
void UUI_StateBar::ValueChanged(float OldValue, float NewValue,float MaxValue)
{
	BarBorderMID->SetScalarParameterValue(FName("Health_Current"), (OldValue / MaxValue));
	BarFillMID->SetScalarParameterValue(FName("Health_Current"), (OldValue / MaxValue));
	BarGlowMID->SetScalarParameterValue(FName("Health_Current"), (OldValue / MaxValue));

	BarBorderMID->SetScalarParameterValue(FName("Health_Updated"), (NewValue / MaxValue));
	BarFillMID->SetScalarParameterValue(FName("Health_Updated"), (NewValue / MaxValue));
	BarGlowMID->SetScalarParameterValue(FName("Health_Updated"), (NewValue / MaxValue));

	
	ResetAnimatedState();
	
	StopAllAnimations();

	if (OldValue== NewValue) return;

	BarBorderMID->SetScalarParameterValue(FName("DamageOrHealing"), 0.f);
	BarFillMID->SetScalarParameterValue(FName("DamageOrHealing"), 0.f);
	BarGlowMID->SetScalarParameterValue(FName("DamageOrHealing"), 0.f);

	
	if (NewValue > OldValue)
	{
		
		PlayAnimation(OnHealed, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f, false);
          
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("OnHealed"));
	} 
	else
	{
		PlayAnimation(OnDamaged, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f, false);
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("OnDamaged"));
	}

	if (NewValue == 0 && OnDamaged && bIsHealth)
	{
        FWidgetAnimationDynamicEvent AnimationFinishedEvent;
        AnimationFinishedEvent.BindUFunction(this, FName("EventOnEliminated"));
        BindToAnimationFinished(OnDamaged, AnimationFinishedEvent);
	}
}

void UUI_StateBar::InitValue(float InitValue,float MaxValue)
{
	BarBorderMID->SetScalarParameterValue(FName("Health_Current"), (InitValue / MaxValue));
	BarFillMID->SetScalarParameterValue(FName("Health_Current"), (InitValue / MaxValue));
	BarGlowMID->SetScalarParameterValue(FName("Health_Current"), (InitValue / MaxValue));

	BarBorderMID->SetScalarParameterValue(FName("Health_Updated"), (InitValue / MaxValue));
	BarFillMID->SetScalarParameterValue(FName("Health_Updated"), (InitValue / MaxValue));
	BarGlowMID->SetScalarParameterValue(FName("Health_Updated"), (InitValue / MaxValue));
}

