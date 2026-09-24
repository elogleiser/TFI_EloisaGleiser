// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Framework/WebsPlayerController.h"
#include "UI/WebsPauseMenu.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WebsCapturePuzzle.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

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

void AWebsPlayerController::OpenCapturePuzzle()
{
	if (!IsLocalController())
	{
		return;
	}

	if (!CapturePuzzle && CapturePuzzleClass)
	{
		CapturePuzzle = CreateWidget<UWebsCapturePuzzle>(this,CapturePuzzleClass);
	}

	if (CapturePuzzle && !CapturePuzzle->IsInViewport())
	{
		CapturePuzzle->AddToViewport();

		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(CapturePuzzle->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		SetInputMode(InputMode);
		bShowMouseCursor = true;
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

	PauseMenu = CreateWidget<UWebsPauseMenu>(this, PauseMenuClass);

	if (PauseMenu)
	{
		PauseMenu->OnResumeRequested.AddDynamic(this,&AWebsPlayerController::OnResumeRequested);
		PauseMenu->OnMainMenuRequested.AddDynamic(this,&AWebsPlayerController::OnMainMenuRequested);
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

void AWebsPlayerController::OnResumeRequested()
{
	ClosePauseMenu();
}

void AWebsPlayerController::OnMainMenuRequested()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UMultiplayerSessionsSubsystem* SessionsSubsystem =GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>())
		{
			SessionsSubsystem->MultijugadorAlDestruirSesionCompletada.AddDynamic(this,&AWebsPlayerController::OnSessionDestroyed);

			SessionsSubsystem->DestruirSesion();
			return;
		}
	}

	ReturnToMainMenu();
}

void AWebsPlayerController::OnSessionDestroyed(bool bWasSuccessful)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UMultiplayerSessionsSubsystem* SessionsSubsystem =GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>())
		{
			SessionsSubsystem->MultijugadorAlDestruirSesionCompletada.RemoveDynamic(this,&AWebsPlayerController::OnSessionDestroyed);
		}
	}

	ReturnToMainMenu();
}

void AWebsPlayerController::ReturnToMainMenu()
{
	UGameplayStatics::OpenLevel(this,FName(TEXT("L_MainMenu")));
}

void AWebsPlayerController::ClientOpenCapturePuzzle_Implementation()
{
	OpenCapturePuzzle();
}
