// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WebsHUDWidget.generated.h"

/**
 * 
 */

class UTextBlock;
class AWebsPlayerState;
class UProgressBar;

UCLASS()
class TFI_ELOISAGLEISER_API UWebsHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateCapturedZones(int32 NewCapturedZones);

	void UpdateScoreboard();
	
	void UpdateSilk(int32 CurrentSilk, int32 MaxSilk);

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
	
	
};
