// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "WebsPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TFI_ELOISAGLEISER_API AWebsPlayerState : public APlayerState
{
	GENERATED_BODY()
	

public:
	AWebsPlayerState();

	int32 GetCapturedZones() const { return CapturedZones; }


protected:

private:
	UPROPERTY(VisibleAnywhere, Category = "Webs|Score")
	int32 CapturedZones = 0;
};
