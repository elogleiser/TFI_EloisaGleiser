// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WebsOnlineMenu.h"
#include "Components/Button.h"

void UWebsOnlineMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (BTN_Volver)
	{
		BTN_Volver->OnClicked.AddDynamic(this,&UWebsOnlineMenu::OnVolverClicked);
	}
}

void UWebsOnlineMenu::OnVolverClicked()
{
	OnBackRequested.Broadcast();
}
