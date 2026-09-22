// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WebsPauseMenu.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void UWebsPauseMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (BTN_Resume)
	{
		BTN_Resume->OnClicked.AddDynamic(this,&UWebsPauseMenu::OnResumeClicked);
	}
	
	if (BTN_Quit)
	{
		BTN_Quit->OnClicked.AddDynamic(this,&UWebsPauseMenu::OnQuitClicked);
	}
	
	if (BTN_MainMenu)
	{
		BTN_MainMenu->OnClicked.AddDynamic(this,&UWebsPauseMenu::OnMainMenuClicked);
	}
}

void UWebsPauseMenu::OnResumeClicked()
{
	OnResumeRequested.Broadcast();	
}

void UWebsPauseMenu::OnMainMenuClicked()
{
	OnMainMenuRequested.Broadcast();	
}

void UWebsPauseMenu::OnQuitClicked()
{
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		UKismetSystemLibrary::QuitGame(this,PlayerController,EQuitPreference::Quit,false);
	}		
}
