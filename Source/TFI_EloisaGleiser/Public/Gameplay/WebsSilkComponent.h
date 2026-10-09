// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WebsSilkComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSilkChanged,int32, Current,int32, Maximum);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TFI_ELOISAGLEISER_API UWebsSilkComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWebsSilkComponent();
	
	UFUNCTION(BlueprintPure, Category="Webs|Silk")
	int32 GetCurrentSilk() const;

	UFUNCTION(BlueprintPure, Category="Webs|Silk")
	int32 GetMaxSilk() const { return MaxSilk; }

	UFUNCTION(BlueprintCallable, Category="Webs|Silk")
	bool HasEnoughSilk(int32 Amount) const;

	// Solo el servidor puede consumir seda.
	bool ConsumeSilk(int32 Amount);

	void AddSilk(int32 Amount);
	
	UPROPERTY(BlueprintAssignable, Category="Webs|Silk")
	FOnSilkChanged OnSilkChanged;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category="Webs|Silk")
	int32 MaxSilk = 100;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentSilk,
		VisibleAnywhere, Category="Webs|Silk")
	int32 CurrentSilk = 100;

	UFUNCTION()
	void OnRep_CurrentSilk();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
