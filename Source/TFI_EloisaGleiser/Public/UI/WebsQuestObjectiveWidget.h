#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/Quests/WebsQuestTypes.h"
#include "WebsQuestObjectiveWidget.generated.h"

class UTextBlock;

UCLASS()
class TFI_ELOISAGLEISER_API UWebsQuestObjectiveWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Quests")
	void UpdateObjective(const FWebsQuestObjective& Objective);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Status;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_ObjectiveName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Progress;
};