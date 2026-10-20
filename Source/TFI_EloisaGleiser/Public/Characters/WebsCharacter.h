// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TFI_EloisaGleiserCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WebsCharacter.generated.h"

class UWebsSilkComponent;
class UWebsQuestComponent;
class UWebsShootingComponent;

UCLASS()
class TFI_ELOISAGLEISER_API AWebsCharacter : public ATFI_EloisaGleiserCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWebsCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category="Webs|Components")
	TObjectPtr<UWebsSilkComponent> SilkComponent;
	
	UWebsSilkComponent* GetSilkComponent() const
	{
		return SilkComponent;
	}
	
	UWebsQuestComponent* GetQuestComponent() const
	{
		return QuestComponent;
	}
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Webs|Components")
	TObjectPtr<UWebsShootingComponent> ShootingComponent;

	UWebsShootingComponent* GetShootingComponent() const
	{
		return ShootingComponent;
	}
	
	UFUNCTION(BlueprintCallable, Category="Webs|Stun")
	void ApplyStun(float Duration = 20.f);

	UFUNCTION(BlueprintPure, Category="Webs|Stun")
	bool IsStunned() const { return bIsStunned; }
	
	virtual bool IsMoveInputIgnored() const override;
	
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Webs|Quests")
	void OnSuccessfulStun(AActor* StunnedActor);


	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Webs|Quests")
	TObjectPtr<UWebsQuestComponent> QuestComponent;
	
	UPROPERTY(ReplicatedUsing=OnRep_IsStunned, BlueprintReadOnly, Category="Webs|Stun")
	bool bIsStunned = false;

	UFUNCTION()
	void OnRep_IsStunned();

	void UpdateStunMovement();

	void EndStun();

	FTimerHandle StunTimerHandle;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Modo de movimiento anterior al stun
	EMovementMode MovementModeBeforeStun = MOVE_Walking;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
