#pragma once

#include "CoreMinimal.h"
#include "WebsQuestTypes.generated.h"

UENUM(BlueprintType)
enum class EQuestObjectiveType : uint8
{
	Kill,
	Collect,
	Interact,
	ReachLocation
};

USTRUCT(BlueprintType)
struct FWebsQuestObjective
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ObjectiveID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EQuestObjectiveType ObjectiveType = EQuestObjectiveType::Interact;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TargetID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RequiredAmount = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentAmount = 0;
};

USTRUCT(BlueprintType)
struct FWebsQuestRuntime
{
	GENERATED_BODY()

	// Identificador único de la misión
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName QuestID = NAME_None;

	// Referencia al Data Asset original
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UWebsQuestDefinition> QuestDefinition = nullptr;

	// Copia de los objetivos con su progreso individual
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FWebsQuestObjective> Objectives;

	// Estado de la misión
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsCompleted = false;
};