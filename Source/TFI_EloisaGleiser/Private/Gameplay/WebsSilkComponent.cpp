// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/WebsSilkComponent.h"

#include "Net/UnrealNetwork.h"


UWebsSilkComponent::UWebsSilkComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

int32 UWebsSilkComponent::GetCurrentSilk() const
{ return CurrentSilk; }

void UWebsSilkComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		CurrentSilk = MaxSilk;
	}
	
}

bool UWebsSilkComponent::HasEnoughSilk(int32 Amount) const
{
	return Amount >= 0 && CurrentSilk >= Amount;
}

bool UWebsSilkComponent::ConsumeSilk(int32 Amount)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
	return false;
	}

	if (!HasEnoughSilk(Amount))
	{
		return false;
	}

	CurrentSilk -= Amount;

	// El servidor no ejecuta OnRep automáticamente.
	OnRep_CurrentSilk();

	return true;
}

void UWebsSilkComponent::AddSilk(int32 Amount)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()
	   || Amount <= 0)
	{
		return;
	}

	CurrentSilk = FMath::Clamp(
		CurrentSilk + Amount,
		0,
		MaxSilk
	);

	OnRep_CurrentSilk();
}


void UWebsSilkComponent::OnRep_CurrentSilk()
{
	OnSilkChanged.Broadcast(CurrentSilk, MaxSilk);
}


void UWebsSilkComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWebsSilkComponent, CurrentSilk);
}

// Called every frame
void UWebsSilkComponent::TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}