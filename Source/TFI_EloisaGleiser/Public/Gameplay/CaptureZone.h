// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CaptureZone.generated.h"

class UBoxComponent;
class ACharacter;
class AWebsPlayerState;


UCLASS()
class TFI_ELOISAGLEISER_API ACaptureZone : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACaptureZone();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	virtual void Tick(float DeltaTime) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere, Category = "Webs|Capture Zone")
	TObjectPtr<UBoxComponent> CaptureArea;
	
	UPROPERTY()
	TObjectPtr<ACharacter> CharacterInZone;
	
	UPROPERTY(ReplicatedUsing = OnRep_OccupyingPlayer)
	TObjectPtr<AWebsPlayerState> OccupyingPlayer;
	
	
	UFUNCTION()
	void OnCaptureAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnCaptureAreaEndOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);
	
	UFUNCTION()
	void OnRep_OccupyingPlayer();
	
};
