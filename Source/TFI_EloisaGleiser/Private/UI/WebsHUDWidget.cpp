// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WebsHUDWidget.h"
#include "Components/TextBlock.h"
#include "Framework/WebsGameState.h"
#include "Framework/WebsPlayerState.h"
#include "Components/ProgressBar.h"
#include "TimerManager.h"
#include "UI/WebsQuestTrackerWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Border.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
#include "Framework/WebsPlayerController.h"


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

		FString PlayerName = TEXT("Player");

		const FLinearColor PlayerColor =
			WebsPlayerState->GetPlayerColor();

		if (PlayerColor == FLinearColor::FromSRGBColor(FColor(88, 38, 143)))
		{
			PlayerName = TEXT("Violet");
		}
		else if (PlayerColor == FLinearColor::FromSRGBColor(FColor(8, 119, 123)))
		{
			PlayerName = TEXT("Cyan");
		}
		else if (PlayerColor == FLinearColor::FromSRGBColor(FColor(163, 75, 10)))
		{
			PlayerName = TEXT("Orange");
		}
		else if (PlayerColor == FLinearColor::FromSRGBColor(FColor(163, 41, 96)))
		{
			PlayerName = TEXT("Pink");
		}

		const FString ScoreText = FString::Printf(
			TEXT("%s: %d"),
			*PlayerName,
			WebsPlayerState->GetTotalScore()
		);

		PlayerTexts[i]->SetText(FText::FromString(ScoreText));
		
		PlayerTexts[i]->SetColorAndOpacity(FSlateColor(WebsPlayerState->GetPlayerColor()));

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
	
	UWebsQuestTrackerWidget* QuestWidget = nullptr;

	if (TObjectPtr<UWebsQuestTrackerWidget>* FoundWidget = QuestWidgets.Find(QuestID))
	{
		QuestWidget = FoundWidget->Get();
	}
	
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
	
	QuestWidget->UpdateQuestDisplay(QuestData);
}


void UWebsHUDWidget::UpdateMatchHUD()
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	const AWebsGameState* WebsGameState =World->GetGameState<AWebsGameState>();

	if (!WebsGameState)
	{
		return;
	}
	
	if (Text_WaitingPlayers)
	{
		const bool bWaiting =
			WebsGameState->MatchPhase == EWebsMatchPhase::Waiting;

		Text_WaitingPlayers->SetVisibility(
			bWaiting
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed
		);
	}
	
	if (Text_Countdown)
	{
		if (WebsGameState->MatchPhase == EWebsMatchPhase::Countdown)
		{
			Text_Countdown->SetVisibility(ESlateVisibility::Visible);

			Text_Countdown->SetText(FText::AsNumber(WebsGameState->CountdownRemaining));
		}
		else
		{
			Text_Countdown->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	
	if (Text_MatchTimer)
	{
		const int32 TotalSeconds =FMath::Max(0, WebsGameState->RemainingTime);

		const int32 Minutes = TotalSeconds / 60;
		const int32 Seconds = TotalSeconds % 60;

		const FString TimeString = FString::Printf(TEXT("%02d:%02d"),Minutes,Seconds);

		Text_MatchTimer->SetText(FText::FromString(TimeString));
	}
}


void UWebsHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (BTN_MainMenu)
	{
		BTN_MainMenu->OnClicked.AddUniqueDynamic(this,&UWebsHUDWidget::OnMainMenuClicked);
	}
	
	AWebsGameState* WebsGameState =GetWorld()->GetGameState<AWebsGameState>();

	if (WebsGameState)
	{
		CachedWebsGameState = WebsGameState;

		WebsGameState->OnMatchDataChanged.AddUObject(this,&UWebsHUDWidget::HandleMatchDataChanged);
	}
	
	HandleMatchDataChanged();
}

void UWebsHUDWidget::HandleMatchDataChanged()
{
	UpdateMatchHUD();
	UpdateMatchResult();
}

void UWebsHUDWidget::NativeDestruct()
{
	if (IsValid(CachedWebsGameState))
	{
		CachedWebsGameState->OnMatchDataChanged.RemoveAll(this);
	}

	CachedWebsGameState = nullptr;

	Super::NativeDestruct();
}

void UWebsHUDWidget::UpdateMatchResult()
{
    if (!Border_MatchResult)
    {
        return;
    }

    const AWebsGameState* WebsGameState =GetWorld()->GetGameState<AWebsGameState>();

    if (!WebsGameState)
    {
        return;
    }
	
    if (WebsGameState->MatchPhase != EWebsMatchPhase::Finished)
    {
        Border_MatchResult->SetVisibility(ESlateVisibility::Collapsed);
        return;
    }
	
    APlayerController* LocalController = GetOwningPlayer();

    if (!LocalController)
    {
        return;
    }

    AWebsPlayerState* LocalPlayerState =LocalController->GetPlayerState<AWebsPlayerState>();

    if (!LocalPlayerState)
    {
        return;
    }
	//

	const bool bIsDraw = WebsGameState->bMatchIsDraw;

	if (!bIsDraw && !WebsGameState->WinnerPlayerState)
	{
		return;
	}
	
    const bool bIsWinner =WebsGameState->WinnerPlayerState == LocalPlayerState;

	if (Text_ResultTitle)
	{
		const FString ResultTitle = bIsDraw? TEXT("DRAW!"): (bIsWinner ? TEXT("VICTORY!") : TEXT("DEFEAT!"));

		Text_ResultTitle->SetText(FText::FromString(ResultTitle));
	}

	if (Text_ResultDescription)
	{
		const FString ResultDescription = bIsDraw? TEXT("The match ended in a tie!"): (bIsWinner? TEXT("You won the match!"): TEXT("Another player won the match!"));

		Text_ResultDescription->SetText(FText::FromString(ResultDescription));
	}
	
	if (Text_FinalScore)
	{
		const int32 FinalScore = LocalPlayerState->GetTotalScore();

		Text_FinalScore->SetText(FText::FromString(FString::Printf(TEXT("FINAL SCORE: %d"),FinalScore)));
	}

	Border_MatchResult->SetVisibility(ESlateVisibility::Visible);

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);

	LocalController->SetInputMode(InputMode);
	LocalController->SetShowMouseCursor(true);
}

void UWebsHUDWidget::OnMainMenuClicked()
{
	AWebsPlayerController* WebsController =Cast<AWebsPlayerController>(GetOwningPlayer());

	if (WebsController)
	{
		WebsController->RequestReturnToMainMenu();
	}
}

