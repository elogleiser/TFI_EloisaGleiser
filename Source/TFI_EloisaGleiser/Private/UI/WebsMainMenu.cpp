// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WebsMainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WidgetLan/MenuLan.h"
#include "UI/WebsOnlineMenu.h"

void UWebsMainMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (BTN_Online)
	{
		BTN_Online->OnClicked.AddDynamic(this, &UWebsMainMenu::OnOnlineClicked);
	}
	if (BTN_Lan)
	{
		BTN_Lan->OnClicked.AddDynamic(this, &UWebsMainMenu::OnLanClicked);
	}
	if (BTN_Exit)
	{
		BTN_Exit->OnClicked.AddDynamic(this, &UWebsMainMenu::OnExitClicked);
	}

	if (BTN_ConfirmExit)
	{
		BTN_ConfirmExit->OnClicked.AddDynamic(this, &UWebsMainMenu::OnConfirmExitClicked);
	}

	if (BTN_CancelExit)
	{
		BTN_CancelExit->OnClicked.AddDynamic(this, &UWebsMainMenu::OnCancelExitClicked);
	}
	
	if (WBP_MenuLan)
	{
		WBP_MenuLan->OnBackRequested.AddDynamic(this,&UWebsMainMenu::OnLanBackRequested);
	}
	if (WBP_MenuWidgetOW)
	{
		WBP_MenuWidgetOW->OnBackRequested.AddDynamic(this,&UWebsMainMenu::OnOnlineBackRequested);
	}
}

void UWebsMainMenu::OnOnlineClicked()
{
	if (WS_MainMenu)
	{
		WS_MainMenu->SetActiveWidgetIndex(1);
	}
}

void UWebsMainMenu::OnLanClicked()
{
	if (WS_MainMenu)
	{
		WS_MainMenu->SetActiveWidgetIndex(2);
	}
}

void UWebsMainMenu::OnExitClicked()
{
	if (WS_MainMenu)
	{
		WS_MainMenu->SetActiveWidgetIndex(3);
	}
}

void UWebsMainMenu::OnConfirmExitClicked()
{
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		UKismetSystemLibrary::QuitGame(this,PlayerController,EQuitPreference::Quit,false);
	}
}

void UWebsMainMenu::OnCancelExitClicked()
{
	if (WS_MainMenu)
	{
		WS_MainMenu->SetActiveWidgetIndex(0);
	}
}

void UWebsMainMenu::OnLanBackRequested()
{
	if (WS_MainMenu)
	{
		WS_MainMenu->SetActiveWidgetIndex(0);
	}
}

void UWebsMainMenu::OnOnlineBackRequested()
{
	if (WS_MainMenu)
	{
		WS_MainMenu->SetActiveWidgetIndex(0);
	}
}
