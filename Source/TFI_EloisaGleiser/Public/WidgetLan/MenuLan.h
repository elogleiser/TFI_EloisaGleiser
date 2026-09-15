// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "MenuLan.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLanBackRequested); 
UCLASS()
class TFI_ELOISAGLEISER_API UMenuLan : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	virtual void NativeOnInitialized() override;
	
	UPROPERTY(BlueprintAssignable, Category = "Webs|Navigation")
	FOnLanBackRequested OnBackRequested;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Hostear;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Unirse;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_DireccionIP;
	
	UPROPERTY(EditDefaultsOnly)	
	TSoftObjectPtr<UWorld> Level;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_Volver;
	
	UFUNCTION()	
	void OnButtonHostearClicked	();
	
	UFUNCTION()	
	void OnButtonUnirseClicked	();
	
	UFUNCTION()
	void OnButtonVolverClicked();

};
