// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Framework/WebsGameState.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Characters/WebsCharacter.h"
#include "EngineUtils.h"

void AWebsGameState::ReportGameplayObjective(EQuestObjectiveType ObjectiveType, FName TargetID, int32 Amount,
	APlayerState* InstigatorPlayer)
{
	// Los eventos de progreso se generan en el servidor
	if (!HasAuthority())
	{
		return;
	}

	if (!InstigatorPlayer || TargetID.IsNone() || Amount <= 0)
	{
		return;
	}

	OnGameplayObjectiveEvent.Broadcast(ObjectiveType,TargetID,Amount,InstigatorPlayer);

	UE_LOG(LogTemp, Warning,TEXT("Quest Event: %s | Player: %s"),*TargetID.ToString(),*InstigatorPlayer->GetPlayerName());
	
	
}

void AWebsGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWebsGameState, MatchPhase);
	DOREPLIFETIME(AWebsGameState, RemainingTime);
	DOREPLIFETIME(AWebsGameState, WinnerPlayerState);
	DOREPLIFETIME(AWebsGameState, CountdownRemaining);
	DOREPLIFETIME(AWebsGameState, bMatchIsDraw);
}

void AWebsGameState::OnRep_MatchPhase()
{
	UE_LOG(LogTemp,Warning,TEXT("Match Phase changed: %d"),static_cast<int32>(MatchPhase));
	RefreshCharactersMovement();
	NotifyMatchDataChanged();
}

void AWebsGameState::OnRep_WinnerPlayerState()
{
	if (WinnerPlayerState)
	{
		UE_LOG(LogTemp,Warning,TEXT("WEBS: Winner replicated: %s"),*WinnerPlayerState->GetPlayerName());
	}
	
	NotifyMatchDataChanged();
}


void AWebsGameState::RefreshCharactersMovement()
{
	for (TActorIterator<AWebsCharacter> It(GetWorld()); It; ++It)
	{
		It->UpdateMovementForMatchPhase();
	}
}
void AWebsGameState::NotifyMatchDataChanged()
{
	OnMatchDataChanged.Broadcast();
}
void AWebsGameState::OnRep_RemainingTime()
{
	NotifyMatchDataChanged();
}

void AWebsGameState::OnRep_CountdownRemaining()
{
	NotifyMatchDataChanged();
}

void AWebsGameState::OnRep_MatchIsDraw()
{
	NotifyMatchDataChanged();
}