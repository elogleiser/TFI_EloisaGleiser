// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/WebsShootingComponent.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Characters/WebsCharacter.h"
#include "Gameplay/WebsSilkComponent.h"


UWebsShootingComponent::UWebsShootingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;	
	SetIsReplicatedByDefault(true);
}


void UWebsShootingComponent::BeginPlay()
{
	Super::BeginPlay();

}


void UWebsShootingComponent::TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UWebsShootingComponent::TryShoot()
{
	APawn* OwnerPawn = GetOwner<APawn>();

	if (!OwnerPawn)
	{
		return;
	}

	AController* Controller = OwnerPawn->GetController();

	if (!Controller)
	{
		return;
	}

	FVector Start;
	FRotator Rotation;

	// Obtener la posición y rotación reales de la cámara
	Controller->GetPlayerViewPoint(Start, Rotation);

	const FVector Direction = Rotation.Vector();

	if (!OwnerPawn->HasAuthority())
	{
		ServerTryShoot(Start, Direction);
		return;
	}

	ProcessShot(Start, Direction);
}

void UWebsShootingComponent::ServerTryShoot_Implementation(FVector_NetQuantize Start,FVector_NetQuantizeNormal Direction)
{
	ProcessShot(Start, Direction);
}

void UWebsShootingComponent::ProcessShot(const FVector& Start,const FVector& Direction)
{
	APawn* OwnerPawn = GetOwner<APawn>();

	if (!OwnerPawn || !OwnerPawn->HasAuthority())
	{
		return;
	}
	
	if (!ValidateShot(Start, Direction))
	{
		return;
	}

	if (const AWebsCharacter* WebsCharacter = GetOwner<AWebsCharacter>())
	{
		if (WebsCharacter->IsStunned())
		{
			UE_LOG(LogTemp, Warning,TEXT("SERVER: Shot rejected - player is stunned"));
			return;
		}
	}
	
	AWebsCharacter* WebsCharacter = GetOwner<AWebsCharacter>();

	if (!WebsCharacter)
	{
		return;
	}

	UWebsSilkComponent* SilkComponent =WebsCharacter->GetSilkComponent();

	if (!SilkComponent)
	{
		return;
	}

	// Descontar Silk en el servidor
	if (!SilkComponent->ConsumeSilk(ShootSilkCost))
	{
		UE_LOG(LogTemp, Warning,TEXT("SERVER: Shot rejected - not enough silk"));
		return;
	}
	
	const FVector End = Start + Direction * ShootRange;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerPawn);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Params);
//
	DrawDebugLine(GetWorld(),Start,bHit ? Hit.ImpactPoint : End,FColor::Red,false,5.f,0,3.f);

	DrawDebugSphere(GetWorld(),Start,15.f,12,FColor::Yellow,false,5.f);

	UE_LOG(LogTemp, Warning,TEXT("SHOT DEBUG | Start: %s | Direction: %s | End: %s"),*Start.ToString(),*Direction.ToString(),*End.ToString());
	
	//

	if (bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("SERVER WEB SHOT HIT: %s"),*GetNameSafe(Hit.GetActor()));

		AWebsCharacter* TargetCharacter = Cast<AWebsCharacter>(Hit.GetActor());

		if (TargetCharacter && TargetCharacter != WebsCharacter)
		{
			TargetCharacter->ApplyStun(2.f);

			// Avisar que este jugador aturdió a otro.
			WebsCharacter->OnSuccessfulStun(TargetCharacter);

			UE_LOG(LogTemp, Warning, TEXT("WEB STUN APPLIED TO: %s"),*GetNameSafe(TargetCharacter));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SERVER WEB SHOT MISSED"));
	}
}

bool UWebsShootingComponent::ValidateShot(const FVector& Start,const FVector& Direction) const
{
	const APawn* OwnerPawn = GetOwner<APawn>();

	if (!OwnerPawn || !OwnerPawn->HasAuthority())
	{
		return false;
	}

	// Validar que los valores recibidos sean finitos
	if (Start.ContainsNaN() || Direction.ContainsNaN())
	{
		return false;
	}

	// Validar que la dirección esté normalizada
	if (!Direction.IsNormalized())
	{
		UE_LOG(LogTemp, Warning,TEXT("SERVER: Invalid shot direction"));
		return false;
	}

	// Validar distancia entre cámara y personaje
	constexpr float MaxStartDistance = 450.f;

	if (FVector::DistSquared(Start,OwnerPawn->GetActorLocation()) >FMath::Square(MaxStartDistance))
	{
		UE_LOG(LogTemp, Warning,TEXT("SERVER: Invalid shot position"));
		return false;
	}

	return true;
}