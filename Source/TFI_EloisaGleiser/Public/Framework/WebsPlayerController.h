// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TFI_EloisaGleiserPlayerController.h"
#include "WebsPlayerController.generated.h"

/**
 * 
 */

class UInputAction;
class UWebsPauseMenu;
class UWebsCapturePuzzle;

UCLASS()
class TFI_ELOISAGLEISER_API AWebsPlayerController : public ATFI_EloisaGleiserPlayerController
{
	GENERATED_BODY()
	
public:
	virtual void SetupInputComponent() override;
	
	void OpenCapturePuzzle();
	
	UFUNCTION(Client, Reliable)
	void ClientOpenCapturePuzzle();


protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Webs|UI")
	TSubclassOf<UWebsCapturePuzzle> CapturePuzzleClass;

	UPROPERTY()
	TObjectPtr<UWebsCapturePuzzle> CapturePuzzle;
	
private:
	// Blueprint del Pause Menu que se elige desde ue
	UPROPERTY(EditDefaultsOnly, Category = "Webs|UI")
	TSubclassOf<UWebsPauseMenu> PauseMenuClass;

	// Instancia concreta del menú creada durante la partida
	UPROPERTY()
	TObjectPtr<UWebsPauseMenu> PauseMenu;
	
	UPROPERTY(EditDefaultsOnly, Category = "Webs|Input")
	TObjectPtr<UInputAction> PauseAction;

	UFUNCTION()
	void TogglePauseMenu();

	void OpenPauseMenu();
	
	void ClosePauseMenu();
	
	UFUNCTION()
	void OnResumeRequested();
	
	UFUNCTION()
	void OnMainMenuRequested();

	UFUNCTION()
	void OnSessionDestroyed(bool bWasSuccessful);

	void ReturnToMainMenu();
	
};
