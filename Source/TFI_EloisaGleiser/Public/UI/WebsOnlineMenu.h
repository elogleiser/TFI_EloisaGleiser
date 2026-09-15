// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Menu.h"
#include "WebsOnlineMenu.generated.h"

/**
 * 
 */

class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOnlineBackRequested);

UCLASS()
class TFI_ELOISAGLEISER_API UWebsOnlineMenu : public UMenu
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Webs|Navigation")
	FOnOnlineBackRequested OnBackRequested;
protected:
	virtual void NativeOnInitialized() override;
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_Volver;
	
	UFUNCTION()
	void OnVolverClicked();
};
