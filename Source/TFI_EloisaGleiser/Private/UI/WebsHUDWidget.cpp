// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WebsHUDWidget.h"
#include "Components/TextBlock.h"
#include "Framework/WebsGameState.h"
#include "Framework/WebsPlayerState.h"
#include "Components/ProgressBar.h"
#include "TimerManager.h"
#include "UI/WebsQuestTrackerWidget.h"
#include "Components/VerticalBox.h"


void UWebsHUDWidget::UpdateCapturedZones(int32 NewCapturedZones)
{
	if (TXT_CapturedZones)
	{
		TXT_CapturedZones->SetText(FText::AsNumber(NewCapturedZones));
	}
}

void UWebsHUDWidget::UpdateScoreboard()
{
	AWebsGameState* WebsGameState =GetWorld()->GetGameState<AWebsGameState>();

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

	const TArray<APlayerState*>& Players =WebsGameState->PlayerArray;

	for (int32 i = 0; i < Players.Num() && i < PlayerTexts.Num(); i++)
	{
		AWebsPlayerState* WebsPlayerState =Cast<AWebsPlayerState>(Players[i]);

		if (!WebsPlayerState || !PlayerTexts[i])
		{
			continue;
		}

		const FString ScoreText = FString::Printf(TEXT("Player %d: %d"),i + 1,WebsPlayerState->GetTotalScore());

		PlayerTexts[i]->SetText(FText::FromString(ScoreText));

		PlayerTexts[i]->SetVisibility(ESlateVisibility::Visible);
	}
}

void UWebsHUDWidget::UpdateSilk(int32 CurrentSilk, int32 MaxSilk)
{
	if (PB_Silk)
	{
		const float SilkPercent = MaxSilk > 0? static_cast<float>(CurrentSilk) / MaxSilk: 0.0f;

		PB_Silk->SetPercent(SilkPercent);
	}

	if (TXT_Silk)
	{
		TXT_Silk->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"),CurrentSilk,MaxSilk)));
	}

}

void UWebsHUDWidget::ShowCaptureMessage(const FString& Message, bool bSuccess)
{
	if (!TXT_CaptureMessage)
	{
		return;
	}

	TXT_CaptureMessage->SetText(FText::FromString(Message));

	TXT_CaptureMessage->SetColorAndOpacity(
		FSlateColor(bSuccess ? FLinearColor::Green : FLinearColor::Red)
	);

	TXT_CaptureMessage->SetVisibility(ESlateVisibility::Visible);
	
	GetWorld()->GetTimerManager().SetTimer(CaptureMessageTimer,this,&UWebsHUDWidget::HideCaptureMessage,3.0f,false);
}

void UWebsHUDWidget::HideCaptureMessage()
{
	if (TXT_CaptureMessage)
	{
		TXT_CaptureMessage->SetVisibility(ESlateVisibility::Collapsed);
	}
}
void UWebsHUDWidget::ToggleQuestTracker()
{
	for (const auto& QuestPair : QuestWidgets)
	{
		UWebsQuestTrackerWidget* QuestWidget = QuestPair.Value.Get();

		if (IsValid(QuestWidget))
		{
			QuestWidget->ToggleQuestContent();
		}
	}
}

void UWebsHUDWidget::UpdateQuestTracker(const FWebsQuestRuntime& QuestData)
{
	if (!VB_QuestList || !QuestTrackerWidgetClass || !QuestData.QuestDefinition)
	{
		return;
	}

	const FName QuestID = QuestData.QuestID;

	// Buscar si ya existe un widget para esta misión.
	UWebsQuestTrackerWidget* QuestWidget = nullptr;

	if (TObjectPtr<UWebsQuestTrackerWidget>* FoundWidget = QuestWidgets.Find(QuestID))
	{
		QuestWidget = FoundWidget->Get();
	}

	// Si todavía no existe, crear uno nuevo.
	if (!IsValid(QuestWidget))
	{
		QuestWidget = CreateWidget<UWebsQuestTrackerWidget>(GetOwningPlayer(),QuestTrackerWidgetClass);

		if (!QuestWidget)
		{
			return;
		}

		VB_QuestList->AddChildToVerticalBox(QuestWidget);

		QuestWidgets.Add(QuestID, QuestWidget);
	}

	// Actualizar únicamente la misión correspondiente.
	QuestWidget->UpdateQuestDisplay(QuestData);
}
