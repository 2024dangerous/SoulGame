// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameHUD/UI_Interaction.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/TextBlock.h"

void UUI_Interaction::SetInteractionText(FText InteractionText)
{
    if (TextBlock_Interaction)
    {
        TextBlock_Interaction->SetText(InteractionText);
    }
}

void UUI_Interaction::PlayInteractionAnimation()
{
    PlayAnimation(Interaction_Visable, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f, false);
}

