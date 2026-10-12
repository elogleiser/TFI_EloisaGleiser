// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gameplay/Quests/WebsQuestTypes.h"
#include "Gameplay/WebsSilkComponent.h"
#include "Gameplay/Quests/WebsQuestDefinition.h"
#include "Characters/WebsCharacter.h"
#include "WebsQuestComponent.generated.h"

class UWebsQuestDefinition;
class APlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestStarted,FWebsQuestRuntime, QuestData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestProgressChanged,FWebsQuestRuntime, QuestData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted,FWebsQuestRuntime, QuestData);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TFI_ELOISAGLEISER_API UWebsQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UWebsQuestComponent();
	
	// Busca una quest mediante su ID
	UFUNCTION(BlueprintCallable, Category="Webs|Quests")
	bool FindQuestByID(FName QuestID,FWebsQuestRuntime& QuestData,int32& QuestIndex) const;

	// Agrega una nueva quest al jugador
	UFUNCTION(BlueprintCallable, Category="Webs|Quests")
	void AddQuest(UWebsQuestDefinition* QuestDefinition);
	
	UFUNCTION(BlueprintCallable, Category="Webs|Quests")
	void UpdateObjectiveProgress(int32 QuestIndex,int32 ObjectiveIndex,int32 Amount);
	
	UFUNCTION(BlueprintCallable, Category="Webs|Quests")
	void CheckQuestCompleted(int32 QuestIndex);
	
	UPROPERTY(BlueprintAssignable, Category="Webs|Quests|Events")
	FOnQuestStarted OnQuestStarted;

	UPROPERTY(BlueprintAssignable, Category="Webs|Quests|Events")
	FOnQuestProgressChanged OnQuestProgressChanged;

	UPROPERTY(BlueprintAssignable, Category="Webs|Quests|Events")
	FOnQuestCompleted OnQuestCompleted;

protected:

	virtual void BeginPlay() override;
	
	UFUNCTION()
	
	void OnRep_ActiveQuests();

	virtual auto GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const -> void override;
	
	UFUNCTION()
	void HandleGameplayObjective(EQuestObjectiveType ObjectiveType,FName TargetID,int32 Amount,APlayerState* InstigatorPlayer);
	
	
	// Misiones aceptadas por el jugador
	UPROPERTY(ReplicatedUsing=OnRep_ActiveQuests,VisibleAnywhere, BlueprintReadOnly, Category="Webs|Quests")
	TArray<FWebsQuestRuntime> ActiveQuests;
	
	TArray<FWebsQuestRuntime> PreviousQuests;
	
	

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
};
