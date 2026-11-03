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
		WebsGameState->RemainingTime = MatchDuration;
		WebsGameState->CountdownRemaining = 5;
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
	WebsGameState->RemainingTime = MatchDuration;

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

	// Detener el cronómetro.
	GetWorldTimerManager().ClearTimer(MatchTimerHandle);

	// Guardar el ganador (nullptr si hubo empate).
	WebsGameState->WinnerPlayerState = Winner;

	// Cambiar el estado de la partida.
	WebsGameState->MatchPhase = EWebsMatchPhase::Finished;

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

	if (!WebsGameState ||
		WebsGameState->MatchPhase != EWebsMatchPhase::Waiting)
	{
		return;
	}

	// Todavía faltan jugadores.
	if (ReadyPlayers.Num() < RequiredPlayers)
	{
		return;
	}

	UE_LOG(LogTemp,Warning,TEXT("WEBS: All players ready! Starting countdown..."));
	
	StartCountdown();
	
}


void AWebsGameMode::StartCountdown()
{
	AWebsGameState* WebsGameState = GetGameState<AWebsGameState>();

	if (!WebsGameState ||WebsGameState->MatchPhase != EWebsMatchPhase::Waiting)
	{
		return;
	}

	WebsGameState->MatchPhase = EWebsMatchPhase::Countdown;
	WebsGameState->CountdownRemaining = 5;

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

	UE_LOG(LogTemp,Warning,TEXT("WEBS: Countdown: %d"),WebsGameState->CountdownRemaining);

	if (WebsGameState->CountdownRemaining <= 0)
	{
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);

		StartWebsMatch();
	}
}
