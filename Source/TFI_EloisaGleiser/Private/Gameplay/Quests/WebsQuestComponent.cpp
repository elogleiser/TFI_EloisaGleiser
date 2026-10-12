// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Quests/WebsQuestComponent.h"
#include "Gameplay/Quests/WebsQuestDefinition.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "Framework/WebsGameState.h"
#include "GameFramework/Pawn.h"



UWebsQuestComponent::UWebsQuestComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);

}

void UWebsQuestComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Solo el servidor procesa los eventos de gameplay
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	AWebsGameState* WebsGameState = GetWorld()->GetGameState<AWebsGameState>();

	if (WebsGameState)
	{
		WebsGameState->OnGameplayObjectiveEvent.AddDynamic(this,&UWebsQuestComponent::HandleGameplayObjective);
	}
}




void UWebsQuestComponent::TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UWebsQuestComponent::FindQuestByID(FName QuestID, FWebsQuestRuntime& QuestData, int32& QuestIndex) const
{
	QuestData = FWebsQuestRuntime();
	QuestIndex = INDEX_NONE;

	for (int32 i = 0; i < ActiveQuests.Num(); i++)
	{
		if (ActiveQuests[i].QuestID == QuestID)
		{
			QuestData = ActiveQuests[i];
			QuestIndex = i;

			return true;
		}
	}

	return false;
}

void UWebsQuestComponent::AddQuest(UWebsQuestDefinition* QuestDefinition)
{
	// Solo el servidor puede agregar misiones
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (!QuestDefinition || QuestDefinition->QuestID.IsNone())
	{
		return;
	}

	// Comprobar si la quest ya existe
	FWebsQuestRuntime ExistingQuest;
	int32 ExistingIndex;

	if (FindQuestByID(QuestDefinition->QuestID,ExistingQuest,ExistingIndex))
	{
		return;
	}

	// Crear el estado inicial de la misión
	FWebsQuestRuntime NewQuest;

	NewQuest.QuestID = QuestDefinition->QuestID;
	NewQuest.QuestDefinition = QuestDefinition;
	NewQuest.Objectives = QuestDefinition->Objectives;

	// Inicializar el progreso
	for (FWebsQuestObjective& Objective : NewQuest.Objectives)
	{
		Objective.CurrentAmount = 0;
	}

	NewQuest.bIsActive = true;
	NewQuest.bIsCompleted = false;

	// Guardar la nueva quest
	ActiveQuests.Add(NewQuest);
	OnQuestStarted.Broadcast(NewQuest);

	UE_LOG(LogTemp, Warning,TEXT("Quest added: %s"),*NewQuest.QuestID.ToString());
}

void UWebsQuestComponent::UpdateObjectiveProgress(int32 QuestIndex, int32 ObjectiveIndex, int32 Amount)
{
	// Solo el servidor modifica el progreso
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	// Validar índice de la quest
	if (!ActiveQuests.IsValidIndex(QuestIndex))
	{
		return;
	}

	FWebsQuestRuntime& Quest = ActiveQuests[QuestIndex];

	// Solo actualizar quests activas
	if (!Quest.bIsActive || Quest.bIsCompleted)
	{
		return;
	}

	// Validar índice del objetivo
	if (!Quest.Objectives.IsValidIndex(ObjectiveIndex))
	{
		return;
	}

	if (Amount <= 0)
	{
		return;
	}

	FWebsQuestObjective& Objective =Quest.Objectives[ObjectiveIndex];

	// Actualizar sin superar la cantidad requerida
	const int32 PreviousAmount = Objective.CurrentAmount;

	Objective.CurrentAmount = FMath::Clamp(Objective.CurrentAmount + Amount,0,Objective.RequiredAmount);

	// Evitar actualizaciones innecesarias
	if (Objective.CurrentAmount == PreviousAmount)
	{
		return;
	}

	UE_LOG(LogTemp, Warning,TEXT("Quest %s | Objective %s: %d/%d"),*Quest.QuestID.ToString(),*Objective.ObjectiveID.ToString(),Objective.CurrentAmount,Objective.RequiredAmount);

	CheckQuestCompleted(QuestIndex);

	if (!Quest.bIsCompleted)
	{
		OnQuestProgressChanged.Broadcast(Quest);
	}
}

