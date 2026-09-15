// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Characters/WebsCharacter.h"


// Sets default values
AWebsCharacter::AWebsCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWebsCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWebsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AWebsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

