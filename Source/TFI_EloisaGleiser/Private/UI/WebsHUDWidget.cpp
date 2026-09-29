// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WebsHUDWidget.h"
#include "Components/TextBlock.h"
#include "Framework/WebsGameState.h"
#include "Framework/WebsPlayerState.h"

void UWebsHUDWidget::UpdateCapturedZones(int32 NewCapturedZones)
{
	if (TXT_CapturedZones)
	{
		TXT_CapturedZones->SetText(
			FText::AsNumber(NewCapturedZones)
		);
	}
}

void UWebsHUDWidget::UpdateScoreboard()
{
	AWebsGameState* WebsGameState =
		GetWorld()->GetGameState<AWebsGameState>();

	if (!WebsGameState)
	{
		return;
	}

	TArray<UTextBlock*> PlayerTexts =
	{
		TXT_Player1,
		TXT_Player2,
		TXT_Player3,
		TXT_Player4
	};

	// Primero ocultamos todas las filas.
	for (UTextBlock* PlayerText : PlayerTexts)
	{
		if (PlayerText)
		{
			PlayerText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	const TArray<APlayerState*>& Players =
		WebsGameState->PlayerArray;

	for (int32 i = 0; i < Players.Num() && i < PlayerTexts.Num(); i++)
	{
		AWebsPlayerState* WebsPlayerState =Cast<AWebsPlayerState>(Players[i]);

		if (!WebsPlayerState || !PlayerTexts[i])
		{
			continue;
		}

		const FString ScoreText = FString::Printf(
			TEXT("Player %d: %d"),
			i + 1,
			WebsPlayerState->GetCapturedZones()
		);

		PlayerTexts[i]->SetText(
			FText::FromString(ScoreText)
		);

		PlayerTexts[i]->SetVisibility(
			ESlateVisibility::Visible
		);
	}
}
