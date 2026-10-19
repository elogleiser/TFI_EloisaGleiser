// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Characters/WebsCharacter.h"
#include "Gameplay/WebsSilkComponent.h"
#include "Gameplay/Quests/WebsQuestComponent.h"
#include "Gameplay/WebsShootingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"



// Sets default values
AWebsCharacter::AWebsCharacter()
{

	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	
	SilkComponent = CreateDefaultSubobject<UWebsSilkComponent>(TEXT("SilkComponent"));
	QuestComponent = CreateDefaultSubobject<UWebsQuestComponent>(TEXT("QuestComponent"));
	ShootingComponent =CreateDefaultSubobject<UWebsShootingComponent>(TEXT("ShootingComponent"));
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

void AWebsCharacter::ApplyStun(float Duration)
{
	if (!HasAuthority() || Duration <= 0.f)
	{
		return;
	}

	bIsStunned = true;

	UpdateStunMovement();

	GetWorldTimerManager().ClearTimer(StunTimerHandle);

	GetWorldTimerManager().SetTimer(StunTimerHandle,this,&AWebsCharacter::EndStun,Duration,false);
}

void AWebsCharacter::EndStun()
{
	if (!HasAuthority())
	{
		return;
	}

	bIsStunned = false;

	UpdateStunMovement();
}

void AWebsCharacter::OnRep_IsStunned()
{
	UpdateStunMovement();
}

void AWebsCharacter::UpdateStunMovement()
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();

	if (!Movement)
	{
		return;
	}

	if (bIsStunned)
	{
		// Guardar el modo anterior solo si todavía no está deshabilitado
		if (Movement->MovementMode != MOVE_None)
		{
			MovementModeBeforeStun = Movement->MovementMode;
		}

		Movement->StopMovementImmediately();
		Movement->DisableMovement();

		UE_LOG(LogTemp, Warning,TEXT("STUN: Movement disabled on %s"),*GetNameSafe(this));
	}
	else
	{
		// Si estaba cayendo, dejar que Unreal resuelva el aterrizaje
		if (MovementModeBeforeStun == MOVE_Falling)
		{
			Movement->SetMovementMode(MOVE_Falling);
		}
		else
		{
			Movement->SetMovementMode(MovementModeBeforeStun);
		}

		UE_LOG(LogTemp, Warning,TEXT("STUN: Movement restored on %s"),*GetNameSafe(this));
	}
}

void AWebsCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWebsCharacter, bIsStunned);
}

bool AWebsCharacter::IsMoveInputIgnored() const
{
	return bIsStunned || Super::IsMoveInputIgnored();
}

