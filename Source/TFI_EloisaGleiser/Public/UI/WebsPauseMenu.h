// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WebsPauseMenu.generated.h"

/**
 * 
 */

class UButton;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResumeRequested);	
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMainMenuRequested);
UCLASS()
class TFI_ELOISAGLEISER_API UWebsPauseMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category = "Webs|Navigation")
	FOnResumeRequested OnResumeRequested;
	UPROPERTY(BlueprintAssignable, Category = "Webs|Navigation")
	FOnMainMenuRequested OnMainMenuRequested;
	
protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_Resume;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_MainMenu;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_Quit;

	UFUNCTION()
	void OnResumeClicked();

	UFUNCTION()
	void OnMainMenuClicked();

	UFUNCTION()
	void OnQuitClicked();
};
