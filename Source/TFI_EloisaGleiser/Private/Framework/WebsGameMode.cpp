// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Framework/WebsGameMode.h"
#include "Framework/WebsGameState.h"
#include "TimerManager.h"
#include "Framework/WebsPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameStateBase.h"


void AWebsGameMode::BeginPlay()
{
	Super::BeginPlay();

	AWebsGameState* WebsGameState = GetGameState<AWebsGameState>();

	if (WebsGameState)
	{
		WebsGameState->MatchPhase = EWebsMatchPhase::Waiting;
		WebsGameState->RefreshCharactersMovement();
		WebsGameState->RemainingTime = MatchDuration;
		WebsGameState->CountdownRemaining = 5;
		WebsGameState->NotifyMatchDataChanged();
	}

	UE_LOG(LogTemp,Warning,TEXT("WEBS: Waiting for players..."));
	CheckAllPlayersReady();
}

void AWebsGameMode::StartWebsMatch()
{
	AWebsGameState* WebsGameState = GetGameState<AWebsGameState>();

	if (!WebsGameState)
	{
		return;
	}

	// Inicializar el estado de la partida.
	WebsGameState->MatchPhase = EWebsMatchPhase::Playing;
	WebsGameState->RefreshCharactersMovement();
	WebsGameState->RemainingTime = MatchDuration;
	WebsGameState->NotifyMatchDataChanged();

	// Actualizar el tiempo una vez por segundo.
	GetWorldTimerManager().SetTimer(MatchTimerHandle,this,&AWebsGameMode::UpdateMatchTimer,1.0f,true);

	UE_LOG(LogTemp, Warning, TEXT("WEBS: Match started!"));
}

void AWebsGameMode::UpdateMatchTimer()
{
	AWebsGameState* WebsGameState = GetGameState<AWebsGameState>();

	if (!WebsGameState ||WebsGameState->MatchPhase != EWebsMatchPhase::Playing)
	{
		return;
	}

	WebsGameState->RemainingTime = FMath::Max(0,WebsGameState->RemainingTime - 1);
	
	WebsGameState->NotifyMatchDataChanged();
	
	CheckWinCondition();
	
	if (WebsGameState->MatchPhase == EWebsMatchPhase::Finished)
	{
		return;
	}

	UE_LOG(LogTemp,Warning,TEXT("WEBS: Time remaining: %d"),WebsGameState->RemainingTime);

	if (WebsGameState->RemainingTime <= 0)
	{
		FinishMatchByTime();
	}
	
}


void AWebsGameMode::CheckWinCondition()
{
	AWebsGameState* WebsGameState = GetGameState<AWebsGameState>();

	if (!WebsGameState ||WebsGameState->MatchPhase != EWebsMatchPhase::Playing)
	{
		return;
	}

	// Revisar los puntajes de todos los jugadores.
	for (APlayerState* Player : WebsGameState->PlayerArray)
	{
		AWebsPlayerState* WebsPlayerState =Cast<AWebsPlayerState>(Player);

		if (!WebsPlayerState)
		{
			continue;
		}

		// Comprobar si alcanzó el puntaje objetivo.
		if (WebsPlayerState->GetTotalScore() >= ScoreToWin)
		{
			UE_LOG(LogTemp,Warning,TEXT("WEBS: Player %s reached %d points and wins!"),*WebsPlayerState->GetPlayerName(),ScoreToWin);

			FinishWebsMatch(WebsPlayerState);
			return;
		}
	}
}


void AWebsGameMode::FinishWebsMatch(AWebsPlayerState* Winner)
{
	AWebsGameState* WebsGameState = GetGameState<AWebsGameState>();

	if (!WebsGameState ||WebsGameState->MatchPhase == EWebsMatchPhase::Finished)
	{
		return;
	}
	
	GetWorldTimerManager().ClearTimer(MatchTimerHandle);

	WebsGameState->WinnerPlayerState = Winner;
	WebsGameState->bMatchIsDraw = (Winner == nullptr);

	WebsGameState->MatchPhase = EWebsMatchPhase::Finished;
	WebsGameState->RefreshCharactersMovement();
	WebsGameState->NotifyMatchDataChanged();

	if (Winner)
	{
		UE_LOG(LogTemp,Warning,TEXT("WEBS: MATCH FINISHED! Winner: %s"),*Winner->GetPlayerName());
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("WEBS: MATCH FINISHED! Draw!"));
	}
}


void AWebsGameMode::FinishMatchByTime()
{
	AWebsGameState* WebsGameState = GetGameState<AWebsGameState>();

	if (!WebsGameState ||WebsGameState->MatchPhase != EWebsMatchPhase::Playing)
	{
		return;
	}

	AWebsPlayerState* Winner = nullptr;
	int32 HighestScore = -1;
	bool bIsDraw = false;

	for (APlayerState* Player : WebsGameState->PlayerArray)
	{
		AWebsPlayerState* WebsPlayerState =Cast<AWebsPlayerState>(Player);

		if (!WebsPlayerState)
		{
			continue;
		}

		const int32 PlayerScore = WebsPlayerState->GetTotalScore();

		if (PlayerScore > HighestScore)
		{
			HighestScore = PlayerScore;
			Winner = WebsPlayerState;
			bIsDraw = false;
		}
		else if (PlayerScore == HighestScore)
		{
			bIsDraw = true;
		}
	}

	FinishWebsMatch(bIsDraw ? nullptr : Winner);
}


void AWebsGameMode::RegisterPlayerReady(APlayerController* PlayerController)
{
	if (!IsValid(PlayerController))
	{
		return;
	}

	// TSet evita registrar dos veces al mismo jugador.
	ReadyPlayers.Add(PlayerController);

	UE_LOG(LogTemp,Warning,TEXT("WEBS: Ready players: %d / %d"),ReadyPlayers.Num(),RequiredPlayers);

	CheckAllPlayersReady();
}

