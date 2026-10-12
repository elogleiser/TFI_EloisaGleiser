// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TFI_EloisaGleiserCharacter.h"
#include "WebsCharacter.generated.h"

class UWebsSilkComponent;
class UWebsQuestComponent;

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

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Webs|Quests")
	TObjectPtr<UWebsQuestComponent> QuestComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
