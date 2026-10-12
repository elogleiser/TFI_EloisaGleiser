#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Gameplay/Quests/WebsQuestTypes.h"
#include "WebsQuestDefinition.generated.h"

UCLASS(BlueprintType)
class TFI_ELOISAGLEISER_API UWebsQuestDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Info")
	FName QuestID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Info")
	FText QuestName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Info", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Objectives")
	TArray<FWebsQuestObjective> Objectives;

	// Recompensa por completar la misión
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Rewards", meta = (ClampMin = "0"))
	int32 SilkReward = 0;
};