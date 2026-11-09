// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "WebsPlayerState.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE(FOnPlayerColorChanged);

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
	
	int32 GetTotalScore() const { return TotalScore; }

	void AddScore(int32 Points);
	
	void RemoveScore(int32 Points);
	
	UFUNCTION(BlueprintPure, Category = "Webs|Appearance")
	FLinearColor GetPlayerColor() const { return PlayerColor; }
	
	void SetPlayerColor(const FLinearColor& NewColor);
	
	FOnPlayerColorChanged OnPlayerColorChanged;



protected:

private:
	UPROPERTY(ReplicatedUsing = OnRep_CapturedZones, VisibleAnywhere, Category = "Webs|Score")
	int32 CapturedZones = 0;

	void UpdateLocalHUD();
	
	void UpdateLocalScoreboard();
	
	UFUNCTION()
	void OnRep_CapturedZones();
	
	UPROPERTY(ReplicatedUsing = OnRep_TotalScore, VisibleAnywhere, Category = "Webs|Score")
	int32 TotalScore = 0;

	UFUNCTION()
	void OnRep_TotalScore();
	
	UPROPERTY(ReplicatedUsing = OnRep_PlayerColor,VisibleAnywhere, Category = "Webs|Appearance")
	FLinearColor PlayerColor = FLinearColor::White;

	UFUNCTION()
	void OnRep_PlayerColor();
};