void UWebsQuestComponent::CheckQuestCompleted(int32 QuestIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (!ActiveQuests.IsValidIndex(QuestIndex))
	{
		return;
	}

	FWebsQuestRuntime& Quest = ActiveQuests[QuestIndex];

	if (!Quest.bIsActive || Quest.bIsCompleted)
	{
		return;
	}

	// Evitar completar misiones sin objetivos
	if (Quest.Objectives.IsEmpty())
	{
		return;
	}

	// Comprobar todos los objetivos
	for (const FWebsQuestObjective& Objective : Quest.Objectives)
	{
		if (Objective.CurrentAmount < Objective.RequiredAmount)
		{
			return;
		}
	}

	// Todos los objetivos están completos
	Quest.bIsCompleted = true;
	Quest.bIsActive = false;
	
	if (Quest.QuestDefinition && Quest.QuestDefinition->SilkReward > 0)
	{
		if (AWebsCharacter* Character = Cast<AWebsCharacter>(GetOwner()))
		{
			if (UWebsSilkComponent* SilkComponent = Character->GetSilkComponent())
			{
				SilkComponent->AddSilk(Quest.QuestDefinition->SilkReward);

				UE_LOG(LogTemp, Warning, TEXT("Quest reward: +%d Silk"),Quest.QuestDefinition->SilkReward);
			}
		}
	}
	
	OnQuestCompleted.Broadcast(Quest);

	UE_LOG(LogTemp, Warning,TEXT("QUEST COMPLETED: %s"),*Quest.QuestID.ToString());
}

void UWebsQuestComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UWebsQuestComponent,ActiveQuests,COND_OwnerOnly);
}



void UWebsQuestComponent::OnRep_ActiveQuests()
{
	for (const FWebsQuestRuntime& Quest : ActiveQuests)
	{
		const FWebsQuestRuntime* PreviousQuest = PreviousQuests.FindByPredicate([&Quest](const FWebsQuestRuntime& Previous)
				{
					return Previous.QuestID == Quest.QuestID;
				}
				);

		// Misión nueva
		if (!PreviousQuest)
		{
			OnQuestStarted.Broadcast(Quest);
		}
		else if (!PreviousQuest->bIsCompleted && Quest.bIsCompleted)
		{
			// Misión completada
			OnQuestCompleted.Broadcast(Quest);
		}
		else
		{
			// Detectar cambios en el progreso
			bool bProgressChanged = false;

			for (int32 i = 0; i < Quest.Objectives.Num(); i++)
			{
				if (!PreviousQuest->Objectives.IsValidIndex(i) ||PreviousQuest->Objectives[i].CurrentAmount != Quest.Objectives[i].CurrentAmount)
				{
					bProgressChanged = true;
					break;
				}
			}

			if (bProgressChanged)
			{
				OnQuestProgressChanged.Broadcast(Quest);
			}
		}
	}

	PreviousQuests = ActiveQuests;
}

void UWebsQuestComponent::HandleGameplayObjective(EQuestObjectiveType ObjectiveType, FName TargetID, int32 Amount,APlayerState* InstigatorPlayer)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (!InstigatorPlayer || Amount <= 0)
	{
		return;
	}

	// Identificar al jugador propietario del componente
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());

	if (!OwnerPawn ||OwnerPawn->GetPlayerState() != InstigatorPlayer)
	{
		return;
	}

	// Recorrer todas las quests
	for (int32 QuestIndex = 0;QuestIndex < ActiveQuests.Num();QuestIndex++)
	{
		const FWebsQuestRuntime& Quest = ActiveQuests[QuestIndex];

		if (!Quest.bIsActive || Quest.bIsCompleted)
		{
			continue;
		}

		// Buscar objetivos que coincidan con el evento
		for (int32 ObjectiveIndex = 0;ObjectiveIndex < Quest.Objectives.Num();ObjectiveIndex++)
		{
			const FWebsQuestObjective& Objective =Quest.Objectives[ObjectiveIndex];

			if (Objective.ObjectiveType == ObjectiveType &&Objective.TargetID == TargetID)
			{
				UpdateObjectiveProgress(QuestIndex,ObjectiveIndex,Amount);
			}
		}
	}
}