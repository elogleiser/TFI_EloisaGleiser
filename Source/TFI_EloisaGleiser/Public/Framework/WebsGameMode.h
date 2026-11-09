// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "WebsGameMode.generated.h"

class AWebsPlayerState;
class APlayerController;

UCLASS()
class TFI_ELOISAGLEISER_API AWebsGameMode : public AGameMode
{
	GENERATED_BODY()


public:

	// Duración de la partida: 5 minutos.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Webs|Match")
	int32 MatchDuration = 300;

	// Puntaje necesario para ganar.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Webs|Match")
	int32 ScoreToWin = 100;
	
	// Cantidad de jugadores que deben estar listos para iniciar.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Webs|Match")
	int32 RequiredPlayers = 2;
	
	void RegisterPlayerReady(APlayerController* PlayerController);
	
	void CheckWinCondition();
	
	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:

	virtual void BeginPlay() override;

private:

	FTimerHandle MatchTimerHandle;

	void StartWebsMatch();

	void UpdateMatchTimer();
	
	void FinishWebsMatch(AWebsPlayerState* Winner);
	
	void FinishMatchByTime();
	
	// Jugadores que confirmaron estar listos.
	UPROPERTY()
	TSet<TObjectPtr<APlayerController>> ReadyPlayers;

	// Comprueba si podemos iniciar la cuenta regresiva.
	void CheckAllPlayersReady();
	
	FTimerHandle CountdownTimerHandle;

	void StartCountdown();
	
	void UpdateCountdown();
	
};
