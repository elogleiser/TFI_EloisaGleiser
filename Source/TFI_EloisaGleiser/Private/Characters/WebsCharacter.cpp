// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Characters/WebsCharacter.h"
#include "Gameplay/WebsSilkComponent.h"
#include "Gameplay/Quests/WebsQuestComponent.h"


// Sets default values
AWebsCharacter::AWebsCharacter()
{

	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	
	SilkComponent = CreateDefaultSubobject<UWebsSilkComponent>(TEXT("SilkComponent"));
	QuestComponent = CreateDefaultSubobject<UWebsQuestComponent>(TEXT("QuestComponent"));
}


void AWebsCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


void AWebsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AWebsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

