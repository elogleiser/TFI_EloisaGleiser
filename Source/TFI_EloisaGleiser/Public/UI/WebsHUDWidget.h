// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/Quests/WebsQuestTypes.h"
#include "WebsHUDWidget.generated.h"

/**
 * 
 */

class UTextBlock;
class AWebsPlayerState;
class UProgressBar;
class UWebsQuestTrackerWidget;
struct FWebsQuestRuntime;

UCLASS()
class TFI_ELOISAGLEISER_API UWebsHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateCapturedZones(int32 NewCapturedZones);

	void UpdateScoreboard();
	
	void UpdateSilk(int32 CurrentSilk, int32 MaxSilk);
	
	void ShowCaptureMessage(const FString& Message, bool bSuccess);
	
	UFUNCTION(BlueprintCallable, Category = "Quests")
	void ToggleQuestTracker();
	
	UFUNCTION(BlueprintCallable, Category = "Quests")
	void UpdateQuestTracker(const FWebsQuestRuntime& QuestData);


private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_CapturedZones;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Player1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Player2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Player3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Player4;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PB_Silk;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Silk;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_CaptureMessage;
	
	FTimerHandle CaptureMessageTimer;

	void HideCaptureMessage();
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWebsQuestTrackerWidget> QuestTracker;
	
	
	
};
