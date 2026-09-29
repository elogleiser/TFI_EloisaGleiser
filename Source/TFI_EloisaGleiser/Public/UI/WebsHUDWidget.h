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

UCLASS()
class TFI_ELOISAGLEISER_API UWebsHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateCapturedZones(int32 NewCapturedZones);

	void UpdateScoreboard();

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
	
	
};
