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
#include "Gameplay/WebPatterns/WebPatternValidator.h"
#include "Framework/WebsPlayerState.h"
#include "Gameplay/Quests/WebsQuestComponent.h"
#include "Gameplay/WebsShootingComponent.h"
#include "EngineUtils.h"
#include "Components/SphereComponent.h"
#include "Framework/WebsGameMode.h"
#include "Framework/WebsGameState.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

void AWebsPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsLocalController())
	{
		return;
	}

	const FString CurrentLevelName =UGameplayStatics::GetCurrentLevelName(this, true);
	
	if (CurrentLevelName == TEXT("L_MainMenu") ||CurrentLevelName == TEXT("Lobby"))
	{
		return;
	}

	
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
	InitializeQuestHUD();
	TrySetPlayerReady();
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
		
		if (EnhancedInput && IA_ToggleQuests)
		{
        		EnhancedInput->BindAction(IA_ToggleQuests,ETriggerEvent::Started,this,&AWebsPlayerController::ToggleQuests);
		}
		
		if (IA_Shoot)
		{
			EnhancedInput->BindAction(IA_Shoot,ETriggerEvent::Started,this,&AWebsPlayerController::Shoot);
		}
		
		if (IA_Interact)
		{
			EnhancedInput->BindAction(IA_Interact,ETriggerEvent::Started,this,&AWebsPlayerController::Interact);
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
	
	AWebsPlayerState* WebsPlayerState = GetPlayerState<AWebsPlayerState>();

	if (!WebsPlayerState)
	{
		return;
	}

	// La zona ya pertenece a este jugador
	if (CurrentCaptureZone->GetOwningPlayer() == WebsPlayerState)
	{
		if (HUDWidget)
		{
			HUDWidget->ShowCaptureMessage(TEXT("This zone is already yours!"),false);
		}

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
		if (HUDWidget)
		{
			HUDWidget->ShowCaptureMessage(TEXT("Not enough silk!"),false);
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

void AWebsPlayerController::OnCapturePuzzleCompleted(const TArray<FVector2D>& DrawnPoints)
{
	if (!CurrentCaptureZone)
	{
		return;
	}

	ServerCompleteCapture(CurrentCaptureZone, DrawnPoints);

}

void AWebsPlayerController::ClientOpenCapturePuzzle_Implementation(ACaptureZone* CaptureZone)
{
	CurrentCaptureZone = CaptureZone;

	OpenCapturePuzzle();
}

void AWebsPlayerController::ServerCompleteCapture_Implementation(ACaptureZone* CaptureZone, const TArray<FVector2D>& DrawnPoints)
{
	AWebsGameState* WebsGameState =GetWorld()->GetGameState<AWebsGameState>();

	if (!WebsGameState ||
		WebsGameState->MatchPhase != EWebsMatchPhase::Playing)
	{
		ClientCaptureResult(false,TEXT("Wait for the match to start!"));

		return;
	}
	
	
	
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
		if (HUDWidget)
		{
			HUDWidget->ShowCaptureMessage(TEXT("Capture rejected: player is not occupying zone!"),false);
		}
		
		return;
	}
	
	// Obtener nuestro Character
	AWebsCharacter* WebsCharacter =Cast<AWebsCharacter>(GetPawn());

	if (!WebsCharacter)
	{
		return;
	}

	// Obtener el componente de seda
	UWebsSilkComponent* SilkComponent =WebsCharacter->GetSilkComponent();

	if (!SilkComponent)
	{
		return;
	}

	// Obtener el Data Asset asignado a la zona
	UWebPatternDefinition* WebPattern =CaptureZone->GetWebPattern();

	if (!WebPattern)
	{
		return;
	}

	// Validar la cantidad de puntos recibidos
	constexpr int32 MaxDrawnPoints = 2048;

	if (DrawnPoints.Num() < 2 || DrawnPoints.Num() > MaxDrawnPoints)
	{
		ClientCaptureResult(false, TEXT("Invalid drawing!"));
		return;
	}

	// Validar que todos los puntos estén normalizados
	for (const FVector2D& Point : DrawnPoints)
	{
		if (!FMath::IsFinite(Point.X) ||!FMath::IsFinite(Point.Y) ||Point.X < 0.0 || Point.X > 1.0 ||Point.Y < 0.0 || Point.Y > 1.0)
		{
			ClientCaptureResult(false,TEXT("Invalid drawing!"));
			return;
		}
	}

	// Calcular la precisión en el servidor
	const float Accuracy = FWebPatternValidator::CalculateAccuracy(WebPattern->PatternData.TargetPoints,DrawnPoints);

	const float RequiredAccuracy =WebPattern->PatternData.RequiredAccuracy;

	if (Accuracy < RequiredAccuracy)
	{
		ClientCaptureResult(false, TEXT("Drawing failed!"));
		return;
	}
	
	if (CaptureZone->GetOwningPlayer() == WebsPlayerState)
	{
		ClientCaptureResult(false, TEXT("This zone is already yours!"));
		return;
	}
	
	// Leer el costo de seda
	const int32 SilkCost =WebPattern->PatternData.SilkCost;

	
	// Comprobar y descontar seda en el servidor
	if (!SilkComponent->ConsumeSilk(SilkCost))
	{
		ClientCaptureResult(false, TEXT("Not enough silk!"));
		return;
	}

	CaptureZone->CaptureZone(WebsPlayerState);
	
	ClientCaptureResult(true, TEXT("Zone captured!"));
	
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
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	if (OnlineSubsystem)
	{
		IOnlineSessionPtr SessionInterface =OnlineSubsystem->GetSessionInterface();

		if (SessionInterface.IsValid() &&SessionInterface->GetNamedSession(NAME_GameSession))
		{
			if (UGameInstance* GameInstance = GetGameInstance())
			{
				if (UMultiplayerSessionsSubsystem* SessionsSubsystem =GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>())
				{
					SessionsSubsystem->MultijugadorAlDestruirSesionCompletada.AddUniqueDynamic(this,&AWebsPlayerController::OnSessionDestroyed);

					SessionsSubsystem->DestruirSesion();
					return;
				}
			}
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
	if (GetNetMode() == NM_Client)
	{
		ClientTravel(TEXT("/Game/Maps/L_MainMenu"), TRAVEL_Absolute);
	}
	else
	{
		UGameplayStatics::OpenLevel(this, FName(TEXT("L_MainMenu")));
	}
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
	
	SilkComponent->OnSilkChanged.AddUniqueDynamic(this,&AWebsPlayerController::HandleSilkChanged);
	
	HandleSilkChanged(SilkComponent->GetCurrentSilk(),SilkComponent->GetMaxSilk());	
}

void AWebsPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	InitializeSilkHUD();
	TrySetPlayerReady();
}

void AWebsPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	InitializeSilkHUD();
	TrySetPlayerReady();
}

void AWebsPlayerController::ClientShowCaptureMessage_Implementation(const FString& Message, bool bSuccess)
{
	if (HUDWidget)
	{
		HUDWidget->ShowCaptureMessage(Message, bSuccess);
	}
}

void AWebsPlayerController::ClientCaptureResult_Implementation(bool bSuccess, const FString& Message)
{
	if (HUDWidget)
	{
		HUDWidget->ShowCaptureMessage(Message, bSuccess);
	}

	if (bSuccess)
	{
		CloseCapturePuzzle();
		CurrentCaptureZone = nullptr;
	}
}
void AWebsPlayerController::ToggleQuests()
{
	
	if (!IsLocalController())
	{
		return;
	}

	if (HUDWidget)
	{
		HUDWidget->ToggleQuestTracker();
	}
}

void AWebsPlayerController::HandleQuestStarted(FWebsQuestRuntime QuestData)
{
	if (IsLocalController() && HUDWidget)
	{
		HUDWidget->UpdateQuestTracker(QuestData);
	}
}

void AWebsPlayerController::HandleQuestProgressChanged(FWebsQuestRuntime QuestData)
{
	if (IsLocalController() && HUDWidget)
	{
		HUDWidget->UpdateQuestTracker(QuestData);
	}
}

void AWebsPlayerController::HandleQuestCompleted(FWebsQuestRuntime QuestData)
{
	if (IsLocalController() && HUDWidget)
	{
		HUDWidget->UpdateQuestTracker(QuestData);
	}
}

void AWebsPlayerController::InitializeQuestHUD()
{
	if (!IsLocalController() || !HUDWidget)
	{
		return;
	}

	AWebsCharacter* WebsCharacter = Cast<AWebsCharacter>(GetPawn());

	if (!WebsCharacter)
	{
		return;
	}

	UWebsQuestComponent* QuestComponent =WebsCharacter->GetQuestComponent();

	if (!QuestComponent)
	{
		return;
	}

	QuestComponent->OnQuestStarted.AddUniqueDynamic(this, &AWebsPlayerController::HandleQuestStarted);

	QuestComponent->OnQuestProgressChanged.AddUniqueDynamic(this, &AWebsPlayerController::HandleQuestProgressChanged);

	QuestComponent->OnQuestCompleted.AddUniqueDynamic(this, &AWebsPlayerController::HandleQuestCompleted);
	
}


void AWebsPlayerController::Shoot()
{
	if (!IsLocalController())
	{
		return;
	}
	
	AWebsGameState* WebsGameState =GetWorld()->GetGameState<AWebsGameState>();

	if (!WebsGameState ||WebsGameState->MatchPhase != EWebsMatchPhase::Playing)
	{
		UE_LOG(LogTemp,Warning,TEXT("WEBS: Shooting blocked - match not playing"));

		return;
	}

	AWebsCharacter* WebsCharacter = GetPawn<AWebsCharacter>();

	if (!WebsCharacter)
	{
		return;
	}
	
	if (WebsCharacter->IsStunned())
	{
		UE_LOG(LogTemp, Warning,TEXT("WEB SHOT BLOCKED: Player is stunned"));
		return;
	}

	UWebsShootingComponent* ShootingComponent =WebsCharacter->GetShootingComponent();

	if (ShootingComponent)
	{
		ShootingComponent->TryShoot();
	}
}

void AWebsPlayerController::Interact()
{
	if (!IsLocalController())
	{
		return;
	}

	ServerInteract();
}

void AWebsPlayerController::ServerInteract_Implementation()
{
	APawn* PlayerPawn = GetPawn();

	if (!IsValid(PlayerPawn))
	{
		return;
	}

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;

		if (!IsValid(Actor) || Actor == PlayerPawn)
		{
			continue;
		}

		USphereComponent* Sphere =Actor->FindComponentByClass<USphereComponent>();

		if (!IsValid(Sphere))
		{
			continue;
		}

		if (Sphere->IsOverlappingActor(PlayerPawn))
		{
			UE_LOG(LogTemp, Warning,TEXT("INTERACTABLE FOUND: %s"),*GetNameSafe(Actor));

			ExecuteInteraction(Actor);
			return;
		}
	}
}

void AWebsPlayerController::TrySetPlayerReady()
{
	if (!IsLocalController() || bReadySent || !IsValid(GetPawn()))
	{
		return;
	}

	bReadySent = true;

	ServerSetPlayerReady();
}

void AWebsPlayerController::ServerSetPlayerReady_Implementation()
{
	UE_LOG(LogTemp,Warning,TEXT("WEBS: Player READY on server: %s"),*GetNameSafe(PlayerState));

	AWebsGameMode* WebsGameMode =
		GetWorld()->GetAuthGameMode<AWebsGameMode>();

	if (WebsGameMode)
	{
		WebsGameMode->RegisterPlayerReady(this);
	}
}

void AWebsPlayerController::RequestReturnToMainMenu()
{
	if (!IsLocalController())
	{
		return;
	}

	OnMainMenuRequested();
}