void AWebsGameMode::CheckAllPlayersReady()
{
	AWebsGameState* WebsGameState = GetGameState<AWebsGameState>();

	if (!WebsGameState ||WebsGameState->MatchPhase != EWebsMatchPhase::Waiting)
	{
		return;
	}

	CheckPlayerJoinWait();
}


void AWebsGameMode::StartCountdown()
{
	AWebsGameState* WebsGameState = GetGameState<AWebsGameState>();

	if (!WebsGameState ||WebsGameState->MatchPhase != EWebsMatchPhase::Waiting)
	{
		return;
	}

	WebsGameState->MatchPhase = EWebsMatchPhase::Countdown;
	WebsGameState->RefreshCharactersMovement();
	WebsGameState->CountdownRemaining = 5;
	WebsGameState->NotifyMatchDataChanged();

	UE_LOG(LogTemp,Warning,TEXT("WEBS: Countdown started! 5"));

	GetWorldTimerManager().SetTimer(CountdownTimerHandle,this,&AWebsGameMode::UpdateCountdown,1.0f,true);
}

void AWebsGameMode::UpdateCountdown()
{
	AWebsGameState* WebsGameState = GetGameState<AWebsGameState>();

	if (!WebsGameState ||WebsGameState->MatchPhase != EWebsMatchPhase::Countdown)
	{
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
		return;
	}

	WebsGameState->CountdownRemaining--;
	WebsGameState->NotifyMatchDataChanged();

	UE_LOG(LogTemp,Warning,TEXT("WEBS: Countdown: %d"),WebsGameState->CountdownRemaining);

	if (WebsGameState->CountdownRemaining <= 0)
	{
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);

		StartWebsMatch();
	}
}

void AWebsGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!NewPlayer)
	{
		return;
	}

	AWebsPlayerState* WebsPlayerState =NewPlayer->GetPlayerState<AWebsPlayerState>();

	if (!WebsPlayerState)
	{
		return;
	}

	// Paleta para los cuatro jugadores
	const TArray<FLinearColor> PlayerColors =
	{
		FLinearColor::FromSRGBColor(FColor(88, 38, 143)),  // Violeta oscuro
		FLinearColor::FromSRGBColor(FColor(8, 119, 123)),  // Turquesa oscuro
		FLinearColor::FromSRGBColor(FColor(163, 75, 10)),  // Naranja oscuro
		FLinearColor::FromSRGBColor(FColor(163, 41, 96))   // Rosa oscuro
	};

	// Buscar colores que ya estén asignados
	TArray<FLinearColor> UsedColors;

	if (AWebsGameState* WebsGameState =GetGameState<AWebsGameState>())
	{
		for (APlayerState* Player : WebsGameState->PlayerArray)
		{
			AWebsPlayerState* OtherPlayer =Cast<AWebsPlayerState>(Player);

			if (OtherPlayer &&
				OtherPlayer != WebsPlayerState)
			{
				UsedColors.Add(OtherPlayer->GetPlayerColor());
			}
		}
	}

	// Asignar el primer color disponible
	for (const FLinearColor& Color : PlayerColors)
	{
		if (!UsedColors.Contains(Color))
		{
			WebsPlayerState->SetPlayerColor(Color);

			UE_LOG(LogTemp, Warning,TEXT("WEBS: Assigned color %s to player %s"),*Color.ToString(),*WebsPlayerState->GetPlayerName());

			return;
		}
	}

	UE_LOG(LogTemp, Warning,TEXT("WEBS: No available player colors"));
}

void AWebsGameMode::StartPlayerJoinWait()
{
	AWebsGameState* WebsGameState = GetGameState<AWebsGameState>();

	if (!WebsGameState ||WebsGameState->MatchPhase != EWebsMatchPhase::Waiting ||GetWorldTimerManager().IsTimerActive(PlayerJoinTimerHandle))
	{
		return;
	}

	UE_LOG(LogTemp, Warning,TEXT("WEBS: Waiting %d seconds for more players..."),PlayerJoinWaitTime);

	GetWorldTimerManager().SetTimer(PlayerJoinTimerHandle,this,&AWebsGameMode::FinishPlayerJoinWait,PlayerJoinWaitTime,false);
}

void AWebsGameMode::FinishPlayerJoinWait()
{
	GetWorldTimerManager().ClearTimer(PlayerJoinTimerHandle);

	AWebsGameState* WebsGameState = GetGameState<AWebsGameState>();

	if (!WebsGameState ||WebsGameState->MatchPhase != EWebsMatchPhase::Waiting)
	{
		return;
	}
	
	if (ReadyPlayers.Num() < RequiredPlayers)
	{
		return;
	}

	StartCountdown();
}

void AWebsGameMode::CheckPlayerJoinWait()
{
	AWebsGameState* WebsGameState = GetGameState<AWebsGameState>();

	if (!WebsGameState ||WebsGameState->MatchPhase != EWebsMatchPhase::Waiting)
	{
		return;
	}

	const int32 PlayerCount = ReadyPlayers.Num();

	if (PlayerCount < RequiredPlayers)
	{
		GetWorldTimerManager().ClearTimer(PlayerJoinTimerHandle);
		return;
	}

	if (PlayerCount >= MaxPlayers)
	{
		FinishPlayerJoinWait();
		return;
	}
	
	StartPlayerJoinWait();
}

void AWebsGameMode::Logout(AController* Exiting)
{
	if (APlayerController* ExitingPlayer =Cast<APlayerController>(Exiting))
	{
		ReadyPlayers.Remove(ExitingPlayer);
	}

	Super::Logout(Exiting);

	CheckPlayerJoinWait();
}