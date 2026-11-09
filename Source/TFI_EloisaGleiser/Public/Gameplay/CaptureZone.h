// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CaptureZone.generated.h"

class UBoxComponent;
class ACharacter;
class AWebsPlayerState;
class UWebPatternDefinition;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZoneOwnerChanged,FLinearColor,NewColor);

UCLASS()
class TFI_ELOISAGLEISER_API ACaptureZone : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACaptureZone();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void CaptureZone(AWebsPlayerState* CapturingPlayer);
	
	bool IsOccupiedBy(const AWebsPlayerState* PlayerState) const;
	
	UWebPatternDefinition* GetWebPattern() const
	{
		return WebPattern;
	}
	AWebsPlayerState* GetOwningPlayer() const
	{
		return OwningPlayer;
	}
	
	UPROPERTY(BlueprintAssignable, Category = "Webs|Capture Zone")
	FOnZoneOwnerChanged OnZoneOwnerChanged;
	
	UFUNCTION(BlueprintCallable, Category = "Webs|Capture Zone")
	void RefreshZoneAppearance();
	
	
public:
	virtual void Tick(float DeltaTime) override;
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Webs|Puzzle")
	TObjectPtr<UWebPatternDefinition> WebPattern;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Webs|Capture Zone")
	TObjectPtr<UBoxComponent> CaptureArea;
	
	UPROPERTY()
	TObjectPtr<ACharacter> CharacterInZone;
	
	UPROPERTY(ReplicatedUsing = OnRep_OccupyingPlayer)
	TObjectPtr<AWebsPlayerState> OccupyingPlayer;
	
	UPROPERTY(ReplicatedUsing = OnRep_OwningPlayer)
	TObjectPtr<AWebsPlayerState> OwningPlayer;
	
	UPROPERTY(Transient)
	TObjectPtr<AWebsPlayerState> BoundColorPlayerState = nullptr;
	
	
	UFUNCTION()
	void OnCaptureAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnCaptureAreaEndOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);
	
	UFUNCTION()
	void OnRep_OccupyingPlayer();
	
	UFUNCTION()
	void OnRep_OwningPlayer();
	
	void UpdateZoneAppearance();
	
	void BindOwnerColor();
};
