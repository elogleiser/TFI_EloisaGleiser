// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WebsMainMenu.generated.h"

/**
 * 
 */

class UButton;
class UWidgetSwitcher;
class UMenuLan;
class UWebsOnlineMenu;	


UCLASS()
class TFI_ELOISAGLEISER_API UWebsMainMenu : public UUserWidget
{
	GENERATED_BODY()

	
protected:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_Online;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_Lan;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_Exit;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_ConfirmExit;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_CancelExit;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Webs|UI")
	TObjectPtr<UWidgetSwitcher> WS_MainMenu;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuLan> WBP_MenuLan;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWebsOnlineMenu> WBP_MenuWidgetOW;

	UFUNCTION()
	void OnOnlineClicked();
	
	UFUNCTION()
	void OnLanClicked();
	
	UFUNCTION()
	void OnExitClicked();

	UFUNCTION()
	void OnConfirmExitClicked();

	UFUNCTION()
	void OnCancelExitClicked();
	
	UFUNCTION()
	void OnLanBackRequested();
	
	UFUNCTION()
	void OnOnlineBackRequested();
};
