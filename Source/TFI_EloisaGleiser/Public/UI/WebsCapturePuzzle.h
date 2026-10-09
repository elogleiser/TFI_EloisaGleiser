// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WebsCapturePuzzle.generated.h"

/**
 * 
 */

class UButton;
class UWebPatternDefinition;
class UWebsDrawingCanvas;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCapturePuzzleExitRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCapturePuzzleCompleted);

UCLASS()
class TFI_ELOISAGLEISER_API UWebsCapturePuzzle : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintAssignable, Category = "Webs|Puzzle")
	FOnCapturePuzzleExitRequested OnExitRequested;
	
	UPROPERTY(BlueprintAssignable, Category = "Webs|Puzzle")
	FOnCapturePuzzleCompleted OnPuzzleCompleted;
	
	void SetWebPattern(UWebPatternDefinition* NewPattern);
	
protected:
	virtual void NativeOnInitialized() override;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UWebsDrawingCanvas> DrawingCanvas;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TXT_Accuracy;

	UFUNCTION()
	void HandleDrawingFinished(float Accuracy);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_Exit;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_FakeComplete;
	
	UFUNCTION()
	void OnExitClicked();
	
	UFUNCTION()
	void OnFakeCompleteClicked();
};
