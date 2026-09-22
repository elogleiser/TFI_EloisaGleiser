// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/CaptureZone.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Framework/WebsPlayerState.h"
#include "Framework/WebsPlayerController.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ACaptureZone::ACaptureZone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	
	CaptureArea = CreateDefaultSubobject<UBoxComponent>(TEXT("CaptureArea"));
	SetRootComponent(CaptureArea);

	CaptureArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CaptureArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	CaptureArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}



// Called when the game starts or when spawned
void ACaptureZone::BeginPlay()
{
	Super::BeginPlay();
	
	CaptureArea->OnComponentBeginOverlap.AddDynamic(this,&ACaptureZone::OnCaptureAreaBeginOverlap);
	CaptureArea->OnComponentEndOverlap.AddDynamic(this,&ACaptureZone::OnCaptureAreaEndOverlap);
	
}

// Called every frame
void ACaptureZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ACaptureZone::OnCaptureAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* EnteringCharacter = Cast<ACharacter>(OtherActor);

	if (!EnteringCharacter)
	{
		return;
	}
	
	if (!HasAuthority())
	{
		return;
	}
	
	// La zona ya esta siendo usada por otro jugador
	if (CharacterInZone)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("CaptureZone is already occupied"));
		}

		return;
	}
	
	AWebsPlayerState* PlayerState = EnteringCharacter->GetPlayerState<AWebsPlayerState>();

	if (!PlayerState)
	{
		return;
	}
	
	CharacterInZone = EnteringCharacter;
	OccupyingPlayer = PlayerState;
	
	AWebsPlayerController* WebsController = Cast<AWebsPlayerController>(EnteringCharacter->GetController());

	if (WebsController)
	{
		WebsController->ClientOpenCapturePuzzle();
	}
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::Green,FString::Printf(TEXT("Zone locked | Character: %s | PlayerState: %s"),*CharacterInZone->GetName(),*OccupyingPlayer->GetName()));
	}
	
}

void ACaptureZone::OnCaptureAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* LeavingCharacter = Cast<ACharacter>(OtherActor);

	if (!LeavingCharacter)
	{
		return;
	}
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::Yellow,TEXT("CharacterInZone cleared"));
	}
	
}

void ACaptureZone::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACaptureZone, OccupyingPlayer);

}

void ACaptureZone::OnRep_OccupyingPlayer()
{
	if (GEngine)
	{
		const FString PlayerName = OccupyingPlayer
			? OccupyingPlayer->GetName()
			: TEXT("None");

		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Cyan,
			FString::Printf(
				TEXT("CLIENT received OccupyingPlayer: %s"),
				*PlayerName
			)
		);
	}
}
