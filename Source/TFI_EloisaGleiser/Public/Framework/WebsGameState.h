// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Gameplay/Quests/WebsQuestTypes.h"
#include "WebsGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnGameplayObjectiveEvent,EQuestObjectiveType, ObjectiveType,FName, TargetID,int32, Amount,APlayerState*, InstigatorPlayer);

UCLASS()
class TFI_ELOISAGLEISER_API AWebsGameState : public AGameState
{
	GENERATED_BODY()
	
	
public:
	UPROPERTY(BlueprintAssignable, Category="Webs|Quests")
	FOnGameplayObjectiveEvent OnGameplayObjectiveEvent;

	UFUNCTION(BlueprintCallable, Category="Webs|Quests")
	void ReportGameplayObjective(EQuestObjectiveType ObjectiveType,FName TargetID,int32 Amount,APlayerState* InstigatorPlayer);
};
