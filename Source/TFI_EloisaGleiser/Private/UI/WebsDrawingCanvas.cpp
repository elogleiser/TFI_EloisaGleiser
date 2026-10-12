// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WebsDrawingCanvas.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Input/Reply.h"
#include "Rendering/DrawElements.h"
#include "Gameplay/WebPatterns/WebPatternValidator.h"


void UWebsDrawingCanvas::SetPattern(UWebPatternDefinition* NewPattern)
{
	CurrentPattern = NewPattern;

	ClearDrawing();

	Invalidate(EInvalidateWidgetReason::Paint);
}

float UWebsDrawingCanvas::CalculateAccuracy() const
{
	if (!CurrentPattern || DrawnPoints.Num() < 2)
	{
		return 0.0f;
	}

	const FVector2D CanvasSize =GetCachedGeometry().GetLocalSize();

	if (CanvasSize.X <= 0.0 || CanvasSize.Y <= 0.0)
	{
		return 0.0f;
	}

	// Convertir los puntos del mouse a coordenadas normalizadas

	TArray<FVector2D> NormalizedDrawnPoints;

	NormalizedDrawnPoints.Reserve(DrawnPoints.Num());

	for (const FVector2D& Point : DrawnPoints)
	{
		NormalizedDrawnPoints.Add(FVector2D(Point.X / CanvasSize.X,Point.Y / CanvasSize.Y));
	}

	// Utilizar el validador compartido

	return FWebPatternValidator::CalculateAccuracy(CurrentPattern->PatternData.TargetPoints,NormalizedDrawnPoints);
}

float UWebsDrawingCanvas::GetRequiredAccuracy() const
{
	if (!CurrentPattern)
	{
		return 1.0f;
	}

	return CurrentPattern->PatternData.RequiredAccuracy;
}

TArray<FVector2D> UWebsDrawingCanvas::GetNormalizedDrawnPoints() const
{
	TArray<FVector2D> NormalizedPoints;

	const FVector2D CanvasSize = GetCachedGeometry().GetLocalSize();

	if (CanvasSize.X <= 0.0 || CanvasSize.Y <= 0.0)
	{
		return NormalizedPoints;
	}

	NormalizedPoints.Reserve(DrawnPoints.Num());

	for (const FVector2D& Point : DrawnPoints)
	{
		NormalizedPoints.Add(FVector2D(Point.X / CanvasSize.X,Point.Y / CanvasSize.Y));
	}

	return NormalizedPoints;
}

FReply UWebsDrawingCanvas::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return FReply::Unhandled();
	}

	bIsDrawing = true;
	DrawnPoints.Empty();

	const FVector2D LocalPosition =InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	DrawnPoints.Add(LocalPosition);

	return FReply::Handled().CaptureMouse(TakeWidget());
}

FReply UWebsDrawingCanvas::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!bIsDrawing)
	{
		return FReply::Unhandled();
	}

	const FVector2D LocalPosition =InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	if (DrawnPoints.IsEmpty() ||FVector2D::Distance(DrawnPoints.Last(),LocalPosition) > 3.0f)
	{
		DrawnPoints.Add(LocalPosition);

		Invalidate(EInvalidateWidgetReason::Paint);
	}

	return FReply::Handled();
}

FReply UWebsDrawingCanvas::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton
	   || !bIsDrawing)
	{
		return FReply::Unhandled();
	}

	bIsDrawing = false;

	const float Accuracy = CalculateAccuracy();

	OnDrawingFinished.Broadcast(Accuracy);

	return FReply::Handled().ReleaseMouseCapture();
}

void UWebsDrawingCanvas::ClearDrawing()
{
	DrawnPoints.Empty();
	bIsDrawing = false;

	Invalidate(EInvalidateWidgetReason::Paint);
}

int32 UWebsDrawingCanvas::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const int32 ParentLayer = Super::NativePaint(Args,AllottedGeometry,MyCullingRect,OutDrawElements,LayerId,InWidgetStyle,bParentEnabled);
	
	if (CurrentPattern)
	{
		const TArray<FVector2D>& TargetPoints =
			CurrentPattern->PatternData.TargetPoints;

		if (TargetPoints.Num() >= 2)
		{
			TArray<FVector2D> LocalTargetPoints;

			const FVector2D CanvasSize = AllottedGeometry.GetLocalSize();

			for (const FVector2D& Point : TargetPoints)
			{
				LocalTargetPoints.Add(FVector2D(Point.X * CanvasSize.X,Point.Y * CanvasSize.Y));
			}

			FSlateDrawElement::MakeLines(OutDrawElements,ParentLayer + 1,AllottedGeometry.ToPaintGeometry(),LocalTargetPoints,ESlateDrawEffect::None,FLinearColor(0.1f, 0.7f, 0.6f, 0.5f),true,5.0f);
		}
	}
	
	if (DrawnPoints.Num() >= 2)
	{
		FSlateDrawElement::MakeLines(OutDrawElements,ParentLayer + 2,AllottedGeometry.ToPaintGeometry(),DrawnPoints,ESlateDrawEffect::None,FLinearColor::White,true,4.0f);
	}

	return ParentLayer + 2;
}


