// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Gameplay/Quests/WebsQuestTypes.h"
#include "WebsGameState.generated.h"

class APlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnGameplayObjectiveEvent,EQuestObjectiveType, ObjectiveType,FName, TargetID,int32, Amount,APlayerState*, InstigatorPlayer);
DECLARE_MULTICAST_DELEGATE(FOnMatchDataChanged);

UENUM(BlueprintType)
enum class EWebsMatchPhase : uint8
{
	Waiting,
	Countdown,
	Playing,
	Finished
};

UCLASS()
class TFI_ELOISAGLEISER_API AWebsGameState : public AGameState
{
	GENERATED_BODY()
	
	
public:
	UPROPERTY(BlueprintAssignable, Category="Webs|Quests")
	FOnGameplayObjectiveEvent OnGameplayObjectiveEvent;

	UFUNCTION(BlueprintCallable, Category="Webs|Quests")
	void ReportGameplayObjective(EQuestObjectiveType ObjectiveType,FName TargetID,int32 Amount,APlayerState* InstigatorPlayer);
	
	// Estado actual de la partida.
	UPROPERTY(ReplicatedUsing = OnRep_MatchPhase,BlueprintReadOnly,Category = "Webs|Match")
	EWebsMatchPhase MatchPhase = EWebsMatchPhase::Waiting;

	UPROPERTY(ReplicatedUsing = OnRep_RemainingTime, BlueprintReadOnly, Category = "Webs|Match")
	int32 RemainingTime = 300;

	UFUNCTION()
	void OnRep_RemainingTime();

	UFUNCTION()
	void OnRep_MatchPhase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Jugador ganador de la partida.
	// nullptr significa que todavía no hay ganador o que la partida terminó en empate.
	UPROPERTY(ReplicatedUsing = OnRep_WinnerPlayerState,BlueprintReadOnly,Category = "Webs|Match")
	TObjectPtr<APlayerState> WinnerPlayerState = nullptr;

	UFUNCTION()
	void OnRep_WinnerPlayerState();
	
	UPROPERTY(ReplicatedUsing = OnRep_CountdownRemaining, BlueprintReadOnly, Category = "Webs|Match")
	int32 CountdownRemaining = 5;

	UFUNCTION()
	void OnRep_CountdownRemaining();
	
	UPROPERTY(ReplicatedUsing = OnRep_MatchIsDraw, BlueprintReadOnly, Category = "Webs|Match")
	bool bMatchIsDraw = false;
	
	void RefreshCharactersMovement();
	
	FOnMatchDataChanged OnMatchDataChanged;

	void NotifyMatchDataChanged();

	UFUNCTION()
	void OnRep_MatchIsDraw();
};
