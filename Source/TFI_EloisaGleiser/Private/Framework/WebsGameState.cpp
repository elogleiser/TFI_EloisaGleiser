// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Framework/WebsGameState.h"
#include "GameFramework/PlayerState.h"

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

	OnGameplayObjectiveEvent.Broadcast(
		ObjectiveType,
		TargetID,
		Amount,
		InstigatorPlayer
	);

	UE_LOG(LogTemp, Warning,TEXT("Quest Event: %s | Player: %s"),*TargetID.ToString(),*InstigatorPlayer->GetPlayerName());
}
