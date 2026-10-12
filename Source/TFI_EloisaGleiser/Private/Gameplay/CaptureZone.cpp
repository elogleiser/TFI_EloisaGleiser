// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/CaptureZone.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Framework/WebsPlayerState.h"
#include "Framework/WebsPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Gameplay/WebPatterns/WebPatternDefinition.h"
#include "Framework/WebsGameState.h"
#include "Gameplay/Quests/WebsQuestTypes.h"


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


void ACaptureZone::OnCaptureAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
		WebsController->ClientOpenCapturePuzzle(this);
	}
	
}

void ACaptureZone::OnCaptureAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority())
	{
		return;
	}

	ACharacter* LeavingCharacter = Cast<ACharacter>(OtherActor);

	if (!LeavingCharacter)
	{
		return;
	}

	// solo libera la zona si sale el jugador que la estaba ocupando
	if (LeavingCharacter != CharacterInZone)
	{
		return;
	}

	CharacterInZone = nullptr;
	OccupyingPlayer = nullptr;
	
	
}

void ACaptureZone::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACaptureZone, OccupyingPlayer);
	
	DOREPLIFETIME(ACaptureZone, OwningPlayer);

}

void ACaptureZone::OnRep_OccupyingPlayer()
{
}

void ACaptureZone::OnRep_OwningPlayer()
{
}

void ACaptureZone::CaptureZone(AWebsPlayerState* CapturingPlayer)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!CapturingPlayer || !WebPattern)
	{
		return;
	}

	// Obtener los puntos que otorga esta zona
	const int32 CapturePoints = WebPattern->PatternData.CapturePoints;

	// La zona ya pertenece a este mismo jugador
	if (OwningPlayer == CapturingPlayer)
	{
		return;
	}

	// Si era de otro jugador, pierde la zona y sus puntos
	if (OwningPlayer)
	{
		OwningPlayer->RemoveCapturedZone();
		OwningPlayer->RemoveScore(CapturePoints);
	}

	// Asignar nuevo dueño
	OwningPlayer = CapturingPlayer;

	// Nuevo dueño suma una zona y sus puntos
	CapturingPlayer->AddCapturedZone();
	CapturingPlayer->AddScore(CapturePoints);
	
	// Identificar la dificultad del patrón conquistado
	FName TargetID = NAME_None;

	switch (WebPattern->PatternData.Difficulty)
	{
	case EWebDifficulty::Easy:TargetID = FName(TEXT("Capture_Easy"));break;

	case EWebDifficulty::Medium:TargetID = FName(TEXT("Capture_Medium"));break;

	case EWebDifficulty::Hard:TargetID = FName(TEXT("Capture_Hard"));break;
	}

	// Reportar el evento al sistema de quests
	if (!TargetID.IsNone())
	{
		AWebsGameState* WebsGameState =GetWorld()->GetGameState<AWebsGameState>();

		if (WebsGameState)
		{
			WebsGameState->ReportGameplayObjective(EQuestObjectiveType::Interact,TargetID,1,CapturingPlayer);
		}
	}
}

bool ACaptureZone::IsOccupiedBy(const AWebsPlayerState* PlayerState) const
{
	return OccupyingPlayer == PlayerState;
}
