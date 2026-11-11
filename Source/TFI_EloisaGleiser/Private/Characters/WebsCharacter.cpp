// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Characters/WebsCharacter.h"
#include "Gameplay/WebsSilkComponent.h"
#include "Gameplay/Quests/WebsQuestComponent.h"
#include "Gameplay/WebsShootingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Framework/WebsGameState.h"
#include "Framework/WebsPlayerState.h"
#include "Materials/MaterialInstanceDynamic.h"


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
	UpdateMovementForMatchPhase();
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
	
	UpdatePlayerAppearance();

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
	UpdatePlayerAppearance();
}

void AWebsCharacter::OnRep_IsStunned()
{
	UpdateStunMovement();
	UpdatePlayerAppearance();
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
		// Guardar el modo anterior solo si estaba habilitado.
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
		const AWebsGameState* WebsGameState =GetWorld()->GetGameState<AWebsGameState>();

		// Solo restaurar movimiento durante Playing.
		if (!WebsGameState ||WebsGameState->MatchPhase != EWebsMatchPhase::Playing)
		{
			return;
		}

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
	const AWebsGameState* WebsGameState =GetWorld()->GetGameState<AWebsGameState>();

	const bool bMatchNotPlaying =!WebsGameState ||WebsGameState->MatchPhase != EWebsMatchPhase::Playing;

	return bIsStunned ||bMatchNotPlaying ||Super::IsMoveInputIgnored();
}


void AWebsCharacter::UpdateMovementForMatchPhase()
{
	const AWebsGameState* WebsGameState =GetWorld()->GetGameState<AWebsGameState>();

	if (!WebsGameState)
	{
		return;
	}

	UCharacterMovementComponent* Movement =GetCharacterMovement();

	if (!Movement)
	{
		return;
	}

	const bool bShouldLockMovement =WebsGameState->MatchPhase != EWebsMatchPhase::Playing;

	if (bMovementStateInitialized &&bShouldLockMovement == bMovementLockedByMatch)
	{
		return;
	}

	bMovementStateInitialized = true;
	bMovementLockedByMatch = bShouldLockMovement;

	if (bShouldLockMovement)
	{
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
	}
	else if (!bIsStunned)
	{
		Movement->SetMovementMode(MOVE_Walking);
	}
}

void AWebsCharacter::UpdatePlayerAppearance()
{
	AWebsPlayerState* WebsPlayerState =GetPlayerState<AWebsPlayerState>();

	if (!WebsPlayerState || !GetMesh())
	{
		return;
	}

	// Crear una instancia dinámica únicamente una vez
	if (!DynamicBodyMaterial)
	{
		DynamicBodyMaterial =GetMesh()->CreateDynamicMaterialInstance(0);
	}

	if (!DynamicBodyMaterial)
	{
		return;
	}

	// Obtener el color original del jugador
	FLinearColor DisplayColor =WebsPlayerState->GetPlayerColor();

	// Si está stuneado, desaturar el color
	if (bIsStunned)
	{
		const float Gray = DisplayColor.GetLuminance();

		const FLinearColor GrayColor(Gray,Gray,Gray,1.0f);

		// 15% del color original y 85% gris
		DisplayColor = FMath::Lerp(GrayColor,DisplayColor,0.15f);
	}

	// Aplicar el color visual al material
	DynamicBodyMaterial->SetVectorParameterValue(TEXT("BaseColorFactor"),DisplayColor);
}

void AWebsCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	BindPlayerColor();
}

void AWebsCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	BindPlayerColor();
}

void AWebsCharacter::BindPlayerColor()
{
	AWebsPlayerState* WebsPlayerState =GetPlayerState<AWebsPlayerState>();

	if (BoundColorPlayerState == WebsPlayerState)
	{
		return;
	}

	// Desconectar del PlayerState anterior, si existía
	if (IsValid(BoundColorPlayerState))
	{
		BoundColorPlayerState->OnPlayerColorChanged.RemoveAll(this);
	}

	BoundColorPlayerState = WebsPlayerState;

	if (IsValid(BoundColorPlayerState))
	{
		BoundColorPlayerState->OnPlayerColorChanged.AddUObject(this,&AWebsCharacter::UpdatePlayerAppearance);
	}

	// Aplicar el color que ya tenga replicado
	UpdatePlayerAppearance();
}

void AWebsCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(BoundColorPlayerState))
	{
		BoundColorPlayerState->OnPlayerColorChanged.RemoveAll(this);
	}

	BoundColorPlayerState = nullptr;

	Super::EndPlay(EndPlayReason);
}