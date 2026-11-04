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
class UVerticalBox;
class UBorder;

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
	
	UFUNCTION(BlueprintCallable, Category = "Webs|Match")
	void UpdateMatchHUD();
	
	void UpdateMatchResult();
	
protected:
	
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;

	void HandleMatchDataChanged();
	
	UPROPERTY()
	TObjectPtr<class AWebsGameState> CachedWebsGameState = nullptr;


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
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Countdown;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_MatchTimer;
	
	FTimerHandle CaptureMessageTimer;

	void HideCaptureMessage();
	
	// Contenedor de las misiones en el HUD
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> VB_QuestList;

	// Clase del widget que representa una mision
	UPROPERTY(EditDefaultsOnly, Category = "Quests")
	TSubclassOf<UWebsQuestTrackerWidget> QuestTrackerWidgetClass;

	// Widgets creados, identificados por QuestID
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<UWebsQuestTrackerWidget>> QuestWidgets;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border_MatchResult;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ResultTitle;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ResultDescription;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_FinalScore;
	
};
