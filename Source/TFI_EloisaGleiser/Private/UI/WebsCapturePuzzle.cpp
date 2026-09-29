// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WebsCapturePuzzle.h"
#include "Components/Button.h"

void UWebsCapturePuzzle::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (BTN_Exit)
	{
		BTN_Exit->OnClicked.AddDynamic(this,&UWebsCapturePuzzle::OnExitClicked);
	}
	if (BTN_FakeComplete)
    	{
    		BTN_FakeComplete->OnClicked.AddDynamic(
    			this,
    			&UWebsCapturePuzzle::OnFakeCompleteClicked
    		);
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

