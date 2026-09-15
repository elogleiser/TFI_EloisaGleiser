// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Framework/WebsPlayerController.h"
#include "UI/WebsPauseMenu.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"


void AWebsPlayerController::BeginPlay()
{
	Super::BeginPlay();
}



void AWebsPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput =Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (PauseAction)
		{
			EnhancedInput->BindAction(PauseAction,ETriggerEvent::Started,this,&AWebsPlayerController::TogglePauseMenu);
		}
	}
}
void AWebsPlayerController::TogglePauseMenu()
{
	if (PauseMenu && PauseMenu->IsInViewport())
	{
		ClosePauseMenu();
	}
	else
	{
		OpenPauseMenu();
	}
}

void AWebsPlayerController::OpenPauseMenu()
{
	if (!PauseMenuClass)
	{
		return;
	}

	if (!PauseMenu)
	{
		PauseMenu = CreateWidget<UWebsPauseMenu>(this, PauseMenuClass);
	}

	if (!PauseMenu)
	{
		return;
	}

	PauseMenu->AddToViewport();

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(PauseMenu->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	SetInputMode(InputMode);
	SetShowMouseCursor(true);
}

void AWebsPlayerController::ClosePauseMenu()
{
	if (PauseMenu)
	{
		PauseMenu->RemoveFromParent();
	}

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	SetShowMouseCursor(false);
}
