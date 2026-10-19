// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WebsShootingComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TFI_ELOISAGLEISER_API UWebsShootingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWebsShootingComponent();

	UFUNCTION(BlueprintCallable, Category="Webs|Shooting")
	void TryShoot();
	
	UFUNCTION(Server, Reliable)
	void ServerTryShoot(FVector_NetQuantize Start, FVector_NetQuantizeNormal Direction);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category="Webs|Shooting")
	float ShootRange = 2000.f;
	
	void ProcessShot(const FVector& Start, const FVector& Direction);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Webs|Shooting")
	int32 ShootSilkCost = 10;
	
	bool ValidateShot(const FVector& Start,const FVector& Direction) const;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
};
