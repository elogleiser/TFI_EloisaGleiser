// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/WebPatterns/WebPatternDefinition.h"
#include "WebsDrawingCanvas.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDrawingFinished,float,Accuracy);

UCLASS()
class TFI_ELOISAGLEISER_API UWebsDrawingCanvas : public UUserWidget
{
	GENERATED_BODY()
	
	
public:

	void ClearDrawing();
	
	const TArray<FVector2D>& GetDrawnPoints() const
	{
		return DrawnPoints;
	}
	
	UFUNCTION(BlueprintCallable, Category="Webs|Drawing")
	void SetPattern(UWebPatternDefinition* NewPattern);
	
	UFUNCTION(BlueprintCallable, Category="Webs|Drawing")
	float CalculateAccuracy() const;
	
	UPROPERTY(BlueprintAssignable, Category="Webs|Drawing")
	FOnDrawingFinished OnDrawingFinished;
	
	UFUNCTION(BlueprintCallable, Category="Webs|Drawing")
	float GetRequiredAccuracy() const;
	
protected:

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;
	virtual int32 NativePaint(const FPaintArgs& Args,const FGeometry& AllottedGeometry,const FSlateRect& MyCullingRect,FSlateWindowElementList& OutDrawElements,int32 LayerId,const FWidgetStyle& InWidgetStyle,bool bParentEnabled) const override;

	
	
private:

	UPROPERTY()
	TArray<FVector2D> DrawnPoints;
	
	bool bIsDrawing = false;
	
	UPROPERTY()
	TObjectPtr<UWebPatternDefinition> CurrentPattern;

	static float DistanceToSegment(const FVector2D& Point,const FVector2D& SegmentStart,const FVector2D& SegmentEnd);
	
};
