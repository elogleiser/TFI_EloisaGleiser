// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WebsCapturePuzzle.h"
#include "Components/Button.h"
#include "UI/WebsDrawingCanvas.h"
#include "Components/TextBlock.h"
#include "Gameplay/WebPatterns/WebPatternDefinition.h"
#include "UI/WebsDrawingCanvas.h"


void UWebsCapturePuzzle::NativeOnInitialized()
{
	if (BTN_Exit)
	{
		BTN_Exit->OnClicked.AddDynamic(this,&UWebsCapturePuzzle::OnExitClicked);
	}

	if (BTN_FakeComplete)
	{
		BTN_FakeComplete->OnClicked.AddDynamic(this,&UWebsCapturePuzzle::OnFakeCompleteClicked);
	}

	// Escuchar cuando el jugador termina de dibujar
	if (DrawingCanvas)
	{
		DrawingCanvas->OnDrawingFinished.AddUniqueDynamic(this,&UWebsCapturePuzzle::HandleDrawingFinished);
	}
}


void UWebsCapturePuzzle::OnExitClicked()
{
	OnExitRequested.Broadcast();
}

void UWebsCapturePuzzle::OnFakeCompleteClicked()
{
	OnPuzzleCompleted.Broadcast();
}

void UWebsCapturePuzzle::HandleDrawingFinished(float Accuracy)
{
	if (!DrawingCanvas)
	{
		return;
	}

	const float RequiredAccuracy =
		DrawingCanvas->GetRequiredAccuracy();

	const bool bSuccess = Accuracy >= RequiredAccuracy;

	if (TXT_Accuracy)
	{
		TXT_Accuracy->SetText(
			FText::FromString(
				FString::Printf(
					TEXT("%s - Accuracy: %.1f%% / Required: %.0f%%"),
					bSuccess ? TEXT("SUCCESS") : TEXT("TRY AGAIN"),
					Accuracy * 100.0f,
					RequiredAccuracy * 100.0f
				)
			)
		);
	}

	if (bSuccess)
	{
		OnPuzzleCompleted.Broadcast();
	}
}

void UWebsCapturePuzzle::SetWebPattern(UWebPatternDefinition* NewPattern)
{
	if (!DrawingCanvas)
	{
		return;
	}

	DrawingCanvas->SetPattern(NewPattern);
}
