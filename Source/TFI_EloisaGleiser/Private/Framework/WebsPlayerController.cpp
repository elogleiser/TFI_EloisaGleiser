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
#include "Gameplay/CaptureZone.h"
#include "Framework/WebsPlayerState.h"
#include "UI/WebsHUDWidget.h"
#include "Gameplay/WebsSilkComponent.h"
#include "Gameplay/WebPatterns/WebPatternDefinition.h"
#include "Characters/WebsCharacter.h"



void AWebsPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UWebsHUDWidget>(this,HUDWidgetClass);

		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
			HUDWidget->UpdateScoreboard();
		}
	}
	
	InitializeSilkHUD();
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
	// Obtener el personaje local
	AWebsCharacter* WebsCharacter =Cast<AWebsCharacter>(GetPawn());

	if (!WebsCharacter || !CurrentCaptureZone)
	{
		return;
	}

	// Obtener el componente de seda
	UWebsSilkComponent* SilkComponent =WebsCharacter->GetSilkComponent();

	if (!SilkComponent)
	{
		return;
	}

	// Obtener el patrón asignado a la zona
	UWebPatternDefinition* WebPattern =CurrentCaptureZone->GetWebPattern();

	if (!WebPattern)
	{
		return;
	}

	// Comprobar si tenemos suficiente seda
	const int32 SilkCost = WebPattern->PatternData.SilkCost;

	if (!SilkComponent->HasEnoughSilk(SilkCost))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("Not enough silk to capture this zone!"));
		}

		return;
	}
//	
	if (!IsLocalController())
	{
		return;
	}

	if (!CapturePuzzle && CapturePuzzleClass)
	{
		CapturePuzzle = CreateWidget<UWebsCapturePuzzle>(this,CapturePuzzleClass);
	}
	
	if (CurrentCaptureZone)
	{
		CapturePuzzle->SetWebPattern(CurrentCaptureZone->GetWebPattern());
	}
	
	if (CapturePuzzle)
	{
		CapturePuzzle->OnExitRequested.AddUniqueDynamic(this,&AWebsPlayerController::OnCapturePuzzleExitRequested);
		CapturePuzzle->OnPuzzleCompleted.AddUniqueDynamic(this,&AWebsPlayerController::OnCapturePuzzleCompleted);
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

void AWebsPlayerController::CloseCapturePuzzle()
{
	if (CapturePuzzle && CapturePuzzle->IsInViewport())
	{
		CapturePuzzle->RemoveFromParent();
	}

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	bShowMouseCursor = false;
}

void AWebsPlayerController::OnCapturePuzzleExitRequested()
{
	CloseCapturePuzzle();

	CurrentCaptureZone = nullptr;
}

void AWebsPlayerController::OnCapturePuzzleCompleted()
{
	if (!CurrentCaptureZone)
	{
		return;
	}

	ServerCompleteCapture(CurrentCaptureZone);

	CloseCapturePuzzle();

	CurrentCaptureZone = nullptr;
}

void AWebsPlayerController::ClientOpenCapturePuzzle_Implementation(ACaptureZone* CaptureZone)
{
	CurrentCaptureZone = CaptureZone;

	OpenCapturePuzzle();
}

void AWebsPlayerController::ServerCompleteCapture_Implementation(ACaptureZone* CaptureZone)
{
	if (!CaptureZone)
	{
		return;
	}

	AWebsPlayerState* WebsPlayerState =GetPlayerState<AWebsPlayerState>();

	if (!WebsPlayerState)
	{
		return;
	}

	if (!CaptureZone->IsOccupiedBy(WebsPlayerState))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.0f,
				FColor::Red,
				TEXT("SERVER | Capture rejected: player is not occupying zone")
			);
		}

		return;
	}
	
	// Obtener nuestro Character
	AWebsCharacter* WebsCharacter =
		Cast<AWebsCharacter>(GetPawn());

	if (!WebsCharacter)
	{
		return;
	}

	// Obtener el componente de seda
	UWebsSilkComponent* SilkComponent =
		WebsCharacter->GetSilkComponent();

	if (!SilkComponent)
	{
		return;
	}

	// Obtener el Data Asset asignado a la zona
	UWebPatternDefinition* WebPattern =
		CaptureZone->GetWebPattern();

	if (!WebPattern)
	{
		return;
	}

	// Leer el costo de seda
	const int32 SilkCost =
		WebPattern->PatternData.SilkCost;

	// Comprobar y descontar seda en el servidor
	if (!SilkComponent->ConsumeSilk(SilkCost))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.0f,
				FColor::Red,
				TEXT("SERVER | Capture rejected: not enough silk")
			);
		}

		return;
	}

	CaptureZone->CaptureZone(WebsPlayerState);
	
	// Debug temporal
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Green,
			FString::Printf(
				TEXT("SERVER | Capture successful! Silk remaining: %d"),
				SilkComponent->GetCurrentSilk()
			)
		);
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

void AWebsPlayerController::UpdateCapturedZonesHUD(int32 NewCapturedZones)
{
	if (!IsLocalController())
	{
		return;
	}

	if (HUDWidget)
	{
		HUDWidget->UpdateCapturedZones(NewCapturedZones);
	}
}

void AWebsPlayerController::UpdateScoreboardHUD()
{
	if (!IsLocalController())
	{
		return;
	}

	if (HUDWidget)
	{
		HUDWidget->UpdateScoreboard();
	}
}

void AWebsPlayerController::HandleSilkChanged(int32 CurrentSilk, int32 MaxSilk)
{
	if (!IsLocalController())
	{
		return;
	}

	if (HUDWidget)
	{
		HUDWidget->UpdateSilk(CurrentSilk, MaxSilk);
	}
}

void AWebsPlayerController::InitializeSilkHUD()
{
	if (!IsLocalController() || !HUDWidget)
	{
		return;
	}

	AWebsCharacter* WebsCharacter =Cast<AWebsCharacter>(GetPawn());

	if (!WebsCharacter)
	{
		return;
	}

	UWebsSilkComponent* SilkComponent =WebsCharacter->GetSilkComponent();

	if (!SilkComponent)
	{
		return;
	}

	// Escuchar los cambios de seda
	SilkComponent->OnSilkChanged.AddUniqueDynamic(this,&AWebsPlayerController::HandleSilkChanged);

	// Mostrar el valor inicial
	HandleSilkChanged(SilkComponent->GetCurrentSilk(),SilkComponent->GetMaxSilk());	
}

void AWebsPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	InitializeSilkHUD();
}

void AWebsPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	InitializeSilkHUD();
}
