#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/Quests/WebsQuestTypes.h"
#include "WebsQuestTrackerWidget.generated.h"

class UBorder;
class UTextBlock;
struct FWebsQuestRuntime;
class UVerticalBox;
class UWebsQuestObjectiveWidget;

UCLASS()
class TFI_ELOISAGLEISER_API UWebsQuestTrackerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Quests")
	void ToggleQuestContent();
	
	UFUNCTION(BlueprintCallable, Category = "Quests")
	void UpdateQuestDisplay(const FWebsQuestRuntime& QuestData);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border_QuestContent;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_Arrow;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_QuestName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_QuestDescription;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Reward;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_QuestStatus;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> VB_Objectives;

	UPROPERTY(EditDefaultsOnly, Category = "Quests")
	TSubclassOf<UWebsQuestObjectiveWidget> ObjectiveWidgetClass;

};