// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Framework/WebsPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Framework/WebsPlayerController.h"


AWebsPlayerState::AWebsPlayerState()
{
}

void AWebsPlayerState::AddCapturedZone()
{
	if (!HasAuthority())
	{
		return;
	}

	CapturedZones++;
	
	UpdateLocalHUD();
	UpdateLocalScoreboard();
	

}

void AWebsPlayerState::RemoveCapturedZone()
{
	if (!HasAuthority())
	{
		return;
	}

	CapturedZones = FMath::Max(0, CapturedZones - 1);
	
	UpdateLocalHUD();
	UpdateLocalScoreboard();
	
}



void AWebsPlayerState::OnRep_CapturedZones()
{
	UpdateLocalHUD();
	
	UpdateLocalScoreboard();
}

void AWebsPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AWebsPlayerState::UpdateLocalHUD()
{
	AWebsPlayerController* WebsController =
		Cast<AWebsPlayerController>(GetPlayerController());

	if (WebsController && WebsController->IsLocalController())
	{
		WebsController->UpdateCapturedZonesHUD(CapturedZones);
	}
}

void AWebsPlayerState::UpdateLocalScoreboard()
{
	if (UWorld* World = GetWorld())
	{
		AWebsPlayerController* LocalController =
			Cast<AWebsPlayerController>(
				World->GetFirstPlayerController()
			);

		if (LocalController && LocalController->IsLocalController())
		{
			LocalController->UpdateScoreboardHUD();
		}
	}
}
