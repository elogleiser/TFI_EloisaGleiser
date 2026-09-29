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
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	int32 GetCapturedZones() const { return CapturedZones; }

	void AddCapturedZone();
	
	void RemoveCapturedZone();
	



protected:

private:
	UPROPERTY(ReplicatedUsing = OnRep_CapturedZones, VisibleAnywhere, Category = "Webs|Score")
	int32 CapturedZones = 0;

	void UpdateLocalHUD();
	
	void UpdateLocalScoreboard();
	
	UFUNCTION()
	void OnRep_CapturedZones();
};
